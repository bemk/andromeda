/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAP_H
#define MAP_H

#define FREE	   0x0000
#define MODULE	   0x0001
#define COMPRESSED 0x0002
#define NOTUSABLE  0xFFFF

#ifdef X86
#define PAGES      0x100000
#define PAGESIZE   0x1000
#endif

extern unsigned short bitmap[];

#ifdef __COMPRESSED
#include <boot/mboot.h>

void buildMap(multiboot_memory_map_t*, int);

#endif
#endif