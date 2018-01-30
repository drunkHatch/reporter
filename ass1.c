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

static char *path;
static int period;
typedef struct{
  char name[256];
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
DIR *dir;

int old_dir_num = 0; //old directory file #
int new_dir_num = 0; // new directory file #
int temp_dir_num = 0; // temp directory file #
FINFO *old_file_info; // old files info
FINFO *new_file_info; // new files info
FINFO *temp_file_info; // temp files info, w8 to be transfered to new_file_info
FINFO *change_file_info; // temp files info, w8 to be transfered to new_file_info

FINFO get_member(FINFO *dir_list, char *file_name, int length){
  FINFO fail = {"miss", -1, 0};
  for (int i = 0; i < length; i++) {
    if (strcmp(dir_list[i].name, file_name) == 0){
      return dir_list[i];
    }
  }
  return fail;
}


int dir_grabber(DIR *dir){
    struct dirent *dp;
    char * file_name;
    int file_type;
    ino_t file_inode;
    int file_num = 0;
    FINFO current_node;

    while ((dp=readdir(dir)) != NULL) {
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )  {}
        else {
            file_num++;
            strcpy(current_node.name, dp->d_name);
            current_node.type = dp->d_type;
            current_node.inode = dp->d_ino;
            temp_file_info = (FINFO *)realloc(temp_file_info, file_num * FILE_INFO_LEN);
            temp_file_info[file_num - 1] = current_node;
        }
    }
    temp_dir_num = file_num;
    return file_num;
}

void reporter(){
  int i = 0;
  FINFO old;
  FINFO new;

  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 10;
  retval = select(fd+1, &rfds, NULL, NULL, &tv);

  if(retval > 0) {
    length = read( fd, buffer, EVENT_BUF_LEN);
  }
  else{
    return;
  }

  if ( length < 0 ) {
    perror( "read" );
  }

  /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
  while ( i < length ) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len ) {
      old = get_member(old_file_info, event->name, old_dir_num);
      new = get_member(new_file_info, event->name, new_dir_num);
      if ( event->mask & IN_CREATE ) {
        if (old.type != -1){// if in old
          if (new.type != -1){//if in old & new
            if (old.type == new.type && old.inode == new.inode){
            }
            else{
              print_crt(new);
            }
          }
          else{//in old but not in new
          }
        }
        else{// not in old
          if (new.type != -1){//if not in old but in new
            print_crt(new);
          }
          else{//not in old and not in new
          }
        }
      }

      else if ( event->mask & IN_DELETE ) {
        if (old.type != -1){// if in old
          if (new.type != -1){//if in old & new
            if (old.type == new.type && old.inode == new.inode){
            }
            else{
              print_del(old);
            }
          }
          else{//in old but not in new
            print_del(old);
          }
        }
        else{// not in old
          if (new.type != -1){//if not in old but in new
          }
          else{//not in old and not in new
          }
        }
      }
      else if ( event->mask & IN_MODIFY ) {
        if (old.type != -1){// if in old
          if (new.type != -1){//if in old & new
            if (old.type == new.type && old.inode == new.inode){
              print_mod(new);
            }
            else{
            }
          }
          else{//in old but not in new
          }
        }
        else{// not in old
          if (new.type != -1){//if not in old but in new
          }
          else{//not in old and not in new
          }
        }
      }
      else if ( event->mask & IN_ATTRIB ) {
        if (old.type != -1){// if in old
          if (new.type != -1){//if in old & new
            if (old.type == new.type && old.inode == new.inode){
              print_mod(new);
            }
            else{
            }
          }
          else{//in old but not in new
          }
        }
        else{// not in old
          if (new.type != -1){//if not in old but in new
          }
          else{//not in old and not in new
          }
        }
      }
      else if ( event->mask & IN_MOVED_FROM ) {
        if (old.type != -1){// if in old
          if (new.type != -1){//if in old & new
            if (old.type == new.type && old.inode == new.inode){
            }
            else{
              print_del(old);
            }
          }
          else{//in old but not in new
            print_del(old);
          }
        }
        else{// not in old
          if (new.type != -1){//if not in old but in new
          }
          else{//not in old and not in new
          }
        }
      }
      else if ( event->mask & IN_MOVED_TO ) {
        if (old.type != -1){// if in old
          if (new.type != -1){//if in old & new
            if (old.type == new.type && old.inode == new.inode){
            }
            else{
              print_crt(new);
            }
          }
          else{//in old but not in new
          }
        }
        else{// not in old
          if (new.type != -1){//if not in old but in new
            print_crt(new);
          }
          else{//not in old and not in new
          }
        }
      }
    }
    i += EVENT_SIZE + event->len;
  }
}

//#define	SIG_BLOCK     0		 /* Block signals.  */
//#define	SIG_UNBLOCK   1		 /* Unblock signals.  */
//#define	SIG_SETMASK   2		 /* Set the set of blocked signals.  */

