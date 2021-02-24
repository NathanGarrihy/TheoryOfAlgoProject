#include <stdio.h>
#include <inttypes.h>

// Pre-processor directive
#define WORD uint64_t
#define PF PRIX64

// operations on 64-bit words
WORD Ch(WORD x, WORD y, WORD z) {
    return (x&y)^(~x&z);
}

WORD Maj(WORD x, WORD y, WORD z) {
    return (x&y)^(x&z)^(y&z);
}

int main(int argc, char *argv[]) {
    // bit-wise variable definition
    WORD x = 0x0F0F0F0F0F0F0F0F;
    WORD y = 0x0A0A0A0A0A0A0A0A;
    WORD z = 0xB0B0B0B0B0B0B0B0;
    
    // Calculation on bit-wise numbers
    WORD chA = Ch(x, y, z);
   // WORD majA = Maj(x, y, z);
    WORD majA = Maj(x, y, z);

    // Outputting answers
    printf("Ch(%08" PF ",%08" PF ",%08" PF ")=%08" PF "\n", x, y, z, chA);

    printf("Maj(%08" PF ",%08" PF ",%08" PF ")=%08" PF "\n", x, y, z, majA);

    return 0;
}