/* Consider the following assembly code:

    x @ %ebp + 8, n @ %ebp + 12

    movl 8(%ebp), %esi   ; Get x
    movl 12(%ebp), %ebx  ; Get n
    movl $-1, %edi       ; result = -1
    movl $1, %edx        ; mask =  1
  .L2
    movl %edx, %eax      ; result = edx
    andl %esi, %eax      ; result = result & x
    xorl %eax, %edi      ; result ^ edi
    movl %ebx, %ecx      ; 
    sall %cl, %edx
    testl %edx, %edx
    jne .L2              ; mask != 0 ?
    movl %edi, %eax

 */

int loop(int x, int n) {
    int result = -1;
    int mask;
    for (mask = 1; mask != 0; mask <<= mask)
        result ^= (mask & x);
    return result;
}

/*
 * A. EDX holds mask
 *    EDI holds result
 *    and x is in ESI.
 *
 * B. result is initially -1, and mask initially 1.
 *
 * C. Test condition is "is mask zero?"
 *
 * D. Mask is updated by shifing itself to the left mask number of times (arithmetic)
 *
 * E. Result updated by xoring it with mask AND x.
 *
 */