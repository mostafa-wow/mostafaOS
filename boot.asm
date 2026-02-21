[org 0x7c00]
[bits 16]

KERNEL_LOCATION equ 0x1000
DATA_SEGMENT_OFFSET equ 0x10
CODE_SEGMENT_OFFSET equ 0x08


mov [disk_num],dl
mov ax,0
mov es,ax

mov ah,2
mov al,KERNEL_SECTORS
mov ch,0
mov cl,2
mov dh,0
mov dl,[disk_num]
mov bx,KERNEL_LOCATION
int 0x13

mov ah,0x0
mov al,0x3
int 0x10



cli
lgdt [gdt_descriptor]
mov eax,cr0
or eax,1
mov cr0,eax

jmp CODE_SEGMENT_OFFSET:protected_mode_start

[bits 32]
protected_mode_start:
    mov ax,DATA_SEGMENT_OFFSET
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    mov sp,ax
    
    mov ebp,0x9fc00
    mov esp,ebp

    jmp KERNEL_LOCATION


disk_num: db 0

gdt:
    dq 0x0000000000000000

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_descriptor:
    dw gdt_descriptor - gdt - 1
    dd gdt

times 510-($-$$) db 0
db 0x55
db 0xaa
