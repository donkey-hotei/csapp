/* Write C code that has similar functionality to assembly code below:
 * x @ %ebp + 8, y @ %ebp + 12, z @ ebp + 16

 * mov1 12(%ebp), %edx  ; Get y
 * subl 16(%ebp), %edx
 * movl %edx, %eax      ; Get y - z
 * sall $31, %eax
 * sarl $31, %eax
 * imull 8($ebp), %edx  ; x * ((y - z) << 31) >> 31
 * xorl %edx, %eax
 */

int decode2(int x, int y, int z) {
    int a = y - z;
    // this could be some compiler trick?
    int b = (a << 31) >> 31;
    return (x * a) ^ b;
}

int main(int argc, char ** argv) {
    decode2(1, 2, 3);
}

/* Compiles to:
 *
;-- decode2:
0x080483cb      55             push ebp
0x080483cc      89e5           mov ebp, esp
0x080483ce      83ec10         sub esp, 0x10
0x080483d1      8b450c         mov eax, dword [ebp + 0xc]  ; [0xc:4]=-1 ; 12
0x080483d4      2b4510         sub eax, dword [ebp + 0x10]
0x080483d7      8945fc         mov dword [ebp - 4], eax
0x080483da      8b45fc         mov eax, dword [ebp - 4]
0x080483dd      c1e01f         shl eax, 0x1f
0x080483e0      c1f81f         sar eax, 0x1f
0x080483e3      8945f8         mov dword [ebp - 8], eax
0x080483e6      8b4508         mov eax, dword [ebp + 8]    ; [0x8:4]=-1 ; 8
0x080483e9      0faf45fc       imul eax, dword [ebp - 4]
0x080483ed      3345f8         xor eax, dword [ebp - 8]
0x080483f0      c9             leave
0x080483f1      c3             ret
 */
