#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
//#include <sys/types.h>

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
    if (signo == SIGALRM){
        system("date");
        change_reporter();
				signal (SIGALRM, sig_handler);
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
    //struct sigaction sigaction_usr;
    //sigaction_usr.sa_flags = 0;
    //sigaction_usr.sa_handler = sig_handler;


		signal(SIGALRM, sig_handler);

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
