#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXSTR 4096

int main(int argc,char *argv[])
{
	FILE *infile;
	char sb[MAXSTR];
	int size, i, j;


	if( (infile = fopen( argv[1], "rb" )) == NULL )
	{
		printf("Error open input file: %s\n", argv[1]);
		return 1;
	}

	size = fread( &sb[0], 1, MAXSTR, infile );
	for( i = 0, j = 0; j < size; i++, j += 2 )
	{
		sb[i] = sb[j];
	}

	size /= 2;

	i = 0;
	for(;;)
	{
		while( sb[i++] != '1' ) { if( i == size ) break; }
		if( i == size ) break;
		if( sb[i++] == '_' ) break;
	}

	if( i == size ) puts("1;1");
	else puts( &sb[i+6] );
	fclose(infile);
}
