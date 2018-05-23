#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N_FREQS		16
#define FREQ_TIME	60

#define MAXSTR 1024

char start[] =
"#ifndef flash_freqs_h\n"
"#define flash_freqs_h\n"
"\n"
"#include <avr/pgmspace.h>\n"
"#include <stdint.h>\n"
"\n"
"#define S( a )	( (uint8_t)( sizeof( a ) / sizeof( a[0] ) ) )\n"
"#define P( x )	( (uint16_t)&x[0] )\n"
"#define K( x )	( (uint32_t)( x * 100.0 ) )\n"
"\n";

char end[] =
"\n"
"\n"
"#define razdel_names( razdel )		( (uint16_t *)pgm_read_word( &pg_names[ razdel ] ) )\n"
"#define razdel_nums( razdel )		( (uint8_t *)pgm_read_word( &pg_nums[ razdel ] ) )\n"
"#define razdel_secs( razdel )		( (uint16_t *)pgm_read_word( &pg_secs[ razdel ] ) )\n"
"#define razdel_freqs( razdel )		( (uint16_t *)pgm_read_word( &pg_freqs[ razdel ] ) )\n"
"\n"
"\n"
"#define prog_name( razdel, i )		( (uint8_t *)pgm_read_word( &razdel_names( razdel )[i] ) )\n"
"#define prog_sec( razdel, i )		( (uint16_t)pgm_read_word( &razdel_secs( razdel )[i] ) )\n"
"#define prog_num( razdel, i )		( (uint8_t)pgm_read_byte( &razdel_nums( razdel )[i] ) )\n"
"#define prog_freqs( razdel, i )		( (uint32_t *)pgm_read_word( &razdel_freqs( razdel )[i] ) )\n"
"\n"
"\n"
"#endif\n";

char r_nam[] =
"\n"
"//******************************************\n"
"const uint8_t pg_%02d[] PROGMEM = \"%s\";\n\n";
char nam[] = "const uint8_t n%02d_%02d[] PROGMEM = \"%s\";\n";
char freq[] = "const uint32_t f%02d_%02d[] PROGMEM = { ";
char freq_i[] = "K(%.2f), ";
char freq_i_last[] = "K(%.2f) };\n";
char time_mask[] = "#define L%02d_%02d ( %d )\n\n";

char r_arr_nam[] = "const uint8_t * const pg_n%02d[] PROGMEM = { ";
char r_arr_nam_i[] = "n%02d_%02d, ";
char r_arr_nam_i_last[] = "n%02d_%02d, (uint8_t *)0 };\n";

char r_arr_freq[] = "const uint32_t * const pg_f%02d[] PROGMEM = { ";
char r_arr_freq_i[] = "f%02d_%02d, ";
char r_arr_freq_i_last[] = "f%02d_%02d };\n";

char r_arr_j[] = "const uint8_t pg_j%02d[] PROGMEM = { ";
char r_arr_j_i[] = "S(f%02d_%02d), ";
char r_arr_j_i_last[] = "S(f%02d_%02d) };\n";

char r_arr_time[] = "const uint16_t pg_t%02d[] PROGMEM = { ";
char r_arr_time_i[] = "L%02d_%02d, ";
char r_arr_time_i_last[] = "L%02d_%02d };\n";

char all_nam[] =
"\n\n"
"//*******************************************\n"
"const uint8_t * const pg_menu[] PROGMEM = { ";
char all_nam_i[] = "pg_%02d, ";
char all_nam_i_last[] = "pg_%02d, (uint8_t *)0 };\n\n";

char all_r_nam[] = "const uint16_t pg_names[] PROGMEM = { ";
char all_r_nam_i[] = "P(pg_n%02d), ";
char all_r_nam_i_last[] = "P(pg_n%02d) };\n\n";

char all_r_freq[] = "const uint16_t pg_freqs[] PROGMEM = { ";
char all_r_freq_i[] = "P(pg_f%02d), ";
char all_r_freq_i_last[] = "P(pg_f%02d) };\n\n";

char all_r_j[] = "const uint16_t pg_nums[] PROGMEM = { ";
char all_r_j_i[] = "P(pg_j%02d), ";
char all_r_j_i_last[] = "P(pg_j%02d) };\n\n";

char all_r_time[] = "const uint16_t pg_secs[] PROGMEM = { ";
char all_r_time_i[] = "P(pg_t%02d), ";
char all_r_time_i_last[] = "P(pg_t%02d) };\n\n";

