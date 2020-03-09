#include "sifs_helper.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  writefile (v1.1)

// add a copy of a new file to an existing volume
int SIFS_writefile(const char *volumename, const char *pathname, void *data, size_t nbytes)
{
    char** path_list = 0;
    int path_length = 0;
    split_path(pathname, &path_list, &path_length); // splits the path

    int cur_error = SIFS_EOK;

    // the file must have a name therefore it is invalid if we don't provide one
    if(path_length <= 1)
    {
        cur_error = SIFS_EINVAL;
    }
    // checking if the name provided by the user is not too big to handle
    else if(strlen(path_list[path_length-1]) + 1 >= SIFS_MAX_NAME_LENGTH)
    {
        cur_error = SIFS_EINVAL;
    }
    else
    {
        VolumeHelper* vh = create_vh(); // creates a volume helper structure
        init_with_file(vh, volumename, "rb+", &cur_error); // saving data from original volume to volume helper
        if(cur_error == SIFS_EOK) // if there are no errors
        {
            int res_block_idx = -1;
            int res_parent_block_idx =-1;
            int res_entry_idx_in_parent_block = -1;
            // searches whether the directories being looked for exists and if does then return it
            void* parent_dir = search_deep(vh, path_list, path_length-1, &res_block_idx, &res_parent_block_idx, &res_entry_idx_in_parent_block, 1);
            if(parent_dir) // if the parent directory to new file exists
            {
                int res_block_idx_tmp = -1;
                int res_parent_block_idx_tmp =-1;
                int res_entry_idx_in_parent_block_tmp = -1;
                // checks if the file already exists
                void* exist_file = search_deep(vh, path_list, path_length, &res_block_idx_tmp, &res_parent_block_idx_tmp, &res_entry_idx_in_parent_block_tmp, 0);
                if(exist_file) // file shouldn't already exist
                {
                    cur_error = SIFS_EEXIST;
                }
                else
                {
                    // creates a new file
                    create_a_file(vh, res_block_idx, path_list[path_length-1], &cur_error);
                }
            }
            else
            {
                // if the parent directory cannot be found
                cur_error = SIFS_ENOENT;
            }
        }
        // free the memory allocated to volume helper structure
        free_vh(vh);
    }
    
    free_list((void**)path_list, path_length); // free the path list
    if(cur_error != SIFS_EOK)
    {
        // check if we accumulated any errors while operations and exit
        SIFS_errno = cur_error;
        return 1;
    }
    else
    {
        // all good!
        return 0;
    }
}
