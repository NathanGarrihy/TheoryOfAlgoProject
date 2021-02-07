#include <stdio.h>

void binPrint(int i)
{
    // number of bits in an integer
    int j = sizeof(int) * 8;

    // temp variable
    int k;

    for (j--; j >= 0; j--)
    {
        k = ((1 << j) & i) ? 1 : 0;
        printf("%d", k);
    }
}

int main(int argc, char const *argv[])
{
    int i = 241;

    printf("Original:  ");
    // returns 241 in decimal printed in
    // binary as a 32-bit integer
    binPrint(i);
    printf("\n");

    for (int j = 0; j < 40; j++)
    {
        printf("%3d << %2d ", i, j);
        binPrint(i << j);
        printf("\n");
    }

    return 0;
}