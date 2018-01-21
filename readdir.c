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
    //char file_type;
    int file_type;
    dir = opendir("."); // what is it for?

    while ((dp=readdir(dir)) != NULL) {
        //printf("debug: %s\n", dp->d_name);
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
        {
            printf("wrong_name: \"%s\"\n",file_name);
        } else {
            file_name = dp->d_name; // use it
            file_type = dp->d_type;
            //printf("file_name: \"%s\" file_type: \"%u\"\n",file_name, file_type);
            /*
            DT_UNKNOWN = 0,
            DT_FIFO = 1,
            DT_CHR = 2,
            DT_DIR = 4,
            DT_BLK = 6,
            DT_REG = 8,
            DT_LNK = 10,
            DT_SOCK = 12,
            DT_WHT = 14
            */
            printf("file_name: \"%s\" file_type: \"%s\" \n",file_name,
                             (file_type == 0) ?  "unknown" :
                             (file_type == 1) ? "FIFO" :
                             (file_type == 2) ?  "char dev" :
                             (file_type == 4) ?  "directory" :
                             (file_type == 6) ?  "block dev" :
                             (file_type == 8) ?  "regular" :
                             (file_type == 10) ?  "symlink" :
                             (file_type == 12) ? "socket" :
                             (file_type == 14) ?  "DT_WHT" : "???");
        }
    }
    closedir(dir);

    //printf("_DIRENT_HAVE_D_TYPE: \"%u\"\n", _DIRENT_HAVE_D_TYPE);
    /*
    printf("DT_REG: \"%u\"\n", DT_REG);
    printf("DT_DIR: \"%u\"\n", DT_DIR);
    printf("DT_FIFO: \"%u\"\n", DT_FIFO);
    printf("DT_SOCK: \"%u\"\n", DT_SOCK);
    printf("DT_CHR: \"%u\"\n", DT_CHR);
    printf("DT_BLK: \"%u\"\n", DT_BLK);
    printf("DT_LNK: \"%u\"\n", DT_LNK);
    */

    return 0;
}
