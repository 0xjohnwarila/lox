#ifndef VALUE_H
#define VALUE_H

#include "common.h"

typedef enum {
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER,
} ValueType;

typedef struct {
	Value type;
	union as
	{
		bool boolean;
		double number;
	};
} Value;

/*
 * Type conversion macros
 * 
 *       ---WARNING---
 * These can be really dangerous!
 * Make sure to only convert to
 * and from correct types!
 * 
 *
 */

// Type checking
#define IS_BOOL(value)    ((Value).type == VAL_BOOL))
#define IS_NIL(value)     ((Value).type == VAL_NIL))
#define IS_NUMBER(value)  ((Value).type == VAL_NUMBER)

// Convert from Value type to C type

#define AS_BOOL(value)    ((Value).as.boolean)
#define AS_NUMBER(value)  ((Value).as.number)

// Convert from C type to Value type
#define BOOL_VAL(value)   ((Value){ VAL_BOOL, { .boolean = value } })
#define NIL_VAL           ((Value){ VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(value) ((Value){ VAL_NUMBER, { .number = value } })


typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void freeValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void printValue(Value value);

#endif
