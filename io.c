/**
 * @file io.c
 *
 * This contains routines to read the contents of a file or write a
 * new file.
 */

#include <stdio.h>

#include "byte-array.h"
#include "io.h"

byte_array_t* byte_array_append_file_contents(byte_array_t* bytes,
                                              char* file_name) {
  FILE* file = fopen(file_name, "r");
  uint8_t buffer[1024];

  while (1) {
    uint64_t n_read = fread(buffer, 1, sizeof(buffer), file);
    if (n_read == 0) {
      break;
    }
    bytes = byte_array_append_bytes(bytes, buffer, n_read);
  }

  fclose(file);

  return bytes;
}

// TODO(jawilson): implement
void byte_array_write_file(byte_array_t* bytes, char* file_name) {}
