#include <stdio.h>
#include <inttypes.h>

// Pre-processor directive
#define W 32
#define WORD uint32_t
#define PF PRIX32

// Page 5 of the secure hash standard
#define ROTL(x,n) (x<<n)|(x>>(W-n)) 
#define ROTR(x,n) (x>>n)|(x<<(W-n)) 

// Page 10 of the secure hash standard
#define CH(x,y,z) (x&y)^(~x&z) // pre-processor directive
#define MAJ(x,y,z) (x&y)^(x&z)^(y&z) 


int main(int argc, char *argv[]) {
    // bit-wise variable definition
    WORD x = 0xF1234567;
    WORD y = 0x0A0A0A0A;
    WORD z = 0xB0B0B0B0;
    
    // Calculation on bit-wise numbers
    WORD ans;
    
    ans = CH(x, y, z);
    // Outputting answers
    printf("Ch(%08" PF ",%08" PF ",%08" PF ")=%08" PF "\n", x, y, z, ans);

    ans = MAJ(x,y,z);
    printf("MAJ(%08" PF ",%08" PF ",%08" PF ")=%08" PF "\n", x, y, z, ans);

    printf("ROTL(%08" PF " -> %08" PF "\n", x, ROTL(x,2));
    printf("ROTR(%08" PF " -> %08" PF "\n", x, ROTR(x,4));

    return 0;
}