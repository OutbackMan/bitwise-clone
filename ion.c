// sentence(set of words that is complete) -> subject + predicate(something that is affirmed) 
// syntax classes/symbols are words
// syntax equations are valid ways of combining syntax classes. this defines words and their structure. They can be represented as syntax trees.
// language is set of valid syntax rules

#include <stdio.h>
#include <stdlib.h>

// perror() for memory allocation failures

// stretchy buffers
typedef struct {
	size_t length;
	size_t capacity;

	char buffer[0];	// allow for array to pointer decay
} BufferHeader;

#define buffer__header(buffer)	\
	(BufferHeader *)((char *)(buffer) - offsetof(BufferHeader, buffer))

#define buffer__fits(buffer, amount) \
	(buffer_length(buffer) + amount <= buffer_capacity(buffer))

#define buffer__fit(buffer, n)	\
	(buffer__fits(buffer, 1) ? 0 : buffer = buffer__grow(buffer, buffer_length(buffer) + n, sizeof(*buffer)))

#define buffer_length(buffer)	\
	(buffer ? buffer__header(buffer)->length : 0)

#define buffer_capacity(buffer)	\
	(buffer ? buffer__header(buffer)->capacity : 0)

#define buffer_push(buffer, elem)	\
	(buffer__fit(buffer, 1), buffer[buffer__length(buffer)] = elem, buffer__header(buffer)->length++) 

#define buffer_free(buffer)	\
	(buffer ? free(buffer__header(buffer)) : 0)

void* buffer__grow(const void* buffer, size_t new_length, size_t elem_size)
{
	size_t new_cap = MAX(1 + 2 * buffer_capacity(buffer), new_length);
	// length of header + length of payload
	size_t new_size = offsetof(BufferHeader, buffer) + new_capacity * elem_size;

	BufferHeader* new_header = NULL;
	
	if (buffer != NULL) {
		new_header = realloc(buffer__header(buffer), new_size); 
	} else {
		new_header = malloc(new_size);
		new_header->length = 0;
	}

	return new_header->buffer; 
}

// Lexing
typedef enum {
	TOKEN_INT = 128,
	TOKEN_NAME
	// ..
} TokenType;

typedef struct {
	TokenType type; 	
	union {
		uint64_t val;		
		struct {
			const char* start;
			const char* end;
		};
	}
} Token;

Token token;
const char* stream;

void next_token()
{
	switch (*stream) {
	 case 0 ... 9:
	 {
		uint64_t val = 0;
		while (isdigit(*stream)) {
			val *= 10;
			val += *stream++ - '0'; // convert ascii to number
			stream++;		
		}
		token.type = TOKEN_INT;
		token.val = val;
	 } break;
	 case 'a' ... 'z':
	 case 'A' ... 'Z':
	 case '_':
	 {
		const char* start = stream++;
		while (isalnum(*stream) || *stream == '_') {
			stream++;		
		}
		token.type = TOKEN_NAME;
		token.start = start;
		token.end = stream;
	 } break;
	 default:
		token.type = *stream++;
	}	
}

// printf("%.*s", str_length, str); useful for non-null terminated strings
// casting to avoid compiler warnings is a bad habit

int main(void)
{
	int* int_buffer = NULL;

	buffer_push(int_buffer, 10);
	buffer_push(int_buffer, 18);
	buffer_push(int_buffer, 10);
	buffer_push(int_buffer, 10);

	for (int i = 0; i < buffer_length(int_buffer); ++i) {
		printf("%d\n", int_buffer[i]); 
	}

	return 0;			
}
