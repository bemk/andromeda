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
#include <andromeda/cpuapi.h>
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

        /* Calculate the cursor position in the array */
        int cursor = height * vga_data.width + width;

        /* If we're out of bounds, do a panic */
        if (cursor > 80 * 25) {
                vga_data.line_cursor = 0;
                vga_data.character_cursor = 0;
                panic("Cursor out of bounds!");
        }

        /* Put the character in place */
        vga_data.framebuffer[cursor] = character;
}

void vga_set_character(unsigned char height, unsigned char width)
{
        /* Turn the cursor into a single location */
        unsigned short location = height * vga_data.width + width;

        /* Write upper half of the cursor location */
        outb(0x3D4, 14);
        outb(0x3D5, location >> 8);
        /* Write lower half of the cursor location */
        outb(0x3D4, 15);
        outb(0x3D5, location);
}

startup void setup_early_printk()
{
        /* Set up the basic arguments */
        vga_data.framebuffer = (unsigned short*) 0xB8000; /* Colour VGA text */
        vga_data.bg_bytes = 0x0F00; /* Set to white text, black background */
        vga_data.height = 25;
        vga_data.width = 80;

        int i = 0;
        int j = 0;
        /* Set the entire screen to empty */
        for (; i < vga_data.height; i++) {
                for (j = 0; j < vga_data.width; j++) {
                        vga_putchar(' ', i, j);
                }
        }

        /* Set the cursor to the top left of the screen */
        vga_data.line_cursor = 0;
        vga_data.character_cursor = 0;

        vga_set_character(0, 0);
}

startup void early_printk(char* str)
{
        /* Make sure we're not referencing null here*/
        if (str == NULL || str[0] == '\0') {
                return;
        }

        /* Loop 'till the end of the string */
        while (*str != '\0') {

                /* If we overflow the line, do carriage return and line feed */
                if (vga_data.character_cursor >= vga_data.width) {
                        vga_data.character_cursor = 0;
                        vga_data.line_cursor++;
                }
                /* If overflow on the screen, scroll up */
                if (vga_data.line_cursor >= vga_data.height) {
                        scroll();
                }

                /* Filter out relevant special characters */
                switch (*str) {
                case '\n':
                        /* Newline also does line feed */
                        vga_data.character_cursor = 0;
                        vga_data.line_cursor++;
                        break;
                case '\r':
                        /* Line feed doesn't also do carriage return */
                        vga_data.character_cursor = 0;
                        break;
                case '\t':
                        /* Go to the next multiple of 8 */
                        vga_data.character_cursor += (8
                                        - (vga_data.character_cursor & 0x7));
                        vga_data.character_cursor --;
                        break;

                default:
                        /*
                         * It seems like this character isn't a special one
                         * Print it
                         */
                        vga_putchar(*str, vga_data.line_cursor,
                                        vga_data.character_cursor);
                        vga_data.character_cursor++;
                }

                str++;
        }
        vga_set_character(vga_data.line_cursor, vga_data.character_cursor);

        return;
}
