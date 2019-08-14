#ifndef INCLUDES_CHUNK_H
#define INCLUDES_CHUNK_H

#include "common.h"
#include "value.h"

/*
 * Opcode enum
 */
typedef enum {
	OP_CONSTANT,
        OP_RETURN,
} OpCode;

/*
 * dynamic array wrapper
 * 
 * The int* lines is to store line numbers for bytecode, braindead but oh well
 */
typedef struct {
        int count;
        int capacity;
        uint8_t* code;
        int* lines;
	ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);

int addConstant(Chunk* chunk, Value value);

#endif
