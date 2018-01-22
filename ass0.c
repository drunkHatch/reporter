#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <bits/sigaction.h>


int sigaction(int signum, const struct sigaction *act,
struct sigaction *oldact);

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
}

static void sig_handler(int signo){
    
    if (signo == SIGINT){
        system("date");
        change_reporter();
    }
    if (signo == SIGALRM){
        system("date");
        change_reporter();
    }
}

int main(void)
{
    struct sigaction sigaction_usr;
    sigaction_usr.sa_flags = 0;
    sigaction_usr.sa_handler = sig_handler;


    sigaction(SIGALRM, &sigaction_usr, NULL);
    
    sigaction(SIGINT, &sigaction_usr, NULL);
    
    while(1)
    {
        alarm(5);
        sleep(5);}
    return 0;
}

