#include "sifs_helper.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  fileinfo (v1.1)

// get information about a requested file
int SIFS_fileinfo(const char *volumename, const char *pathname,
		  size_t *length, time_t *modtime)
{
char **path_list = 0;
    int path_length = 0;
    split_path(pathname, &path_list, &path_length);

    int cur_error = SIFS_EOK;
    // since the root diretory is not a file therefore the path length shoud be greater than 1
    if (path_length <= 1)
    {
        cur_error = SIFS_EINVAL;
    }
    else
    {
        VolumeHelper *vh = create_vh(); // creates a volume helper structure
        init_with_file(vh, volumename, "rb", &cur_error); // saving data from original volume to volume helper
        if (cur_error == SIFS_EOK) // if there are no errors
        {
            int res_block_idx = -1;
            int res_parent_block_idx = -1;
            int res_entry_idx_in_parent_block = -1;
            // searches whether the file being looked for exists and saves it
            SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK*)search_deep(vh, path_list, path_length, &res_block_idx, &res_parent_block_idx, &res_entry_idx_in_parent_block,0);
            if (file) // there exists a file
            {
                *length = file->length;
                *modtime = file->modtime;
            }
            else
            {
                // file doesn't exist
                cur_error = SIFS_ENOENT;
            }
        }
        free_vh(vh); // free's the memory allocated to volume helper function
    }

    free_list((void **)path_list, path_length); // free the memory used by pathlist
    if (cur_error != SIFS_EOK)
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
