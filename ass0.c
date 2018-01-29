#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> //optional
#include <sys/inotify.h>
#include <inttypes.h>

//#include <cstdio>
#include <signal.h>
#include <limits.h>
#include <sys/inotify.h>
#include <fcntl.h>
//#include <iostream>

#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
// inode -> change name, modify 1231412
/*
filename, type, time, inode
if delete and create same file:
stay the same:
filename -> same?
diff:
filename + inode -> same?
type&time -> sign

struct name,type,time
struct name,type,time,inode
*/

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int length;
int fd;
int wd;
char buffer[EVENT_BUF_LEN];
//FD_ZERO(&inputs)
//FD_SET(fd,&inputs);

int fstat(int fd, struct stat *statbuf);

void change_reporter(){
    DIR *dir;
    struct dirent *dp;
    char * file_name;
    int file_type;
    dir = opendir(".");

    while ((dp=readdir(dir)) != NULL) {
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
        {
            //printf("wrong_name: \"%s\"\n",file_name);
        }
        else {
            file_name = dp->d_name; // use it
            file_type = dp->d_type;
            printf("file_name: \"%s\" file_type: \"%s\" \n",file_name,
                   (file_type == 4) ?  "directory" :
                   (file_type == 8) ?  "regular" :
                   (1) ? "other" : "???");
        }
    }
		closedir(dir);
}

void reporter(){
  //int check;
///////////////////////////////
  // result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);
  // /***********************////////
  fd_set rfds;
  struct timeval tv;
  int retval;
  int i = 0;

  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  tv.tv_sec = 0;
  tv.tv_usec = 10;

  retval = select(fd+1, &rfds, NULL, NULL, &tv);

  

  /************************/
  //check = select()
  //printf("%s\n", buffer);
  if(retval > 0) {
    length = read( fd, buffer, EVENT_BUF_LEN);

  }
  else{
    return;
  }

  /*checking for error*/
  if ( length < 0 ) {
    perror( "read" );
  }


////////////////////////////////
  /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
  while ( i < length ) {struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];     if ( event->len ) {
      if ( event->mask & IN_CREATE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "dNew directory %s create.\n", event->name );
        }
        else {
          printf( "New file %s created.\n", event->name );
        }
      }
      else if ( event->mask & IN_DELETE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "Directory %s deleted.\n", event->name );
        }
        else {
          printf( "File %s deleted.\n", event->name );
        }
      }
      else if ( event->mask & IN_MODIFY ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "Directory %s modified.\n", event->name );
        }
        else {
          printf( "File %s modified.\n", event->name );
        }
      }
      else if ( event->mask & IN_ATTRIB ) {
        printf( "cookie: %" PRIu32 " !!\n", event->cookie );
        if ( event->mask & IN_ISDIR ) {
          printf( "* %s .\n", event->name );
        }
        else {
          printf( "* %s .\n", event->name );
        }
      }
      else if ( event->mask & IN_MOVED_FROM ) {
        printf( "cookie: %" PRIu32 " !!\n", event->cookie );
        if ( event->mask & IN_ISDIR ) {
          printf( "IN_MOVED_FROM %s .\n", event->name );
        }
        else {
          printf( "IN_MOVED_FROM %s .\n", event->name );
        }
      }
      else if ( event->mask & IN_MOVED_TO ) {
        printf( "cookie: %" PRIu32 " !!\n", event->cookie );

        if ( event->mask & IN_ISDIR ) {
          printf( "IN_MOVED_TO %s .\n", event->name );
        }
        else {
          printf( "IN_MOVED_TO %s .\n", event->name );
        }
      }
    }
    i += EVENT_SIZE + event->len;
  }
  /*removing the “/tmp” directory from the watch list.*/
   //inotify_rm_watch( fd, wd );


  /*closing the INOTIFY instance*/
   //close( fd );
}
static void sig_handler(int signo){

    if (signo == SIGINT){
        system("date");
        reporter();
				exit(1);
    }
    else if (signo == SIGALRM){
        system("date");
        reporter();
				signal (SIGALRM, sig_handler);
    }
    else if (signo == SIGUSR1){
        system("date");
        reporter();
        signal(SIGUSR1, sig_handler);
    }
		/*reset handler reason
		https://cboard.cprogramming.com/linux-programming/150239-when-re-enable-signal-handlers.html
		The signal() function defines the handling of the next received signal only,
		after which the default handling is reinstated.
		So it is necessary for the signal handler to call signal()
		if the program needs to continue handling signals using a non-default handler.
		*/
}

int main(int argc, char *argv[])
{
    int period = atoi(argv[1]);

		signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sig_handler);
		signal(SIGINT, sig_handler);
		system("date");
		//change_reporter();
    
    
    
    while(1)
    {
      // init inotify////
      // begin watching for 5 sec
      fd = inotify_init();
      
      if ( fd < 0 ) {
        perror( "inotify_init" );
      }

      wd = inotify_add_watch( fd, argv[2], IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO | IN_ATTRIB);
      /*************************/
        alarm(period);
        sleep(period);
		}
    return 0;
}

