# Whitebox-AES

This is an academic project on AES encryption and its Whitebox variant, carried out at the University of Bordeaux, France. The goal was to study and experiment with white-box cryptography, focusing on the AES algorithm and the seminal paper: J.A. Muir, *A tutorial on white-box AES*, 2013.

## Objectives

The project was divided into three main parts:
1. **Classical AES implementation**
   - Development of a standard AES implementation (documented in the report).
   - Verification of correct encryption/decryption.
2. **Whitebox AES implementation**
   - Construction of a whitebox AES following the method proposed by Chow et al.
   - Use of lookup tables with embedded keys.
3. **Attack on Whitebox AES**
   - Implementation of a possible attack on the whitebox AES.
   - Analysis of the results and discussion of weaknesses.

## Repository structure

```bash
├── src/             # Source code (classical AES and Whitebox AES)
├── attacks/         # Attack scripts
├── report/          # Academic report
└── README.md        # Project overview
```

## Usage

Compile and run the Whitebox AES, use file test.txt like this :
```bash
. test.txt
```
This script cleans the repository and compiles the program used to create the Whitebox. The executable generate_whitebox takes a key as an argument and produces the tables.h file.

Example attack :
```bash
make attack
./attack_aes
```

## Results

- The classical AES implementation behaves as expected.
- The Whitebox AES implementation follows the construction proposed in Chow et al.’s paper.
- The attack demonstrates that masking is not an effective protection for Whitebox AES (especially without using mixing bijections as mentioned in the article).

## References

- National Institute of Standards and Technology, *Specification for the ADVANCED ENCRYPTION STANDARD (AES)*, 2001
- Olivier Billet, Henri Gilbert and Charaf Ech-Chatbi, *Cryptanalysis of a White Box AES Implementation*, 2005
- J.A. Muir, *A tutorial on white-box AES*, 2013.
