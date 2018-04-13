// sentence(set of words that is complete) -> subject + predicate(something that is affirmed) 
// syntax classes/symbols are words (grammar is a more general term that encompasses syntax)
// syntax equations are valid ways of combining syntax classes. this defines words and their structure. They can be represented as syntax trees.
// language is set of valid syntax rules

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS // This removes warnings for not using '_s' variants
#endif

// perror() for memory allocation failures

#define MAX(x, y)	\
	((x) >= (y) ? (x) : (y))

void fatal(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("FATAL ERROR: ");
	vprintf(fmt, args);	
	printf("\n");
	va_end(args);
}

// stretchy bufs
typedef struct {
	size_t length;
	size_t capacity;

	char buffer[0];	// flexible array, i.e. allow to declare a variable length array
} BufferHeader;

#define BUF__HEADER(b)	\
	((BufferHeader *)((char *)(b) - offsetof(BufferHeader, buffer)))

#define BUF_LENGTH(b)	\
	(((b) != NULL) ? BUF__HEADER(b)->length : 0)

#define BUF_CAPACITY(b)	\
	(((b) != NULL) ? BUF__HEADER(b)->capacity : 0)

#define BUF__FITS(b, amount) \
	(BUF_LENGTH(b) + (amount) <= BUF_CAPACITY(b))

#define BUF__FIT(b, amount)	\
	(BUF__FITS(b, 1) ? 0 : ((b) = buf__grow((b), BUF_LENGTH(b) + (amount), sizeof(*(b)))))

#define BUF_PUSH(b, element)	\
	(BUF__FIT(b, 1), b[BUF_LENGTH(b)] = (element), BUF__HEADER(b)->length++) 

#define BUF_FREE(b)	\
	(((b) != NULL) ? free(BUF__HEADER(b)) : 0, (b) = NULL)

void* buf__grow(const void* buf, size_t new_length, size_t elem_size)
{
	size_t new_cap = MAX(1 + 2 * BUF_CAPACITY(buf), new_length);
	// length of header + length of payload
	size_t new_size = offsetof(BufferHeader, buffer) + new_cap * elem_size;

	BufferHeader* new_header = NULL;
	
	if (buf != NULL) {
		new_header = realloc(BUF__HEADER(buf), new_size); 
	} else {
		new_header = malloc(new_size);
		new_header->length = 0;
	}

	new_header->capacity = new_cap;

	return new_header->buffer; 
}

typedef struct {
	size_t length;	
	const char* str;
} InternStr;

InternStr* interns;

const char* str_intern_range(const char* start, const char* end)
{
	size_t length = end - start;	
	for (size_t i = 0; i < BUF_LENGTH(interns); ++i) {
		if (interns[i].length == length && strncmp(interns[i].str, start, length) == 0) {
			return interns[i].str;	
		}
	}
	char* str = malloc(length + 1); // fix memory leak
	memcpy(str, start, length);
	str[length] = '\0';
	BUF_PUSH(interns, ((InternStr){length, str})); // pre-processor requires these to be wrapped in parentheses to avoid parameter delimiting
	return str;
}

// lua interns strings by default, so string comparisions are implicitly converted to pointer comparisons
const char* str_intern(const char* str)
{
	return str_intern_range(str, str + strlen(str));	
}

// Lexing
typedef enum {
	// reserve first 128 values for one char tokens
	TOKEN_LAST_CHAR = 127,
	TOKEN_INT,
	TOKEN_NAME
	// ..
} TokenType;

const char* token_type_name(TokenType type)
{
	static char buf[256] = {0};

	switch(type) {
	 case TOKEN_INT:
	 {
		sprintf(buf, "integer");
	 } break;
	 case TOKEN_NAME:
	 {
		sprintf(buf, "name");
	 } break;
	 default:
	 {
		if (type < 128 && isprint(type)) {
			sprintf(buf, "%c", type);	
		} else {
			sprintf(buf, "<ASCII %d>", type);	
		} 
	 }
		
	}

	return buf;
}

typedef struct {
	TokenType type; 	
	const char* start;
	const char* end;
	union {
		int val;		
		const char* name;
	}
} Token;

Token token;
const char* stream;

const char* keyword_if;

void intern_keywords(void)
{
	keyword_if = str_intern("if");	
}

