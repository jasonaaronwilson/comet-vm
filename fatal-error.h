#ifndef _FATAL_ERROR_
#define _FATAL_ERROR_

#define ERROR_UKNOWN 0
#define ERROR_MEMORY_ALLOCATION 1
#define ERROR_MEMORY_READ 2
#define ERROR_MEMORY_WRITE 3
#define ERROR_OPCODE_UNKNOWN 4
#define ERROR_OPCODE_DECODE_ERROR 5
#define ERROR_ARG_DECODE_ERROR 6
#define ERROR_UNIMLEMENTED_OPCODE 7
#define ERROR_TOKEN_LIST_GET 8
#define ERROR_EXPECTED_GENERAL_REGISTER 9
#define ERROR_EXPECTED_FLOATING_REGISTER 10
#define ERROR_ARRAY_ACCESS_OUT_OF_BOUNDS 11
#define ERROR_UNKNOWN_ASSEMBLER_DIRECTIVE 12
#define ERROR_DEBUGGER_EXPECT_FAILURE 13
#define ERROR_ILLEGAL_LIST_INDEX 14
#define ERROR_CANT_EVAL_EMPTY_EXPRESSION 15
#define ERROR_VARIABLE_NOT_FOUND 16
#define ERROR_REFERENCE_NOT_EXPECTED_TYPE 17
#define ERROR_NOT_REACHED 18
#define ERROR_MAX_PRIMITIVE_ARGS 19
#define ERROR_WRONG_NUMBER_OF_ARGS 20
#define ERROR_CLOSURE_HAS_NO_BODY 21

extern _Noreturn void fatal_error_impl(char* file, int line, int error_code);
extern const char* fatal_error_code_to_string(int error_code);

#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#endif /* _FATAL_ERROR_ */
