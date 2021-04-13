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
#include "test.h"
#include "util.h"

#define PROGRESS_BAR_SEGMENTS   58
#define MCM6876X_DEFAULT_RETRIES    5

typedef enum
{
    None = -1,
    Read,
    Write,
    Verify,
    BlankCheck,
    Measure12V,
    Test
} operation_t;

typedef enum
{
    InvalidCmd,
    NoCmd,
    NextTest,
    PrevTest,
    StartStop,
    Quit
} test_cmd_t;

int _g_last_error;
int _g_segments_printed;

static bool target_read(port_handle_t port, device_type_t dev_type, const char *filename);
static bool target_blank_check(port_handle_t port, device_type_t dev_type);
static bool target_write(port_handle_t port, device_type_t dev_type, const char *filename, int num_passes, bool blank_check, bool verify, bool hit_till_set, uint8_t parameter);
static bool target_verify(port_handle_t port, device_type_t dev_type, const char *filename);
static bool target_measure_12v(port_handle_t port, device_type_t dev_type);
static bool work_blank_check(port_handle_t port, device_type_t dev_type, bool *blank);
static bool work_verify(port_handle_t port, device_type_t dev_type, const char *filename, bool *matches);
static bool target_test(port_handle_t port, device_type_t dev_type);
static void print_progress(int pct);
static void print_passes(int pass, int num_passes);
static void print_progress_outline();
static void print_target_error(bool nl);
static void help(const char *progname);
static test_cmd_t get_test_cmd(bool nonblock);

int main(int argc, char *argv[])
{
    bool operation_result = false;
    bool hit_until_set = true;
    bool blank_check = false;
    bool verify = false;
    bool slow = false;
    int opt = 0;
    int baud = 38400;
    int num_passes = 0;
    int parameter = 0;
    char port_name[32];
    char *filename = NULL;
    operation_t operation = None;
    device_type_t dev_type = NotSet;
    port_handle_t port = DEFAULT_PORT_HANDLE;

    if (!argv[1] || !strcmp(argv[1], "/?"))
    {
        // Can't parse '/' options - but a Windows person is likely to do at least this
        help(argv[0]);
        return EXIT_FAILURE;
    }

    terminal_setup();

    _g_last_error = PGM_ERR_OK;

    memset(port_name, 0, sizeof(port_name));

    while ((opt = getopt(argc, argv, "o:p:u:d:f:n:r:mbv?")) != -1)
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
                else if (!_stricmp(optarg, "test"))
                    operation = Test;
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
                else if (!_stricmp(optarg, "8741"))
                    dev_type = D8741;
                else if (!_stricmp(optarg, "8742"))
                    dev_type = D8742;
                else if (!_stricmp(optarg, "8748"))
                    dev_type = D8748;
                else if (!_stricmp(optarg, "8749"))
                    dev_type = D8749;
                else if (!_stricmp(optarg, "8048"))
                    dev_type = P8048;
                else if (!_stricmp(optarg, "8049"))
                    dev_type = P8049;
                else if (!_stricmp(optarg, "8050"))
                    dev_type = P8050;
                else if (!_stricmp(optarg, "8755"))
                    dev_type = D8755;
                break;
            }
            case 'p':
            {
                strcpy_s(port_name, sizeof(port_name), optarg);
#ifdef _WIN32
                _strupr_s(port_name, sizeof(port_name));
#endif /* _WIN32 */
                break;
            }
            case 'u':
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
                parameter = atoi(optarg);
                break;
            }
            case 'm':
            {
                hit_until_set = false;
                break;
            }
            case 'b':
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
                help(argv[0]);
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
#ifdef _WIN32
        fprintf(stderr, "\r\nFailed to open serial port.\r\n");
#else
        fprintf(stderr, "\r\nFailed to open serial port (%s).\r\n", strerror(errno));
