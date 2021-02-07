#include <stdio.h>

int main(int argc, char const *argv[])
{
    int i = 0xF1;

    printf("Dec: %d\n", i);
    printf("Hex: %X\n", i);

    printf("Size of int: %d\n", sizeof(i));
    printf("Size of char: %d\n", sizeof(char));

    char c = 41;
    printf("c in char is: %c\n", c);
    printf("c in int is: %d\n", c);

    char j = 1000000000;
    printf("j in char is: %c\n", j);
    printf("j in int is: %c\n", (char)j);
    printf("j in int from char is: %d\n", (int) (char) j);

    return 0;
    
}