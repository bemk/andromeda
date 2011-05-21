;
;    The masterboot record. Loads the first sector of the active partition.
;    Copyright (C) 2011 Michel Megens
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

%include "masterboot.h"

[BITS 16]
[ORG 0x7c00]

; This code is located at the first sector of the harddisk. It is loaded by the bios to address 0x0:0x7c00.
; The code will migrate itself to address 0x50:0x0. From there it will load the first sector of the active partition
; to address 0x0:0x7c00.

_start:
jmp short start
nop
; The bios parameter block, in the case it is a floppy
%ifdef __FLOPPY
	times 8 db 0			; fill up 
	dw 512				; bytes per sector 
	db 1 				; sectors per cluster 
	dw 2 				; reserved sector count 
	db 2 				;number of FATs 
	dw 16*14 			; root directory entries 
	dw 18*2*80 			; 18 sector/track * 2 heads * 80 (density) 
	db 0F0h 			; media byte 
	dw 9 				; sectors per fat 
	dw 18 				; sectors per track 
	dw 2 				; number of heads 
	dd 0 				; hidden sector count 
	dd 0 				; number of sectors huge 
	bootdisk db 0 			; drive number 
	db 0 				; reserved 
	db 29h 				; signature 
	dd 0 				; volume ID 
	times 11 db 0	 		; volume label 
	db 'FAT12   '                 ; file system type
%else
	bootdisk db 0

dap:
	db 0x10      	; register size
	db 0      	; reserved, must be 0
	dw 0x1      	; sectors to read
	dw 0x7c00   	; memory offset
	dw 0x0   	; memory segment
	dq 0x1		; starting sector (sector to read, s1 = 0)

%endif

start:
	cli
; 	we are not safe here
	jmp 0x0:.flush
; 	there are buggy bioses which load you in at 0x7c0:0x0
.flush:
	xor ax, ax
	mov ds, ax 	; tiny memory model

	mov es, ax	; general and extra segmets
	mov gs, ax
	mov fs, ax

	mov ss, ax	; stack segment
	mov sp, GEBL_LOADOFF
	sti
main:
; 	es is already set to 0
	mov byte [bootdisk], dl
	mov di, GEBL_BUFOFF
	mov si, _start ; beginning of the source
	mov cx, 512/2
	; we will move 2 bytes (words) at ones
	cld
	rep movsw
	
	jmp GEBL_BUFSEG:GEBL_JUMPOFF
;
; GEBL_JUMPOFF gets us to the offset of migrate in the new seg:offset address. It is defined as: 
; (0x7c00 - addressof(migrate)) + 0x500 (0x500 = buffer offeset)
;

migrate:
; here we should use the partition table to indicate what offset we should use to load the first sector
; of the active (bootable) partition. Keep in mind that we moved our ass to here in a wicked way.

	xor ax, ax
	mov dl, byte [bootdisk]
	int 0x13
%ifdef __FLOPPY
	; read 1 sector
	; int 13h function 0x2
	mov ax, 0x201
	
	; track 0 and read at sector 2
	xor ch, ch
	mov cl, 0x2

	; head 0 and the drive number
	xor dh, dh
	mov dl, byte [bootdisk]
%elifdef __HDD
	mov ax, 0x41
	mov dl, byte [bootdisk]
	mov bx, 0x55aa
	int 0x13

%else
; nothing is defined about FDD's and HDD's, could be usb. Use CHS to be sure.
%endif
; 	issue an bios interrupt to read the sectors from the disk as defined above depending 
; 	on how it is compiled
	int 0x13
	jc .error

	mov si, GEBL_BUFOFF+GEBL_PART_TABLE
	test byte [si], 0x80
	jz .error

	mov al, 0x41 	; new line
	call print
	jmp end

.error:
	mov al, 0x42
	call print

end:
	cli
	hlt
	jmp end

; -- print routine
;   character is expected in al

print:
	mov ah, 0x0E
	xor bh, bh
	int 0x10
	ret

times 446 - ($-$$) db 0
db 0x80

times 510 - ($-$$) db 0
dw 0xaa55