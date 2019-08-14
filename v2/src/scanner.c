#include <stdio.h>
#include <string.h>

#include "../includes/common.h"
#include "../includes/scanner.h"

// Evil Global Var
Scanner scanner;

void initScanner(const char* source) {
        scanner.start = source;
        scanner.current = source;
        scanner.line = 1;
}