static void sig_handler(int signo){
    sigset_t signal_set;

    sigemptyset(&signal_set);

    if (signo == SIGINT){
        sigaddset(&signal_set, SIGALRM);
        sigaddset(&signal_set, SIGUSR1);
        sigprocmask(0, &signal_set, NULL);
        system("date");
        dir = opendir(path);
        if (ENOENT == errno){
      //directory does not exist
          printf("Directory has been deleted!\n");
          exit(1);
    }
        dir_grabber(dir);
        closedir(dir);
        struct_move();
        reporter();
        sigprocmask(1, &signal_set, NULL);
				exit(1);
    }
    else if (signo == SIGALRM){
        sigaddset(&signal_set, SIGINT);
        sigaddset(&signal_set, SIGUSR1);
        sigprocmask(0, &signal_set, NULL);
        system("date");
        dir = opendir(path);
        if (ENOENT == errno){
      //directory does not exist
          printf("Directory has been deleted!\n");
          exit(1);
    }
        dir_grabber(dir);
        closedir(dir);
        struct_move();
        reporter();
				signal (SIGALRM, sig_handler);
        alarm(period);
        sigprocmask(1, &signal_set, NULL);

    }
    else if (signo == SIGUSR1){
        sigaddset(&signal_set, SIGALRM);
        sigaddset(&signal_set, SIGINT);
        sigprocmask(0, &signal_set, NULL);
        system("date");
        dir = opendir(path);
        if (ENOENT == errno){
      //directory does not exist
          printf("Directory has been deleted!\n");
          exit(1);
    }
        dir_grabber(dir);
        closedir(dir);
        struct_move();
        reporter();
        signal(SIGUSR1, sig_handler);
        sigprocmask(1, &signal_set, NULL);

    }
		/*reset handler reason////
		https://cboard.cprogramming.com/linux-programming/150239-when-re-enable-signal-handlers.html
		The signal() function defines the handling of the next received signal only,
		after which the default handling is reinstated.
		So it is necessary for the signal handler to call signal()
		if the program needs to continue handling signals using a non-default handler.
		*//////////
}
int main(int argc, char *argv[])
{
    int leng = 0;
    period = atoi(argv[1]);
    path = argv[2];

		signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sig_handler);
		signal(SIGINT, sig_handler);


    // init all struct array with null pointer
    old_file_info = (FINFO *)malloc(0);
    new_file_info = (FINFO *)malloc(0);
    temp_file_info = (FINFO *)malloc(0);
    change_file_info = (FINFO *)malloc(0);

    // init for first report
    dir = opendir(path);
    if (ENOENT == errno){
      //directory does not exist
      printf("Directory does not exist!\n");
      exit(1);
    }
    system("date");
		leng = dir_grabber(dir);
    struct_move();
    print_dir(new_file_info, new_dir_num);
    closedir(dir);

    fd = inotify_init();
    wd = inotify_add_watch( fd, path, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO | IN_ATTRIB);
    if ( fd < 0 ) {
      perror( "inotify_init" );
    }
    alarm(period);
    while(1){
      ;
    }
    return 0;
}



void struct_copy(FINFO *dst, FINFO *src,int length){
  for (int i = 0; i < length; i++) {
      dst[i].type = src[i].type;
      dst[i].inode = src[i].inode;
      strcpy(dst[i].name, src[i].name);
  }
}

void print_dir(FINFO *dir, int length){
  for (int i = 0; i < length; i++) {
    printf("\"%s\" \"%s\"\n",
           (dir[i].type == 4) ?  "d" :
           (dir[i].type == 8) ?  "+" :
           (1) ? "o" : "???",dir[i].name);
  }
}

void print_crt(FINFO a_file){
  printf("\"%s\" \"%s\"\n",
         (a_file.type == 4) ?  "d" :
         (a_file.type == 8) ?  "+" :
         (1) ? "o" : "???",a_file.name);
}

void print_del(FINFO a_file){
  printf("\"%s\" \"%s\"\n",
         (a_file.type == 4) ?  "-" :
         (a_file.type == 8) ?  "-" :
         (1) ? "-" : "???",a_file.name);
}

void print_mod(FINFO a_file){
  printf("\"%s\" \"%s\"\n",
         (a_file.type == 4) ?  "*" :
         (a_file.type == 8) ?  "*" :
         (1) ? "*" : "???",a_file.name);
}

void struct_move(){
  old_file_info = (FINFO *)realloc(old_file_info, new_dir_num * FILE_INFO_LEN);
  old_dir_num = new_dir_num;
  struct_copy(old_file_info, new_file_info, new_dir_num);
  new_file_info = (FINFO *)realloc(new_file_info, temp_dir_num * FILE_INFO_LEN);
  new_dir_num = temp_dir_num;
  struct_copy(new_file_info, temp_file_info, new_dir_num);
  temp_file_info = (FINFO *)realloc(temp_file_info, 0);
  temp_dir_num = 0;
}
