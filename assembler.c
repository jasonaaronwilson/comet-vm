/**
 * @file assembler.c
 *
 * This is an interface for compiling a single instruction statement
 * to "memory" or updating the symbol table for a label.
 *
 * The file will likely be replaced with a pure scheme based assembler
 * with a more consistent syntax (folks seem to have gotten over
 * parenthesize syntax for assembly because of WARM...)
 */

#include <stdio.h>
#include <string.h>

#include "array.h"
#include "assembler.h"
#include "fatal-error.h"
#include "instruction-info.h"
#include "opcodes.h"
#include "string-util.h"
#include "tokenizer.h"
#include "uleb128.h"

assembly_result_t make_assembly_result(symbol_table_t* symbols,
                                       uint64_t address);

uint64_t parse_imm_argument(symbol_table_t* symbols, char* str);
uint64_t parse_gr_argument(char* str);
uint64_t parse_fp_argument(char* str);

uint64_t parse_argument(uint8_t type, symbol_table_t* symbols, char* str) {
  switch (type) {
  case ARG_TYPE_GR:
    return parse_gr_argument(str);

  case ARG_TYPE_FP:
    return parse_fp_argument(str);

  case ARG_TYPE_IMM:
    return parse_imm_argument(symbols, str);
  }
  // TODO(jawilson): error!
  return 0;
}

// TODO(jawilson): max passes (and also print out the number of passes).
assembly_result_t assemble_statements(paged_memory_t* memory, uint64_t address,
                                      symbol_table_t* symbols,
                                      array_t* statements) {
  uint64_t start_address = address;

  // This is similar to branch tensioning in other assemblers. The
  // first pass gets symbols close to their final addresses. A second
  // pass is always performed unless there are no labels (which isn't
  // obvious but all symbols are dirty even if all labels are only
  // backward referenced and therefore we know their address when they
  // are used). If code grows enough in the second pass, then some
  // labels might move enough to make another pass necesary, etc.
  while (1) {
    address = start_address;
    for (int i = 0; i < array_length(statements); i++) {
      assembly_result_t result = assemble(memory, address, symbols,
                                          (char*) array_get(statements, i));
      address = result.address_end;
      symbols = result.symbols;
    }
    if (!is_dirty(symbols)) {
      break;
    }
  }

  assembly_result_t result = make_assembly_result(symbols, start_address);
  result.address_end = address;
  return result;
}

assembly_result_t assemble(paged_memory_t* memory, uint64_t address,
                           symbol_table_t* symbols, char* statement) {

  assembly_result_t result = make_assembly_result(symbols, address);

  // Comments
  if (string_starts_with(statement, "#")) {
    return result;
  }

  token_list_t* tokens = tokenize(statement, " ,\n");

  if (tokens == NULL) {
    return result;
  }

  char* opcode = token_list_get(tokens, 0);

  if (string_starts_with(opcode, ".")) {
    if (string_equal(opcode, ".align")) {
      char* amount_str = token_list_get(tokens, 1);
      uint64_t amount = string_parse_uint64(amount_str);
      while ((address % amount) != 0) {
        encodeULEB128(memory, address++, NOP);
      }
      result.address_end = address;
      return result;
    }
    fatal_error(ERROR_UNKNOWN_ASSEMBLER_DIRECTIVE);
  }

  if (string_ends_with(opcode, ":")) {
    char* label_name = string_substring(opcode, 0, strlen(opcode) - 1);
    result.symbols = add_symbol(symbols, label_name, address);
    free(label_name);
    return result;
  }

  instruction_info_t* info = find_instruction_info_by_name(opcode);
  if (info == NULL) {
    fprintf(stderr, "WARNING: opcode not found '%s'\n", opcode);
    return result;
  }

  address += encodeULEB128(memory, address, info->opcode_value);

  if (info->number_of_arguments >= 1) {
    uint64_t value
        = parse_argument(info->arg0_type, symbols, token_list_get(tokens, 1));
    address += encodeULEB128(memory, address, value);
  }
  if (info->number_of_arguments >= 2) {
    uint64_t value
        = parse_argument(info->arg1_type, symbols, token_list_get(tokens, 2));
    address += encodeULEB128(memory, address, value);
  }
  if (info->number_of_arguments >= 3) {
    uint64_t value
        = parse_argument(info->arg2_type, symbols, token_list_get(tokens, 3));
    address += encodeULEB128(memory, address, value);
  }

  result.address_end = address;
  return result;
}

assembly_result_t make_assembly_result(symbol_table_t* symbols,
                                       uint64_t address) {
  assembly_result_t empty_statement_result;
  empty_statement_result.address_start = address;
  empty_statement_result.address_end = address;
  empty_statement_result.symbols = symbols;
  return empty_statement_result;
}

uint64_t parse_imm_argument(symbol_table_t* symbols, char* str) {
  if (str[0] >= '0' && str[0] <= '9') {
    return string_parse_uint64(str);
  }
  symbol_t* sym = find_symbol_by_name(symbols, str);
  if (sym) {
    return sym->value;
  } else {
    return 0;
  }
}

uint64_t parse_gr_argument(char* str) {
  if (!string_starts_with(str, "r")) {
    fatal_error(ERROR_EXPECTED_GENERAL_REGISTER);
  }
  return string_parse_uint64(&str[1]);
}

uint64_t parse_fp_argument(char* str) {
  if (!string_starts_with(str, "f")) {
    fatal_error(ERROR_EXPECTED_FLOATING_REGISTER);
  }
  return string_parse_uint64(&str[1]);
}
