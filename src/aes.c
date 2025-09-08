#include "aes.h"

#include <stdint.h>
#include <stdlib.h>

#include <string.h>

struct block_t {
  uint8_t **block;
};

#define POLYNOMIAL 0x1B

uint8_t s_box[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B,
    0xFE, 0xD7, 0xAB, 0x76, 0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0, 0xB7, 0xFD, 0x93, 0x26,
    0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2,
    0xEB, 0x27, 0xB2, 0x75, 0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84, 0x53, 0xD1, 0x00, 0xED,
    0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F,
    0x50, 0x3C, 0x9F, 0xA8, 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2, 0xCD, 0x0C, 0x13, 0xEC,
    0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14,
    0xDE, 0x5E, 0x0B, 0xDB, 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79, 0xE7, 0xC8, 0x37, 0x6D,
    0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F,
    0x4B, 0xBD, 0x8B, 0x8A, 0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E, 0xE1, 0xF8, 0x98, 0x11,
    0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F,
    0xB0, 0x54, 0xBB, 0x16};

uint8_t inv_s_box[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E,
    0x81, 0xF3, 0xD7, 0xFB, 0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
    0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB, 0x54, 0x7B, 0x94, 0x32,
    0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49,
    0x6D, 0x8B, 0xD1, 0x25, 0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92, 0x6C, 0x70, 0x48, 0x50,
    0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05,
    0xB8, 0xB3, 0x45, 0x06, 0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
    0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B, 0x3A, 0x91, 0x11, 0x41,
    0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8,
    0x1C, 0x75, 0xDF, 0x6E, 0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
    0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B, 0xFC, 0x56, 0x3E, 0x4B,
    0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59,
    0x27, 0x80, 0xEC, 0x5F, 0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
    0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF, 0xA0, 0xE0, 0x3B, 0x4D,
    0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63,
    0x55, 0x21, 0x0C, 0x7D};

/* powers of x in the field GF(2^8) */
static const uint8_t rcon[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10,
                                 0x20, 0x40, 0x80, 0x1b, 0x36};

uint8_t gf_mul(uint8_t a, uint8_t b) {
  uint8_t result = 0;
  for (; b; b >>= 1) {
    if (b & 1) {
      result ^= a;
    }
    if (a & 0x80) {
      a = (a << 1) ^ POLYNOMIAL;
    } else {
      a <<= 1;
    }
  }
  return result;
}

block_t *block_alloc(const size_t size) {
  block_t *block = malloc(sizeof(block_t));
  if (!block)
    return NULL;

  block->block = calloc(size, sizeof(uint8_t *));
  if (!block->block) {
    free(block);
    return NULL;
  }

  for (size_t i = 0; i < size; i++) {
    block->block[i] = calloc(size, sizeof(uint8_t));

    if (!block->block[i]) {
      for (size_t j = 0; j < i; j++) {
        free(block->block[j]);
      }
      free(block->block);
      free(block);
      return NULL;
    }
  }
  return block;
}

void block_free(block_t *block) {
  if (!block)
    return;

  for (size_t i = 0; i < Nb; i++) {
    free(block->block[i]);
  }
  free(block->block);
  free(block);
}

uint8_t block_get(const block_t *block, const size_t row, const size_t column) {
  if (!block || row >= Nb || column >= Nb)
    return 0;

  return block->block[row][column];
}

void block_set(const block_t *block, uint8_t value, const size_t row,
               const size_t column) {
  if (!block)
    return;
  block->block[row][column] = value;
}

void sub_bytes(block_t *block) {
  if (!block) {
    return;
  }

  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      uint8_t byte = block_get(block, i, j);
      uint8_t substituted = s_box[byte];
      block_set(block, substituted, i, j);
    }
  }
}

static void shift_left(block_t *block, size_t row) {
  if (!block)
    return;

  uint8_t tmp;
  tmp = block->block[row][0];
  for (size_t column = 0; column < Nb; column++) {
    if (column == Nb - 1)
      block->block[row][column] = tmp;
    else
      block->block[row][column] = block->block[row][column + 1];
  }
}

void shift_rows(block_t *block) {
  if (!block)
    return;

  /* shift value depends on the row number */
  for (size_t row = 0; row < Nb; row++) {
    for (size_t j = 0; j < row; j++) {
      shift_left(block, row);
    }
  }
}

static void shift_right(block_t *block, size_t row) {
  if (!block)
    return;

  uint8_t tmp;
  tmp = block->block[row][Nb - 1];
  for (size_t column = Nb - 1; column > 0; column--) {
    block->block[row][column] = block->block[row][column - 1];
  }

  block->block[row][0] = tmp;
}

