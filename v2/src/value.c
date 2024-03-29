#include <stdio.h>

#include "../includes/memory.h"
#include "../includes/value.h"

void initValueArray(ValueArray* array) {
	array->values = NULL;
	array->capacity = 0;
	array->count = 0;
}

void freeValueArray(ValueArray* array) {
	FREE_ARRAY(Value, array->values, array->capacity);
	initValueArray(array);
}

void writeValueArray(ValueArray* array, Value value) {
	if (array->capacity < array->count + 1) {
		int old_capacity = array->capacity;
		array->capacity = GROW_CAPACITY(old_capacity);
		array->values = GROW_ARRAY(array->values, Value, old_capacity, array->capacity);
	}

	array->values[array->count] = value;
	array->count++;
}

void printValue(Value value) {
	switch (value.type) {
		case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
		case VAL_NIL:    printf("nil"); break;
		case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
	}
}
