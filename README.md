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
The message and its padding are parsed into N 1024-bit blocks, M<sup>(1)</sup>, M<sup>(2)</sup>, …, M<sup>(N)</sup>.

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
By using the big-endian convention throughout, for each 64-bit word, the left-most bit is stored in the most signficant bit position.
The final piece of pre-processing, setting the initial hash value, is performed in the main method.
<br/><br/>
##### The next_hash function
is in charge of Hash Computation, where it uses the pre-defined bitwise and logical functions and constants
and performs addition modulo 2<sup>64</sup>. Each message block is processed in order:
1. Preparing the working schedule:
![Prepare Message Schedule](https://i.gyazo.com/8618261e0b3019d2852567bd5f5c8ac4.png "Prepare Message Schedule")
![Message schedule](https://i.gyazo.com/9d70cdd72d621b53e9e31ecb586a645b.png "SHA-512 Message schedule")

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
##### Other files
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
To compile and run this code, you must first clone the repository onto your own device by navigating to an empty directory, opening a new  
terminal (can be windows or linux) and typing:
```
git clone https://github.com/NathanGarrihy/TheoryOfAlgoProject
```
After cloning the repo, navigate to the correct directory and typing 

```
cd TheoryOfAlgoProject
```
Once there, in order to run the algorithm you must first: 
* put the desired input text into the abc.txt file
* type 'make sha512'
* type './sha512 abc.txt'

This will execute the sha512 algorithm on whatever text is inside the abc.txt file.

To run the tests:
* type 'make clean'
* next, type 'make'
* then type 'make test'

![Terminal](https://i.gyazo.com/4625521628ee2d77cecf5959697f405d.png "Terminal Commands")
 
## What the SHA512 algorithm is and why it's important
SHA-512 is a function of cryptographic algorithm SHA-2, derived from SHA-1. The algorithm is a secure hash algorithm
which is part of the Secure Hash Standard (SHS) (FIPS PUB 180-4) and by is a Computer security standard 
in Cryptography. It takes a message of any length < 2<sup>128</sup> bits as input into the SHA-512 hash 
algorithm. The 512-bit hash value is computed using the compression function f in a Merkle–Damgård construction, which is a method of
building hash functions which make it hard to find two inputs that hash to the same output.
SHA-512 finally returns an output known as a message digest, which has a length of 512 bits.
<br/>
![MerkleDamgård](https://i.gyazo.com/fcea45249b1fc6e7ac4bb84a104458d1.png "Merkle–Damgård construction")
<br/>
According to the secure hash standard, SHA512 is "secure" because any change to a message will, with a very high probability, 
result in a different message digest. It is also computationally infeasible to:
* find a message that corresponds to a given message digest, or
* find two different messages that produce the same message digest.


I also previously explained how this algorithm works [in the description](#Description). In terms of the importance of SHA-512, 
it is mainly used for internet security, with hashing functions being used in Transport Layer Security, Secure Sockets Layer,
Pretty Good Privacy, SSH and a wide range of protocols and security applications. These internet layers are vulnerable to attacks and 
in many cases, the data which is being transported accross the internet is quite sensitive and it could be very consequential if it falls
into the wrong hands through hacking. There are 2<sup>512</sup> hash combinations for SHA-512, it is almost impossible to decrypt the output from a 
SHA-512 function and performing this function on this sensitive data plays an essential role in protecting private information and reducing 
a large number of vulnerabilities. 

```c
// Number of possible SHA512 hash combinations =
13,407,807,929,942,594,536,800,336,840,032,056,096,056,320,224,120,632,584,568,920,184,736,920,912,928,608,384,136,552,016,760,840,912,392,072,016,104,496,552,640,888,296,464,288,576,984,520,184,520,112,096
```
 
A digital certificate or identity certificate, is an electronic document used to prove the ownership of a public key. To assure authenticity,
only the sender has their private key so only they can encrypt by using their private key which can be decrypted by sender’s public key. These
keys are vulnerable when they're being passed around on the internet so its important that they use a hashing algorithm like SHA-512 to encrypt
the data, making it virtually unhackable by a third party and decreasing the vulnerabilities on the keys.
<br/>
![Digital Certificate](https://id4d.worldbank.org/sites/id4d-ms8.extcc.com/files/inline-images/18%20digital%20certificates.png "Digital Certificate")
<br/>
Blockchain is a decentralized, digital ledger that contains a systematic series of blocks which uses hashing algorithms including SHA-512 to
link all of the blocks together. It allows digital information to be recorded and distributed, but not edited. Blockchain is basically a database which verifies 
transactions fairly inexpensively and stores the information in the form of secure hash values, which significantly increases the safety of this information. Blockchain
is used as a cryptocurrency wallet and cryptocurrency exchange and supports popular cryptocurrencies such as Bitcoin and Etherium. These coins are rising in popularity and 
importance as bitcoin and co are beginning to become commonly accepted as a form of payment. This adds to the importance of SHA-512 as cryptocurrency wallets and exchange
require high level encryption with low computational cost, which is achievable through the SHA-512 algorithm. There could soon come a time where
cryptocurrencies are the main form of payment around the world and this evolution will be driven by the secure hashing of the SHA-512 algorithm.
<br/>
![Blockchain](https://i.gyazo.com/984edfe83ff026363b01a16f98fbbc0f.png "How Blockchain Works")
<br/>
Hashing is a good way validate messages, allows passwords to be stored securely, and makes determining whether or not a message or file has been tampered with a lot quicker.



# Questions:
## Why can't we reverse the SHA-512 algorithm to retrieve the original message from a hash digest?
The operations performed by SHA-512 provide a result which is computationally unfeasible to be calculated backwards. To find the original message,
one would first have to first get the correct hash digest, then construct an input which produces the same hash by generating about 2<sup>512</sup> inputs
and computing the hash for each input until a match is found. Given the current state of technology, it would take less time for mankind to die out completely
than it would to find the original message from a hash digest using this method, which according to top hashing experts, is one of the only reasonable ways to go about
reversing SHA-512, even though it is extremely impractical.
<br/> 
The number of possible hash values (2<sup>512</sup>), is greatly exceeded by the number of possible inputs. Because of this, an input linked to a hash 
won’t always match the original input/message. Vulnerabilities which made reversing of hashing algorithms possible was one of the main reasons for the
upgrade from MD5/SHA1 hashing up to SHA2 (set of cryptographic hash functions which contains SHA-512). The NIST mainly published SHA-2 to combat these
vulnerabilities and wouldn't have published SHA-512 as a federal standard or a royalty-free license if it was unable to do so successfully.
<br/>
Since we are getting the modulo (what is left over after division) instead of a direct number input, the returned number from the modulo operation 
can be found in a large number of different ways. In fact, there are infinite combinations which could get the result of a modulo operation, making it impossible
to crack. A preimage attack attempts to find a message associated with a specific hash value. When compared to other hashing algorithms, sha512 and other SHA-2
algorithms are much more tolerant to preimage attacks. In fact, since the SHA-512 algorithm carries 80 steps, it doesn't currently face any major security
concerns related to preimage attacks. However, in the future it is important to account for advances in technology as any major technological advances could improve the
strength of the preimage attacks to a detrimental extent.

## Can you design an algorithm that, given enough time, will find input messages that give each of the possible 512-bit strings?
I must restate that there are  2<sup>512</sup> possible outputs, while the number of possible inputs is much larger, being 2<sup>2<sup>512</sup></sup>
<br/>

```c
// Number of possible SHA512 hash combinations =
13,407,807,929,942,594,536,800,336,840,032,056,096,056,320,224,120,632,584,568,920,184,736,920,912,928,608,384,136,552,016,760,840,912,392,072,016,104,496,552,640,888,296,464,288,576,984,520,184,520,112,096

// Possible outputs = 2 to the power of the above number, which provides a significantly larger value
```

This question caused a bit of confusion for me at first, but I feel I understand what it is asking now as it appears to be looking to see
if you can design an algorithm that could find one input message for every potential 512-bit output. <br/>
With the current technology that we have, it would be possible to design an algorithm that would find input messages that give each of the 
possible 512-bit strings. However, due to the large number of potential outputs and even larger number of potential inputs, it would be 
highly time consuming to try and find unique input messages for every single possible 512-bit string. So time consuming in fact, that I would
re-assess the meaning of 'given enough time' in this question, as 'enough time' for this to work, is a lot more time than any human could offer.
<br/>
There are so many different hash combinations to account for and in most cases, the algorithm will be outputting hashes which have already been retrieved and
there will be a struggle to find input messages that correspond to the remaining hash. The algorithm would involve continually parsing code through the SHA-512
algorithm, recording any unique hashes with their relevant input message and repeating this process until all 2<sup>512</sup> hash numbers have been accounted for.
Although SHA-512 is considered to be collision resistent, collisions when be unavoidable when trying to execute an algorithm like this.
<br/>
So, yes. one could design an algorithm that, given enough time, will find input messages that give each of the possible 512-bit strings. However, 
I can't see anybody ever devoting the time or resources to perform such a task as I don't see any practical reason to get an input message for every
every 512 hash output, other than a heartwarming sense of accomplishment.  

## How difficult is it to find a hash digest beginning with at least twelve zeros?
SHA-512 is a one way function which is aimed at creating hash values which are difficult to predict in the sense that there is no real 
correlation between input text and output hash. At first glance, it seems hard to find a hash value with 12 leading hex zeroes. Mathematically,
one out of every 2<sup>48</sup> hash values are ones beginning with 12 zeros. This, coupled with the fact that there are 2<sup>512</sup> 
possible hash outputs means that there is very little probability that the hash value returned is one which contains 12 or more zeros. The fact
that SHA-512 is a one way function also means that to find a hash digest beginning with at least twelve zeros, one must re-run the SHA-512
algorithm over and over again until eventually, they find the desired number of leading zeros.

Bitcoin mining involves systems trying to achieve a hash. This hash has a value that must be below a certain number in order to be mined. This
number is denoted as the target hash and the number of zeros this hash begins with is referred to as the difficulty since it's harder to mine
coins with more leading zeros. To this date, thanks to advances in technology and growing numbers of machines which are mining, bitcoin miners
have to find a hash beginning with nineteen zeroes. But with this we have to consider the extremely wide scale of high quality michenes which
are set up to mine bitcoin, which is proven in studies like [this one]((https://www.bbc.com/news/technology-22153687)) which one shows part of 
bitcoins worldwide mining operations.

In a centralized system, the difficulty involved in finding a hash digest beginning with at least twelve zeros would depend on the hardware
which is in use. There is quite a low probability in finding a hash digest with all these leading zeros so a lot of iterations would be involved
before the correct output is obtained. To perform all of these calculations within a reasonable amount of time, a good system would be required,
but for the most part it would be a case of trial and error until the correct hash is found, which doesn't stray too far in technical diffculty
from executing the SHA-512 algorithm.

For large distributed systems, finding a hash digest with twelve zeros or more is a lot easier as the time taken to iterate through the possibilities
would be greatly decreased by the sheer amount of systems working on the problem. This is why bitcoin now has a difficulty of 19 (19 leading zeros).

All and all it is evident from my research that it is not all that difficult to find a hash digest beginning with at least twelve zeros. In 
terms of technical difficulty, it's just as easy as repeating the SHA-512 function until the desired hash is achieved. The time taken to do 
such a thing could be quite lengthy but fast improving technology is making it easier and easier every year.

**Analogy:** <br/>
The way I see it, finding a hash digest beginning with at least twelve zeros is similar to trying to win a raffle. Buying the ticket is not
that difficult, but you're probably not going to win unless you keep buying tickets! If you keep buying loads of tickets, eventually you're bound
to win, or find a hash digest full of leading zeros.

# References for research elements and images
* [SHA-512 BitcoinWiki](https://en.bitcoinwiki.org/wiki/SHA-512)

* [SHA-2 Wiki](https://en.wikipedia.org/wiki/SHA-2)

* [Analysis of SHA-512 for encryption purposes](https://www.researchgate.net/publication/327392778_Analysis_of_Secure_Hash_Algorithm_SHA_512_for_Encryption_Process_on_Web_Based_Application)

* [Descriptions of SHA-256, SHA-384, and SHA-512](http://www.iwar.org.uk/comsec/resources/cipher/sha256-384-512.pdf)

* [Why can't we reverse a hash?](https://stackoverflow.com/questions/6603849/why-is-it-not-possible-to-reverse-a-cryptographic-hash)

* [Security Evaluation of SHA-224, SHA-512/224, and SHA-512/256](https://www.cryptrec.go.jp/exreport/cryptrec-ex-2401-2014.pdf)

* [Merkle Damgård construction](https://en.wikipedia.org/wiki/Merkle–Damgård_construction)

* [Digital certificate i](https://www.geeksforgeeks.org/digital-signatures-certificates/)

* [Digital certificate ii](https://id4d.worldbank.org/guide/digital-certificates-and-pki)

* [Public key certificate](https://en.wikipedia.org/wiki/Public_key_certificate)

* [Blockchain i](https://rishi30-mehta.medium.com/hashing-algorithms-the-brain-of-blockchain-sha-256-sha-512-and-more-7b5f80b99b00)

* [Blockchain ii](https://www.bitpanda.com/academy/en/lessons/how-does-a-blockchain-work/)

* [Preimage attacks](https://eprint.iacr.org/2009/479.pdf)

* [Leading Zeros](https://youtu.be/_LPaIL6AqSQ)