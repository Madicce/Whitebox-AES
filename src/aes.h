#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <stdio.h>

#define Nb 4  // Block Size
#define Nk 4  // Key Length
#define Nr 10 // Number of Rounds

typedef struct block_t block_t;

extern uint8_t s_box[256];
extern uint8_t inv_s_box[256];

block_t *block_alloc(const size_t size);
void block_free(block_t *state);
uint8_t block_get(const block_t *state, const size_t row, const size_t column);
void block_set(const block_t *state, uint8_t value, const size_t row,
               const size_t column);
void block_copy(block_t *src, block_t *dest);
void get_key(uint8_t round_key[Nb * (Nr + 1)][Nb], block_t *key, size_t round);
void read_in_stdout(char *hexkey, uint8_t data[16]);
/* return new_state modified by XOR operation */
void add_round_key(block_t *block, block_t *round_key);
uint8_t gf_mul(uint8_t a, uint8_t b);
void inverse_cipher(block_t *block, uint8_t round_key[Nb * (Nr + 1)][Nb]);
void key_expansion(uint8_t round_key[Nb * (Nr + 1)][Nb], block_t *key);
void inverse_mix_columns(block_t *block);
void inverse_sub_bytes(block_t *block);
void inverse_shift_rows(block_t *block);
void mix_columns(block_t *block);
void shift_rows(block_t *block);
void sub_bytes(block_t *block);
void convert_to_block(uint8_t *input, block_t *block);
void convert_to_output(block_t *block, uint8_t *output);
void print_block(block_t *block);
void print_data(uint8_t *data, size_t size);
uint8_t **key_round_alloc(size_t rows, size_t columns);
void print_round_key(uint8_t round_key[Nb * (Nr + 1)][Nb]);
void key_to_block(uint8_t *key, block_t *block);

#endif /* AES_H */