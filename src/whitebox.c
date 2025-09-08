#include "aes.h"
#include "tables.h"

#include <stdint.h>
#include <stdlib.h>

#include <string.h>

#define NB_ROUNDS 10
#define BLOCK_SIZE 16
#define POSSIBLE_VALUES 256
#define TABLE_NUMBER 4

extern uint8_t final_mask;
static void
calculate_t_boxes (block_t *state, int round)
{
  for (int i = 0; i < Nb; i++)
    {
      for (int j = 0; j < Nb; j++)
        {
          uint8_t tmp = block_get (state, i, j);
          if (round == 9){
            block_set (state, (t_boxes[round][(i * Nb) + j][tmp]) ^ final_mask, i, j);
          }
          else {
            block_set (state, t_boxes[round][(i * Nb) + j][tmp], i, j);
          }
          
        }
    }
}

static void
calculate_tyi_tables_and_xor_tables (block_t *input, int round)
{
  uint32_t mixed_column;
  uint8_t aa, bb, cc, dd;
  for (int j = 0; j < Nb; j++)
    {
      uint32_t a = tyi_tables[round][j][0][block_get (input, 0, j)];
      uint32_t b = tyi_tables[round][j][1][block_get (input, 1, j)];
      uint32_t c = tyi_tables[round][j][2][block_get (input, 2, j)];
      uint32_t d = tyi_tables[round][j][3][block_get (input, 3, j)];
      for (int i = 0; i < Nb; i++)
        {
          int shift1 = 28 - 8 * i;
          int shift2 = shift1 - 4;
          int index = 24 * j + 6 * i;
          aa = xor_tables[round][index + 0][(a >> shift1) & 0xf]
                         [(b >> shift1) & 0xf];
          bb = xor_tables[round][index + 1][(c >> shift1) & 0xf]
                         [(d >> shift1) & 0xf];
          cc = xor_tables[round][index + 2][(a >> shift2) & 0xf]
                         [(b >> shift2) & 0xf];
          dd = xor_tables[round][index + 3][(c >> shift2) & 0xf]
                         [(d >> shift2) & 0xf];
          mixed_column = (xor_tables[round][index + 4][aa][bb] << 4)
                         | xor_tables[round][index + 5][cc][dd];
          block_set (input, mixed_column, i, j);
        }
    }
}

void
cipher (block_t *input)
{
  if (!input)
    return;

  printf ("input:\n");
  print_block (input);
  for (size_t round = 0; round < Nr - 1; round++)
    {
      printf ("round %zu\n", round);
      printf ("**** After shift_rows****\n");
      shift_rows (input);
      print_block (input);
      printf ("*****After t_box ****\n");
      calculate_t_boxes (input, round);
      print_block (input);
      printf ("*****After t_y ****\n");
      calculate_tyi_tables_and_xor_tables (input, round);
      print_block (input);
    }
  printf ("*****round 10 after shift_rows ****\n");
  shift_rows (input);
  print_block (input);
  printf ("*****round 10 after t_box ****\n");
  calculate_t_boxes (input, Nr - 1);
  print_block (input);
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stdout, "Usage : ./whitebox <message>\n");
      return EXIT_SUCCESS;
    }
  uint8_t input_data[16];
  block_t *input = block_alloc (Nb);
  read_in_stdout (argv[1], input_data);
  convert_to_block (input_data, input);
  cipher (input);
  printf ("result\n");
  print_block (input);
  block_free (input);
  return EXIT_SUCCESS;
}