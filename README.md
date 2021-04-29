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

We had completed sha256 in class so the main part of this project consisted of learning how SHA512 differs
from SHA256 and figuring out how to implement these changes. This was aided by [the secure hash standard](https://www.nist.gov/publications/secure-hash-standard)
which clearly outlines hash algorithms that can be used to generate message digests.

## Description
While they are very similar in structure, the main differences between sha512 and sha256 were:
* With sha512, the message is broken into 1024-bit chunks
* Initial hash values and round constants are increased to 64 bits
* Rounds are increased from 64 to 80
* Message sechedule array contains 80 64-bit words(sha512) as opposed to 64 32-bit words(sha256)
* The loop to extend message schedule array goes from 16 to 79
* First 80 prime numbers (2 -> 409) are the base of the round constants
* Calculation word size is 64-bits long
* The appended message length is a 128-bit big-endian integer and uses different shift and rotate amounts

The sha512.c file is the file which contains all of the main code for this project. For this file, I first
set up my pre-processor directive by assigning some constant variable values. I set up 'WORD' as a 'uint64_t', which is
an unsigned integer that carries a guarenteed width of 64 bits. PY as a PRIx64, which is a string literal that's used to
notify printf's of a 64 bit unsigned hexadecimal value. As well as BYTE, which is a uint8_t, similar to WORD but only
carries a width of 8 bits.

Page 5 of the secure hash standard outlines symbols and operations which are used in the secure hash algorithm specifications.
I only implemented the Rotate Right and Shift Right functions as they are the only functions
required by SHA512. Section 4.1.3 outlines six logical functions, where each
function operates on 64-bit words, which are represented as x, y, and z. Each function
returns a new 64-bit word. These functions are also defined at the top of sha512.c.



## Compilation instructions


## What the SHA512 algorithm is and why it's important


# Questions:
## Why can't we reverse the SHA512 algorithm to retrieve the original message from a hash digest?

## Can you design an algorithm that, given enough time, will find input messages that give each of the possible 512-bit strings?

## How difficult is it to find a hash digest beginning with at least twelve zeros?

#References
[SHA-512 BitcoinWiki](https://en.bitcoinwiki.org/wiki/SHA-512)