#endif /* _WIN32 */
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
        case D8741:
        case D8742:
        case D8748:
        case D8749:
        case P8048:
        case P8049:
        case P8050:
        case D8755:
        {
            if (!num_passes)
                num_passes = 1;
            parameter = 0;
            break;
        }
        case MCM6876X:
        {
            if (!num_passes)
                num_passes = 1;
            if (!parameter)
                parameter = 5;
            break;
        }
        default:
        {
            fprintf(stderr, "\r\nInvalid or no device type specified.\r\n");
            operation_result = false;
            goto out;
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
            operation_result = target_write(port, dev_type, filename, num_passes, blank_check, verify, hit_until_set, parameter);
            break;
        case Measure12V:
            operation_result = target_measure_12v(port, dev_type);
            break;
        case Test:
            operation_result = target_test(port, dev_type);
            break;
        default:
            fprintf(stderr, "\r\nNo operation specified.\r\n");
            operation_result = false;
            goto out;
    }

out:
    serial_close(port);

    if (filename)
        free(filename);

#ifndef _WIN32
    printf("\r\n");
#endif

    if (!operation_result)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

static void help(const char *progname)
{
    fprintf(stderr, "\r\nUsage: %s -o operation [options]\r\n\r\n"
        "Read device to file:\r\n\r\n"
        "\t%s -o read -p PORT -d DEVICE -f FILE\r\n\r\n"
#ifdef _WIN32
        "\tPORT must be in the format COMx\r\n"
#else
        "\tPORT must be in the format /dev/ttySx\r\n\r\n"
#endif
        "\tDEVICE must be one of 1702A/2704/2708/MCM6876X/8748/8749/8741/8742/8048/8049/8050/8755\r\n\r\n"
        "Blank check device:\r\n\r\n"
        "\t%s -o blankcheck -p PORT -d DEVICE\r\n\r\n"
        "Write device from file:\r\n\r\n"
        "\t%s -o write -p PORT -d DEVICE -f FILE [-b] [-v] [-r REWRITES] [-m] [-n PASSES] [-s]\r\n\r\n"
        "\tPass '-b' to blank check before write. Pass '-v' to verify device after write.\r\n\r\n"
        "\tFor MCM68676x each byte is written until it matches the desired value, then written\r\n"
        "\ta further REWRITES (-r) times. This defaults to %u if REWRITES is not specified.\r\n\r\n"
        "\tAlso for MCM68676x '-m' can be optionally passed to force the programmer\r\n"
        "\tto the simpler 'fixed passes' mode for older versions of the chip.\r\n\r\n"
        "\tFor MCS48 '-m' can be optionally passed to stop the programmer from verifying\r\n"
        "\tand retrying writes inline.\r\n\r\n"
        "\tFor all device types use '-n' to specify the number of passes.\r\n"
        "\tManufacturer recommended defaults are used if this option is not specified.\r\n\r\n"
        "Verify device against file:\r\n\r\n"
        "\t%s -o verify -p PORT -d DEVICE -f FILE [-b]\r\n\r\n"        
        "Start the hardware test for the shield of a given device type:\r\n\r\n"
        "\t%s -o test -p PORT -d DEVICE\r\n\r\n",
        progname, progname, progname, progname, MCM6876X_DEFAULT_RETRIES, progname, progname);
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
        print_target_error(true);
        success = false;
        goto out;
    }

    printf("\r\n\r\nRead successful.\r\n");

#ifdef _WIN32
    if (fopen_s(&output_file, filename, "wb"))
#else
    if (!(output_file = fopen(filename, "wb")))
#endif /* _WIN32 */
    {
        fprintf(stderr, "\r\nFailed to open output file for writing.\r\n");
        success = false;
        goto out;
    }

    fwrite(read_buffer, sizeof(uint8_t), dev_size, output_file);
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

