#include <stdlib.h>

#include "../includes/chunk.h"
#include "../includes/memory.h"

/*
init chunk to default vals
*/
void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
}

/*
free a chunk from memory
*/
void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  initChunk(chunk);
}

/*
write some data into chunk
*/
void writeChunk(Chunk* chunk, uint8_t byte) {
  if (chunk->capacity < chunk->count + 1) {
    int old_capacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(old_capacity);
    chunk->code = GROW_ARRAY(chunk->code, uint8_t, 
                              old_capacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->count++;
}