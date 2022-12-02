/*
 *   File:   serial_win32.c
 *   Author: Matthew Millman (inaxeon@hotmail.com)
 *
 *   1702A/270x/TMS2716/MCM6876x/MCS48 Programmer
 *
 *   Serial routines for Windows
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

#ifdef UNICODE
#error Can't build this with UNICODE defined
#endif

bool serial_open(const char *port_name, int baud, port_handle_t *port_handle)
{
    DCB serial_params;
    COMMTIMEOUTS timeouts;
    HANDLE port;
    char port_buffer[32];

    _snprintf_s(port_buffer, _countof(port_buffer), _TRUNCATE, "\\\\.\\%s", port_name);

    port = CreateFile(port_buffer, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    *port_handle = (port_handle_t)port;

    if (port == INVALID_HANDLE_VALUE)
        return false;

    memset(&serial_params, 0, sizeof(DCB));
    serial_params.DCBlength = sizeof(serial_params);

    if (!GetCommState(port, &serial_params))
    {
        CloseHandle(port);
        return false;
    }

    switch (baud)
    {
    case 9600:
        serial_params.BaudRate = CBR_9600;
        break;
    case 38400:
        serial_params.BaudRate = CBR_38400;
        break;
    case 115200:
        serial_params.BaudRate = CBR_115200;
        break;
    default:
        CloseHandle(port);
        return false;
    }

    serial_params.ByteSize = 8;
    serial_params.StopBits = ONESTOPBIT;
    serial_params.Parity = NOPARITY;

    if (!SetCommState(port, &serial_params))
    {
        CloseHandle(port);
        return false;
    }

    memset(&timeouts, 0, sizeof(COMMTIMEOUTS));

    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutConstant = 3000;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 3000;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(port, &timeouts))
    {
        CloseHandle(port);
        return false;
    }

    PurgeComm(port, PURGE_RXCLEAR | PURGE_TXCLEAR);

    return true;
}

void serial_close(port_handle_t port)
{
    if (port != INVALID_HANDLE_VALUE)
        CloseHandle((HANDLE)port);
}

bool serial_write(port_handle_t port, uint8_t *buffer, int count)
{
    DWORD bytes_written = 0;
    
    if (WriteFile((HANDLE)port, buffer, count, &bytes_written, NULL))
        return bytes_written == count;

    return false;
}

bool serial_read(port_handle_t port, uint8_t *buffer, int count)
{
    DWORD num_bytes_read;

    ReadFile((HANDLE)port, buffer, count, &num_bytes_read, NULL);

    if (num_bytes_read < (DWORD)count)
    {
        _g_last_error = PGM_ERR_TIMEOUT;
        return false;
    }

    return true;
}