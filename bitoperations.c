#include <stdio.h>

void binPrint(unsigned int i)
{
    // number of bits in an integer
    int j = sizeof(unsigned int) * 8;

    // temp variable
    int k;

    // Loop over the number of bits in i, left to right
    for (j--; j >= 0; j--)
    {
        // Pick out the j^th bit of i, check if t/f
        k = ((1 << j) & i) ? 1 : 0;
        // print k
        printf("%d", k);
    }
}

int main(int argc, char const *argv[])
{
    // set i to literal value
    unsigned int i = 0x0f0f0f0f; // 4294967295

    printf("Original:\t");
    // returns 241 in decimal printed in
    // binary as a 32-bit integer
    binPrint(i);
    printf("\t%X\t%u\n\n", i, i);

    // 32
    int j = sizeof(unsigned int) * 8;

    for (j--; j >= 0; j--)
    {
        // i shifted left j times
        binPrint(1 << j); printf("\n");
        // printf("(1 << %3u) & %08x:", j, i);
        // i
        binPrint(i); printf("\n");
        printf("-------------------------------- &\n");
        // 1 shifted left j times bitwise logical and i
        binPrint((1 << j) & i); printf("\n\n");
    }

    return 0;
}