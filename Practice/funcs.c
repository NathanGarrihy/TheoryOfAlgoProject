#include <stdio.h>
#include <inttypes.h>

// Pre-processor directive
#define WORD uint32_t
#define PF PRIX32

// operations on 32-bit words
WORD Ch(WORD x, WORD y, WORD z) {
    return (x&y)^(~x&z);
}

int main(int argc, char *argv[]) {
    // bit-wise variable definition
    WORD x = 0x0F0F0F0F;
    WORD y = 0x0A0A0A0A;
    WORD z = 0xB0B0B0B0;
    
    // Calculation on bit-wise numbers
    WORD ans = Ch(x, y, z);

    // Outputting answers
    printf("Ch(%08" PF ",%08" PF ",%08" PF ")=%08" PF "\n", x, y, z, ans);

    return 0;
}