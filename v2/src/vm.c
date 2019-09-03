#include <stdio.h>
#include <stdarg.h>

#include "../includes/common.h"
#include "../includes/vm.h"
#include "../includes/debug.h"
#include "../includes/compiler.h"

VM vm;

static void resetStack() {
        vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fputs("\n", stderr);

        size_t instruction = vm.ip - vm.chunk->code;
        int line = vm.chunk->lines[instruction];
        fprintf(stderr, "[line %d] in script\n", line);

        resetStack();
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

static Value peek(int distance) {
        return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value) {
        return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
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
#define BINARY_OP(valueType, op) \
        do {\
                if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
                        runtimeError("Operands must be numbers."); \
                        return INTERPRET_RUNTIME_ERROR;\
                } \
                \
                double b = AS_NUMBER(pop()); \
                double a = AS_NUMBER(pop()); \
                push(valueType(a op b)); \
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
                disassembleInstruction(vm.chunk, 
                                       (int)(vm.ip - vm.chunk->code)
                                      );
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
                        case OP_NIL: push(NIL_VAL); break;
                        case OP_TRUE: push(BOOL_VAL(true)); break;
                        case OP_FALSE: push(BOOL_VAL(false)); break;
                        case OP_ADD:      BINARY_OP(NUMBER_VAL, +); break;
                        case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
                        case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
                        case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
                        case OP_NOT:
                                push(BOOL_VAL(isFalsey(pop())));
                                break;
                        case OP_NEGATE: {
                                if (!IS_NUMBER(peek(0))) {
                                        runtimeError(
                                                "Operand must be a number."
                                                );
                                        return INTERPRET_RUNTIME_ERROR;
                                }

                                push(NUMBER_VAL(-AS_NUMBER(pop())));
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
        // Chunk initialization
        Chunk chunk;
        initChunk(&chunk);

        // Compile & error check
        if (!compile(source, &chunk)) {
                freeChunk(&chunk);
                return INTERPRET_COMPILE_ERROR;
        }

        // Load chunk
        vm.chunk = &chunk;
        vm.ip = vm.chunk->code;

        // Run chunk
        InterpretResult result = run();

        // Spin down
        freeChunk(&chunk);
        return result;
}