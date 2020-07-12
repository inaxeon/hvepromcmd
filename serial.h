/*
 *   File:   serial.h
 *   Author: Matthew Millman (inaxeon@hotmail.com)
 *
 *   1702A/270x/MCM6876x/MCS48 Programmer
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


#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef _WIN32
typedef FILE * port_handle_t;
#define DEFAULT_PORT_HANDLE NULL
#else
typedef int port_handle_t;
#define DEFAULT_PORT_HANDLE -1
#endif

bool serial_open(const char *port_name, int baud, port_handle_t *port_handle);
void serial_close(port_handle_t port);
bool serial_write(port_handle_t port, uint8_t *buffer, int count);
bool serial_read(port_handle_t port, uint8_t *buffer, int count);

#endif /* __SERIAL_H__ */
