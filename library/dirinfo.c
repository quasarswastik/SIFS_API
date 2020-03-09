#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "sifs_helper.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  dirinfo (v1.1)

// get information about a requested directory
int SIFS_dirinfo(const char *volumename, const char *pathname,
                 char ***entrynames, uint32_t *nentries, time_t *modtime)
{
    char **path_list = 0;
    int path_length = 0;
    split_path(pathname, &path_list, &path_length); // splits the path and adds it to path list along with path_length
    
    // check if the length of directory name is larger than what it is allowed
    assert(path_length >= 1);

    int cur_error = SIFS_EOK; // there are no errors at the moment (temp location to store the error)

        VolumeHelper *vh = create_vh(); // creating a temporary volume to store necesary information
        init_with_file(vh, volumename, "rb", &cur_error); // saving data from original volume to new volume
        char *bitmap = vh->bitmap;
        void **block_list = vh->block_list;
        if (cur_error == SIFS_EOK) // if there is no error
        {
            int res_block_idx = -1;
            int res_parent_block_idx = -1;
            int res_entry_idx_in_parent_block = -1;
            // searches whether the directories being looked for exists and if does then return it
            SIFS_DIRBLOCK *dir = (SIFS_DIRBLOCK *)search_deep(vh, path_list, path_length, &res_block_idx, &res_parent_block_idx, &res_entry_idx_in_parent_block, 1);
            if (dir) // if we get valid directory details
            {
                if (bitmap[res_block_idx] != SIFS_DIR) // if bitmap corresponds to not a directory
                {
                    cur_error = SIFS_ENOENT;
                }
                else
                {
                    // setting the variables
                    *nentries = dir->nentries;
                    *modtime = dir->modtime;
                    // allocating space based on the number of entries expected
                    *entrynames = (char **)malloc(sizeof(char *) * (*nentries));
                    // looping through the entries
                    for (int i = 0; i < dir->nentries; i++)
                    {
                        int idx = dir->entries[i].blockID;
                        get_ith_block(vh, idx); // gets everything in the block
                        assert(bitmap[idx] == SIFS_DIR || bitmap[idx] == SIFS_FILE); // we want it either of thse
                        // if the resulting block has a directory
                        if (bitmap[idx] == SIFS_DIR)
                        {
                            SIFS_DIRBLOCK *idir = (SIFS_DIRBLOCK *)block_list[idx]; // stores all info of dir block
                            (*entrynames)[i] = clone_string(idir->name); // copies name of file to entry name variable
                        }
                        else // if the resulting block is file
                        {
                            SIFS_FILEBLOCK *ifile = (SIFS_FILEBLOCK *)block_list[idx]; // store all info of the file block
                            int fidx = dir->entries[i].fileindex; // gets the index of file found
                            (*entrynames)[i] = clone_string(ifile->filenames[fidx]); // copes the file name
                        }
                    }
                }
            }
            else
            {
                // directory does not exist
                cur_error = SIFS_ENOENT;
            }
        }
        // free the memory used by volume helper structure
        free_vh(vh);

    free_list((void **)path_list, path_length); // free the memory used by 
    
    // checks if there were any errors during the run and if not returns 0
    if (cur_error != SIFS_EOK)
    {
        SIFS_errno = cur_error;
        return 1;
    }
    else
    {
        return 0;
    }
}

