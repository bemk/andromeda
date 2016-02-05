;   Orion OS, The educational operatingsystem
;   Copyright (C) 2016  Bart Kuivenhoven

;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.

;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.

;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <http://www.gnu.org/licenses/>.


mutex   dd      0 ;The mutex variable

[GLOBAL mutex_lock]
mutex_lock:
        push ebp
        mov ebp, esp

        push eax
        push ebx

        mov eax, 1
        mov ebx, [ebp+8]
.spin:
        mfence
        xchg [ebx], eax
        mfence
        test eax, eax
        jnz .spin

        pop ebx
        pop eax

        mov esp, ebp
        pop ebp
        ret

[GLOBAL mutex_test] ; // Return 0 if mutex was unlocked, 1 if locked
mutex_test:
        push ebp
        mov ebp, esp

        push ebx

        mov eax, 1
        mov ebx, [ebp+8]
        mfence
        xchg [ebx], eax
        mfence

        pop ebx

        mov esp, ebp
        pop ebp
        ret

[GLOBAL mutex_unlock]
mutex_unlock:
        push ebp
        mov ebp, esp

        push eax
        push ebx

        xor eax, eax
        mov ebx, [ebp+8]
        mfence
        mov [ebx], eax

        pop ebx
        pop eax

        mov esp, ebp
        pop ebp
        ret

[GLOBAL cpu_set_pagetable]
cpu_set_pagetable:
        push ebp
        mov ebp, esp

        pusha
        mov eax, mutex
        push eax

        call mutex_lock
        add esp, 4
        mov eax, [ebp+8]
        mov cr3, eax
        mov eax, mutex
        push eax
        call mutex_unlock
        add esp, 4
        popa

        mov esp, ebp
        pop ebp
        ret

[GLOBAL cpu_enable_paging]
cpu_enable_paging:
        push eax
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax
        pop eax
        ret
