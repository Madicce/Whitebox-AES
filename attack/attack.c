#include "../src/aes.h"
#include "../src/tables.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool found = false;
uint8_t mask0;
void extract_mask(void) {
  uint8_t mask_r_plus_1 = final_mask;
  uint8_t mask_r;
  printf("Mask 9 : 0x%02x\n", final_mask);

  for (int r = 8; r >= 0; r--) {
    found = false;
    for (uint8_t x = 0; x < 256; x++) {
      uint32_t expected =
          (gf_mul(2, x) << 24) | (x << 16) | (x << 8) | gf_mul(3, x);
      expected ^= mask_r_plus_1;
      for (uint16_t i = 0; i < 256; i++) {
        if (tyi_tables[r][0][0][i] == expected) {
          mask_r = x ^ i;
          printf("Mask %d : 0x%02x\n", r, mask_r);
          found = true;
          break;
        }
      }
      if (found) {
        break;
      }
    }
    mask_r_plus_1 = mask_r;
  }
  mask0 = mask_r;
  return;
}

void extract_key(block_t *key) {
  for (int i = 0; i < 16; i++) {
    uint8_t sbox_val = t_boxes[0][i][0] ^ mask0;
    uint8_t key_byte = inv_s_box[sbox_val];
    block_set(key, key_byte, i / 4, i % 4);
  }
}

int main(int argc, char *argv[]) {
  extract_mask();
  block_t *key = block_alloc(Nb);
  extract_key(key);
  printf("key found : \n");
  inverse_shift_rows(key);
  print_block(key);
  block_free(key);
  return EXIT_SUCCESS;
}