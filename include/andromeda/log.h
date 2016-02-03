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

#ifndef __ANDROMEDA_LOG_H
#define __ANDROMEDA_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <andromeda/types.h>

struct sys_log_message {
        time_t timestamp;
        struct sys_log_message* continued;
        struct sys_log_message* next;
        size_t message_length;
        char message[228];
};

struct sys_log {
// To be implemented!
};
void log(struct sys_log* log, char* fmt, ...);

extern struct sys_log std_log;
extern struct sys_log err_log;

#ifdef __cplusplus
}
#endif

#endif
