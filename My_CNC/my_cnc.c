#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAXSTR 1024

/*
void process_init( char *exe_file, double *z_up, double *z_dn, int *xy_fd, int *z_fd, double *scale )
{
	FILE *ini_file;
	char file[MAXSTR], str[MAXSTR];

	sprintf(file, "%s.ini", exe_file );

	*z_up = 1.0; *z_dn = -0.2; *xy_fd = 80; *z_fd = 50; *scale = 0.025;

	if ( (ini_file = fopen( file, "r" )) != NULL )
		{
		if( fgets(str, MAXSTR, ini_file) != NULL ) *z_up = strtod( str, NULL );
		if( fgets(str, MAXSTR, ini_file) != NULL ) *z_dn = strtod( str, NULL );
		if( fgets(str, MAXSTR, ini_file) != NULL ) * xy_fd = atoi( str );
		if( fgets(str, MAXSTR, ini_file) != NULL ) * z_fd = atoi( str );
		if( fgets(str, MAXSTR, ini_file) != NULL ) *scale = strtod( str, NULL );
		fclose(ini_file);
		}
	ini_file = fopen( file, "w" );
	fprintf(ini_file, "%.2f ; Z up\n%.2f ; Z down\n%d ; X and Y feed\n%d ; Z feed/drilling\n%.3f ; X scale factor units to G code mm",
						*z_up, *z_dn, *xy_fd, *z_fd, *scale );
	fclose(ini_file);
}
*/

void get_params( char * str, double *z_up, double *z_dn, int *z_fd, int *xy_fd, double *scale )
{
	char * ptr;

	*z_up = strtod( str, &ptr ); ptr++;
	if( *z_up <= 0 || *z_up > 2 ) *z_up = 2;
		*z_dn = strtod( ptr, &ptr ); ptr++;
		if( *z_dn > 0 || *z_dn < -3 ) *z_dn = -0.2;
	*z_fd = (int) strtol( ptr, &ptr, 10); ptr++;
	if( *z_fd > 500 || *z_fd < 1 ) *z_fd = 50;
		*xy_fd = (int) strtol( ptr, &ptr, 10); ptr++;
		if( *xy_fd > 500 || *xy_fd < 1 ) *xy_fd = 80;
	*scale = strtod( ptr, NULL );
	if( *scale <= 0 || *scale > 0.1 ) *scale = 0.025;
}

void get_rotate( char * str, double * sin_alfa, double * cos_alfa, double *x0, double *y0 )
{
	double x, y, x1, y1, alfa;
	char * ptr;

	*x0 = strtod( str, &ptr ); ptr++;
	*y0 = strtod( ptr, &ptr ); ptr++;
	x = strtod( ptr, &ptr ); ptr++;
	y = strtod( ptr, &ptr ); ptr++;
	x1 = strtod( ptr, &ptr ); ptr++;
	y1 = strtod( ptr, NULL );
	alfa = atan2( x - *x0, y - *y0 ) - atan2( x1 - *x0, y1 - *y0 ); // double y, double x
	*sin_alfa = asin( alfa );
	*cos_alfa = acos( alfa );
}

int get_vals(char *str, double *x, double *y)
{
	char * ptr;

	while( !isdigit(*str) && *str != '-' )
	{
		if( *str == 0 || *str == ';' ) return 0; // No values
		str ++;
	}

	*x = strtod( str, &ptr );
	ptr ++;
	*y = strtod( ptr, NULL );

	return 1; // Get values
}

int get_next_cmd( char *str, double *x, double *y, char * cmd )
{
	strupr(str);

	if( *str++ = 'P' )
	{
		if( *str=='U' || *str=='A' || *str=='D' )
		{
			*cmd = *str++;
			return get_vals(str, x, y);
		}
	}

	return -1; // No cmd
}

