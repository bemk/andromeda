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
#include <andromeda/panic.h>

/* The config for the early printk */
struct vga_config {
        unsigned short* framebuffer;
        unsigned short line_cursor;
        unsigned short character_cursor;
        unsigned short bg_bytes;
        unsigned char width;
        unsigned char height;
};
struct vga_config vga_data;

startup static void scroll()
{
        /* Cache the screen width */
        unsigned short width = vga_data.width;
        /* Set the limit to 24 lines */
        unsigned short limit = width * (vga_data.height - 1);

        unsigned short idx = 0;

        /* Cycle though all the characters in the 24 top most lines
         */
        for (; idx < limit; idx++) {
                unsigned short cache;
                /* Cache the character 80 characters (or 1 line) ahead */
                cache = vga_data.framebuffer[idx + width];
                /* put the character in place */
                vga_data.framebuffer[idx] = cache;
        }

        /* Move the limit one line over */
        limit += width;
        for (; idx < width; idx++) {
                vga_data.framebuffer[idx] = ' ' | vga_data.bg_bytes;
        }
        /* Make sure nobody writes out of the buffer */
        vga_data.line_cursor--;
}

/* Put the actual character on screen */
static void vga_putchar(unsigned char c, char height, char width)
{
        /* Format the character to be written */
        unsigned short character = c | vga_data.bg_bytes;

        int cursor = height * vga_data.width + width;

        if (cursor > 80 * 25) {
                vga_data.line_cursor = 0;
                vga_data.character_cursor = 0;
                panic("Cursor out of bounds!");
        }

        vga_data.framebuffer[cursor] = character;
}

startup void setup_early_printk()
{
        vga_data.framebuffer = (unsigned short*) 0xB8000;
        vga_data.bg_bytes = 0x0F00;
        vga_data.height = 25;
        vga_data.width = 80;

        int i = 0;
        int j = 0;
        for (; i < vga_data.height; i++) {
                for (j = 0; j < vga_data.width; j++) {
                        vga_putchar(' ', i, j);
                }
        }

        vga_data.line_cursor = 0;
        vga_data.character_cursor = 0;
}

startup void early_printk(char* str)
{
        if (str == NULL) {
                return;
        }

        while (*str != '\0') {

                if (vga_data.character_cursor >=vga_data.width) {
                        vga_data.character_cursor = 0;
                        vga_data.line_cursor++;
                }
                if (vga_data.line_cursor >= vga_data.height) {
                        scroll();
                }

                switch (*str) {
                case '\n':
                        vga_data.character_cursor = 0;
                        vga_data.line_cursor++;
                        break;
                case '\r':
                        vga_data.character_cursor = 0;
                        break;
                case '\t':
                        vga_data.character_cursor += 8;
                        break;
                default:
                        vga_putchar(*str, vga_data.line_cursor, vga_data.character_cursor);
                        vga_data.character_cursor++;
                }

                str++;
        }

        return;
}
