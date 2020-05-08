/*
 * who2.c - read /etc/utmp and list info therein
 *        - suppressses empty records  消除空的记录
 *        - formats time nicely
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>
// #ddefine SHOWHOST
typedef struct utmp utmp ;
void showtime( long );
void show_info(utmp* );
int main(){
	utmp utbufp ; // read info into here 
	int utmpfd ; // read from this descriptor
	if( (utmpfd = open( UTMP_FILE , O_RDONLY) ) == -1 ){
		perror( UTMP_FILE );
		exit(1);
	}
	while( read( utmpfd , &utbufp , sizeof( utbufp ) ) == sizeof( utbufp ) ){
		show_info( &utbufp ) ;
	}
	close( utmpfd );
	return 0;
}
 
void show_info( utmp* utbufp){
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
			printf( "(%s)",utbufp->host); // the host
	#endif
	printf("\n");
}
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
