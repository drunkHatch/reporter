#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> //optional
#include <sys/inotify.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <sys/time.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )
#define FILE_INFO_LEN ( sizeof(FINFO))

typedef struct{
  char name[256];
  //char *name;
  int type; //0->dir, 1->reg, 2->other
  ino_t inode;
} FINFO;

int length;
int fd;
int wd;
fd_set rfds;
struct timeval tv;
int retval;
char buffer[EVENT_BUF_LEN];

int old_dir_num; //old directory file #
int new_dir_num; // new directory file #
FINFO *old_file_info; // old files info
FINFO *new_file_info; // new files info
FINFO *temp_file_info; // temp files info, w8 to be transfered to new_file_info

int change_reporter(DIR *dir){
    struct dirent *dp;
    char * file_name;
    int file_type;
    ino_t file_inode;
    int file_num = 0;
    FINFO current_node;

    while ((dp=readdir(dir)) != NULL) {
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
        {
            //printf("wrong_name: \"%s\"\n",file_name);
        }
        else {
            file_num++;
            //file_name = dp->d_name; // use it
            //file_type = dp->d_type;
            //file_inode = dp->d_ino;
            strcpy(current_node.name, dp->d_name)
            current_node.name = dp->d_name;
            current_node.type = dp->d_type;
            current_node.inode = dp->d_ino;
            temp_file_info = (FINFO *)realloc(temp_file_info, file_num * FILE_INFO_LEN);
            /*printf("file_name: \"%s\" file_type: \"%s\" \n",file_name,
                   (file_type == 4) ?  "directory" :
                   (file_type == 8) ?  "regular" :
                   (1) ? "other" : "???");*/
        }
    }

    return file_num;
}

void reporter(){
  int i = 0;
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  tv.tv_sec = 0;
  tv.tv_usec = 10;

  retval = select(fd+1, &rfds, NULL, NULL, &tv);

  /************************///
  if(retval > 0) {
    printf( "%d\n", retval);

    length = read( fd, buffer, EVENT_BUF_LEN);

  }
  else{
    //printf( "%d\n", retval);
    return;
  }

  /*checking for error*////////
  if ( length < 0 ) {
    perror( "read" );
  }
  /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
  while ( i < length ) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];     if ( event->len ) {

      if ( event->mask & IN_CREATE ) {
        if ( event->mask & IN_ISDIR ) {
          printf( "New directory %s created.\n", event->name );
        }
        else {///////
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
        if ( event->mask & IN_ISDIR ) {
          printf( "IN_MOVED_FROM %s .\n", event->name );
        }
        else {
          printf( "IN_MOVED_FROM %s .\n", event->name );
        }
      }
      else if ( event->mask & IN_MOVED_TO ) {
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
		/*reset handler reason////
		https://cboard.cprogramming.com/linux-programming/150239-when-re-enable-signal-handlers.html
		The signal() function defines the handling of the next received signal only,
		after which the default handling is reinstated.
		So it is necessary for the signal handler to call signal()
		if the program needs to continue handling signals using a non-default handler.
		*//////////
}
int main(void)
{
    int leng = 0;
    DIR *dir;

		signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sig_handler);
		signal(SIGINT, sig_handler);
		system("date");
    // init all struct array with null pointer
    new_file_info = (FINFO *)malloc(0);
    old_file_info = (FINFO *)malloc(0);
    temp_file_info = (FINFO *)malloc(0);


    // init for first report

    dir = opendir(".");
		leng = change_reporter(dir);
    closedir(dir);
    for (size_t i = 0; i < leng; i++) {
      /* code */
      printf("%s\n", temp_file_info[i].name);
    }
    // allocate memory for old and new lists

    fd = inotify_init();//////////////////
    wd = inotify_add_watch( fd, ".", IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO | IN_ATTRIB);
    if ( fd < 0 ) {
      perror( "inotify_init" );
    }
    exit(1);
    while(1)
    {

      /*************************/
        alarm(5);
        sleep(5);
		}
    return 0;
}