void my_trim( char * s )
{
	while( *s )
	{
		if( *s == '\r' || *s == '\n' ) *s = 0;
		s++;
	}
}

void print_all( FILE *out, int cnt )
{
	int i;

	fprintf( out, all_nam );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, all_nam_i, i );
	}
	fprintf( out, all_nam_i_last, cnt );

	fprintf( out, all_r_nam );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, all_r_nam_i, i );
	}
	fprintf( out, all_r_nam_i_last, cnt );

	fprintf( out, all_r_freq );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, all_r_freq_i, i );
	}
	fprintf( out, all_r_freq_i_last, cnt );

	fprintf( out, all_r_j );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, all_r_j_i, i );
	}
	fprintf( out, all_r_j_i_last, cnt );

	fprintf( out, all_r_time );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, all_r_time_i, i );
	}
	fprintf( out, all_r_time_i_last, cnt );
}

void print_arr( FILE *out, int razd, int cnt )
{
	int i;

	fprintf( out, r_arr_nam, razd );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, r_arr_nam_i, razd, i );
	}
	fprintf( out, r_arr_nam_i_last, razd, cnt );

	fprintf( out, r_arr_freq, razd );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, r_arr_freq_i, razd, i );
	}
	fprintf( out, r_arr_freq_i_last, razd, cnt );

	fprintf( out, r_arr_j, razd );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, r_arr_j_i, razd, i );
	}
	fprintf( out, r_arr_j_i_last, razd, cnt );

	fprintf( out, r_arr_time, razd );
	if( cnt > 9 ) fprintf( out, "\n" );
	for( i = 1; i < cnt; i++ )
	{
		if( (i%9) == 0 && i != 0 ) fprintf( out, "\n" );
		fprintf( out, r_arr_time_i, razd, i );
	}
	fprintf( out, r_arr_time_i_last, razd, cnt );
}

void print_freqs( FILE *out, int razd, int cnt, char *s ) // float f1, float f2 )
{
	char *p;
	int i = 0, pt;
	float fq;

	fprintf( out, freq, razd, cnt );

	while( 1 )
	{
		fq = strtof( s, &p );
		s = p + 1;
		i++;
		if( *p == 0 )
		{
			fprintf( out, freq_i_last, fq );
			break;
		}
		else
		{
			if( (i%6) == 0 && i != 0 ) fprintf( out, "\n" );
			fprintf( out, freq_i, fq );
		}
	}

	pt = i * FREQ_TIME;
	if( pt > 1500 ) pt = 1500;
	else if( pt < 1200 ) pt = 1200;
	fprintf( out, time_mask, razd, cnt, pt / i );
}

char * get_name( char *str )
{
	char *p;
	while( *str != ';' )str++;
	*str = 0;
	str++;
	return str;
}


int main( int argc, char *argv[] )
{
	FILE *infile, *outfile;
	char *ptr;
	float f1 = 0, f2 = 0;
	char str[MAXSTR];
	int r_cnt = 1, i_cnt = 1, is_razdel = 1; // cnt start from 1

	if( argc < 2 )
	{
		printf( "Usege: Freq2c.exe freq.txt\n" );
		return 1;
	}

	if( (infile = fopen( argv[1], "r" )) == NULL )
	{
		printf("Error open input file: %s\n", argv[1]);
		return 1;
	}

	outfile = fopen( "flash_freqs.h", "w" );
	fputs( start, outfile );

	while( fgets( str, MAXSTR, infile ) != NULL )
	{
		my_trim( str );
		//printf( "[%s]\n", str );

		if( str[0] == 0 )
		{
			print_arr( outfile, r_cnt, i_cnt-1 );
			is_razdel = 1;
			r_cnt++;
			i_cnt = 1;
		}
		else
		{
			if( is_razdel )
			{
				fprintf( outfile, r_nam, r_cnt, str );
				is_razdel = 0;
			}
			else
			{
				ptr = get_name( str );
				fprintf( outfile, nam, r_cnt, i_cnt, str );
				print_freqs( outfile, r_cnt, i_cnt, ptr );
				i_cnt++;
			}
		}
	}

	print_arr( outfile, r_cnt, i_cnt-1 );
	print_all( outfile, r_cnt );

	fputs( end, outfile );
	fclose(infile);
	fclose(outfile);

	return 0;
}
