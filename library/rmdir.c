#include "sifs_helper.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  rmdir (v1.1)

// remove an existing directory from an existing volume
int SIFS_rmdir(const char *volumename, const char *dirname)
{
    // SIFS_errno	= SIFS_ENOTYET;
    char **path_list = 0;
    int path_length = 0;
    split_path(dirname, &path_list, &path_length); // splits ath
    assert(path_length >= 1); // path can't be less than 1

    int cur_error = SIFS_EOK; // stores the error
    //can not remove root dir
    if (path_length == 1)
    {
        cur_error = SIFS_EINVAL;
    }
    else
    {
        VolumeHelper *vh = create_vh(); // creates a volume helper structure
        init_with_file(vh, volumename, "rb+", &cur_error); // saving data from original volume to volume helper
        if (cur_error == SIFS_EOK) // considering there are no errors
        {
            int res_block_idx = -1;
            int res_parent_block_idx = -1;
            int res_entry_idx_in_parent_block = -1;
            // searches whether the directories being looked for exists and if does then return it
            void *dir = search_deep(vh, path_list, path_length, &res_block_idx, &res_parent_block_idx, &res_entry_idx_in_parent_block,1);
            if (dir) // if directory exists
            {
                if (vh->bitmap[res_block_idx] != SIFS_DIR) // correct directory does not exist
                {
                    cur_error = SIFS_ENOENT;
                }
                else
                {
                    SIFS_DIRBLOCK* block = (SIFS_DIRBLOCK*)(dir);
                    if(block->nentries > 0)
                    {
                        // if the directory is not empty
                        cur_error = SIFS_ENOTEMPTY;
                    }
                    else
                    {
                        // deletes the directory and also from the bitmap
                        delete_a_dir(vh, res_block_idx, res_parent_block_idx, res_entry_idx_in_parent_block);
                    }
                    
                }
            }
            else
            {
                cur_error = SIFS_ENOENT;
            }
        }
        free_vh(vh); // free's the memory allocated to volume helper function
    }

    free_list((void **)path_list, path_length); // free the memory used by pathlist
    // checks whether we got any error during the process
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