void inverse_shift_rows(block_t *block) {
  if (!block)
    return;

  /* shift value depends on the row number */
  for (size_t row = 0; row < Nb; row++) {
    for (size_t j = 0; j < row; j++)
      shift_right(block, row);
  }
}

void mix_columns(block_t *block) {
  if (!block)
    return;

  for (size_t i = 0; i < Nb; i++) {
    uint8_t s0 = block->block[0][i];
    uint8_t s1 = block->block[1][i];
    uint8_t s2 = block->block[2][i];
    uint8_t s3 = block->block[3][i];

    uint8_t s0x2 = gf_mul(s0, 0x02);
    uint8_t s1x2 = gf_mul(s1, 0x02);
    uint8_t s2x2 = gf_mul(s2, 0x02);
    uint8_t s3x2 = gf_mul(s3, 0x02);

    uint8_t s0x3 = gf_mul(s0, 0x03);
    uint8_t s1x3 = gf_mul(s1, 0x03);
    uint8_t s2x3 = gf_mul(s2, 0x03);
    uint8_t s3x3 = gf_mul(s3, 0x03);

    block->block[0][i] = s0x2 ^ s1x3 ^ s2 ^ s3;
    block->block[1][i] = s0 ^ s1x2 ^ s2x3 ^ s3;
    block->block[2][i] = s0 ^ s1 ^ s2x2 ^ s3x3;
    block->block[3][i] = s0x3 ^ s1 ^ s2 ^ s3x2;
  }
}

void add_round_key(block_t *block, block_t *round_key) {
  if (!block || !round_key)
    return;

  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      block->block[i][j] ^= round_key->block[i][j];
    }
  }
}

static void rotate_block(block_t *block) {
  if (!block)
    return;

  block_t *tmp = block_alloc(Nb);
  block_copy(block, tmp);
  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      block->block[i][j] = tmp->block[j][i];
    }
  }
  block_free(tmp);
}

void get_key(uint8_t round_key[Nb * (Nr + 1)][Nb], block_t *key, size_t round) {
  if (!round_key || !key)
    return;

  size_t start = (round * 4);
  size_t k = 0;
  for (size_t i = start; i < start + 4; i++) {
    for (size_t j = 0; j < Nb; j++) {
      key->block[k][j] = round_key[i][j];
    }
    k++;
  }
  rotate_block(key);
}

void inverse_sub_bytes(block_t *block) {
  if (!block) {
    return;
  }

  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      uint8_t byte = block_get(block, i, j);
      uint8_t substituted = inv_s_box[byte];
      block_set(block, substituted, i, j);
    }
  }
}

void inverse_mix_columns(block_t *block) {
  if (!block)
    return;

  for (size_t i = 0; i < Nb; i++) {
    uint8_t s0 = block->block[0][i];
    uint8_t s1 = block->block[1][i];
    uint8_t s2 = block->block[2][i];
    uint8_t s3 = block->block[3][i];
    block->block[0][i] = gf_mul(s0, 0x0e) ^ gf_mul(s1, 0x0b) ^
                         gf_mul(s2, 0x0d) ^ gf_mul(s3, 0x09);
    block->block[1][i] = gf_mul(s0, 0x09) ^ gf_mul(s1, 0x0e) ^
                         gf_mul(s2, 0x0b) ^ gf_mul(s3, 0x0d);
    block->block[2][i] = gf_mul(s0, 0x0d) ^ gf_mul(s1, 0x09) ^
                         gf_mul(s2, 0x0e) ^ gf_mul(s3, 0x0b);
    block->block[3][i] = gf_mul(s0, 0x0b) ^ gf_mul(s1, 0x0d) ^
                         gf_mul(s2, 0x09) ^ gf_mul(s3, 0x0e);
  }
}

void inverse_cipher(block_t *block, uint8_t round_key[Nb * (Nr + 1)][Nb]) {
  if (!block || !round_key)
    return;

  block_t *key;
  key = block_alloc(Nb);
  get_key(round_key, key, Nr);
  add_round_key(block, key);
  block_free(key);
  for (size_t round = Nr - 1; round >= 1; round--) {
    inverse_shift_rows(block);
    inverse_sub_bytes(block);
    key = block_alloc(Nb);
    get_key(round_key, key, round);
    add_round_key(block, key);
    inverse_mix_columns(block);
    block_free(key);
  }
  block_t *last_key = block_alloc(Nb);
  get_key(round_key, last_key, 0);
  inverse_shift_rows(block);
  inverse_sub_bytes(block);
  add_round_key(block, last_key);
}

