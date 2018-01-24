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

static void sig_handler(int signo){

    if (signo == SIGINT){
        system("date");
        change_reporter();
				exit(1);
    }
    else if (signo == SIGALRM){
        system("date");
        change_reporter();
				signal (SIGALRM, sig_handler);
    }
    else if (signo == SIGUSR1){
        system("date");
        change_reporter();
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

int main(void)
{
		signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sig_handler);
		signal(SIGINT, sig_handler);
		system("date");
		change_reporter();

    while(1)
    {
        alarm(3);
        sleep(3);
		}
    return 0;
}
