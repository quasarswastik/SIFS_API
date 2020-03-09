#include "sifs_helper.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  mkdir (v1.1)

// make a new directory within an existing volume
int SIFS_mkdir(const char *volumename, const char *dirname)
{
    char** path_list; // stores the hierarchy as a list of strings
    int path_length = 0; // number of herarchy position
    split_path(dirname, &path_list, &path_length); // splits the path and adds it to path list along with path_length
    
    // Checks if a path is provided
    if (path_length < 1){
        SIFS_errno = SIFS_EINVAL;
        return 1;
    }

    SIFS_errno = SIFS_EOK; // there are no errors at the moment (temp location to store the error)

    // Cannot make root dir again
    if(path_length == 1)
    {
        SIFS_errno = SIFS_EINVAL;
        return 1;
    }

    // check if the length of directory name is larger than what it is allowed
    else if(strlen(path_list[path_length-1]) + 1 >= SIFS_MAX_NAME_LENGTH)
    {
        SIFS_errno = SIFS_EINVAL;
        return 1;
    }

    // moving ahead as all the error are cleared
    else
    {
        VolumeHelper* vh = create_vh(); // creates a temporary new volume
        init_with_file(vh, volumename, "rb+", &SIFS_errno); // saving data from original volume to new volume
        if(SIFS_errno == SIFS_EOK) // if there were no errors in previous function
        {
            int res_block_idx = -1;
            int res_parent_block_idx =-1;
            int res_entry_idx_in_parent_block = -1;
            void* parent_dir = search_deep(vh, path_list, path_length-1, &res_block_idx, &res_parent_block_idx, &res_entry_idx_in_parent_block, 1); // checks whether the directory exist
            if(parent_dir) // if we got a valid parent directory
            {
                create_a_dir(vh, res_block_idx, path_list[path_length-1], &SIFS_errno); // creates a new directory
            }
            else // if there does not exist the parent directory we are looking for
            {
                SIFS_errno = SIFS_ENOENT;
                return 1;
            }
        }
        free_vh(vh); // freeing the temporary volume
    }
    
    free_list((void**)path_list, path_length); //freeing the result list
    return 0;
}
