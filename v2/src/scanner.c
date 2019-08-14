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


static bool isAtEnd() {
        // Return the equality of the current lexeme and the null byte
        return *scanner.current == '\0';
}

static char peek() {
        // Peek but don't consume
        return *scanner.current;
}

static char peekNext() {
        if (isAtEnd()) return '\0';
        // Peek one ahead
        return scanner.current[1];
}

static char advance() {
        scanner.current++;
        return scanner.current[-1];
}


static void skipWhitespace() {
        for (;;) {
                char c = peek();
                switch (c) {
                case ' ':
                case '\r':
                case '\t':
                        advance();
                        break;
                case '\n':
                        scanner.line++;
                        advance();
                        break;
                case '/':
                        if (peekNext() == '/') {
                                // Comments go to end of line
                                while (peek() != '\n' && !isAtEnd()) advance();
                        }
                        else {
                                return;
                        }
                        break;
                default:
                        return;
                }
        }
}

static bool isAlpha(char c) {
        // Check if c is in the alphabet
        return (c >= 'a' && c <='z') ||
               (c >= 'A' && c <='Z') ||
               (c == '_');
}

static bool isDigit(char c) {
        return c >= '0' && c <= '9';
} 



static bool match(char expected) {
        // Check for EOF
        if (isAtEnd()) return false;
        // Check for mismatch
        if (*scanner.current != expected) return false;

        // Current is expected, consume and return true
        scanner.current++;
        return true;
}


static TokenType checkKeyword(int start, int length, 
                              const char* rest, TokenType type) {
        // Check the rest of the keyword
        if (scanner.current - scanner.start == start + length && 
            memcmp(scanner.start + start, rest, length) == 0) {
                    return type;
        }

        // Did not match, return identifier type
        return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
        // Trie
        switch(scanner.start[0]) {
                case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
                case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
                case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
                case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
                case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
                case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
                case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
                case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
                case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
                case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
                case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);

                case 'f':
                        if (scanner.current - scanner.start > 1) {
                                switch(scanner.start[1]) {
                                        case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                                        case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                                        case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN); 
                                }
                        }
                        break;
                case 't':
                        if (scanner.current - scanner.start > 1) {
                                switch(scanner.start[1]) {
                                        case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                                        case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                                }
                        }
                        break;
        }

        return TOKEN_IDENTIFIER;
}


static Token makeToken(TokenType type) {
        Token token;
        token.type = type;
        token.start = scanner.start;
        token.length = (int)(scanner.current - scanner.start);
        token.line = scanner.line;

        return token;
}

static Token errorToken(const char* message) {
        Token token;
        token.type = TOKEN_ERROR;
        token.start = message;
        token.length = (int)strlen(message);
        token.line = scanner.line;

        return token;
}

static Token string() {
        // Consume until the closing "
        while (peek() != '"' && !isAtEnd()) {
                // If a multi-line string, count the lines
                if (peek() == '\n') scanner.line++;
                advance();
        }

        // Check for EOF
        if (isAtEnd()) return errorToken("Unterminated string");

        // Closing "
        advance();
        
        return makeToken(TOKEN_STRING);
}

static Token number() {
        // Consume all digits
        while (isDigit(peek())) advance();

        // Look for a fractional part
        if (peek() == '.' && isDigit(peekNext())) {
                // Consume the '.'
                advance();

                // Continue consuming digits
                while (isDigit(peek())) advance();
        }

        return makeToken(TOKEN_NUMBER);
}

static Token identifier() {
        // Consume all alphanumeric chars
        while (isAlpha(peek()) || isDigit(peek())) advance();

        return makeToken(identifierType());
}

Token scanToken() {
        // Sanitize
        skipWhitespace();

        // Set the start pointer
        scanner.start = scanner.current;

        // If we reached the EOF, return TOKEN_EOF
        if (isAtEnd()) return makeToken(TOKEN_EOF);

        // Step one char forward
        char c = advance();

        // Check for alpha
        if (isAlpha(c)) return identifier();

        // Check for digits
        if (isDigit(c)) return number();

        switch(c) {
                // Single char lexeme cases
                case '(': return makeToken(TOKEN_LEFT_PAREN);
                case ')': return makeToken(TOKEN_RIGHT_PAREN);
                case '{': return makeToken(TOKEN_LEFT_BRACE);
                case '}': return makeToken(TOKEN_RIGHT_BRACE);
                case ';': return makeToken(TOKEN_SEMICOLON);
                case ',': return makeToken(TOKEN_COMMA);
                case '.': return makeToken(TOKEN_DOT);
                case '-': return makeToken(TOKEN_MINUS);
                case '+': return makeToken(TOKEN_PLUS);
                case '/': return makeToken(TOKEN_SLASH);
                case '*': return makeToken(TOKEN_STAR);

                // Two char lexeme cases
                case '!':
                        return makeToken(
                                match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG
                                );
                case '=':
                        return makeToken(
                                match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL
                        );
                case '<':
                        return makeToken(
                                match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS
                        );
                case '>':
                        return makeToken(
                                match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER
                        );
                case '"': return string();
        }

        // If character is not recognized, throw an error
        return errorToken("Unexpected character.");
}