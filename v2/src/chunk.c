#include <stdlib.h>

#include "../includes/chunk.h"
#include "../includes/memory.h"

/*
 * init chunk to default vals
 */
void initChunk(Chunk* chunk) {
        chunk->count = 0;
        chunk->capacity = 0;
        chunk->code = NULL;
        chunk->lines = NULL;
	initValueArray(&chunk->constants);
}

/*
 * free a chunk from memory
 */
void freeChunk(Chunk* chunk) {
        FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
        FREE_ARRAY(int, chunk->lines, chunk->capacity);
	/*
	 * here we call init to reset the values to 0 and NULL
	 */
        initChunk(chunk);
}

/*
 * write some data into chunk
 */
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
        if (chunk->capacity < chunk->count + 1) {
                int old_capacity = chunk->capacity;
                chunk->capacity = GROW_CAPACITY(old_capacity);
                chunk->code = GROW_ARRAY(chunk->code, uint8_t, 
                                old_capacity, chunk->capacity);
                chunk->lines = GROW_ARRAY(chunk->lines, int,
                                old_capacity, chunk->capacity);
        }

        chunk->code[chunk->count] = byte;
        chunk->lines[chunk->count] = line;
        chunk->count++;
}

/*
 * add a constant to contsants, and return the index where it was appended
 */
int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}
