    .section .text
    .global _start

_start:
    la    a0, my_string_1
    call  put_string

    la    a0, my_string_2
    call  put_string

    ebreak

put_string:
    li   a2, 0x10000000

.loop_start:
    lbu  a1, 0(a0)
    beqz a1, .put_string_end
    sb   a1, 0(a2)
    addi a0, a0, 1
    j    .loop_start

.put_string_end:
    ret

    .section .rodata
my_string_1:
    .string "Hello, World!\n"
my_string_2:
    .string "What's up?\n"
