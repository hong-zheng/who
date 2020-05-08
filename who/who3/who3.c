
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
#include <unistd.h>
#include <sys/types.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>

#define SHOWHOST
typedef struct utmp UTMP;
#define NRECS 16
#define NULLUT ( ( UTMP* )NULL );
#define UTSIZE (sizeof(struct utmp ) )

static char utmpbuf[NRECS * UTSIZE ] ; // storage
static int num_recs ;			// num stored
static int cur_rec ;			// next to go
static int fd_utmp = -1 ;		// read from

int utmp_open( char* filename){
	fd_utmp = open( filename ,O_RDONLY );  // open it
	cur_rec = num_recs = 0 ;
	return fd_utmp ;
}


int utmp_reload(){
/*
 * read next bunch of records into buffer
 */
	int amt_read ;		// read them in 
	amt_read = read( fd_utmp  , utmpbuf , NRECS * UTSIZE ) ;   // how many did we get?
	num_recs = amt_read/UTSIZE ;
	cur_rec = 0 ;
	return num_recs ;
}
void utmp_close(){
	if( fd_utmp != -1 ){
		close( fd_utmp ) ;
	}
}
UTMP* utmpnext(){
	UTMP* recp ;
	if( fd_utmp == -1 )	// error ?
		return NULLUT ;	
	if( cur_rec == num_recs && utmp_reload() == 0 ) // anymore?
		return NULLUT ;
	recp = (UTMP*)&utmpbuf[ cur_rec * UTSIZE ] ;	// get address of next record
	cur_rec++ ;
	return recp ;
}
/*
 * who3.c - who with buffered reads
 *        - suppresses empty records
 *        - formats time nicely
 *        - buffers input ( using utmplib )
 */


void showtime( long timeval ){
/*
 * display time in a format fit for human consumption
 * uses ctime to build a string then picks parts out of it
 * Note: %12.12s prints a string 12 chars wide and LIMITS
 * it to 12 chars
 */
	char* cp ;		// to hold address of time
	cp = ctime( &timeval ) ;// convert time to string
				// string looks like Mon Feb 4 00:46:40 EST 1991
				// `12345678912345
	printf("%12.12s" , cp+4);
}
void show_info( UTMP* utbufp){
	if( utbufp->ut_type !=  USER_PROCESS ){ // utmp 的结构成员ut_type ，当它的值为7(USER_PROCESSES)时，
						// 表示这是一个已经登录的用户，根据这个可以消除who1.c的空白行
		return ;
	}
	printf( "%-8.8s" , utbufp->ut_name ) ; // the logname
	printf( " " ) ;
	printf( "%-8.8s",  utbufp->ut_line ) ; // the tty
	printf( " " ) ;
	showtime( utbufp->ut_time ) ;
	#ifdef SHOWHOST
		if( utbufp->ut_host[0] != '\0' )
			printf( "(%s)",utbufp->ut_host); // the host
	#endif
	printf("\n");
}



int main(){
	UTMP* utbufp ,  // holds pointer to next rec
	    * utmp_next ; // return pointer to next
	if( utmp_open( UTMP_FILE ) == -1 ){
		perror( UTMP_FILE );
		exit( 1 );
	}
	while( (utbufp = utmpnext() ) != ((UTMP*)NULL) )
		show_info( utbufp );
	utmp_close();
	return 0 ;
}
