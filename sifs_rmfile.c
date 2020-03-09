#include <stdio.h>
#include <stdlib.h>
#include "sifs.h"

int main(int argcount, char *argvalue[])
{
    char	*volumename;
    char	*dirname;
    volumename	= argvalue[1];
    dirname	= argvalue[2];
    SIFS_rmfile(volumename, dirname);
}

