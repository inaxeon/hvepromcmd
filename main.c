/*
 *   File:   main.c
 *   Author: Matthew Millman (inaxeon@hotmail.com)
 *
 *   1702A/270x/MCM6876x/MCS48 Programmer
 *
 *   Main routine
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
#include "getopt.h"
#include "serial.h"
#include "pgm.h"

#define PROGRESS_BAR_SEGMENTS   58

typedef enum
{
    None = -1,
    Read,
    Write,
    Verify,
    BlankCheck,
    Measure12V,
} operation_t;

int _g_last_error;
int _g_segments_printed;

static bool target_read(port_handle_t port, device_type_t dev_type, const char *filename);
static bool target_blank_check(port_handle_t port, device_type_t dev_type);
static bool target_write(port_handle_t port, device_type_t dev_type, const char *filename, int num_passes, bool blank_check, bool verify, bool hit_till_set, uint8_t num_retries);
static bool target_verify(port_handle_t port, device_type_t dev_type, const char *filename);
static bool target_measure_12v(port_handle_t port, device_type_t dev_type);
static bool work_blank_check(port_handle_t port, device_type_t dev_type, bool *blank);
static bool work_verify(port_handle_t port, device_type_t dev_type, const char *filename, bool *matches);
static void print_progress(int pct);
static void print_passes(int pass, int num_passes);
static void print_progress_outline();
static void print_target_error(void);

int main(int argc, char *argv[])
{
    bool operation_result = false;
    bool hit_until_set = true;
    bool blank_check = false;
    bool verify = false;
    int opt = 0;
    int baud = 38400;
    int num_passes = 0;
    int num_retries = 0;
    char port_name[32];
    char *filename = NULL;
    operation_t operation = None;
    device_type_t dev_type;
    port_handle_t port = NULL;

    _g_last_error = PGM_ERR_OK;

    memset(port_name, 0, sizeof(port_name));

    while ((opt = getopt(argc, argv, "o:p:b:d:f:n:r:mcv?")) != -1)
    {
        switch (opt)
        {
            case 'o':
            {
                if (!_stricmp(optarg, "read"))
                    operation = Read;
                else if (!_stricmp(optarg, "write"))
                    operation = Write;
                else if (!_stricmp(optarg, "verify"))
                    operation = Verify;
                else if (!_stricmp(optarg, "blankcheck"))
                    operation = BlankCheck;
                else if (!_stricmp(optarg, "measure12v"))
                    operation = Measure12V;
                break;
            }
            case 'd':
            {
                if (!_stricmp(optarg, "1702A"))
                    dev_type = C1702A;
                else if (!_stricmp(optarg, "2704"))
                    dev_type = C2704;
                else if (!_stricmp(optarg, "2708"))
                    dev_type = C2708;
                else if (!_stricmp(optarg, "MCM6876X"))
                    dev_type = MCM6876X;
                break;
            }
            case 'p':
            {
                strcpy_s(port_name, sizeof(port_name), optarg);
                _strupr_s(port_name, sizeof(port_name));
                break;
            }
            case 'b':
            {
                baud = atoi(optarg);
                break;
            }
            case 'f':
            {
                filename = _strdup(optarg);
                break;
            }
            case 'n':
            {
                num_passes = atoi(optarg);
                break;
            }
            case 'r':
            {
                num_retries = atoi(optarg);
                break;
            }
            case 'm':
            {
                hit_until_set = false;
                break;
            }
            case 'c':
            {
                blank_check = true;
                break;
            }
            case 'v':
            {
                verify = true;
                break;
            }

            default:
            {
                fprintf(stderr, "\r\nUsage: %s -o operation [options]\r\n\r\n"
                    , argv[0]);
                return EXIT_FAILURE;
            }
        }
    }

    if (baud != 9600 && baud != 38400 && baud != 115200)
    {
        fprintf(stderr, "\r\nInvalid baud rate. Must be 9600, 38400 or 115200\r\n");
        operation_result = false;
        goto out;
    }

    if (port_name[0] == 0)
    {
        fprintf(stderr, "\r\nNo serial port specified.\r\n");
        operation_result = false;
        goto out;
    }

#ifdef _WIN32
    if (strncmp(port_name, "COM", 3))
    {
        fprintf(stderr, "\r\nInvalid serial port format.\r\n");
        operation_result = false;
        goto out;
    }
#endif

    if (operation == None)
    {
        fprintf(stderr, "\r\nNo operation specified.\r\n");
        operation_result = false;
        goto out;
    }

    if (dev_type == NotSet)
    {
        fprintf(stderr, "\r\nNo device type specified.\r\n");
        operation_result = false;
        goto out;
    }

    if (operation == Read || operation == Write || operation == Verify)
    {
        if (!filename)
        {
            fprintf(stderr, "\r\nNo filename specified.\r\n");
            operation_result = false;
            goto out;
        }
    }

    if (!serial_open(port_name, baud, &port))
    {
        fprintf(stderr, "\r\nFailed to open serial port.\r\n");
        operation_result = false;
        goto out;
    }
    
    switch (dev_type)
    {
        case C1702A:
        {
            if (!num_passes)
                num_passes = 32;
            hit_until_set = false;
            break;
        }
        case C2704:
        case C2708:
        {
            if (!num_passes)
                num_passes = 100;
            hit_until_set = false;
            break;
        }
        case MCM6876X:
        {
            if (!num_passes)
                num_passes = 1;
            if (!num_retries)
                num_retries = 5;
            break;
        }
    }

    switch (operation)
    {
        case Read:
            operation_result = target_read(port, dev_type, filename);
            break;
        case BlankCheck:
            operation_result = target_blank_check(port, dev_type);
            break;
        case Verify:
            operation_result = target_verify(port, dev_type, filename);
            break;
        case Write:
            operation_result = target_write(port, dev_type, filename, num_passes, blank_check, verify, hit_until_set, num_retries);
            break;
        case Measure12V:
            operation_result = target_measure_12v(port, dev_type);
            break;
    }

out:
    serial_close(port);

    if (filename)
        free(filename);

    if (!operation_result)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

static bool target_read(port_handle_t port, device_type_t dev_type, const char *filename)
{
    bool success = false;
    uint8_t *read_buffer = NULL;
    FILE *output_file;
    int dev_size = pgm_get_dev_size(dev_type);

    if (!target_measure_12v(port, dev_type))
        return false;

    printf("Reading device...\r\n\r\n");

    read_buffer = malloc(dev_size);

    print_progress_outline();

    if (!pgm_read(port, dev_type, read_buffer, NULL, &print_progress, NULL))
    {
        print_target_error();
        success = false;
        goto out;
    }

    printf("\r\n\r\nRead successful.\r\n");

    if (fopen_s(&output_file, filename, "wb"))
    {
        fprintf(stderr, "\r\nFailed to open output file for writing.\r\n");
        success = false;
        goto out;
    }

    fwrite(read_buffer, sizeof(byte), dev_size, output_file);
    fclose(output_file);
    success = true;

out:
    pgm_reset(port);
    if (read_buffer)
        free(read_buffer);
    return success;
}

static bool target_blank_check(port_handle_t port, device_type_t dev_type)
{
    bool success = false;

    if (!target_measure_12v(port, dev_type))
        return false;

    if (!work_blank_check(port, dev_type, NULL))
    {
        success = false;
        goto out;
    }
    
    success = true;

out:
    pgm_reset(port);
    return success;
}

static bool target_write(port_handle_t port, device_type_t dev_type, const char *filename, int num_passes, bool blank_check, bool verify, bool hit_till_set, uint8_t num_retries)
{
    bool success = false;
    bool blank;
    bool matches;
    uint8_t *write_buffer = NULL;
    write_result_t write_result;
    FILE *input_file;
    int file_size;
    int dev_size = pgm_get_dev_size(dev_type);

    if (fopen_s(&input_file, filename, "rb"))
    {
        fprintf(stderr, "\r\nFailed to open input file for reading.\r\n");
        return false;
    }

    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    write_buffer = malloc(file_size);

    fread(write_buffer, sizeof(byte), file_size, input_file);
    fclose(input_file);

    if (file_size > dev_size)
    {
        fprintf(stderr, "\r\nInput file too large for device.\r\n");
        success = false;
        goto out;
    }

    if (!target_measure_12v(port, dev_type))
        return false;

    if (blank_check)
    {
        if (!work_blank_check(port, dev_type, &blank))
        {
            success = false;
            goto out;
        }

        if (!blank)
        {
            success = false;
            goto out;
        }
    }

    printf("Writing device...\r\n\r\n");

    if (hit_till_set)
        num_passes = 1;

    print_progress_outline();

    for (int pass = 0; pass < num_passes; pass++)
    {
        if (!hit_till_set)
            print_passes(pass + 1, num_passes);

        if (!pgm_write(port, dev_type, write_buffer, pass, num_passes, hit_till_set, num_retries, &write_result, &print_progress, NULL))
        {
            print_target_error();
            success = false;
            goto out;
        }
    }

    if (verify)
    {
        printf("\r\n\r\n");
        if (!work_verify(port, dev_type, filename, &matches))
        {
            success = false;
            goto out;
        }
    }

    if (!verify)
        printf("\r\n\r\nWrite successful.\r\n");

    if (hit_till_set)
    {
        printf("\r\nMaximum number of retries on a single byte: %u\r\n", write_result.max_writes_per_byte);
        printf("Total number of writes across entire device: %u\r\n", write_result.total_writes);
    }

    success = true;

out:
    pgm_reset(port);
    if (write_buffer)
        free(write_buffer);
    return success;
}

static bool target_verify(port_handle_t port, device_type_t dev_type, const char *filename)
{
    bool success = false;

    if (!target_measure_12v(port, dev_type))
        return false;

    if (!work_verify(port, dev_type, filename, NULL))
    {
        success = false;
        goto out;
    }

    success = true;

out:
    pgm_reset(port);
    return success;
}

static bool target_measure_12v(port_handle_t port, device_type_t dev_type)
{
    float measured_voltage;

    if (!pgm_check_supply_voltage(port, &measured_voltage))
    {
        print_target_error();
        return false;
    }

    if (dev_type == C2704 || dev_type == C2708)
    {
        if (measured_voltage < 12.0 || measured_voltage >= 13.0)
        {
            fprintf(stderr, "\r\nSupply voltage is out of range for this device type.\r\n\r\nVin=%.2fV Min=12.00V Max=13.00V\r\n", measured_voltage);
            return false;
        }
    }

    if (measured_voltage < 10.0 || measured_voltage >= 14.0)
    {
        fprintf(stderr, "\r\nProgrammer host must be connected to a 12V supply.\r\n\r\nVin=%.2fV Min=10.00V Max=14.00V\r\n", measured_voltage);
        return false;
    }

    printf("\r\nTarget supply voltage: %.2f V (OK)\r\n\r\n", measured_voltage);

    return true;
}

static bool work_verify(port_handle_t port, device_type_t dev_type, const char *filename, bool *matches)
{
    bool success = false;
    uint8_t *input_buffer = NULL;
    verify_result_t verify_result;
    FILE *input_file;
    int dev_size = pgm_get_dev_size(dev_type);
    int file_size;

    if (fopen_s(&input_file, filename, "rb"))
    {
        fprintf(stderr, "\r\nFailed to open input file for reading.\r\n");
        return false;
    }

    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    input_buffer = malloc(file_size);

    fread(input_buffer, sizeof(byte), file_size, input_file);
    fclose(input_file);

    if (file_size > dev_size)
    {
        fprintf(stderr, "\r\nInput file is larger than device.\r\n");
        *matches = false;
        success = true;
        goto out;
    }

    printf("Verifying device...\r\n\r\n");

    print_progress_outline();

    if (!pgm_read(port, dev_type, input_buffer, &verify_result, &print_progress, NULL))
    {
        print_target_error();
        success = false;
        goto out;
    }

    if (!verify_result.matches)
    {
        fprintf(stderr, "\r\n\r\nVerify failed at 0x%04X. File=0x%02X Device=0x%02X\r\n", verify_result.offset, verify_result.file, verify_result.device);
        if (matches)
            *matches = false;
        success = true;
        goto out;
    }

    printf("\r\n\r\nVerify successful.\r\n");
    success = true;

    if (matches)
        *matches = true;

out:
    pgm_reset(port);
    if (input_buffer)
        free(input_buffer);
    return success;
}

static bool work_blank_check(port_handle_t port, device_type_t dev_type, bool *blank)
{
    blank_check_result_t blank_check;

    printf("Blank checking device...\r\n");

    if (!pgm_blank_check(port, dev_type, &blank_check, NULL))
    {
        print_target_error();
        return false;
    }

    if (blank_check.blank)
        printf("\r\nDevice is blank.\r\n\r\n");
    else
        printf("\r\nDevice not blank. Offset = 0x%04X Data = 0x%02X\r\n", blank_check.offset, blank_check.data);

    if (blank)
        *blank = blank_check.blank;

    return true;
}

static void print_progress_outline()
{
    _g_segments_printed = 0;

    printf("\033[%dC]\r[", PROGRESS_BAR_SEGMENTS + 1);
}

static void print_passes(int pass, int num_passes)
{
    int offset = PROGRESS_BAR_SEGMENTS - _g_segments_printed + 2;
    int passlen;
    printf("\033[%dC", offset);
    passlen = printf("%d/%d", pass, num_passes);
    printf("\033[%dD", offset + passlen);
}

static void print_progress(int pct)
{
    int segments_needed = (PROGRESS_BAR_SEGMENTS * (pct * 100)) / 10000;
    int segments_to_print = (segments_needed - _g_segments_printed);
    if (segments_needed > _g_segments_printed)
    {
        for (int i = 0; i < segments_to_print; i++)
            fputc('=', stdout);
        _g_segments_printed += segments_to_print;
    }
}

static void print_target_error(void)
{
    fprintf(stderr, "\r\n\r\nOperation failed: ");

    switch (_g_last_error)
    {
    case PGM_ERR_BADACK:
        fprintf(stderr, "A protocol error occurred communicating with the target");
        break;
    case PGM_ERR_TIMEOUT:
        fprintf(stderr, "Timed out attempting to communicate with the target");
        break;
    case PGM_ERR_INVALID_COMMAND:
        fprintf(stderr, "The target does not support this command");
        break;
    case PGM_ERR_NOTSUPPORTED:
        fprintf(stderr, "The target does not support this device");
        break;
    case PGM_ERR_NO_HARDWARE:
        fprintf(stderr, "No shield is attached to the target");
        break;
    case PGM_ERR_INCORRECT_HARDWARE:
        fprintf(stderr, "The shield attached to the target does not support this device");
        break;
    case PGM_ERR_INCORRECT_SWITCH_POSITION:
        fprintf(stderr, "The selection switch on the target is not in the correct position for this device");
        break;
    case PGM_ERR_MAX_RETRIES_EXCEEDED:
        fprintf(stderr, "The maximum number of attempt to write a byte to the device was exceeded");
        break;
    }

    fprintf(stderr, "\r\n");
}
