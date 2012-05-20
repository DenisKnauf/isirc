#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXREAD 1024

int scan( const char *key, const char *arg) {
	int i = 0;
	for( i = 0; key[i] && arg[i] && key[i] == arg[i]; i++);
	if( key[i])
		return 0;
	switch( arg[i]) {
		case '=':
			puts( arg+i+1);
		case 0:
			return 1;
	}
	return 0;
}

#define IFS( X) ((X) == 0 || (X) == ' ' || (X) == '\t' || (X) == '\n' || (X) == '\r')
#define BUFREAD( I, S, D, F, M, E) \
	if( !( ++i < s)) { \
		if( -1 == (s = read( fd, d, MAXREAD))) \
			goto ERROR; \
		if( !s) \
			break; \
		i = 0; \
	}

int scan_fd( char *key, int fd, char **val) {
	ssize_t s = 0;
	char *d = malloc( MAXREAD);
	int i = 0, j, ret = 0;

	if( -1 == (s = read( fd, d, MAXREAD)))
		goto ERROR;
	while( s) {
		// liest solange ein leeres zeichen da ist.
		while( IFS( d[i]))
			BUFREAD( i, s, d, fd, MAXREAD, ERROR);
		// vergleicht das naechste wort mit dem schluessel.
		j = 0;
		while( key[j] && d[i] == key[j]) {
			j++;
			BUFREAD( i, s, d, fd, MAXREAD, ERROR);
		}
		// wenn das letzte zeichen vom schluessel erreicht wurde,
		// stimmt der schluessel mit dem gescanten wort ueberein.
		if( !key[j] && ( d[i] == '=' || IFS( d[i])))
			goto FOUND;
		while( !IFS( d[i]))
			BUFREAD( i, s, d, fd, MAXREAD, ERROR);
	}
	free( d);
	return 0;

FOUND:
	// key gefunden und der wert wird ausgegeben.
	if( d[i] == '=') {
		char *p;
		i++;
		for( j = 0; i+j < s && !IFS( d[i+j]); j++);
		if( !(p = malloc( j+1)))
			goto ERROR;
		memcpy( p, d+i, j);
		i = 0;
		if( !IFS( d[i+j])) {
			if( -1 == (s = read( fd, d, MAXREAD)))
				goto ERROR;
			if( s) {
				while( !IFS( d[i]) && s) {
					if( !( ++i < s)) {
						if( !(p = realloc( p, j+i+1)))
							goto ERROR;
						memcpy( p+j, d, i);
						j += i;
						if( -1 == (s = read( fd, d, MAXREAD)))
							goto ERROR;
						if( !s)
							break;
					}
				}
				if( !(p = realloc( p, j+i+1)))
					goto ERROR;
				memcpy( p+j, d, i);
			}
		}
		p[j+i] = 0;
		*val = p;
		ret = 1;
	}
	// key gefunden, hat aber keinen wert, der ausgegeben werden muss.
	else
		ret = 2;
	free( d);
	return ret;

ERROR:
	free( d);
	return -1;
}

extern int cmdline_main( int argc, char **argv) {
	int i = 0, action = 'a', fd = 0, ret = 255;
	char c, *key = NULL, *val, *file = "/proc/cmdline";

	while( (c = getopt( argc, argv, "+hcf:")) != -1)
		switch( c) {
			case 'h':
				fprintf( stderr, "usage: %s [-c] [-f file] keyword [args...]\n", argv[0]);
				fprintf( stderr, "parse the linux-like cmdline, which are given via args or:\n");
				fprintf( stderr, "\t-c: cmdline;  -f file: file. file can be '-'.\n");
				exit( 0);
			case 'f': file = optarg;
			case 'c': action = 'f';
		}
	if( !( argc > optind))
		return 3;
	key = argv[optind];

	switch( action) {
		case 'a':
			for( i = optind+1; i < argc; i++)
				if( scan( key, argv[i])) {
					ret = 0;
					break;
				}
			break;
		case 'f':
			if( !( file[0] == '-' && file[1] == 0))
				if( -1 == (fd = open( file, O_RDONLY))) {
					perror( argv[0]);
					ret = 2;
				}
			switch( scan_fd( key, fd, &val)) {
				case 1:
					puts( val);
				case 2:
					ret = 0;
					break;
				case 0:
					ret = 1;
					break;
				case -1:
					ret = 2;
					perror( argv[0]);
			}
			if( !fd)
				close( fd);
	}

	return ret;
}
