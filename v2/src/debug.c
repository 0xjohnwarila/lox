#include <stdio.h>

#include "../includes/debug.h"

void disassembleChunk(Chunk* chunk, const char* name) {
	printf("== %s ==\n", name);

	for (int offset = 0; offset < chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

static int constantInstr(const char* name, Chunk* chunk, int offset) {
	uint8_t constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");

	return offset + 2;
}


static int simpleInstr(const char* name, int offset) {
       printf("%s\n", name);
       return offset + 1;
}

/*
 * print the offset of instruction, read a single byte from bytecode
 * display opcode
 */

int disassembleInstruction(Chunk* chunk, int offset) {
	printf("%04d ", offset);

	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
		printf("   | ");
	}
	else {
		printf("%4d ", chunk->lines[offset]);
	}

	uint8_t instruction = chunk->code[offset];
	switch (instruction) {
		case OP_CONSTANT:
			return constantInstr("OP_CONSTANT", chunk, offset);
		case OP_NIL:
                        return simpleInstr("OP_NIL", offset);
                case OP_TRUE:
                        return simpleInstr("OP_TRUE", offset);
                case OP_FALSE:
                        return simpleInstr("OP_FALSE", offset);
		case OP_ADD:
			return simpleInstr("OP_ADD", offset);
		case OP_SUBTRACT:
			return simpleInstr("OP_SUBTRACT", offset);
		case OP_MULTIPLY:
			return simpleInstr("OP_MULTIPLY", offset);
		case OP_DIVIDE:
			return simpleInstr("OP_DIVIDE", offset);
                case OP_NOT:
                        return simpleInstr("OP_NOT", offset);
		case OP_NEGATE:
			return simpleInstr("OP_NEGATE", offset);
		case OP_RETURN:
			return simpleInstr("OP_RETURN", offset);
		default:
			printf("Unkown opcode %d\n", instruction);
			return offset + 1;
	}
}

