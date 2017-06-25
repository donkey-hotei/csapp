#include <stdio.h>
#include <stlib.h>

void phase_1(char ** input){
    if (strings_not_equal(input, "Public speaking is very easy."))
        explode_bomb();
}

void phase_2(char ** input){
    int ar[6];
    ar = read_six_numbers(input);
    for (int i = 0; i <= 5; i++)
        if (ar[i] != ar[i-1] * (i+1))
            explode_bomb();
}

void phase_3(char ** input){
    char a, b, c; int d;
    if (sscanf(input, "%d %c %d", a, d, b) <  2)
        explode_bomb();
    switch (a) {
	    case 1 : c = "b";
		      
    }
}

int func4(int n){
    if (n <= 1)
        return n;
    return func4(n-1) + func4(n-2);
}

void phase_4(char ** input){
    int n;
    if(sscanf("%d", input, n) != 1)
        explode_bomb();
    if (n == 0)
        explode_bomb();
    if (func4(n) != 55)
        explode_bomb();
        
}

void phase_5(char ** input){
    char ** string = "isrveawhobpnutfg";	
    if (string_length(input) != 6)
        explode_bomb();
    for (int i = 0; i <= 5; i++){
        char c = input[0];
     
    }
    if (strings_not_equal(input, "giants"))
        explode_bomb();
}

void phase_6(char ** input){
}

int main( ){
    return 0;
}
