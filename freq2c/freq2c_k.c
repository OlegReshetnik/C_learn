#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N_FREQS		16
#define PROG_TIME	1200

#define MAXSTR 1024

char start[] =
"#ifndef flash_kfreqs_h\n"
"#define flash_kfreqs_h\n"
"\n"
"#include <avr/pgmspace.h>\n"
"#include <stdint.h>\n"
"\n"
"#define K_S( a )	( (uint8_t)( sizeof( a ) / sizeof( a[0] ) ) )\n"
"#define K_P( x )	( (uint16_t)&x[0] )\n"
"#define K_K( x )	( (uint32_t)( x * 100.0 ) )\n"
"\n";

char end[] =
"\n"
"\n"
"\n"
"#define razdel_knames( razdel )		( (uint16_t *)pgm_read_word( &kpg_names[ razdel ] ) )\n"
"#define razdel_knums( razdel )		( (uint8_t *)pgm_read_word( &kpg_nums[ razdel ] ) )\n"
"#define razdel_ksecs( razdel )		( (uint16_t *)pgm_read_word( &kpg_secs[ razdel ] ) )\n"
"#define razdel_kfreqs( razdel )		( (uint16_t *)pgm_read_word( &kpg_freqs[ razdel ] ) )\n"
"\n"
"\n"
"#define prog_kname( razdel, i )		( (uint8_t *)pgm_read_word( &razdel_knames( razdel )[i] ) )\n"
"#define prog_ksec( razdel, i )		( (uint16_t)pgm_read_word( &razdel_ksecs( razdel )[i] ) )\n"
"#define prog_knum( razdel, i )		( (uint8_t)pgm_read_byte( &razdel_knums( razdel )[i] ) )\n"
"#define prog_kfreqs( razdel, i )	( (uint32_t *)pgm_read_word( &razdel_kfreqs( razdel )[i] ) )\n"
"\n"
"\n"
"#endif\n";

char r_nam[] =
"\n"
"//******************************************\n"
"const uint8_t kpg_%02d[] PROGMEM = \"%s\";\n\n";
char nam[] = "const uint8_t kn%02d_%02d[] PROGMEM = \"%s\";\n";
char freq[] = "const uint32_t kf%02d_%02d[] PROGMEM = { ";
char freq_i[] = "K_K(%.2f), ";
char freq_i_last[] = "K_K(%.2f) };\n";
char time_mask[] = "#define KL%02d_%02d ( %d )\n\n";

char r_arr_nam[] = "const uint8_t * const kpg_n%02d[] PROGMEM = { ";
char r_arr_nam_i[] = "kn%02d_%02d, ";
char r_arr_nam_i_last[] = "kn%02d_%02d, (uint8_t *)0 };\n";

char r_arr_freq[] = "const uint32_t * const kpg_f%02d[] PROGMEM = { ";
char r_arr_freq_i[] = "kf%02d_%02d, ";
char r_arr_freq_i_last[] = "kf%02d_%02d };\n";

char r_arr_j[] = "const uint8_t kpg_j%02d[] PROGMEM = { ";
char r_arr_j_i[] = "K_S(kf%02d_%02d), ";
char r_arr_j_i_last[] = "K_S(kf%02d_%02d) };\n";

char r_arr_time[] = "const uint16_t kpg_t%02d[] PROGMEM = { ";
char r_arr_time_i[] = "KL%02d_%02d, ";
char r_arr_time_i_last[] = "KL%02d_%02d };\n";

char all_nam[] =
"\n\n"
"//*******************************************\n"
"const uint8_t * const kpg_menu[] PROGMEM = { ";
char all_nam_i[] = "kpg_%02d, ";
char all_nam_i_last[] = "kpg_%02d, (uint8_t *)0 };\n\n";

char all_r_nam[] = "const uint16_t kpg_names[] PROGMEM = { ";
char all_r_nam_i[] = "K_P(kpg_n%02d), ";
char all_r_nam_i_last[] = "K_P(kpg_n%02d) };\n\n";

char all_r_freq[] = "const uint16_t kpg_freqs[] PROGMEM = { ";
char all_r_freq_i[] = "K_P(kpg_f%02d), ";
char all_r_freq_i_last[] = "K_P(kpg_f%02d) };\n\n";

char all_r_j[] = "const uint16_t kpg_nums[] PROGMEM = { ";
char all_r_j_i[] = "K_P(kpg_j%02d), ";
char all_r_j_i_last[] = "K_P(kpg_j%02d) };\n\n";

char all_r_time[] = "const uint16_t kpg_secs[] PROGMEM = { ";
char all_r_time_i[] = "K_P(kpg_t%02d), ";
char all_r_time_i_last[] = "K_P(kpg_t%02d) };\n\n";

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

void print_freqs( FILE *out, int razd, int cnt, float f1, float f2 )
{
	int i;
	float delta;

	fprintf( out, freq, razd, cnt );

	if( f1 == f2 )
	{
		fprintf( out, freq_i_last, f1 );
		fprintf( out, time_mask, razd, cnt, PROG_TIME );
	}
	else
	{
		fprintf( out, "\n" );
		delta = ( f2 - f1 ) / (float)(N_FREQS-1);
		for( i = 0; i < (N_FREQS-1); i++ )
		{
			if( (i%4) == 0 && i != 0 ) fprintf( out, "\n" );
			fprintf( out, freq_i, f1 + (delta * (float)i) );
		}
		fprintf( out, freq_i_last, f2 );
		fprintf( out, time_mask, razd, cnt, PROG_TIME / N_FREQS );
	}
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

void my_get( char *str, char *name, float *f1, float *f2 )
{
	char *p;
	while( *str != '-' ) *name++ = *str++;
	*name = 0;
	str++;
	p = str;
	while( *p != '-' ) p++;
	*p++ = 0;
	*f1 = atof( str );
	*f2 = atof( p );
}

int main( int argc, char *argv[] )
{
	FILE *infile, *outfile;
	char name[20];
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

	outfile = fopen( "flash_kfreqs.h", "w" );
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
				my_get( str, name, &f1, &f2 );
				fprintf( outfile, nam, r_cnt, i_cnt, name );
				print_freqs( outfile, r_cnt, i_cnt, f1, f2 );
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
