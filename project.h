/*
 *   File:   project.h
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

#ifndef __PROJECT_H__
#define __PROJECT_H__

extern int _g_last_error;

#ifndef _WIN32

bool posix_kbhit();

#define _stricmp strcasecmp
#define _strdup strdup
#define strcpy_s(dst, sz, src) strcpy(dst, src)
#define Sleep sleep
#define getch() getchar()
#define _kbhit posix_kbhit

#endif

#endif /* __PROJECT_H__ */