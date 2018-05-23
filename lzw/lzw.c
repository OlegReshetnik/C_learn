#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS 12
#define HASHING_SHIFT ( BITS - 8 )
#define MAX_VALUE ( ( 1 << BITS ) - 1 )
#define MAX_CODE ( MAX_VALUE - 1 )

#define TABLE_SIZE 5021

int code_value[ TABLE_SIZE ];
unsigned int prefix_code[ TABLE_SIZE ];
unsigned char append_character[ TABLE_SIZE ];
unsigned char decode_stack[ 4000 ];

void compress( FILE *input, FILE *output, FILE *hex );
void expand( FILE *input, FILE *output );
int find_match( int hash_prefix, unsigned int hash_character );
void output_code( FILE *output, FILE *hex, unsigned int code );
unsigned int input_code( FILE *input );
unsigned char *decode_string( unsigned char *buffer, unsigned int code );

int main(int argc, char *argv[])
{
	FILE *input_file;
	FILE *output_file;
	FILE *hex_file;
	FILE *lzw_file;

	if( argc < 2 )
	{
		if( ( lzw_file = fopen( "test.lzw", "rb" ) ) == NULL )
		{
			printf( "Error opening input LZW file.\n" );
		}
		else
		{
			output_file = fopen( "test.out", "wb" );
			expand( lzw_file, output_file );
			fclose( lzw_file );
			fclose( output_file );
		}

		return 1;
	}

	if( ( input_file = fopen( argv[1], "rb" ) ) == NULL )
	{
		printf( "Error opening input file.\n" );
		return 1;
	}

	if( ( lzw_file = fopen( "test.lzw", "wb" ) ) == NULL )
	{
		fclose( input_file );
		printf( "Error opening output file.\n" );
		return 1;
	}

	if( ( hex_file = fopen( "hta_prog.c", "wb" ) ) == NULL )
	{
		fclose( input_file );
		printf( "Error opening hex file.\n" );
		return 1;
	}

	fprintf( hex_file, "unsigned char bb[] = {\n" );
	compress( input_file, lzw_file, hex_file );
	fprintf( hex_file, "\n};\n#define SB sizeof( bb )\n" );

	fclose( input_file );
	fclose( lzw_file );

	return 0;
}

void compress( FILE *input, FILE *output, FILE *hex )
{
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;
	int i;

	next_code = 256;
	for( i = 0; i < TABLE_SIZE; i++ ) code_value[i] = -1;

	string_code = getc( input );

	while( ( character = getc( input ) ) != (unsigned)EOF )
	{
		index = find_match( string_code, character );
		if( code_value[ index ] != -1 ) string_code = code_value[ index ];
		else
		{
			if( next_code <= MAX_CODE )
			{
				code_value[ index ] = next_code++;
				prefix_code[ index ] = string_code;
				append_character[ index ] = character;
			}

			output_code( output, hex, string_code );
			string_code = character;
		}
	}

	output_code( output, hex, string_code );
	output_code( output, hex, MAX_VALUE );
	output_code( output, hex, 0 );
}

int find_match( int hash_prefix, unsigned int hash_character )
{
	int index;
	int offset;

	index = ( hash_character << HASHING_SHIFT ) ^ hash_prefix;
	if( index == 0 ) offset = 1;
	else offset = TABLE_SIZE - index;

	while(1)
	{
		if( code_value[ index ] == -1 ) return index;
		if( prefix_code[ index ] == hash_prefix && append_character[ index ] == hash_character ) return index;
		index -= offset;
		if( index < 0 ) index += TABLE_SIZE;
	}

	return index;
}

void expand( FILE *input, FILE *output )
{
	unsigned int next_code;
	unsigned int new_code;
	unsigned int old_code;
	int character;
	unsigned char *string;

	next_code = 256;
	old_code = input_code( input );
	character = old_code;
	putc( old_code, output );

	while( ( new_code = input_code( input ) ) != MAX_VALUE )
	{
		if( new_code >= next_code )
		{
			*decode_stack = character;
			string = decode_string( decode_stack + 1, old_code );
		}
		else string = decode_string( decode_stack, new_code );

		character = *string;
		while( string >= decode_stack ) putc( *string--, output );

		if( next_code <= MAX_CODE )
		{
			prefix_code[ next_code ] = old_code;
			append_character[ next_code ] = character;
			next_code++;
		}

		old_code = new_code;
	}
}

unsigned char *decode_string( unsigned char *buffer, unsigned int code )
{
	int i = 0;

	while( code > 255 )
	{
		*buffer++ = append_character[ code ];
		code = prefix_code[ code ];
		if( i++ >= MAX_CODE )
		{
			printf( "Fatal error during code expansion.\n" );
			exit(-3);
		}
	}

	*buffer = code;

	return buffer;
}

unsigned int input_code( FILE *input )
{
	unsigned int return_value;
	static int input_bit_count = 0;
	static unsigned long input_bit_buffer = 0;

	while( input_bit_count <= 24 )
	{
		input_bit_buffer |= (unsigned long)getc( input ) << ( 24 - input_bit_count );
		input_bit_count += 8;
	}

	return_value = input_bit_buffer >> ( 32 - BITS );
	input_bit_buffer <<= BITS;
	input_bit_count -= BITS;

	return return_value;
}

void output_code( FILE *output, FILE *hex, unsigned int code )
{
	static int cc = 0;
	static int output_bit_count = 0;
	static unsigned long output_bit_buffer = 0;

	output_bit_buffer |= (unsigned long)code << ( 32 - BITS - output_bit_count );
	output_bit_count += BITS;

	while( output_bit_count >= 8 )
	{
		putc( output_bit_buffer >> 24, output );
		fprintf( hex, "0x%02X,", ( output_bit_buffer >> 24 ) ^ 0xAB );
		if( ++cc == 31 )
		{
			fprintf( hex, "\n" );
			cc = 0;
		}
		output_bit_buffer <<= 8;
		output_bit_count -= 8;
	}
}
