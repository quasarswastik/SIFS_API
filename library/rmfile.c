#include "sifs_helper.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  rmfile (v1.1)

// remove an existing file from an existing volume
int SIFS_rmfile(const char *volumename, const char *pathname)
{
    char **path_list = 0;
    int path_length = 0;
    split_path(pathname, &path_list, &path_length); // splits the path

    int cur_error = SIFS_EOK;
    // since the root diretory is not a file therefore the path length shoud be greater than 1
    if (path_length <= 1)
    {
        cur_error = SIFS_EINVAL;
    }
    else
    {
        VolumeHelper *vh = create_vh(); // creates a volume helper structure
        init_with_file(vh, volumename, "rb+", &cur_error); // saving data from original volume to volume helper
        if (cur_error == SIFS_EOK) // if there are no errors
        {
            int res_block_idx = -1;
            int res_parent_block_idx = -1;
            int res_entry_idx_in_parent_block = -1;
            // searches whether the file being looked for exists
            SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK*)search_deep(vh, path_list, path_length, &res_block_idx, &res_parent_block_idx, &res_entry_idx_in_parent_block,0);
            if (file) // there exists a file
            {
                int res_block_idx_dir = -1;
                int res_parent_block_idx_dir = -1;
                int res_entry_idx_in_parent_block_dir = -1;
                // searches the directry where the file is saved
                SIFS_DIRBLOCK *dir = (SIFS_DIRBLOCK*)search_deep(vh, path_list, path_length-1, &res_block_idx_dir, &res_parent_block_idx_dir, &res_entry_idx_in_parent_block_dir,1);
                assert(dir); // the directory must exist
                assert(res_parent_block_idx == res_block_idx_dir); // the directory received must be the parent directory
                uint32_t old_file_index = dir->entries[res_entry_idx_in_parent_block].fileindex; //get the file index  of the file
                delete_file_index(vh, res_block_idx, old_file_index); // remove the file
                delete_idx_entry(vh, res_block_idx_dir, res_entry_idx_in_parent_block); // remove the entry of file from the directory

                // make changes in actual volume
                // go through the bitmap by going from 0 to toatal blocks
                for(int i=0; i<vh->header->nblocks; i++)
                {
                    // check from the bitmap if we have a directory
                    if(vh->bitmap[i] == SIFS_DIR)
                    {
                        SIFS_DIRBLOCK* dir = (SIFS_DIRBLOCK*)get_ith_block(vh, i); //get one of the directory blocks
                        assert(dir); // it must be a successful operation
                        int change = 0;
                        for(int j=0; j<dir->nentries; j++)
                        {
                            if(dir->entries[j].blockID == res_block_idx)
                            {
                                // the file index should change
                                assert(dir->entries[j].fileindex !=  old_file_index);
                                // the file is to be removed
                                if(dir->entries[j].fileindex > old_file_index)
                                {
                                    dir->entries[j].fileindex--;
                                    change = 1;
                                }
                            }
                        }
                        // if a file is removed make change to the volume
                        if(change)
                        {
                            write_block_to_file(vh, i);
                        }
                    }
                }

                //try to clean up the file, if file reference is 0.
                if(file->nfiles == 0)
                {
                    // delete the file blockas well
                    delete_file_block(vh,res_block_idx, file->firstblockID, ((file->length*1.0)-1/vh->header->blocksize+1));
                }
            }
            else
            {
                // if the file couldn't be found
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
