#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define ARDUINO_WAIT_TIME	1500
#define COM_OK				( 'O' )
#define COM_CLEAR			( 'C' )
#define COM_WRITE			( 'S' )

#define COM_TIME_OUT		2000

#define ERR_OK				0
#define ERR_BAD_COMMAND		1
#define ERR_NO_DATA			2
#define ERR_NO_ELECTRON		3
#define ERR_WRITE			4
#define ERR_FULL			5


HANDLE serial_open( int port )
{
	HANDLE hSerial;
	char name[16];

	sprintf( name, "\\\\.\\COM%d", port );

	hSerial = CreateFileA( name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hSerial == INVALID_HANDLE_VALUE ) return INVALID_HANDLE_VALUE;

	DCB dcbSerialParams = {0};

	if( !GetCommState( hSerial, &dcbSerialParams ) )
	{
		CloseHandle( hSerial );
		return INVALID_HANDLE_VALUE;
	}

	dcbSerialParams.BaudRate = 115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

	if( !SetCommState( hSerial, &dcbSerialParams ) )
	{
		CloseHandle( hSerial );
		return INVALID_HANDLE_VALUE;
	}

	PurgeComm( hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR );
	Sleep( ARDUINO_WAIT_TIME );

	return hSerial;
}

void serial_close( HANDLE hSerial )
{
	CloseHandle( hSerial );
}

int serial_read( HANDLE hSerial, BYTE * buf, DWORD n )
{
	COMSTAT status;
	DWORD errors, bytesRead, toRead;

	ClearCommError( hSerial, &errors, &status );

	if( ( toRead = status.cbInQue ) > 0 )
	{
		toRead = ( toRead > n ) ? n : toRead;
		if( ReadFile( hSerial, buf, toRead, &bytesRead, NULL ) ) return bytesRead;
	}
	return 0;
}

int serial_write( HANDLE hSerial, BYTE * buf, DWORD n )
{
	COMSTAT status;
	DWORD errors, bytesSend;

	if( !WriteFile( hSerial, (void *)buf, n, &bytesSend, 0) )
	{
		ClearCommError( hSerial, &errors, &status);
		return 0;
	}
	return 1;
}


int serial_rx_ready( HANDLE hSerial )
{
	COMSTAT status;
	DWORD errors;

	ClearCommError( hSerial, &errors, &status );

	if( status.cbInQue > 0 ) return 1;
	return 0;
}

int serial_rx( HANDLE hSerial )
{
	BYTE bb;
	if( serial_read( hSerial, &bb, 1 ) == 0 ) return -1;
	return (int)bb;
}

int serial_tx( HANDLE hSerial, BYTE bb )
{
	return serial_write( hSerial, &bb, 1 );
}

int get_pribor( HANDLE hSerial )
{
	int to = COM_TIME_OUT;

	while( !serial_rx_ready( HANDLE hSerial ) && --to ) Sleep( 1 );
	if( !to ) return -1; // time out
	return serial_rx( hSerial );
}

int put_pribor( HANDLE hSerial, BYTE bb )
{
	int to = COM_TIME_OUT;

	while( serial_rx( HANDLE hSerial ) != COM_OK && --to ) Sleep( 1 );
	if( !to ) return 0; // time out
	serial_tx( hSerial, bb );
	return 1;
}

BYTE hex2b( char c )
{
	if( c >= '0' && c <= '9' ) return ( c - '0' );
	if( c >= 'A' && c <= 'F' ) return ( c - 'A' + 10 );
	//if( c >= 'a' && c <= 'f' ) return ( c - 'a' + 10 );
	return 0;
}

int write_to_pribor( HANDLE hSerial )
{
	BYTE buf[ 4096 ], bb;
	BYTE *ss = buf;

	fgets( buf, stdin );
	if( buf[0] == 0 || buf[1] == 0 ) return ERR_NO_DATA;

	if( serial_rx( hSerial ) != COM_OK ) return ERR_NO_ELECTRON; // no electron
	serial_tx( hSerial, COM_OK ); // send OK too

	bb = ( hex2b( *ss++ ) << 4 ) | hex2b( *ss++ ); // get command

	if( bb == COM_CLEAR )
	{
		if( !put_pribor( hSerial, COM_CLEAR ) ) return ERR_WRITE; // err write
		return ERR_OK; // ok
	}

	if( bb != COM_WRITE ) return ERR_BAD_COMMAND; // bad command

	if( !put_pribor( hSerial, COM_WRITE ) ) return ERR_WRITE; // err write

	bb = ( hex2b( *ss++ ) << 4 ) | hex2b( *ss++ ); // put n freqs
	if( !put_pribor( hSerial, bb ) ) return ERR_WRITE;
	if( get_pribor( hSerial ) != COM_OK ) return ERR_FULL; // no memory

	while( *ss )
	{
		bb = ( hex2b( *ss++ ) << 4 ) | hex2b( *ss++ );
		if( !put_pribor( hSerial, bb ) ) return ERR_WRITE;
	}

	return ERR_OK;
}

int main( int argc, char *argv[] )
{
	HANDLE hSerial;
	int port;
	char buff[1024];

	while(1)
	{
		gets( buff );
		if( buff[0] == 0 ) break;
		puts( buff );
	}

	if( argc < 2 )
	{
		printf("use com_list.exe n\n");
		return 1;
	}
/*
	if( argc == 3 )
	{
		puts( argv[2] );
		return 1;
	}
*/
	port = atoi( argv[1] );
	hSerial = serial_open( port );

	if( hSerial != INVALID_HANDLE_VALUE )
	{
		serial_close( hSerial );
		printf( "Ok to commect COM%d\n", port );
		return 0;
	}

	printf( "ERROR commect COM%d\n", port );
	return 2;
}
