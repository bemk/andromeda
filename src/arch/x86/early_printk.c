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

#include <andromeda/types.h>
#include <andromeda/system.h>
#include <arch/x86/early_printk.h>

/* The config for the early printk */
struct early_printk_config {
        unsigned short* framebuffer;
        unsigned short line_cursor;
        unsigned short character_cursor;
        unsigned short bg_bytes;
        unsigned char width;
        unsigned char height;
};
startup_data struct early_printk_config early_printk_data;

startup static void scroll()
{
        unsigned char width = early_printk_data.width;
        unsigned short limit = width * (early_printk_data.height - 1);

        unsigned short idx = 0;

        for (; idx < limit; idx++) {
                unsigned short fetch =
                                early_printk_data.framebuffer[idx + width];
                early_printk_data.framebuffer[idx] = fetch;
        }

        limit = idx + width;
        for (; idx < width; idx++) {
                early_printk_data.framebuffer[idx] = ' '
                                | early_printk_data.bg_bytes;
        }
}

/* Put the actual character on screen */
startup static void early_putchar(unsigned char c)
{
        unsigned short character = c | early_printk_data.bg_bytes;

        unsigned short cursor = early_printk_data.line_cursor;
        cursor *= early_printk_data.width;
        cursor += early_printk_data.character_cursor;

        early_printk_data.character_cursor++;
        if (early_printk_data.character_cursor >= early_printk_data.width) {
                early_printk_data.line_cursor++;
                early_printk_data.character_cursor = 0;
                if (early_printk_data.line_cursor > early_printk_data.height) {
                        scroll();
                }
        }

        early_printk_data.framebuffer[cursor] = character;
}

startup void setup_early_printk()
{
        early_printk_data.framebuffer = (unsigned short*) 0xB8000;
        early_printk_data.bg_bytes = 0x0F00;
        early_printk_data.height = 25;
        early_printk_data.width = 80;

        int lim = early_printk_data.height * early_printk_data.width;
        int i = 0;
        for (; i < lim; i++) {
                early_putchar(' ');
        }

        early_printk_data.line_cursor = 0;
        early_printk_data.character_cursor = 0;
}

startup void early_printk(char* str)
{
        if (str == NULL) {
                return;
        }

        while (*str != '\0') {
                switch (*str) {
                case '\n':
                        early_printk_data.line_cursor++;
                        break;
                case '\r':
                        early_printk_data.character_cursor = 0;
                        break;
                case '\t':
                        early_printk_data.character_cursor += 8;
                        break;
                default:
                        early_putchar(*str);
                }

                str++;
        }

        return;
}
