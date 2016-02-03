/*
 Andromeda
 (C) Bart Kuivenhoven - 2016

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESbuffer_initS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ARCH_X86_EARLY_PRINTK_H
#define __ARCH_X86_EARLY_PRINTK_H

#ifdef __cplusplus
extern "C" {
#endif

void setup_early_printk();
void early_printk(char* str);

#ifdef __cplusplus
}
#endif

#endif
