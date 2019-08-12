#ifndef INCLUDES_CHUNK_H
#define INCLUDES_CHUNK_H

#include "common.h"

/*
Opcode enum
*/
typedef enum {
  OP_RETURN,
} OpCode;

/*
dynamic array wrapper
*/
typedef struct {
  int count;
  int capacity;
  uint8_t* code;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);


#endif
