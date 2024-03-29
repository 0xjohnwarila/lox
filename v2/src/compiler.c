#include <stdio.h>
#include <stdlib.h>

#include "../includes/common.h"
#include "../includes/compiler.h"
#include "../includes/scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "../includes/debug.h"
#endif

typedef struct {
        Token current;
        Token previous;
        bool had_error;
        bool panic_mode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -      
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . () []
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

typedef struct {
        ParseFn prefix;
        ParseFn infix;
        Precedence precedence;
} ParseRule;


Parser parser;
Chunk* compiling_chunk;

static Chunk* currentChunk() {
        return compiling_chunk;
}

static void errorAt(Token* token, const char* message) {
        if (parser.panic_mode) return;
        // Panic the parser
        parser.panic_mode = true;

        // Print line number of error
        fprintf(stderr, "[line %d] Error", token->line);

        if (token->type == TOKEN_EOF) {
                fprintf(stderr, " at end");
        }
        else if (token->type == TOKEN_ERROR) {
                // Nothing
        }
        else {
                fprintf(stderr, " at '%.*s'", token->length, token->start);
        }

        fprintf(stderr, ": %s\n", message);
        parser.had_error = true;
}

static void error(const char* message) {
        errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message) {
        errorAt(&parser.current, message);
}

static void advance() {
        parser.previous = parser.current;

        for (;;) {
                // Ask scanner for a token
                parser.current = scanToken();
                // If there is no error, break;
                if (parser.current.type != TOKEN_ERROR) break;
                // If there is an error, throw it
                errorAtCurrent(parser.current.start);
        }
}

static void consume(TokenType type, const char* message) {
        // Check if the type is the desired one, consume if so
        if (parser.current.type == type) {
                advance();
                return;
        }

        // If type is not desired type, throw error
        errorAtCurrent(message);
}

static void emitByte(uint8_t byte) {
        writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte_1, uint8_t byte_2) {
        emitByte(byte_1);
        emitByte(byte_2);
}

static void emitReturn() {
        emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value) {
        int constant = addConstant(currentChunk(), value);
        if (constant > UINT8_MAX) {
                error("Too many constants int one chunk.");
                return 0;
        }

        return (uint8_t)constant;
}

static void emitConstant(Value value) {
        emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler() {
        emitReturn();

#ifdef DEBUG_PRINT_CODE
        if (!parser.had_error) {
                disassembleChunk(currentChunk(), "code");
        }
#endif
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary() {
        // Remember Operator
        TokenType operator_type = parser.previous.type;

        // Compile right operand
        ParseRule* rule = getRule(operator_type);
        parsePrecedence((Precedence)(rule->precedence + 1));

        // Emit operator instruction
        switch (operator_type) {
                case TOKEN_PLUS:          emitByte(OP_ADD); break;
                case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
                case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
                case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
                default:
                        return; // Unreachable
        }
}

static void literal() {
        switch (parser.previous.type) {
                case TOKEN_FALSE: emitByte(OP_FALSE); break;
                case TOKEN_NIL: emitByte(OP_NIL); break;
                case TOKEN_TRUE: emitByte(OP_TRUE); break;
                default:
                        return; // Unreachable
        }
}

static void grouping() {
        expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number() {
        double value = strtod(parser.previous.start, NULL);
        emitConstant(NUMBER_VAL(value));
}

static void unary() {
        TokenType operator_type = parser.previous.type;

        // Compile operand
        parsePrecedence(PREC_UNARY);

        switch (operator_type) {
                case TOKEN_BANG: emitByte(OP_NOT); break;
                case TOKEN_MINUS: emitByte(OP_NEGATE); break;
                default:
                        return; // Unreachable
        }
}

ParseRule rules[] = {                                              
  { grouping, NULL,    PREC_NONE },       // TOKEN_LEFT_PAREN
  { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_PAREN
  { NULL,     NULL,    PREC_NONE },       // TOKEN_LEFT_BRACE
  { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_BRACE
  { NULL,     NULL,    PREC_NONE },       // TOKEN_COMMA
  { NULL,     NULL,    PREC_NONE },       // TOKEN_DOT
  { unary,    binary,  PREC_TERM },       // TOKEN_MINUS
  { NULL,     binary,  PREC_TERM },       // TOKEN_PLUS
  { NULL,     NULL,    PREC_NONE },       // TOKEN_SEMICOLON
  { NULL,     binary,  PREC_FACTOR },     // TOKEN_SLASH
  { NULL,     binary,  PREC_FACTOR },     // TOKEN_STAR
  { unary,    NULL,    PREC_NONE },       // TOKEN_BANG
  { NULL,     NULL,    PREC_NONE },       // TOKEN_BANG_EQUAL
  { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL
  { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL_EQUAL
  { NULL,     NULL,    PREC_NONE },       // TOKEN_GREATER
  { NULL,     NULL,    PREC_NONE },       // TOKEN_GREATER_EQUAL
  { NULL,     NULL,    PREC_NONE },       // TOKEN_LESS
  { NULL,     NULL,    PREC_NONE },       // TOKEN_LESS_EQUAL
  { NULL,     NULL,    PREC_NONE },       // TOKEN_IDENTIFIER
  { NULL,     NULL,    PREC_NONE },       // TOKEN_STRING
  { number,   NULL,    PREC_NONE },       // TOKEN_NUMBER
  { NULL,     NULL,    PREC_NONE },       // TOKEN_AND
  { NULL,     NULL,    PREC_NONE },       // TOKEN_CLASS
  { NULL,     NULL,    PREC_NONE },       // TOKEN_ELSE
  { literal,  NULL,    PREC_NONE },       // TOKEN_FALSE
  { NULL,     NULL,    PREC_NONE },       // TOKEN_FOR
  { NULL,     NULL,    PREC_NONE },       // TOKEN_FUN
  { NULL,     NULL,    PREC_NONE },       // TOKEN_IF
  { literal,  NULL,    PREC_NONE },       // TOKEN_NIL
  { NULL,     NULL,    PREC_NONE },       // TOKEN_OR
  { NULL,     NULL,    PREC_NONE },       // TOKEN_PRINT
  { NULL,     NULL,    PREC_NONE },       // TOKEN_RETURN
  { NULL,     NULL,    PREC_NONE },       // TOKEN_SUPER
  { NULL,     NULL,    PREC_NONE },       // TOKEN_THIS
  { literal,  NULL,    PREC_NONE },       // TOKEN_TRUE
  { NULL,     NULL,    PREC_NONE },       // TOKEN_VAR
  { NULL,     NULL,    PREC_NONE },       // TOKEN_WHILE
  { NULL,     NULL,    PREC_NONE },       // TOKEN_ERROR
  { NULL,     NULL,    PREC_NONE },       // TOKEN_EOF
};

static void parsePrecedence(Precedence precedence) {
        advance();
        ParseFn prefixRule = getRule(parser.previous.type)->prefix;
        if (prefixRule == NULL) {
                error("Expect expression.");
                return;
        }

        prefixRule();

        while (precedence <= getRule(parser.current.type)->precedence) {
                advance();
                ParseFn infixRule = getRule(parser.previous.type)->infix;
                infixRule();
        }
}

static ParseRule* getRule(TokenType type) {
        return &rules[type];
}

void expression() {
        parsePrecedence(PREC_ASSIGNMENT);
}

bool compile(const char* source, Chunk* chunk) {
        // Initialization
        initScanner(source);
        compiling_chunk = chunk;

        // Reset error flags
        parser.had_error = false;
        parser.panic_mode = false;

        // Compile
        advance();
        expression();
        consume(TOKEN_EOF, "Expect end of expression.");

        // Wind down compiler
        endCompiler();
        return !parser.had_error;
}