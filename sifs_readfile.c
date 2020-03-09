#include <stdio.h>
#include <stdlib.h>
#include "sifs.h"

int main(int argcount, char *argvalue[])
{
    char	*volumename;
    char	*pathname;
    void    *data;
    size_t  nbytes;

    volumename	= argvalue[1];
    pathname	= argvalue[2];

// read the contents of an existing file from an existing volume
int SIFS_readfile(const char *volumename, const char *pathname, void &data, size_t *nbytes);