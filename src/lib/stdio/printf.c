/*
 * Andromeda
 * Copyright (C) 2011  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \AddToGroup stdio
 * @{
 */
#include <stdio.h>
#include <stdlib.h>

extern char HEX[];
extern char hex[];

#define PRINTNUM_PADDING '0'
/**
 * \fn sprintnum
 * \brief Write integer numbers according to a format and base number.
 * \param str
 * \brief The string to write to
 * \param min_size
 * \brief How many chars must this fill?
 * \param num
 * \brief What number do we print?
 * \param base
 * \brief The base number for the printed number
 * \param capital
 * \brief Can we use capitals in the text?
 * \param sign
 * \brief Is the integer signed or not?
 * \return The number of chars printed.
 */
int
sprintnum(char* str, size_t min_size, int num, int base, bool capital, bool sign)
{
        if (base > 36 || base < 2)
                return -E_INVALID_ARG;
        /* If num == 0, the result is always the same, so optimize that out */
        if (num == 0)
        {
                int i = 0;
                for (; i < min_size-1; i++)
                {
                        *(str++) = PRINTNUM_PADDING;
                }
                *(str++) = '0';
                return min_size;
        }
        int32_t idx = 0;
        uint32_t unum = (uint32_t)num;
        /* If signedness is allowed, check for signedness */
        if (num < 0 && sign)
                unum = -num;

        char tmp_str[32];
        memset(tmp_str, 0, sizeof(tmp_str));

        /*
         * Convert the integer into an ascii representation
         * This unfortunately reverses the string order
         */
        for (;unum != 0; idx++)
        {
                tmp_str[sizeof(tmp_str) - idx] = (capital) ? HEX[unum % base] :
                hex[unum % base];
                unum /= base;
        }
        /* If signed and negative, append the - sign */
        if (num < 0 && sign)
        {
                tmp_str[sizeof(tmp_str) - idx] = '-';
                idx++;
        }
        int ret = idx;
        /*
         * If the string doesn't cut the minimal length requirements, make it a
         * little longer by appending a couple of characters
         */
        if (idx < min_size)
        {
                int i = 0;
                for (; i < min_size - idx; i++)
                        *(str++) = PRINTNUM_PADDING;
                ret = min_size;
        }
        idx --;
        /*
         * Now take the temp string, reverse it and put it in the output string
         * The reversal to get the correct order again.
         */
        for (; idx >= 0; idx--)
                *(str++) = tmp_str[sizeof(tmp_str) - idx];
        return ret;
}

/**
 * \fn sprintf
 * \brief Print a format to a string
 * \param str
 * \brief The string to print to
 * \param fmt
 * \brief The format
 * \param ...
 * \brief Random arguments which have to match the format
 * \return The number of characters succesfully printed
 */
int sprintf(char* str, char* fmt, ...)
{
        /* Check the preconditions first. */
        if (str == NULL || fmt == NULL)
                return 0;
        int num = 0;
        va_list list;
        va_start(list, fmt);

        /* Itterate through the string to put every character in place. */
        for (; *fmt != '\0'; fmt++, str++, num++)
        {
                /* If formatted? */
                if (*fmt == '%')
                {
                        /* Interpret the format numbering. */
                        int pre  = 0;
                        int post = 0;
                        bool dotted = false;
                        for (; *(fmt + 1) >= '0' && *(fmt + 1) <= '9' ||
                                *(fmt + 1) == '.'; fmt++)
                        {
                                if (*(fmt + 1) == '.')
                                {
                                        dotted = true;
                                        continue;
                                }
                                if (dotted)
                                {
                                        post *= 10;
                                        post += (*(fmt+1) - '0');
                                }
                                else
                                {
                                        pre *= 10;
                                        pre += (*(fmt+1) - '0');
                                }
                        }
                        if (pre == 0)
                                pre = 1;
                        if (post == 0)
                                post = 1;
                        int inc = 0;
                        /* Now finally choose the type of format. */
                        switch(*(++fmt))
                        {
                                case 'x':
                                        inc = sprintnum(str, pre, (int)va_arg(list, int), 16, false, false);
                                        break;
                                case 'X':
                                        inc = sprintnum(str, pre, (int)va_arg(list, int), 16, true, false);
                                        break;
                                case 'f':
                                        break;
                                case 'i':
                                        inc = sprintnum(str, pre, (int)va_arg(list, int), 10, false, false);
                                        break;
                                case 'd':
                                        inc = sprintnum(str, pre, (int)va_arg(list, int), 10, false, true);
                                        break;
                                case 'c':
                                        inc = 1;
                                        *str = (char)va_arg(list, int);
                                        break;
                                case 's':
                                        inc = sprintf(str, va_arg(list, char*));
                                        break;
                                default:
                                        *str = *fmt;
                                        continue;
                        }
                        /*
                         * Update the looping info.
                         * The -1 below compensates for the increment by the for
                         * loop.
                         */
                        num += inc - 1;
                        str += inc - 1;
                }
                /* Else just copy the character over. */
                else
                        *str = *fmt;
        }

        va_end(list);
        return num;
}

/** @} \file */
