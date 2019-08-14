#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/common.h"
#include "../includes/chunk.h"
#include "../includes/debug.h"
#include "../includes/vm.h"

// Command line interpreter
static void repl() {
        char line[1024];
        for (;;) {
                printf("> ");

                if (!fgets(line, sizeof(line), stdin)) {
                        printf("\n");
                        break;
                }

                interpret(line);
        }
}

static char* readFile(const char* path) {
        // Open file
        FILE* file = fopen(path, "rb");
        // Check if the file opened properly
        if (file = NULL) {
                fprintf(stderr, "Could not open file \"%s\".\n", path);
                exit(74);
        }

        // Get file size
        fseek(file, 0L, SEEK_END);
        size_t file_size = ftell(file);
        rewind(file);

        // Allocate a buffer the size of the file + 1
        char* buffer = (char*)malloc(file_size + 1);
        // Check if the file is too large to fit in memory
        if (buffer == NULL) {
                fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
                exit(74);
        }

        // Read the entire file into the buffer
        size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
        // Check if the read failed
        if (bytes_read < file_size) {
                fprintf(stderr, "Could not read file \"%s\".\n", path);
                exit(74);
        }
        
        // Write the null character to the end of the buffer
        buffer[bytes_read] = '\0';

        fclose(file);
        return buffer;
}

// File based interpreter
static void runFile(const char* path) {
        char* source = readFile(path);
        InterpretResult result = interpret(source);
        free(source);

        if (result == INTERPRET_COMPILE_ERROR) exit(65);
        if (result == INTERPRET_RUNTIME_ERROR) exit(70); 
}

int main(int argc, const char* argv[]) {
        // Spin up VM
        initVM();
        
        // Spin up Interpreter
        if (argc == 1) {
                repl();
        }
        else if (argc == 2) {
                runFile(argv[1]);
        }
        else {
                fprintf(stderr, "Usage: ./main [path]\n");
                exit(64);
        }

        // Wind down VM
        freeVM();
        return 0;
}
