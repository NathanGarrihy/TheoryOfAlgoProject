#include <stdio.h>

void binPrint(unsigned int i)
{
    // number of bits in an integer
    int j = sizeof(int) * 8;

    // temp variable
    int k;

    // Loop over the number of bits in i
    for (j--; j >= 0; j--)
    {
        // 
        k = ((1 << j) & i) ? 1 : 0;
        // print k
        printf("%d", k);
    }
}

int main(int argc, char const *argv[])
{
    // set i to literal value
    unsigned int i = 0xffffffff; // 4294967295

    printf("Original:  ");
    // returns 241 in decimal printed in
    // binary as a 32-bit integer
    binPrint(i);
    printf("\t%X\t%u\n", i, i);

    for (int j = 0; j < 40; j++)
    {
        printf("%3u << %2d ", i, j);
        // i shifted left j times
        binPrint(i << j);
        printf("\n");
    }

    return 0;
}