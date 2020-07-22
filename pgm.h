/*
 *   File:   pgm.h
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

#ifndef __PGM_H__
#define __PGM_H__

#define CMD_START_WRITE                     0x10
#define CMD_WRITE_CHUNK                     0x11
#define CMD_START_READ                      0x12
#define CMD_READ_CHUNK                      0x13
#define CMD_START_BLANK_CHECK               0x14
#define CMD_BLANK_CHECK                     0x15
#define CMD_DEV_RESET                       0x16
#define CMD_MEASURE_12V                     0x17
#define CMD_TEST                            0x18
#define CMD_TEST_READ                       0x19

#define PGM_ERR_BADACK                      -2
#define PGM_ERR_TIMEOUT                     -1
#define PGM_ERR_OK                          0x00
#define PGM_ERR_PROCEED_DUAL_SOCKET         0x01
#define PGM_ERR_COMPLETE                    0x02
#define PGM_ERR_INVALID_COMMAND             0x03
#define PGM_ERR_NOTSUPPORTED                0x04
#define PGM_ERR_NO_HARDWARE                 0x05
#define PGM_ERR_INCORRECT_HARDWARE          0x06
#define PGM_ERR_INCORRECT_SWITCH_POSITION   0x07
#define PGM_ERR_NOT_BLANK                   0x09
#define PGM_ERR_MAX_RETRIES_EXCEEDED        0x0A

typedef enum
{
    NotSet = -1,
    C1702A = 0x00,
    C2704 = 0x01,
    C2708 = 0x02,
    MCM6876X = 0x03
} device_type_t;

typedef struct
{
    bool blank;
    uint16_t offset;
    uint8_t data;
} blank_check_result_t;

typedef struct
{
    uint8_t max_writes_per_byte;
    uint32_t total_writes;
} write_result_t;

typedef struct
{
    bool matches;
    int offset;
    uint8_t file;
    uint8_t device;
} verify_result_t;

bool pgm_check_supply_voltage(port_handle_t port, float *measured_voltage);
bool pgm_read(port_handle_t port, device_type_t dev_type, uint8_t *buffer, verify_result_t *verify_result, void(*pct_callback)(int pct), void(*ds_callback)(void));
bool pgm_blank_check(port_handle_t port, device_type_t dev_type, blank_check_result_t *blank_check, void(*ds_callback)(void));
bool pgm_write(port_handle_t port, device_type_t dev_type, uint8_t *buffer, int pass, int num_passes, bool hit_till_set,
    uint8_t num_retries, write_result_t *write_result, void(*pct_callback)(int pct), void(*ds_callback)(void));
bool pgm_reset(port_handle_t port);
bool pgm_test(port_handle_t port, device_type_t dev_type, uint8_t test_index);
bool pgm_test_read(port_handle_t port, device_type_t dev_type, uint8_t *data_read);
int pgm_get_dev_size(device_type_t device_type);

#endif /* __PGM_H__ */