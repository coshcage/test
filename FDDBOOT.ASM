
; FDDBOOT.IMG
; 05142017
DB 0xEB, 0xFE, 0x90 ; Disassemble code. As an infinite loop.
DB "ABCDEFGH"       ; OEM Identifier. For 8 Bytes.
DW 512              ; Number of each sector. (Must be 512 bytes for FAT12.)
DB 1                ; Number of each cluster. (Which must equal to one sector.)
DW 1                ; Boot record sector offset. (Usually start with the 1st sector.)
DB 2                ; Number of File Allocation tables. (Must be 2 for FAT12.)
DW 224              ; Number of root directories.
DW 2880             ; Size of the disk. (Must be 2880 sectors for FAT12.)
DB 0xF0             ; Media type. (Must be 0xF0 for FAT12.)
DW 9                ; Length of FAT. (Must be 9 sectors for FAT12.)
DW 18               ; How many sectors are there in a track. (Must be 18 for FAT12.)
DW 2                ; Number of disk heads. (Wich must be 2.)
DD 0                ; No hidden sectors. No extra partitions.
DD 2880             ; Disk size again.
DB 0x00             ; 0x00 for a floppy disk. 0x80 for a hard disk.
DB 0x00             ; Reserved. For WinNT use only.
DB 0x29             ; Signature (Must be 0x28 or 0x29.)
DD 0xFFFFFFFF       ; Volume ID.
DB "ABCDEFGHIJ "    ; Disk name. (11 bytes padding with spaces.)
DB "FAT12   "       ; Disk format type. (8 bytes. Padding with spaces.)
RESB 448            ; Boot code.
DB 0x55, 0xAA       ; 0xAA55 signature.
RESB 1474048        ; Reserve byte with 0x00.
; End. 26 lines totally. Please compile with NASM assembler. nasm -o fddboot.img FDDBOOT.ASM
