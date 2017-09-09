#!/usr/bin/python
from pwn import *
PHASE_ONE = "Border relations with Canada have never been better."
PHASE_TWO = "1 2 4 8 16 32"
PHASE_THREE = "1 311"
PHASE_FOUR = "7 0"
PHASE_FIVE = "ionuvw"
PHASE_SIX = "4 3 2 1 6 5"
if __name__ == "__main__":
    io = process("./cmubomb")

    io.sendline(PHASE_ONE)
    print io.recv()
    io.sendline(PHASE_TWO)
    print io.recv()
    io.sendline(PHASE_THREE)
    print io.recv()
    io.sendline(PHASE_FOUR)
    print io.recv()
    io.sendline(PHASE_FIVE)
    print io.recv()
    io.sendline(PHASE_SIX)
    print io.recv()
