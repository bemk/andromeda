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
#include <andromeda/atomic.h>
#include <andromeda/types.h>

void halt()
{
        asm (
                        "pushf\n\t"
                        "sti\n\t"
                        "hlt\n\t"
                        "popf\n\t"
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

void out_byte(uint16_t port, uint8_t data)
{
        __asm__ __volatile__("outb %%al, %%dx"
                        : /* no output */
                        : "a" (data),
                        "d" (port)
        );
}

void out_doublebyte(uint16_t port, uint16_t data)
{
        __asm__ __volatile__("outw %%ax, %%dx"
                        : /* no output */
                        : "a" (data),
                        "d" (port)
        );
}

void out_quadbyte(uint16_t port, uint32_t data)
{
        __asm__ __volatile__("outl %%eax, %%dx"
                        : /* no output */
                        : "a" (data),
                        "d" (port)
        );
}

uint8_t in_byte(uint16_t port)
{
        register uint8_t ret;
        __asm__ __volatile__("inb %%dx, %%al"
                        : "=a" (ret)
                        : "d" (port)
        );
        return ret;
}

uint16_t in_doublebyte(uint16_t port)
{
        register uint16_t ret;
        __asm__ __volatile__("inw %%dx, %%ax"
                        : "=a" (ret)
                        : "d" (port)
        );
        return ret;
}

uint32_t in_quadbyte(uint16_t port)
{
        register uint32_t ret;
        __asm__ __volatile__("inl %%dx, %%eax"
                        : "=a" (ret)
                        : "d" (port)
        );
        return ret;
}

void iowait(void)
{
        __asm__ __volatile__("xorl %%eax, %%eax\n\t"
                        "outb %%al, $0x80\n\t"
                        : /* no output */
                        : /* no input */
                        : "%eax");
}

void cpu_enable_interrupts()
{
        __asm__ __volatile__("sti\n\t");
}

#define X86_INTERRUPT_BIT (1 << 9)
int cpu_disable_interrupts()
{
        unsigned int flags = 0;
        __asm__ __volatile__(
                        "pushf\n\t"
                        "sti\n\t"
                        "popl %[flags]"
                        : [flags] "=r" (flags)
                        :
                        :
        );
        if (flags & X86_INTERRUPT_BIT) {
                return 1;
        }
        return 0;
}
