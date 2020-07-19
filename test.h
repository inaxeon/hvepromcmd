/*
 *   File:   test.h
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


#ifndef __TEST_H__
#define __TEST_H__

typedef struct
{
	uint8_t cmd;
	bool is_read;
	const char *desc;
} test_t;

extern const test_t _g_1702a_tests[];
extern const test_t _g_270x_tests[];
extern const test_t _g_mcm6876x_tests[];

#endif /* __TEST_H__ */