void next_token(void)
{
	token.start = stream;
	switch (*stream) {
	 case 0:
	 case 1:
	 case 2:
	 case 3:
	 case 4:
	 case 5:
	 case 6:
	 case 7:
	 case 8:
	 case 9:
	 {
		int val = 0;
		while (isdigit(*stream)) {
			val *= 10;
			val += *stream++ - '0'; // convert ascii to number
		}
		token.type = TOKEN_INT;
		token.val = val;
	 } break;
	 case 'a':
	 case 'b':
	 case 'c':
	 case 'd':
	 case 'e':
	 case 'f':
	 case 'g':
	 case 'h':
	 case 'i':
	 case 'j':
	 case 'k':
	 case 'l':
	 case 'm':
	 case 'n':
	 case 'o':
	 case 'p':
	 case 'q':
	 case 'r':
	 case 's':
	 case 't':
	 case 'u':
	 case 'v':
	 case 'w':
	 case 'x':
	 case 'y':
	 case 'z':
	 case 'A':
	 case 'B':
	 case 'C':
	 case 'D':
	 case 'E':
	 case 'F':
	 case 'G':
	 case 'H':
	 case 'I':
	 case 'J':
	 case 'K':
	 case 'L':
	 case 'M':
	 case 'N':
	 case 'O':
	 case 'P':
	 case 'Q':
	 case 'R':
	 case 'S':
	 case 'T':
	 case 'U':
	 case 'V':
	 case 'W':
	 case 'X':
	 case 'Y':
	 case 'Z':
	 case '_':
	 {
		while (isalnum(*stream) || *stream == '_') {
			stream++;		
		}
		token.type = TOKEN_NAME;
		token.name = str_intern_range(token.start, stream);
	 } break;
	 default:
	 {
		token.type = *stream++;
	 } break;
	}	
	token.end = stream;
}

void init_stream(const char* str)
{
	stream = str;
	next_token();
}

void print_token(Token token)
{
	switch (token.type) {
	 case TOKEN_INT:
	 {
		printf("TOKEN_INT: %d\n", token.val); 
	 } break;
	 case TOKEN_NAME:
	 {
		printf("TOKEN_NAME: %.*s\n", (size_t)(token.start - token.end), token.name); 
	 } break;
	 default:
	 {
		printf("UNKNOWN TOKEN\n");	 
	 }
	}	
}

inline bool is_token(TokenType type)
{
	return token.type == type;	
}

inline bool is_token_name(const char* name)
{
	return token.type == TOKEN_NAME && token.name == name;	
}

inline bool match_token(TokenType type)
{
	if (is_token(type)) {
		next_token();
		return true;
	} else {
		return false;	
	}	
}

inline bool expect_token(TokenType type)
{
	if (is_token(type)) {
		next_token();
		return true;
	} else {
		fatal("Expected token: %s, got %s", token_type_name(type), token_type_name(token.type));
		return false;
	}	
}

/* Extended Backus Naur Form
expr0 = expr1 ([+-] expr1)*
expr = expr0
As we recurse, higher level precendences will already have been consumed
Current implementation is leftfold in all cases
*/
int parse_expr(void);

int parse_expr3()
{
	if (is_token(TOKEN_INT)) {
		int val = token.val;
		next_token();		
		return val;
	} else if (match_token('(')) {
		int val = parse_expr();
		expect_token(')');
		return val;
	} else {
		fatal("expected integer or (, got %s", token_type_name(token.type));		
		return 0;
	}
}

int parse_expr2()
{
	if (match_token('-')) {
		return -parse_expr3();
	} else {
		return parse_expr3();
	}
}

int parse_expr1()
{
	int val = parse_expr2();
	while (is_token('/') || is_token('*')) {
		char op = token.kind; // we know must be an ascii literal
		next_token();
		int rval = parse_expr2();
		if (op == '*') {
			val *= rval;		
		} else {
			val /= rval;		
		}
	}
	return val;
}

// lookahead, consume, parse recursively
int parse_expr0()
{
	int val = parse_expr1();
	while (is_token('+') || is_token('-')) {
		char op = token.kind; // we know must be an ascii literal
		next_token();
		int rval = parse_expr1();
		if (op == '+') {
			val += rval;		
		} else {
			val -= rval;		
		}
	}
	return val;
}

int parse_expr()
{
	return parse_expr0();
}

// printf("%.*s", str_length, str); useful for non-null terminated strings
// casting to avoid compiler warnings is a bad habit

int main(void)
{
	int* int_buf = NULL;

	BUF_PUSH(int_buf, 10);
	BUF_PUSH(int_buf, 18);
	BUF_PUSH(int_buf, 10);
	BUF_PUSH(int_buf, 10);

	for (int i = 0; i < BUF_LENGTH(int_buf); ++i) {
		printf("%d\n", int_buf[i]); 
	}

	BUF_FREE(int_buf);

	return 0;			
}
