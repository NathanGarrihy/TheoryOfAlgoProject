#include <stdio.h>
#include <inttypes.h>

#define BYTE uint8_t

void binPrint(BYTE i)
{
    // number of bits in an integer
    int j = sizeof(BYTE) * 8;

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

int main(int argc, char *argv[])
{
    BYTE b;

    FILE *f;
    
    f = fopen(argv[1], "r");

    size_t nobytes;

    nobytes = fread(&b, 1, 1, f);
    while (nobytes) {
        binPrint(b);
        nobytes = fread(&b, 1, 1, f);
    }
    fclose(f);
    printf("\n");

    return 0;
}