/* performs a cyclic permutation,take [a0,a1,a2,a3]  and returns the word
 * [a1,a2,a3,a0] */
static void rot_word(uint8_t *word) {
  if (!word)
    return;

  uint8_t tmp = word[0];
  for (size_t i = 0; i < Nb - 1; i++)
    word[i] = word[i + 1];

  word[Nb - 1] = tmp;
}

/* applies the S-box */
static void sub_word(uint8_t *word) {
  if (!word)
    return;

  for (size_t i = 0; i < Nb; i++) {
    word[i] = s_box[word[i]];
  }
}

void block_copy(block_t *src, block_t *dest) {
  if (!src || !dest)
    return;

  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      dest->block[i][j] = src->block[i][j];
    }
  }
}

uint8_t **key_round_alloc(size_t rows, size_t columns) {
  uint8_t **key = calloc(rows, sizeof(uint8_t *));
  if (!key) {
    return NULL;
  }

  for (size_t i = 0; i < rows; i++) {
    key[i] = calloc(columns, sizeof(uint8_t));
    if (!key[i]) {
      for (size_t j = 0; j < i; j++) {
        free(key[j]);
      }
      free(key);
      return NULL;
    }
  }

  return key;
}

void key_expansion(uint8_t round_key[Nb * (Nr + 1)][Nb], block_t *key) {
  if (!round_key || !key)
    return;

  //* The first round_key is the key itself
  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      round_key[i][j] = key->block[i][j];
    }
  }

  uint8_t tmp[Nb];
  for (size_t i = Nk; i < Nb * (Nr + 1); i++) {

    for (size_t j = 0; j < Nb; j++) {
      tmp[j] = round_key[i - 1][j];
    }

    if (i % Nk == 0) {
      rot_word(tmp);
      sub_word(tmp);
      tmp[0] ^= rcon[(i / Nk)];
    }

    for (size_t j = 0; j < Nb; j++) {
      round_key[i][j] = round_key[i - Nk][j] ^ tmp[j];
    }
  }
}

void print_round_key(uint8_t round_key[Nb * (Nr + 1)][Nb]) {
  if (!round_key)
    return;

  for (size_t i = 0; i < Nb * (Nr + 1); i++) {
    for (size_t j = 0; j < Nb; j++) {
      printf("%02x ", round_key[i][j]);
    }
    printf("\n");
  }
}

void convert_to_block(uint8_t *input, block_t *block) {
  if (!input || !block)
    return;

  int k = 0;
  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      block_set(block, input[k], j, i);
      k++;
    }
  }
}

void key_to_block(uint8_t *key, block_t *block) {
  if (!key || !block)
    return;

  int k = 0;
  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      block_set(block, key[k], i, j);
      k++;
    }
  }
}

void print_block(block_t *block) {
  if (!block)
    return;

  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      printf("%hhx ", block_get(block, i, j));
    }
    printf("\n");
  }
}

void convert_to_output(block_t *block, uint8_t *output) {
  if (!block || !output)
    return;

  int k = 0;
  for (size_t i = 0; i < Nb; i++) {
    for (size_t j = 0; j < Nb; j++) {
      output[k] = block_get(block, j, i);
      k++;
    }
  }
}

void print_data(uint8_t *data, size_t size) {
  if (!data)
    return;

  for (size_t i = 0; i < size; i++) {
    printf("%hhx ", data[i]);
  }
}

void read_in_stdout(char *hex, uint8_t data[16]) {
  for (int i = 0; i < 16; i++) {
    sscanf(hex + 2 * i, "%2hhx", &data[i]);
  }
}

// void cipher(block_t *input, uint8_t round_key[Nb * (Nr + 1)][Nb]) {
//   if (!input || !round_key)
//     return;
//   block_t *key;
//   key = block_alloc(Nb);
//   get_key(round_key, key, 0);
//   add_round_key(input, key);
//   block_free(key);

//   /*All Nr rounds are identical with the exception of the final round,
//   which does not include the MixColumns()transformation */
//   for (size_t round = 1; round < Nr; round++) {
//     sub_bytes(input);
//     shift_rows(input);
//     mix_columns(input);
//     key = block_alloc(Nb);
//     get_key(round_key, key, round);

//     add_round_key(input, key);
//     block_free(key);
//   }
//   sub_bytes(input);
//   shift_rows(input);
//   block_t *last_key = block_alloc(Nb);
//   get_key(round_key, last_key, Nr);
//   add_round_key(input, last_key);
//   block_free(last_key);
// }
