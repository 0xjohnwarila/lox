#include <stdio.h>

#include "../includes/common.h"
#include "../includes/vm.h"
#include "../includes/debug.h"
#include "../includes/compiler.h"

VM vm;

static void resetStack() {
        vm.stackTop = vm.stack;
}

void initVM() {
        resetStack();
}

void freeVM() {

}

void push(Value value) {
        *vm.stackTop = value;
        vm.stackTop++;
}

Value pop() {
        vm.stackTop--;
        return *vm.stackTop;
}

static InterpretResult run() {
// READ_BYTE: read the next byte in and increment the IP
#define READ_BYTE() (*vm.ip++)
// READ_CONSTANT: read the next byte, treat it like an index number and look up
// constant in the chunk's constant table
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

// Kind of a hack, but reduces complexity when typechecking
// The weird do while(false) is to ensure that all expressions are in the same 
// scope when the binary is called. Also who knew you could pass operators as an
// argument to macros??
#define BINARY_OP(op) \
        do {\
                double b = pop();\
                double a = pop();\
                push(a op b); \
        } while (false)


        for (;;) {
// Diagnostic Logging
#ifdef DEBUG_TRACE_EXECUTION
                printf("          ");
                // Stack logging
                for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                        printf("[ ");
                        printValue(*slot);
                        printf(" ]");
                }
                printf("\n");
                // Instruction logging
                disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

                uint8_t instruction;
                switch(instruction = READ_BYTE()) {
                        case OP_CONSTANT: {
                                // Read constant
                                Value constant = READ_CONSTANT();
                                // Load constant
                                push(constant);
                                break;
                        }
                        case OP_ADD:      BINARY_OP(+); break;
                        case OP_SUBTRACT: BINARY_OP(-); break;
                        case OP_MULTIPLY: BINARY_OP(*); break;
                        case OP_DIVIDE:   BINARY_OP(/); break;
                        case OP_NEGATE: {
                                push(-pop());
                                break;
                        }
                        case OP_RETURN: {
                                printValue(pop());
                                printf("\n");
                                return INTERPRET_OK;
                        }
                }
        }
// This is not explicitly needed, but keeps the macros from creeping out
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
        compile(source);
        return INTERPRET_OK;
}