static bool target_write(port_handle_t port, device_type_t dev_type, const char *filename, int num_passes, bool blank_check, bool verify, bool hit_till_set, uint8_t parameter)
{
    bool success = false;
    bool blank;
    bool matches;
    uint8_t *write_buffer = NULL;
    uint8_t *file_buffer = NULL;
    write_result_t write_result;
    FILE *input_file;
    size_t file_size;
    size_t file_read;
    int dev_size = pgm_get_dev_size(dev_type);

#ifdef _WIN32
    if (fopen_s(&input_file, filename, "rb"))
#else
    if (!(input_file = fopen(filename, "rb")))
#endif /* _WIN32 */
    {
        fprintf(stderr, "\r\nFailed to open input file for reading.\r\n");
        return false;
    }

    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    write_buffer = malloc(dev_size);
    file_buffer = malloc(file_size);

    file_read = fread(file_buffer, sizeof(uint8_t), file_size, input_file);
    fclose(input_file);

    if (dev_type == D8748 || dev_type == D8749 || dev_type == D8741 || dev_type == D8742 || dev_type == C1702A)
        memset(write_buffer, 0x00, dev_size);
    else
        memset(write_buffer, 0xFF, dev_size);

    if (file_read != file_size)
    {
        fprintf(stderr, "\r\nFailed to read all of input file.\r\n");
        success = false;
        goto out;
    }

    if (file_size > dev_size)
    {
        fprintf(stderr, "\r\nInput file too large for device.\r\n");
        success = false;
        goto out;
    }

    memcpy(write_buffer, file_buffer, file_size);

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

        if (!pgm_reset(port))
        {
            print_target_error(true);
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

        if (!pgm_write(port, dev_type, write_buffer, pass, num_passes, hit_till_set, parameter, &write_result, &print_progress, NULL))
        {
            print_target_error(true);
            success = false;
            goto out;
        }
    }

    if (verify)
    {
        printf("\r\n\r\n");

        if (!pgm_reset(port))
        {
            print_target_error(true);
            success = false;
            goto out;
        }

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
        printf("Maximum number of retries on a single byte: %u\r\n", write_result.max_writes_per_byte);
        printf("Total number of writes across entire device: %u\r\n", write_result.total_writes);
    }

    success = true;

out:
    pgm_reset(port);
    if (write_buffer)
        free(write_buffer);
    if (file_buffer)
        free(file_buffer);
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
        print_target_error(true);
        return false;
    }

    if (dev_type == C2704 || dev_type == C2708)
    {
        if (measured_voltage < 11.9 || measured_voltage >= 13.0)
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
    uint8_t *file_buffer = NULL;

    verify_result_t verify_result;
    FILE *input_file;
    int dev_size = pgm_get_dev_size(dev_type);
    size_t file_size;
    size_t file_read;

#ifdef _WIN32
    if (fopen_s(&input_file, filename, "rb"))
#else
    if (!(input_file = fopen(filename, "rb")))
#endif /* _WIN32 */
    {
        fprintf(stderr, "\r\nFailed to open input file for reading.\r\n");
        return false;
    }

    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    input_buffer = malloc(dev_size);
    file_buffer = malloc(file_size);

    file_read = fread(file_buffer, sizeof(uint8_t), file_size, input_file);
    fclose(input_file);

    if (dev_type == D8748 || dev_type == D8749 || dev_type == D8741 || dev_type == D8742 || dev_type == C1702A)
        memset(input_buffer, 0x00, dev_size);
    else
        memset(input_buffer, 0xFF, dev_size);

    if (file_read != file_size)
    {
        fprintf(stderr, "\r\nFailed to read all of input file.\r\n");
        *matches = false;
        success = false;
        goto out;
    }

    if (file_size > dev_size)
    {
        fprintf(stderr, "\r\nInput file is larger than device.\r\n");
        *matches = false;
        success = false;
        goto out;
    }

    memcpy(input_buffer, file_buffer, file_size);

    printf("Verifying device...\r\n\r\n");

    print_progress_outline();

    if (!pgm_read(port, dev_type, input_buffer, &verify_result, &print_progress, NULL))
    {
        print_target_error(true);
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

    printf("\r\n\r\nVerify successful.\r\n\r\n");
    success = true;

    if (matches)
        *matches = true;

out:
    pgm_reset(port);
    if (input_buffer)
        free(input_buffer);
    if (file_buffer)
        free(file_buffer);
    return success;
}

static bool work_blank_check(port_handle_t port, device_type_t dev_type, bool *blank)
{
    blank_check_result_t blank_check;

    printf("Blank checking device...\r\n");

    if (!pgm_blank_check(port, dev_type, &blank_check, NULL))
    {
        print_target_error(true);
        return false;
    }

    if (blank_check.blank)
        printf("\r\nDevice is blank.\r\n\r\n");
    else
        printf("\r\nDevice not blank. Offset = 0x%04X Data = 0x%02X\r\n\r\n", blank_check.offset, blank_check.data);

    if (blank)
        *blank = blank_check.blank;

    return true;
}

static bool target_test(port_handle_t port, device_type_t dev_type)
{
    const test_t *tests;
    int testidx;
    int testcnt = 0;
    int testlast = -1;
    bool run = false;
    int nlflag = 0;

    switch (dev_type)
    {
        case C1702A:
            tests = _g_1702a_tests;
            break;
        case C2704:
        case C2708:
            tests = _g_270x_tests;
            break;
        case MCM6876X:
            tests = _g_mcm6876x_tests;
            break;
        case D8741:
        case D8748:
        case D8742:
        case D8749:
        case P8048:
        case P8049:
        case P8050:
        case D8755:
            tests = _g_mcs48_tests;
            break;
        default:
            return false;
    }

    for (testidx = 0; tests[testidx].index; testidx++)
        testcnt++;

    testidx = 0;

    printf("\r\n");

    if (!target_measure_12v(port, dev_type))
        return false;

#ifndef _WIN32
    terminal_set_raw_mode();
#endif /* _WIN32 */

    while (1)
    {
        if (testlast != testidx)
        {
            if (nlflag)
            {
                nlflag = 0;
                printf("\r\n");
            }

            printf("-------------------------------------------------------------------------------\r\n");
            printf("\r\nTest %u of %u:\r\n\r\n", testidx + 1, testcnt);
            printf("%s", tests[testidx].desc);
            if (tests[testidx].is_read)
                printf(" [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]\r\n");
            printf("\r\n-------------------------------------------------------------------------------\r\n");
            printf("\r\nPress [spacebar] to start/stop test. Use arrow keys <-/-> to navigate through tests. Press 'Q' to quit.\r\n\r\n");
            
            testlast = testidx;
        }

        switch (get_test_cmd(tests[testidx].is_read && run))
        {
            case NextTest:
            {
                if (testidx < (testcnt - 1))
                {
                    testidx++;
                }
                else
                {
                    printf("*** Already at last test ***\r\n");
                    nlflag++;
                }

                if (run)
                {
                    run = false;
                    pgm_reset(port);
                }

                break;
            }
            case PrevTest:
            {
                if (testidx > 0)
                {
                    testidx--;
                }
                else
                {
                    printf("*** Already at first test ***\r\n");
                    nlflag++;
                }

                if (run)
                {
                    run = false;
                    pgm_reset(port);
                }

                break;
            }
            case StartStop:
            {
                if (!run)
                {
                    if (!pgm_test(port, dev_type, tests[testidx].index))
                    {
                        if (nlflag)
                        {
                            printf("\r\n");
                            nlflag = 0;
                        }
                        print_target_error(false);
                    }
                    else
                    {
                        printf("*** Test RUNNING ***\r\n");
                        nlflag++;
                        run = true;
                    }
                }
                else
                {
                    if (!pgm_reset(port))
                    {
                        if (nlflag)
                        {
                            printf("\r\n");
                            nlflag = 0;
                        }
                        print_target_error(false);
                    }
                    else
                    {
                        printf("*** Test STOPPED ***\r\n");
                        nlflag++;

                        if (tests[testidx].is_read)
                            printf("\033[%dA [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]\r\033[%dB", nlflag + 6, nlflag + 6);

                        run = false;
                    }
                }

                break;
            }
            case InvalidCmd:
            {
                printf("*** Invalid command ***\r\n");
                nlflag++;
                break;
            }
            case NoCmd:
            {
                uint8_t data;

                Sleep(500);

                if (!pgm_test_read(port, dev_type, &data))
                {
                    if (nlflag)
                    {
                        printf("\r\n");
                        nlflag = 0;
                    }
                    print_target_error(false);
                }

                printf("\033[%dA [%c]  [%c]  [%c]  [%c]  [%c]  [%c]  [%c]  [%c]\r\033[%dB",
                    nlflag + 6,
                    (data & 0x01) == 0x01 ? 'H' : 'L',
                    (data & 0x02) == 0x02 ? 'H' : 'L',
                    (data & 0x04) == 0x04 ? 'H' : 'L',
                    (data & 0x08) == 0x08 ? 'H' : 'L',
                    (data & 0x10) == 0x10 ? 'H' : 'L',
                    (data & 0x20) == 0x20 ? 'H' : 'L',
                    (data & 0x40) == 0x40 ? 'H' : 'L',
                    (data & 0x80) == 0x80 ? 'H' : 'L',
                    nlflag + 6);

                break;
            }
            case Quit:
            {
                goto done;
            }
        }
    }

done:
#ifndef _WIN32
    terminal_unset_raw_mode();
#endif /* _WIN32 */

    if (run && !pgm_reset(port))
    {
        if (nlflag)
        {
            printf("\r\n");
            nlflag = 0;
        }

        print_target_error(false);
        return false;
    }

    return true;
}

static test_cmd_t get_test_cmd(bool nonblock)
{
    if (nonblock && !_kbhit())
        return NoCmd;

    switch (getch())
    {
#ifdef _WIN32
    case 0xE0:
        switch (getch())
        {
        case 'M':
            return NextTest;
        case 'K':
            return PrevTest;
        default:
            return InvalidCmd;
        }
        break;
#else
    case 0x1B:
        switch (getch())
        {
            case '[':
                switch (getch())
                {
                    case 'C':
                        return NextTest;
                    case 'D':
                        return PrevTest;
                    default:
                        return InvalidCmd;
                }
                break;
            default:
                return InvalidCmd;
        }
        break;
#endif
    case ' ':
        return StartStop;
    case 'q':
    case 'Q':
    case 0x03:
        return Quit;
    default:
        return InvalidCmd;
    }
}

static void print_target_error(bool nl)
{
    if (nl)
        fprintf(stderr, "\r\n\r\n");

    fprintf(stderr, "Operation failed: ");

    switch (_g_last_error)
    {
    case PGM_ERR_BADACK:
        fprintf(stderr, "A protocol error occurred communicating with the target.");
        break;
    case PGM_ERR_TIMEOUT:
        fprintf(stderr, "Timed out attempting to communicate with the target.");
        break;
    case PGM_ERR_INVALID_COMMAND:
        fprintf(stderr, "The target does not support this command.");
        break;
    case PGM_ERR_NOTSUPPORTED:
        fprintf(stderr, "The target does not support this device.");
        break;
    case PGM_ERR_NO_HARDWARE:
        fprintf(stderr, "No shield is attached to the target.");
        break;
    case PGM_ERR_INCORRECT_HARDWARE:
        fprintf(stderr, "The shield attached to the target does not support this device.");
        break;
    case PGM_ERR_INCORRECT_SWITCH_POSITION:
        fprintf(stderr, "The selection switch on the target is not in the correct position for this device.");
        break;
    case PGM_ERR_MAX_RETRIES_EXCEEDED:
        fprintf(stderr, "The maximum number of attempt to write a byte to the device was exceeded.");
        break;
    default:
        fprintf(stderr, "An error ocurred. Error code not set.");
        break;
    }

    fprintf(stderr, "\r\n\r\n");
}

static void print_progress_outline(void)
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
    fflush(stdout);
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
    fflush(stdout);
}
