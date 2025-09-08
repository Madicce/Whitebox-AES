#include "main.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "aes.h"

#define NB_ROUNDS 10
#define BLOCK_SIZE 16
#define POSSIBLE_VALUES 256
#define POSSIBLE_NIBBLES_VALUES 16
#define NB_COPY_XOR_TABLES 96
#define TABLE_NUMBER 4

uint8_t t_boxes_data[NB_ROUNDS][BLOCK_SIZE][POSSIBLE_VALUES] = {0};
uint32_t tyi_tables_data[NB_ROUNDS - 1][TABLE_NUMBER][TABLE_NUMBER]
                        [POSSIBLE_VALUES] = {0};
uint8_t xor_tables_data[NB_ROUNDS - 1][NB_COPY_XOR_TABLES]
                       [POSSIBLE_NIBBLES_VALUES][POSSIBLE_NIBBLES_VALUES] = {0};

uint8_t final_mask;
uint8_t mask[NB_ROUNDS];

static void print_tables(FILE *fd) {
  if (!fd) {
    exit(EXIT_FAILURE);
  }
  fprintf(fd, "uint8_t final_mask = 0x%.2x; \n", final_mask);
  //  t_boxes
  fprintf(fd, "uint8_t t_boxes[%d][%d][%d] = {\n", NB_ROUNDS, BLOCK_SIZE,
          POSSIBLE_VALUES);
  for (int r = 0; r < NB_ROUNDS; r++) {
    fprintf(fd, "\t{\n");
    for (int i = 0; i < BLOCK_SIZE; i++) {
      fprintf(fd, "\t\t{");
      for (int x = 0; x < POSSIBLE_VALUES; x++) {
        fprintf(fd, "0x%.2x%s", t_boxes_data[r][i][x],
                (x != POSSIBLE_VALUES - 1) ? ", " : "");
        if (x % 8 == 7)
          fprintf(fd, "\n\t\t ");
      }
      fprintf(fd, "}%s\n", (i != BLOCK_SIZE - 1) ? "," : "");
    }
    fprintf(fd, "\t}%s\n", (r != NB_ROUNDS - 1) ? "," : "");
  }
  fprintf(fd, "};\n\n");

  // tyi_tables
  fprintf(fd, "uint32_t tyi_tables[%d][%d][%d][%d]= {\n", NB_ROUNDS - 1,
          TABLE_NUMBER, TABLE_NUMBER, POSSIBLE_VALUES);
  for (int r = 0; r < NB_ROUNDS - 1; r++) {
    fprintf(fd, "\t{\n");
    for (int byte = 0; byte < TABLE_NUMBER; byte++) {
      fprintf(fd, "\t\t{\n");
      for (int table = 0; table < TABLE_NUMBER; table++) {
        fprintf(fd, "\t\t\t{");
        for (int x = 0; x < POSSIBLE_VALUES; x++) {
          fprintf(fd, "0x%08X%s", tyi_tables_data[r][byte][table][x],
                  (x != POSSIBLE_VALUES - 1) ? ", " : "");
          if (x % 8 == 7)
            fprintf(fd, "\n\t\t\t ");
        }
        fprintf(fd, "}%s\n", (table != TABLE_NUMBER - 1) ? "," : "");
      }
      fprintf(fd, "\t\t}%s\n", (byte != BLOCK_SIZE - 1) ? "," : "");
    }
    fprintf(fd, "\t}%s\n", (r != NB_ROUNDS - 1) ? "," : "");
  }

  fprintf(fd, "};\n\n");

  // xor_tables
  fprintf(fd, "uint8_t xor_tables[%d][%d][%d][%d] = {\n", NB_ROUNDS - 1,
          NB_COPY_XOR_TABLES, POSSIBLE_NIBBLES_VALUES, POSSIBLE_NIBBLES_VALUES);
  for (int r = 0; r < NB_ROUNDS - 1; r++) {
    fprintf(fd, "\t{\n");
    for (int i = 0; i < NB_COPY_XOR_TABLES; i++) {
      fprintf(fd, "\t\t{\n");
      for (int x = 0; x < POSSIBLE_NIBBLES_VALUES; x++) {
        fprintf(fd, "\t\t\t{");
        for (int y = 0; y < POSSIBLE_NIBBLES_VALUES; y++) {
          fprintf(fd, "0x%.2x%s", xor_tables_data[r][i][x][y],
                  (y != POSSIBLE_NIBBLES_VALUES - 1) ? ", " : "");
        }
        fprintf(fd, "}%s\n", (x != POSSIBLE_NIBBLES_VALUES - 1) ? "," : "");
      }
      fprintf(fd, "\t\t}%s\n", (i != NB_COPY_XOR_TABLES - 1) ? "," : "");
    }
    fprintf(fd, "\t}%s\n", (r != NB_ROUNDS - 2) ? "," : "");
  }
  fprintf(fd, "};\n\n");
}

