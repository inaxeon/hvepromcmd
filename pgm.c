/*
 *   File:   pgm.c
 *   Author: Matthew Millman (inaxeon@hotmail.com)
 *
 *   1702A/270x/MCM6876x/MCS48 Programmer
 *
 *   'C' version of IspDevice.cs
 *
 *   This is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *   This software is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License
 *   along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "pch.h"

#include "project.h"
#include "serial.h"
#include "pgm.h"

// excruciatingly small chunk sizes, to ensure that 16550A FIFO's are not overflowed - 
// because there is no flow control on this thing

#define READ_CHUNK_SIZE     8
#define WRITE_CHUNK_SIZE    8

#define MAKE_U16(b1, b2) ((b1 << 8) | (b2))
#define MAKE_U32(b1, b2, b3, b4) ((b1 << 24) | (b2 << 16) | (b3 << 8) | (b4))

static bool check_return_code(port_handle_t port, uint8_t command);

bool pgm_check_supply_voltage(port_handle_t port, float *measured_voltage)
{
    uint8_t buffer[2];
    unsigned short deci_voltage;

    buffer[0] = CMD_MEASURE_12V;
    buffer[1] = ~CMD_MEASURE_12V;

    if (!serial_write(port, buffer, 2))
        return false;

    if (!check_return_code(port, CMD_MEASURE_12V))
        return false;

    if (!serial_read(port, buffer, sizeof(buffer)))
        return false;

    deci_voltage = MAKE_U16(buffer[0], buffer[1]);

    *measured_voltage = ((float)deci_voltage / 100);

    return true;
}

bool pgm_read(port_handle_t port, device_type_t dev_type, uint8_t *buffer, verify_result_t *verify_result, void (*pct_callback)(int pct), void (*ds_callback)(void))
{
    uint8_t write_buffer[3];
    int total_size = pgm_get_dev_size(dev_type);
    int bytes_read = 0;

    write_buffer[0] = CMD_START_READ;
    write_buffer[1] = ~CMD_START_READ;
    write_buffer[2] = (uint8_t)dev_type;

    if (pct_callback)
        pct_callback(0);

    if (!serial_write(port, write_buffer, 3))
        return false;

    if (!check_return_code(port, CMD_START_READ))
        return false;

    if (_g_last_error == PGM_ERR_PROCEED_DUAL_SOCKET)
    {
        if (ds_callback)
            ds_callback();
    }

    while (bytes_read < total_size)
    {
        uint8_t verify_buffer[READ_CHUNK_SIZE];
        int this_read;

        write_buffer[0] = CMD_READ_CHUNK;
        write_buffer[1] = ~CMD_READ_CHUNK;

        if (!serial_write(port, write_buffer, 2))
            return false;

        if (!check_return_code(port, CMD_READ_CHUNK))
            return false;

        this_read = (total_size - bytes_read) > READ_CHUNK_SIZE ? READ_CHUNK_SIZE : (total_size - bytes_read);

        if (!serial_read(port, verify_result ? verify_buffer : (buffer + bytes_read), this_read))
            return false;

        if (verify_result)
        {
            for (int i = 0; i < this_read; i++)
            {
                uint8_t *input_buffer = (buffer + bytes_read);
                if (input_buffer[i] != verify_buffer[i])
                {
                    verify_result->matches = false;
                    verify_result->offset = bytes_read + i;
                    verify_result->file = input_buffer[i];
                    verify_result->device = verify_buffer[i];
                    
                    return true;
                }
            }
        }

        bytes_read += this_read;

        if (pct_callback)
            pct_callback((bytes_read * 100) / total_size);
    }

    if (_g_last_error != PGM_ERR_COMPLETE)
    {
        _g_last_error = PGM_ERR_BADACK;
        return false;
    }

    if (verify_result)
        verify_result->matches = true;

    return true;
}

bool pgm_blank_check(port_handle_t port, device_type_t dev_type, blank_check_result_t *blank_check_result, void (*ds_callback)(void))
{
    uint8_t write_buffer[3];
    uint8_t read_buffer[3];

    write_buffer[0] = CMD_START_BLANK_CHECK;
    write_buffer[1] = ~CMD_START_BLANK_CHECK;
    write_buffer[2] = (uint8_t)dev_type;

    if (!serial_write(port, write_buffer, 3))
        return false;

    if (!check_return_code(port, CMD_START_BLANK_CHECK))
        return false;

    if (_g_last_error == PGM_ERR_PROCEED_DUAL_SOCKET)
    {
        if (ds_callback)
            ds_callback();
    }

    write_buffer[0] = CMD_BLANK_CHECK;
    write_buffer[1] = ~CMD_BLANK_CHECK;

    if (!serial_write(port, write_buffer, 2))
        return false;

    if (!check_return_code(port, CMD_BLANK_CHECK))
        return false;

    if (_g_last_error == PGM_ERR_COMPLETE)
    {
        blank_check_result->blank = true;
        return true;
    }

    if (!serial_read(port, read_buffer, 3))
        return false;

    blank_check_result->blank = false;
    blank_check_result->offset = MAKE_U16(read_buffer[0], read_buffer[1]);
    blank_check_result->data = read_buffer[2];

    return true;
}

bool pgm_write(port_handle_t port, device_type_t dev_type, uint8_t *buffer, int pass, int num_passes, bool hit_till_set,
    uint8_t num_retries, write_result_t *write_result, void (*pct_callback)(int pct), void (*ds_callback)(void))
{
    uint8_t cmd_buffer[5];
    uint8_t read_buffer[5];
    int total_size = pgm_get_dev_size(dev_type);
    int bytes_written = 0;

    cmd_buffer[0] = CMD_START_WRITE;
    cmd_buffer[1] = ~CMD_START_WRITE;
    cmd_buffer[2] = (uint8_t)dev_type;
    cmd_buffer[3] = hit_till_set ? 0x01 : 0x00;
    cmd_buffer[4] = num_retries;

    if (pct_callback)
        pct_callback(0);

    if (!serial_write(port, cmd_buffer, 5))
        return false;

    if (!check_return_code(port, CMD_START_WRITE))
        return false;

    if (_g_last_error == PGM_ERR_PROCEED_DUAL_SOCKET)
    {
        if (ds_callback)
            ds_callback();
    }

    while (bytes_written < total_size)
    {
        uint8_t write_buffer[2 + WRITE_CHUNK_SIZE];
        int this_write;

        write_buffer[0] = CMD_WRITE_CHUNK;
        write_buffer[1] = ~CMD_WRITE_CHUNK;

        this_write = ((total_size - bytes_written) > WRITE_CHUNK_SIZE ? WRITE_CHUNK_SIZE : (total_size - bytes_written));

        memcpy(write_buffer + 2, buffer + bytes_written, this_write);

        if (!serial_write(port, write_buffer, 2 + this_write))
            return false;

        if (!check_return_code(port, CMD_WRITE_CHUNK))
            return false;

        bytes_written += this_write;

        if (pct_callback)
            pct_callback((((pass * 10000) / num_passes) + (((bytes_written * 10000) / total_size) / num_passes)) / 100);
    }

    if (_g_last_error != PGM_ERR_COMPLETE)
    {
        _g_last_error = PGM_ERR_BADACK;
        return false;
    }

    if (!serial_read(port, read_buffer, 5))
        return false;

    // Only non-zero when hit_till_set = 1
    write_result->max_writes_per_byte = read_buffer[0];
    write_result->total_writes = MAKE_U32(read_buffer[1], read_buffer[2], read_buffer[3], read_buffer[4]);

    return true;
}

bool pgm_test(port_handle_t port, device_type_t dev_type, uint8_t test_index)
{
    uint8_t write_buffer[4];

    write_buffer[0] = CMD_TEST;
    write_buffer[1] = ~CMD_TEST;
    write_buffer[2] = (uint8_t)dev_type;
    write_buffer[3] = test_index;

    if (!serial_write(port, write_buffer, 4))
        return false;

    if (!check_return_code(port, CMD_TEST))
        return false;

    return true;
}

bool pgm_test_read(port_handle_t port, device_type_t dev_type, uint8_t *data_read)
{
    uint8_t write_buffer[3];

    write_buffer[0] = CMD_TEST_READ;
    write_buffer[1] = ~CMD_TEST_READ;
    write_buffer[2] = (uint8_t)dev_type;

    if (!serial_write(port, write_buffer, 3))
        return false;

    if (!check_return_code(port, CMD_TEST_READ))
        return false;

    if (_g_last_error != PGM_ERR_OK)
        return false;

    if (!serial_read(port, data_read, 1))
        return false;

    return true;
}

bool pgm_reset(port_handle_t port)
{
    uint8_t buffer[2];

    buffer[0] = CMD_DEV_RESET;
    buffer[1] = ~CMD_DEV_RESET;

    if (!serial_write(port, buffer, 2))
        return false;

    if (!check_return_code(port, CMD_DEV_RESET))
        return false;

    return true;
}

static bool check_return_code(port_handle_t port, uint8_t command)
{
    uint8_t c;

    if (!serial_read(port, &c, sizeof(c)))
        return false;

    if (c != command)
    {
        _g_last_error = PGM_ERR_BADACK;
        return false;
    }

    if (!serial_read(port, &c, sizeof(c)))
        return false;

    _g_last_error = c;

    if (_g_last_error == PGM_ERR_OK ||
        _g_last_error == PGM_ERR_PROCEED_DUAL_SOCKET ||
        _g_last_error == PGM_ERR_COMPLETE ||
        _g_last_error == PGM_ERR_NOT_BLANK)
    {
        return true;
    }

    return false;
}

int pgm_get_dev_size(device_type_t device_type)
{
    switch (device_type)
    {
    case C1702A:
        return 0x100;
    case C2704:
        return 0x200;
    case C2708:
        return 0x400;
    case MCM6876X:
        return 0x2000;
    default:
        return -1;
    }
}
