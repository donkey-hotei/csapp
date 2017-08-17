#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

char * int2bin(int a, char * buffer, int buf_size) {
    buffer += (buf_size -1);
    for (int i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';
        a >>= 1;
    }
    return buffer;
}

int main(int argc, char ** argv) {
    char buffer[65];
    buffer[64] = "\0";
    int2bin(0xff000000, buffer, 64);
    printf("a = %s", buffer);
}