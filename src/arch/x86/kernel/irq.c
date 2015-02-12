/*
 *   The interrupts
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rtc.h"
#include <text.h>
#include <stdlib.h>

#include <arch/x86/idt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/irq.h>
#include <arch/x86/pic.h>

#include <interrupts/int.h>

#include <sys/keyboard.h>
#include <sys/dev/ps2.h>
#include <sys/dev/pci.h>

#include <andromeda/system.h>

#if !defined __PIE__ && !defined __PIC__
#error "The x86 kernel module should be compiled position independently!"
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter"

uint64_t pit_timer = 0;
uint64_t sleepTime = 0;
bool isSleeping = FALSE;
struct irq_data irq_data[MAX_IRQ_NUM];
uint32_t irqs[IRQ_BASE];

void cIRQ0(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        if (isSleeping) {
                if (!(sleepTime == 0))
                        sleepTime--;
        }
        pit_timer += 1;
#ifdef TIMER_DBG
        if ((pit_timer & 0xF) == 0)
        printf("[ DEBUG ] timer tick %i\n", pit_timer);
#endif

        pic_8259_eoi(0);

        return;
}

void cIRQ1(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 1, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        uint8_t c = ol_ps2_get_keyboard_scancode();
        kb_handle(c);
        pic_8259_eoi(1);
        return;
}

void cIRQ2(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 2, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(2);
        return;
}

void cIRQ3(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 3, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(3);
        return;
}

void cIRQ4(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 4, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(4);
        return;
}

void cIRQ5(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 5, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(5);
        return;
}

void cIRQ6(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 6, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(6);
        return;
}

void cIRQ7(irq_stack_t* regs)
{
        int spurious = pic_8259_detect_spurious(7);
        do_interrupt(X86_8259_INTERRUPT_BASE + 7, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        if (!spurious) {
                pic_8259_eoi(7);
        }
        return;
}

void cIRQ8(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 8, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        printf("test\n");
        outb(CMOS_SELECT, CMOS_RTC_IRQ);
        inb(CMOS_DATA);
        pic_8259_eoi(8);
        return;
}

void cIRQ9(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 9, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        putc('a');
        pic_8259_eoi(9);
        return;
}

void cIRQ10(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 10, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(10);
        return;
}

void cIRQ11(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 11, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(11);
        return;
}

void cIRQ12(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 12, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(12);
        return;
}

void cIRQ13(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 13, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        pic_8259_eoi(13);
        return;
}

void cIRQ14(irq_stack_t* regs)
{
        do_interrupt(X86_8259_INTERRUPT_BASE + 14, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        putc('a');
        pic_8259_eoi(14);
        return;
}

void cIRQ15(irq_stack_t* regs)
{
        int spurious = pic_8259_detect_spurious(15);
        do_interrupt(X86_8259_INTERRUPT_BASE + 15, (uint64_t) regs->eax,
                        (uint64_t) regs->ebx, (uint64_t) regs->ecx,
                        (uint64_t) regs->edx);
        putc('b');
        if (spurious) {
                pic_8259_eoi(7);
        } else {
                pic_8259_eoi(15);
        }
        return;
}