int main( int argc, char *argv[] )
{
	double z_up_value, z_down_value, dpi, G_x, T_x, G_y, T_y, sin_alfa=0, cos_alfa=1, x0=0, y0=0;
	int G_z, T_z, xy_feed, z_feed, is_z_feed, is_rotate = 0;
	FILE *infile, *outfile;
	char sb[MAXSTR], cmd;

	G_x = T_x = 0;
	G_y = T_y = 0;
	G_z = T_z = 1; // Up
	is_z_feed = 0; // No feed


	if( argc < 3 )
	{
		printf(
		"Usage: my_cnc.exe infile params1 [params2]\n\n"
		"params1: 1,-0.2,50,80,0.025\n"
		"Z up(double), Z down(double), Z feed(int), XY feed(int), scale factor(float)\n"
		"no space between arguments!\n\n"
		"params2 (if rotate and mirrow): 1.23,2.34,3.45,4.56,5.67,6.78\n"
		"X0(double),Y0(double),X(double),Y(double),X1(double),Y1(double)\n"
		"no space between arguments!\n\n"
		"Press any key ...\n"
		);
		getch();

		return 1;
	}

	get_params( argv[2], &z_up_value, &z_down_value, &z_feed, &xy_feed, &dpi );
	printf( "Use parametrs: Z_UP=%.2f, Z_DN=%.2f, Z_FD=%d, XY_FD=%d, K=%.4f\n", z_up_value, z_down_value, z_feed, xy_feed, dpi );

	if( argc > 3 )
	{
		get_rotate( argv[3], &sin_alfa, &cos_alfa, &x0, &y0 );
		is_rotate = 1;
		printf( "Use rotate: SINUS=%.3f, COSINUS=%.3f, X0=%.3f, Y0=%.3f\n", sin_alfa, cos_alfa, x0, y0 );
	}

	if( (infile = fopen( argv[1], "r" )) == NULL )
	{
		printf("Error open input file: %s ...\n", argv[1]);
		getch();
		return 1;
	}

	sprintf( sb, "%s.tap", argv[1] );
	if( ( outfile = fopen( sb, "w" ) ) == NULL )
	{
		fclose( infile );
		printf( "Error open output file: %s ...\n", sb );
		getch();
		return 1;
	}

	fprintf( outfile, "G21\nG90\nM3 S30000\nG0 X0 Y0\nF%d\n\n", xy_feed );

	while( fgets(sb, MAXSTR, infile) != NULL )
	{
		if( get_next_cmd( sb, &T_x, &T_y, &cmd ) == 1 ) break; // Skip to first coordinates
	}

	int ret;
 	do
	{
		if( (ret = get_next_cmd( sb, &T_x, &T_y, &cmd )) != -1 ) // Find CMD
		{
			if( cmd == 'U' ) T_z = 1; // UP
			else if( cmd == 'D' ) T_z = 0; // Down
												// 'A' cmd - travel
			if( T_z == 0 && G_z == 1 ) // Move down
			{
				if( is_z_feed == 0 )
				{
					fprintf(outfile, "F%d\n", z_feed); // set Z feed
					is_z_feed = 1;
				}
				fprintf(outfile, "G1 Z%.2f\n", z_down_value);
				G_z = 0; // current down
			}
			else if( T_z == 1 && G_z == 0 ) // Move up
			{
				fprintf(outfile, "G0 Z%.2f\n", z_up_value);
				G_z = 1; // current up
			}

			if( (ret == 1) && (T_x != G_x || T_y != G_y) ) // Если нашел координаты и они другие
			{
				G_x = T_x;
				G_y = T_y;
				T_x = G_x * dpi; // convert coordinates
				T_y = G_y * dpi;

				if( is_rotate )
				{
					T_x = 2 *x0 - T_x;
					T_x = x0 + ( T_x - x0 ) * cos_alfa - ( T_y - y0 ) * sin_alfa;
					T_y = y0 + ( T_x - x0 ) * sin_alfa + ( T_y - y0 ) * cos_alfa;
				}

				if( G_z == 1 ) // up
				{
					fprintf(outfile, "G0 X%.3f Y%.3f\n", T_x, T_y );
				}
				else
				{
					if( is_z_feed == 1 )
					{
						fprintf(outfile, "F%d\n", xy_feed); // change feed from Z to XY
						is_z_feed = 0;
					}
					fprintf(outfile, "G1 X%.3f Y%.3f\n", T_x, T_y );
				}
			}
		}
	}
	while( fgets(sb, MAXSTR, infile ) != NULL );

	fprintf( outfile, "\nM5\nG0 X0 Y0\nM30\n" );
	fclose(infile);
	fclose(outfile);

	printf( "Process OK.\n" );
	return 0;
}
