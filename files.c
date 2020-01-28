#include <stdio.h>
#include <dirent.h>
#include <string.h>
#define DIM 50

int main(void)
{
    struct dirent *de;  // Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(".");

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }

    char req[DIM], snd[DIM];
    printf("What file do you want to print?\n");
    scanf("%s",&req);

    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL){
            printf("%s\n", de->d_name);
            if(strcmp(de->d_name, req) == 0){
              strcpy(snd, req);
            };
    };

    if(strcmp(snd, req) == 0){
      printf("File requested: %s\n", req);
      printf("File sent: %s\n", snd);s
    }
    else{
      printf("File not found!\n");
    };

    closedir(dr);
    return 0;
}