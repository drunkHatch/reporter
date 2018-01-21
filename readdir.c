#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>

// use macro in included library
//failed in c99

int main()
{
    DIR *dir;
    struct dirent *dp;
    char * file_name;
    int file_type;
    dir = opendir(".");

    while ((dp=readdir(dir)) != NULL) {
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
        {
            //printf("wrong_name: \"%s\"\n",file_name);
        } else {
            file_name = dp->d_name; // use it
            file_type = dp->d_type;
            printf("file_name: \"%s\" file_type: \"%s\" \n",file_name,
                             (file_type == 4) ?  "directory" :
                             (file_type == 8) ?  "regular" :
                             (1) ? "other" : "???");
        }
    }
    closedir(dir);
    return 0;
}