static void prng_init(unsigned int seed) {
  static bool seed_initialized = false;
  if (!seed_initialized) {
    srand(seed);
    seed_initialized = true;
  }
}

void generate_random_mask(uint8_t mask[NB_ROUNDS]) {
  for (int i = 0; i < NB_ROUNDS; i++) {
    prng_init((time(NULL) * getpid()) % INT_MAX);
    mask[i] = rand() % 256;
  }
}

static void generate_t_boxes(uint8_t round_key[Nb * (Nr + 1)][Nb]) {
  final_mask = mask[9];
  // de 1 à 9
  for (int r = 0; r < NB_ROUNDS - 1; r++) {
    block_t *key = block_alloc(Nb);
    // kr-1
    get_key(round_key, key, r);
    shift_rows(key);
    for (int i = 0; i < BLOCK_SIZE; i++) {
      for (int x = 0; x < POSSIBLE_VALUES; x++) {
        t_boxes_data[r][i][x] =
            s_box[x ^ block_get(key, i / Nb, i % Nb)] ^ mask[r];
      }
    }

    block_free(key);
  }

  // k9 et k10
  block_t *key_round_9 = block_alloc(Nb);
  block_t *key_round_10 = block_alloc(Nb);
  get_key(round_key, key_round_9, Nr - 1);
  get_key(round_key, key_round_10, Nr);
  shift_rows(key_round_9);
  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int x = 0; x < POSSIBLE_VALUES; x++) {
      t_boxes_data[NB_ROUNDS - 1][i][x] =
          s_box[x ^ block_get(key_round_9, i / Nb, i % Nb)] ^
          block_get(key_round_10, i / Nb, i % Nb) ^ mask[9];
    }
  }

  block_free(key_round_9);
  block_free(key_round_10);
}

static void generate_tyi_tables() {
  for (int r = 0; r < NB_ROUNDS - 1; r++) {
    for (int byte = 0; byte < TABLE_NUMBER; byte++) {
      for (int x = 0; x < POSSIBLE_VALUES; x++) {
        tyi_tables_data[r][byte][0][x ^ mask[r]] =
            (((gf_mul(2, x) << 24) | (x << 16) | (x << 8) | gf_mul(3, x))) ^
            mask[r + 1];
        tyi_tables_data[r][byte][1][x ^ mask[r]] =
            ((gf_mul(3, x) << 24) | (gf_mul(2, x) << 16) | (x << 8) | x) ^
            mask[r + 1];
        tyi_tables_data[r][byte][2][x ^ mask[r]] =
            ((x << 24) | (gf_mul(3, x) << 16) | (gf_mul(2, x) << 8) | x) ^
            mask[r + 1];
        tyi_tables_data[r][byte][3][x ^ mask[r]] =
            ((x << 24) | (x << 16) | (gf_mul(3, x) << 8) | gf_mul(2, x)) ^
            mask[r + 1];
      }
    }
  }
}

static void generate_xor_tables() {
  for (int r = 0; r < NB_ROUNDS - 1; r++) {
    for (int i = 0; i < NB_COPY_XOR_TABLES; i++) {
      for (int x = 0; x < POSSIBLE_NIBBLES_VALUES; x++) {
        for (int y = 0; y < POSSIBLE_NIBBLES_VALUES; y++) {
          xor_tables_data[r][i][x][y] = x ^ y;
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage : ./generate_whitebox <key>\n");
    return EXIT_SUCCESS;
  }

  FILE *fd = fopen("src/tables.h", "w");
  if (!fd) {
    fprintf(stderr, "Error : opening file !\n");
    exit(EXIT_FAILURE);
  }

  fprintf(fd, "#ifndef TABLES_H\n#define TABLES_H\n\n#include "
              "<stdint.h>\n#include <stdio.h>\n\n");
  uint8_t key_data[BLOCK_SIZE];
  read_in_stdout(argv[1], key_data);
  uint8_t round_key[Nb * (Nr + 1)][Nb];
  block_t *key = block_alloc(Nb);
  key_to_block(key_data, key);
  key_expansion(round_key, key);
  generate_random_mask(mask);
  generate_t_boxes(round_key);
  generate_xor_tables();
  generate_tyi_tables();
  print_tables(fd);
  fprintf(fd, "#endif /* TABLES_H */");
  fclose(fd);
  int result = system("make build_whitebox");
  if (result != 0) {
    fprintf(stdout, "Error : generate whitebox !\n");
    exit(EXIT_FAILURE);
  }
  block_free(key);
  return EXIT_SUCCESS;
}
