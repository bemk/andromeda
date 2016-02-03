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

#include <mboot/mboot.h>
#include <andromeda/cpuapi.h>
#include <andromeda/system.h>
#include <types.h>
#include <arch/x86/early_printk.h>

int startup_cleanup = 0;

struct sys_log {
// To be implemented!
};

struct sys_log std_log;
struct sys_log err_log;

void log(struct sys_log* log, char* fmt, ...)
{
        if (log == NULL || fmt == NULL) {
                return;
        }
}

#define panic(msg) panic_func(msg, __FILE__, __LINE__)

__attribute__((noreturn))
void panic_func(char* msg, char* file, int line)
{
        if (startup_cleanup) {
                early_printk("Shit's fucked up way too quickly!\n");
                early_printk(msg);
        } else {
#ifdef CAS
                // Little easter egg, a request from Cas van Raan
                log("Shit's fucked up at line %i in file %s\n%s\nTry again!", line, file, msg);
#else
                log(&err_log, "Andromeda panic in %s at line %i\n%s\n", file,
                                line, msg);
#endif
                log(&err_log, "Shits fucked up!\n");
        }

        for (;;) {

        }
}

void core()
{
        for (;;) {
                halt();
        }
}

startup int init(unsigned long magic)
{
        setup_early_printk();

        char* str = "a";

        int i = 0;
        int j = 0;

        for (i = 0; i < 20; i++) {
                for (j = 0; j < 70; j++) {
                        early_printk(str);
                }
                (*str)++;
                early_printk("blaat");
                early_printk("\r\n");
        }

        if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
                panic("Incorrect loader magic");
        }

        core();

        return 1;
}

