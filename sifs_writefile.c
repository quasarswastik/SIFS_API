#include <stdio.h>
#include <stdlib.h>
#include "sifs.h"

int main(int argcount, char *argvalue[])
{
    char	*volumename;
    char	*dirname;
    char    *name;
    void    *data;
    size_t  nbytes;

    volumename	= argvalue[1];
    dirname	= argvalue[2];
    name = argvalue[3];

    FILE *vol = fopen(name, "rb"); // FILE POINTER DECLARATION
    fseek (vol, 0, SEEK_END); // SEEK TO END
    nbytes = ftell(vol); // CALCULATE SIZE
    fseek(vol, 0, SEEK_SET); // SEEK TO BEGINNING
    data = malloc(nbytes); // ALLOCATE MEMORY FOR DATA
    fread(data, nbytes, 1, vol); // PUT STULL IN DATA FROM VOLUME

    //for (int i = 0 ; i < nbytes ; ++i)
    //    printf("%c", ((char* ) data)[i]);

    SIFS_writefile(volumename, dirname, data, nbytes);
}

