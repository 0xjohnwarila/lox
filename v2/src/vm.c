#include <stdio.h>

#include "../includes/common.h"
#include "../includes/vm.h"
#include "../includes/debug.h"

VM vm;

void initVM() {

}

void freeVM() {

}

static InterpretResult run() {
// READ_BYTE: read the next byte in and increment the IP
#define READ_BYTE() (*vm.ip++)
// READ_CONSTANT: read the next byte, treat it like an index number and look up
// constant in the chunk's constant table
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

        for (;;) {
// Diagnostic Logging
#ifdef DEBUG_TRACE_EXECUTION
                disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

                uint8_t instruction;
                switch(instruction = READ_BYTE()) {
                        case OP_CONSTANT: {
                                Value constant = READ_CONSTANT();
                                printValue(constant);
                                printf("\n");
                                break;
                        }
                        case OP_RETURN: {
                                return INTERPRET_OK;
                        }
                }
        }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk* chunk) {
        // Set VM chunk
        vm.chunk = chunk;
        // Set VM instruction pointer
        vm.ip = vm.chunk->code;

        return run();
}