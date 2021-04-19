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

enum Status
{
    READ,
    PAD,
    END
};

const WORD K[] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

int next_block(FILE *f, union Block *B, enum Status *S, uint64_t *nobits)
{
    // Num of bytes to read
    size_t nobytes;

    if (*S == END)
    {
        return 0;
    }
    else if (*S == READ)
    {
        // Try to read 128 bytes.
        nobytes = fread(&B->bytes, 1, 128, f);
        // Calculate the total bits read so far.
        *nobits = *nobits + (8 * nobytes);
        // Enough room for padding
        if (nobytes == 128)
        {
            return 1;
        }
        else if (nobytes < 112)
        {
            // Append a 1 bit (and seven 0 bits to make a full byte).
            B->bytes[nobytes++] = 0x80; // In bits: 1000000
            // Append enough 0 bits, leaving 64 at the end
            while (nobytes++ < 112)
            {
                B->bytes[nobytes] = 0x00; // In bits: 00000000
            }
            // Append length of original input (CHECK ENDIANESS)
            B->sixf[15] = (islilend() ? bswap_64(*nobits) : *nobits);
            // Say this is the last block.
            *S = END;
        }
        else
        {
            // End of input message.
            // Not enough room in this block for all padding
            // Append a 1 bit (and seven 0 bits to make a full byte)
            B->bytes[nobytes] = 0x80;
            // Append 0 bits.
            while (nobytes++ < 128)
            {
                B->bytes[nobytes] = 0x00;
            }
            // Change the status of PAD.
            *S = PAD;
        }
    }
    else if (*S == PAD)
    {
        nobytes = 0;
        // Append 0 bits.
        while (nobytes++ < 112) {
            B->bytes[nobytes] = 0x00; // In bits: 00000000
        }
        // Append nobits as an integer.
        B->sixf[15] = (islilend() ? bswap_64(*nobits) : *nobits);;
        // Change the status to END.
        *S = END;
    }

    // If little endian, wwap the byte order of the words
    if (islilend())
        for (int i = 0; i <= 15; i++)
            B->words[i] = bswap_64(B->words[i]);

    return 1;
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