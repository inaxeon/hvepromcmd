/*
 *   File:   serial_posix.c
 *   Author: Matthew Millman (inaxeon@hotmail.com)
 *
 *   1702A/270x/MCM6876x/MCS48 Programmer
 *
 *   Serial routines for Linux
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

bool serial_open(const char *port_name, int baud, port_handle_t *port_handle)
{
    int fd;
    int rc;
    speed_t speed;
    struct termios termios;
    fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);

    *port_handle = (port_handle_t)fd;

    if (fd < 0)
        return false;

    switch (baud)
    {
    case 9600:
        speed = B9600;
        break;
    case 38400:
        speed = B38400;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        close(fd);
        return false;
    }

    rc = tcgetattr(fd, &termios);
    if (rc < 0) {
        close(fd);
        return false;
    }

    termios.c_iflag = IGNBRK;
    termios.c_oflag = 0;
    termios.c_lflag = 0;
    termios.c_cflag = (CS8 | CREAD | CLOCAL);
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;

    cfsetospeed(&termios, speed);
    cfsetispeed(&termios, speed);

    rc = tcsetattr(fd, TCSANOW, &termios);
    if (rc < 0) {
        close(fd);
        return false;
    }

    rc = fcntl(fd, F_GETFL, 0);
    if (rc != -1)
        fcntl(fd, F_SETFL, rc & ~O_NONBLOCK);

    return true;
}

void serial_close(port_handle_t port)
{
    int fd = (int)port;

    if (fd < 0)
        return;

    close(fd);
}

bool serial_write(port_handle_t port, uint8_t *buffer, int count)
{
    int fd = (int)port;
    int num_bytes_written = 0;

    while (num_bytes_written < count)
    {
        int rc = write(fd, buffer, count);
        if (rc < 0)
            break;

        buffer += rc;
        num_bytes_written += rc;
    }

    if (num_bytes_written < count)
        return false;

    return true;
}

bool serial_read(port_handle_t port, uint8_t *buffer, int count)
{
    struct timeval timeout;
    fd_set rfds;
    int nfds;
    int fd = (int)port;
    int rc;
    size_t num_bytes_read = 0;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (num_bytes_read < count)
    {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        nfds = select(fd + 1, &rfds, NULL, NULL, &timeout);
        if (nfds == 0) {
            break;
        }
        else if (nfds == -1) {
            break;
        }

        rc = read(fd, buffer, count - num_bytes_read);
        if (rc < 0) {
            break;
        }
        buffer += rc;
        num_bytes_read += rc;
    }

    if (num_bytes_read < count)
    {
        _g_last_error = PGM_ERR_TIMEOUT;
        return false;
    }

    return true;
}
