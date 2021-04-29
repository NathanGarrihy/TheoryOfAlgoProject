# Theory Of Algorithms Project, 2021
# Nathan Garrihy - G00354922
<br/>

## Introduction
As part of my final year Theory Of Algorithms module for B.Sc.(Hons) in Software Development, 
I had to write a program in the C programming language to calculate the SHA512 value of an input file. 
We were asked to ensure the program has the following functionality:

* The program must take the name or path of the file as a command line
argument and output the SHA512 digest of it. 
* The program must be coded from scratch
* We cannot use any external libraries other than what is included in the C standard library.
* Must compile using 'gcc' or 'clang'
* The program must include a Makefile which compiles it upon make being called in the project folder.
* It must also include tests which run upon 'make test' being called
* Last commit on or before the 3rd May 2021

We had completed SHA256 in class so the main part of this project consisted of learning how SHA512 differs
from SHA256 and figuring out how to implement these changes. This was aided by [the secure hash standard](https://www.nist.gov/publications/secure-hash-standard)
which clearly outlines hash algorithms that can be used to generate message digests.

## Description
While they are very similar in structure, the main differences between sha512 and SHA256 were:
* With sha512, the message is broken into 1024-bit chunks
* Initial hash values and round constants are increased to 64 bits
* Rounds are increased from 64 to 80
* Message schedule array contains 80 64-bit words(sha512) as opposed to 64 32-bit words(SHA256)
* The loop to extend message schedule array goes from 16 to 79
* First 80 prime numbers (2 to 409) are the base of the round constants
* Calculation word size is 64-bits long
* The appended message length is a 128-bit big-endian integer and uses different shift and rotate amounts

The sha512.c file is the file which contains all of the main code for this project. For this file, I first
set up my pre-processor directive by assigning some constant variable values. I set up 'WORD' as a 'uint64_t', which is
an unsigned integer that carries a guaranteed width of 64 bits. PY as a PRIx64, which is a string literal that's used to
notify printf's of a 64 bit unsigned hexadecimal value. As well as BYTE, which is a uint8_t, like WORD but only
carries a width of 8 bits.

```C
// Pre-processor directive
#define WORD uint64_t
#define PF PRIx64
#define BYTE uint8_t
```

Page 5 of the secure hash standard outlines symbols and operations which are used in the secure hash algorithm specifications.
I only implemented the Rotate Right and Shift Right functions as they are the only functions
required by SHA512. Section 4.1.3 outlines six logical functions, where each
function operates on 64-bit words, which are represented as x, y, and z. Each function
returns a new 64-bit word. These functions are also defined at the top of sha512.c.

```C
// Rotate right function
#define ROTR(_x, _n) ((_x >> _n) | (_x << ((sizeof(_x) * 8) - _n)))
// The right shift operation
#define SHR(_x, _n) (_x >> _n)

// SHA512 logical functions
#define CH(_x, _y, _z) ((_x & _y) ^ (~_x & _z)) 
#define MAJ(_x, _y, _z) ((_x & _y) ^ (_x & _z) ^ (_y & _z))
#define SIG0(_x) (ROTR(_x, 28) ^ ROTR(_x, 34) ^ ROTR(_x, 39)) 
#define SIG1(_x) (ROTR(_x, 14) ^ ROTR(_x, 18) ^ ROTR(_x, 41)) 
#define Sig0(_x) (ROTR(_x, 1) ^ ROTR(_x, 8) ^ SHR(_x, 7)) 
#define Sig1(_x) (ROTR(_x, 19) ^ ROTR(_x, 61) ^ SHR(_x, 6))
```

The next thing that was added was a 1024-bit union Block to split the input into a series of equally sized blocks and an 
enumerator for Status which keeps track of where we are with the input message and helps to keep the correct order.

```c
// 1024 bit block for sha-512
union Block
{
    BYTE bytes[64];
    WORD words[16];
    uint64_t sixf[8];
};
```
```c
// Used with the input message
enum Status
{
    READ,
    PAD,
    END
};
```

For my final variable declaration I setup an array of constant 64 bit unsigned integers, which contains the 80 constant 64-bit 
words which are used by SHA-512.
<br/>
##### The next_block function 
handles the majority of the preprocessing. It takes in a file, block, status and number of bits. It's in charge of
first padding the message, which is first done to ensure that the padded message is a multiple of 1024 bits. Padding is inserted before 
hash computation begins on a message.
<br/>
The message and its padding are parsed into N 1024-bit blocks, M<sup>(1)</sup>, M<sup>(2)</sup>, …, M<sup>(N)</sup>
. Since the 1024 bits of the input block may be expressed as 16 64-bit words.

```c
int next_block(FILE *f, union Block *B, enum Status *S, uint64_t *nobits)
{
    // Num of bytes to read
    size_t nobytes;

    if (*S == END)
    {
        // Last block
        return 0;
    }
    else if (*S == READ)
    {
        // Try to read 128 bytes.
        nobytes = fread(B->bytes, 1, 128, f);
        // Calculate the total bits read so far.
        *nobits = *nobits + (8 * nobytes);
        // Enough room for padding
        if (nobytes == 128)
        {
            // Don't do anything
            return 1;
        }
        else if (nobytes < 112)
        {
            // We have enough room for all of the padding
            // Append a 1 bit (and seven 0 bits to make a full byte).
            B->bytes[nobytes] = 0x80; // In bits: 10000000
            // Append enough 0 bits, leaving 64 at the end
            while (nobytes++ < 112)
            {
                B->bytes[nobytes] = 0x00; // In bits: 00000000
            }
            // Append length of original input (CHECK ENDIANESS)
            B->sixf[15] = (islilend() ? bswap_64(*nobits) : *nobits);
            // Set status to END / last block.
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
                B->bytes[nobytes] = 0x00;// In bits: 00000000
            }
            // Set status to PAD.
            *S = PAD;
        }
    }
    else if (*S == PAD)
    {
        // Append 0 bits.
        while (nobytes++ < 112) {
            B->bytes[nobytes] = 0x00; // In bits: 00000000
        }
        // Append nobits as an integer
        B->sixf[15] = (islilend() ? bswap_64(*nobits) : *nobits);;
        // Change the status to END.
        *S = END;
    }

    // If little endian, swap the byte order of the words
    if (islilend())
        for (int i = 0; i <= 15; i++)
            B->words[i] = bswap_64(B->words[i]);
    return 1;
}
```

Setting the initial hash value is performed in the main method.
<br/><br/>
##### The next_hash function
is in charge of Hash Computation, where it uses the pre-defined bitwise and logical functions and constants
and performs addition modulo 2<sup>64</sup>. Each message block is processed in order:
1. Preparing the working schedule:
![Prepare Message Schedule](https://i.gyazo.com/8618261e0b3019d2852567bd5f5c8ac4.png "Prepare Message Schedule")
    ```c
    for (t = 0; t < 16; t++)
        W[t] = M->words[t];
    for (t = 16; t < 80; t++)
        W[t] = Sig1(W[t-2]) + W[t-7] + Sig0(W[t-15]) + W[t-16];
    ```

2. Initialize the eight working variables with the (i-1)<sup>st</sup> hash value:
![Init working](https://i.gyazo.com/0708c88fbb72f4793044111a33d8497c.png "Initialize wv")

    ```c
    a = H[0]; b = H[1]; c = H[2]; d = H[3];
    e = H[4]; f = H[5]; g = H[6]; h = H[7];
    ```

3. Swap values For t= 0 to 79:
![Swap values](https://i.gyazo.com/dde576b9c15249b7ba6798ce63c4208a.png "Rearrange Values")

    ```c
   for (t = 0; t < 80; t++) {
           T1 = h + SIG1(e) + CH(e, f, g) + K[t] + W[t];
           T2 = SIG0(a) + MAJ(a, b, c);
           h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
       }
    ```
   
4. Compute the i<sup>th</sup> intermediate hash value H<sup>(i)</sup>
![Compute Intermediate](https://i.gyazo.com/99ba25cd25b66af3df1a3a5af8ae5e8c.png "Compute Intermediate")

    ```c
   H[0] = a + H[0]; H[1] = b + H[1]; H[2] = c + H[2]; H[3] = d + H[3];
   H[4] = e + H[4]; H[5] = f + H[5]; H[6] = g + H[6]; H[7] = h + H[7];
    ```
   
##### The sha512 function
The sha512 function takes in a file and an array of unsigned 64 bit integers. Its main purpose is to
loop the next_block function which executes the message padding. It then passes the Block and unsigned 
integers to the next_hash function which performs message scheduling  
the hash computation. 

```c
    while (next_block(f, &M, &S, &nobits)) {
        next_hash(&M, H);
        }
```
    
Once it has completed these steps it forms the 512-bit message digest:
![Message Digest](https://i.gyazo.com/c0fc38771e8aadd34acf5a0fbaaf5208.png "Message Digest")

##### Main code
The main code first initializes an array of 8 unsigned 64-bit integers which are the official initial hash values for sha512. They can also be
found by taking the first 64-bits of the fractional parts of the square roots of the first 8 prime numbers. I then create the file pointer for 
reading and read in the file from the command line for reading.

```c
    FILE *f;
    f = fopen(argv[1], "r");
```
It then runs the sha512 function, providing the input file and initial hash values as parameters. It then prints each character of the hash digest to the screen.

```c
    for (int i = 0; i < 8; i++)
        printf("%016" PF, H[i]);
        printf("\n");
```
and finally closes the file and returns 0 to exit the main method.
<br/>
The repository also contains a Makefile which is essentially a text file that contains the instructions for building the program on the command line. Each command
is its own separate rule inside the Makefile.
<br/>
![Makefile](https://i.gyazo.com/c8cf3a2a54b76089b5956091e4e7069b.png "Makefile")
<br/>
In my tests.sh file I created a test for the abc.txt file and the empty text file which both compare the output of sha512 to the output of the built in sha512sum
function and returns a message to the user informing them whether the output was as expected or if the test failed.
My repo also contains a gitignore file which restricts the unnecessary files from being uploaded, as well as the empty text
file and abc.txt file which contains the string "abc". 

## Compilation instructions


## What the SHA512 algorithm is and why it's important


# Questions:
## Why can't we reverse the SHA512 algorithm to retrieve the original message from a hash digest?

## Can you design an algorithm that, given enough time, will find input messages that give each of the possible 512-bit strings?

## How difficult is it to find a hash digest beginning with at least twelve zeros?

# References
[SHA-512 BitcoinWiki](https://en.bitcoinwiki.org/wiki/SHA-512)
