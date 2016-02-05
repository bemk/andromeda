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

#include <arch/x86/early_printk.h>
#include <andromeda/cpuapi.h>
#include <andromeda/system.h>
#include <andromeda/log.h>
#include <andromeda/panic.h>
#include <mboot/mboot.h>
#include <andromeda/types.h>
#include <mm/cache.h>

int startup_cleanup = 1;

struct sys_log std_log;
struct sys_log err_log;
struct sys_log warning_log;
struct sys_log debug_log;


void log(struct sys_log* log, char* fmt, ...)
{
        if (log == NULL || fmt == NULL) {
                return;
        }
}

void core()
{
        for (;;) {
                iowait();
                //halt();
        }
}

startup int init(unsigned long magic, multiboot_info_t* mem_map)
{
        setup_early_printk();

        early_printk("Loading Andromeda...");

        if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
                panic("Incorrect loader magic");
        }
        if (mem_map == NULL) {
                panic("Invalid memory map!");
        }

#ifdef SLAB
        slab_alloc_init();
#endif

        core();

        for (;;);

        return 1;
}

