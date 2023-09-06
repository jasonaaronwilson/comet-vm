#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "cpu-thread-state.h"
#include <stdint.h>

extern uint64_t print_instructions(const uint8_t *memory, uint64_t address,
                                   uint64_t number_of_instructions);

extern void print_registers(cpu_thread_state *state, int num_gr_registers,
                            int num_fp_registers);

#endif /* _PRINTER_H_ */