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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <andromeda/atomic.h>

#ifdef X86
#include <arch/x86/x86.h>
#endif

typedef volatile unsigned int spinlock_t;

void halt();
void shutdown();
void out_byte(unsigned short port, unsigned char data);
void out_doublebyte(unsigned short port, unsigned short data);
void out_quadbyte(unsigned short port, unsigned int data);
unsigned char in_byte(unsigned short port);
unsigned short in_doublebyte(unsigned short port);
unsigned int in_quadbyte(unsigned short port);
void iowait(void);

void cpu_enable_interrupts();
int cpu_disable_interrupts();

extern int mutex_lock(spinlock_t* );
extern int mutex_unlock(spinlock_t* );
extern int mutex_test(spinlock_t* );

#ifdef __cplusplus
}
#endif
