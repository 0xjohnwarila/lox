#ifndef INCLUDES_MEMORY_H
#define INCLUDES_MEMORY_H

/*
if capacity is less than 8, set capacity to 8
else grow by factor of 2
*/
#define GROW_CAPACITY(capacity) \
  ((capacity) < 8 ? 8 : (capacity) * 2)

/*
get size of array element type, cast void* back to pointer
of correct type
*/
#define GROW_ARRAY(previous, type, old_count, count) \
  (type*)reallocate(previous, sizeof(type) * (old_count), \
    sizeof(type) * (count))

/*
call reallocate with 0 for new size, freeing the array
*/
#define FREE_ARRAY(type, pointer, old_count) \
  reallocate(pointer, sizeof(type) * (old_count), 0)

void* reallocate(void* previous, size_t old_size, size_t new_size);

#endif