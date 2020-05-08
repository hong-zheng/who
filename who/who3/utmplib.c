/*
 * utmplib.c -functions to buffer reads from utmp file
 *
 * 	functions are
 * 		utmp_open(filename) -open file
 * 			return -1 on error
 * 		utmp_next()	    - return pointer to next
 * 			return NULL in EOF
 * 		utmp_close()	    - close file
 *
 * 	reads NRECS per read and then doles them out from the buffer
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>
typedef struct utmp UTMP;
#define NRECS 16
#define NULLUT ( ( UTMP* )NULL );
#define UTSIZE (sizeof(UTMP))

static char utmpbuf[NRECS * UTSIZE ] ; // storage
static int num_recs ;			// num stored
static int cur_rec ;			// next to go
static int fd_utmp = -1 ;		// read from

int utmp_open( char* filename){
	fd_utmp = fopen( filename ,O_RDONLY ) // open it
	cur_rec = num_recs = 0 ;
	return fd_utmp ;
}


int utmp_reload(){
/*
 * read next bunch of records into buffer
 */
	int amt_read ;		// read them in 
	amt_read = read( fd_tmp  , utmpbuf , NRECS * UTSIZE ) ;   // how many did we get?
	num_recs = amt_read/UTSIZE ;
	cur_rec = 0 ;
	return num_recs ;
}
void utmp_close(){
	if( fd_utmp != -1 ){
		fclose( fd_utmp ) ;
	}
}
UTMP* utmp_next(){
	UTMP* recp ;
	if( fd_utmp == -1 )	// error ?
		return NULLUT ;	
	if( cur_rec == num_recs && utmp_reload() == 0 ) // anymore?
		return NULLUT ;
	recp = (UTMP*)&utmpbuf[ cur_rec * UTSIZE ] ;	// get address of next record
	cur_rec++ ;
	return recp ;
}	
}
 












