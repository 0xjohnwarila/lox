#include "../includes/common.h"
#include "../includes/chunk.h"
#include "../includes/debug.h"
#include "../includes/vm.h"

int main(int argc, const char* argv[]) {
        // Spin up VM
        initVM();
        Chunk chunk;
        initChunk(&chunk);

        // Hand built Chunk
	int constant = addConstant(&chunk, 1.2);
	writeChunk(&chunk, OP_CONSTANT, 123);
        writeChunk(&chunk, constant, 123);
        
        constant = addConstant(&chunk, 3.4);
        writeChunk(&chunk, OP_CONSTANT, 123);
        writeChunk(&chunk, constant, 123);

        writeChunk(&chunk, OP_ADD, 123);

        constant = addConstant(&chunk, 5.6);
        writeChunk(&chunk, OP_CONSTANT, 123);
        writeChunk(&chunk, constant, 123);

        writeChunk(&chunk, OP_DIVIDE, 123);
        writeChunk(&chunk, OP_NEGATE, 123);

        writeChunk(&chunk, OP_RETURN, 123);

        disassembleChunk(&chunk, "test chunk");

        // Have the VM interpret the chunk
        interpret(&chunk);

        // Wind down VM
        freeVM();
        
        freeChunk(&chunk);
        return 0;
}
