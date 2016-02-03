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

#include <andromeda/panic.h>
#include <andromeda/system.h>
#include <arch/x86/early_printk.h>
#include <andromeda/log.h>

/* This function will never return */
__attribute__((noreturn))
void panic_func(char* msg, char* file, int line)
{
        /* If we haven't set up the logger system, use early printk */
        if (startup_cleanup) {
#ifdef CAS
                early_printk("Shit's fucked up! Try again!\nfile: ");
#else
                early_printk("Andromeda panic!\nfile: ");
#endif
                early_printk(file);
                early_printk("\n");
                early_printk(msg);
        } else {
#ifdef CAS
                // Little easter egg, a request from Cas van Raan
                log(&err_log, "Shit's fucked up at line %i in file %s\n%s\n"
                                "Try again!", line, file, msg);
#else
                log(&err_log, "Andromeda panic in %s at line %i\n%s\n", file,
                                line, msg);
#endif
        }

        for (;;) {

        }
}
