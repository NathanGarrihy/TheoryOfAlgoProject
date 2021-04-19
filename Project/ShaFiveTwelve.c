#include <stdio.h>
#include <inttypes.h>

// Pre-processor directive
#define WORD uint64_t
#define PF PRIX64
#define BYTE uint8_t

// Page 5 of the secure hash standard
// Rotate right functions
#define ROTR(x,n) (x>>n)|(x<<(W-n)) 

// Page 10 of the secure hash standard
#define CH(x,y,z) (x&y)^(~x&z) 
#define MAJ(x,y,z) (x&y)^(x&z)^(y&z)
#define SHR(x,n) x>>n

#define SIG0(x) ROTR(x,2)^ROTR(x,13)^ROTR(x,22)
#define SIG1(x) ROTR(x,6)^ROTR(x,11)^ROTR(x,25)
#define Sig0(x) ROTR(x,7)^ROTR(x,18)^SHR(x,3)
#define Sig1(x) ROTR(x,17)^ROTR(x,19)^SHR(x,10)

union Block
{
    BYTE bytes[64];
    WORD words[16];
    uint64_t sixf[8];
};

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