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

#include <andromeda/cpuapi.h>

void halt()
{
        asm (
                        "hlt\n\t"
        );

}

void shutdown()
{
        for (;;) {
                asm (
                                "cli\n\t"
                                "hlt\n\t"
                );
        }
}

void outb(unsigned short port, unsigned char data)
{
        __asm__ __volatile__("outb %%al, %%dx"
                        : /* no output */
                        : "a" (data),
                        "d" (port)
        );
}

void outw(unsigned short port, unsigned short data)
{
        __asm__ __volatile__("outw %%ax, %%dx"
                        : /* no output */
                        : "a" (data),
                        "d" (port)
        );
}

void outl(unsigned short port, unsigned int data)
{
        __asm__ __volatile__("outl %%eax, %%dx"
                        : /* no output */
                        : "a" (data),
                        "d" (port)
        );
}

unsigned char inb(unsigned short port)
{
        register unsigned char ret;
        __asm__ __volatile__("inb %%dx, %%al"
                        : "=a" (ret)
                        : "d" (port)
        );
        return ret;
}

unsigned short inw(unsigned short port)
{
        register unsigned short ret;
        __asm__ __volatile__("inw %%dx, %%ax"
                        : "=a" (ret)
                        : "d" (port)
        );
        return ret;
}

unsigned int inl(unsigned short port)
{
        register unsigned int ret;
        __asm__ __volatile__("inl %%dx, %%eax"
                        : "=a" (ret)
                        : "d" (port)
        );
        return ret;
}

void iowait(void)
{
        __asm__ __volatile__("xorl %%eax, %%eax\n\t"
                        "outb %%al, $0x80"
                        : /* no output */
                        : /* no input */
                        : "%eax");
}

