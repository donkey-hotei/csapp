/*
 * Write C function called cread_alt as the code below:
 *
 * int cread(int * xp) {
 *     return (xp ? *xp : 0);
 * }
 *
 * except that, when compiles, it uses an instruction from the cmov family.
 * cread itself could fail if *xp dereferences a null pointer, so branching
 * is still used by the compiler.
 */

int cread_alt(int * xp) {
    // TODO: Compiling with -m32 or -m64 does not give conditional move isntruction
    if (*xp == 0)
        return 0;
    return (xp ? *xp : 0);
}

int main(int argc, char ** argv) {
    int x = 42;

    cread_alt(&x);
}