#include "sifs_helper.h"
#include "md5.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  sifs_helper (v1.1)

/*
    frees the memory of what the second rank pointer points to (clears list of directory)
*/
void free_list(void **result_list, int result_length)
{
    for (int i = 0; i < result_length; i++)
    {
        free(result_list[i]);
    }
    free(result_list);
}

/*
    COPIES THE DIRECTORY LEVELS TO TEMPORARY LOCATIONS
*/
char *clone_string(const char *str)
{
    int len = strlen(str);
    // allocate "sizeof(char) * (len + 1)" amount of memory
    char *res = (char *)malloc(sizeof(char) * (len + 1));
    res[len] = 0;
    // copies string to res
    if (len > 0)
        memcpy(res, str, len);
    return res;
}

/*
    concatenate two strings and return the pointer pointes to the cocatenated string
*/
char *cat_string(const char *s1, const char *s2)
{
    // if there are no strings (root directory) so no string modifications needed
    if (!(s1) && !(s2))
    {
        return 0;
    }

    // if there is only first string
    else if (!s2)
    {
        return clone_string(s1);
    }

    // if there is only second string
    else if (!s1)
    {
        return clone_string(s2);
    }

    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int len = len1 + len2;
    char *res = (char *)malloc(len + 1);
    // initialize the res string with 0
    memset(res, 0, len + 1);

    // save directory on the basis of number of hierarchy levels
    if (len1 > 0)
        memcpy(res, s1, len1);
    if (len2 > 0)
        memcpy(res + len1, s2, len2);
    return res;
}

/*
    splits the path on the basis of '/'
*/
int split_path(const char *path, char ***result_list, int *result_length)
{
    int pl = strlen(path);
    char *fix_path = 0;

    // when there is no path(pl == 0) provided or the path is not begun with '/'
    if (pl == 0 || path[0] != '/')
    {
        // manually fix the path by adding a "/" before it to make it a standard path
        fix_path = cat_string("/", path);
    }

    // copy the converted path to fix_path variable
    else
    {
        fix_path = clone_string(path);
    }

    char *search_start = fix_path;
    int ivol = strlen(fix_path); //calculate the effective(fixed) length of our path
    int i_count_word = 0;
    
    // loop i broken on the basis of occurence of directory path therefore always true
    while (1)
    {
        char *str1 = strstr(search_start, "/"); // locates / in the list
        // if no more / (last directory)
        if (!str1)
            break;
        char *str2 = strstr(str1 + 1, "/");
        i_count_word++;
        // move to next hierarchy level
        if (!str2)
        {
            str2 = fix_path + ivol;
        }
        // if the last element then break the loop
        if (str2 == (fix_path + ivol - 1) || str2 == fix_path + ivol)
        {
            break;
        }
        // change the upper hierachy to next lower hierarchy
        search_start = str2;
    }

    // the total number of levels must e greater than or equal to 1 else there is a problem move out of this file
    assert(i_count_word >= 1);
    *result_length = i_count_word + 1;
    *result_list = (char **)malloc(sizeof(char *) * (*result_length));

    int idx = 1;
    search_start = fix_path;
    while (1)
    {
        // searches for directory division using "/"
        char *str1 = strstr(search_start, "/");
        // break if there is no more division
        if (!str1)
            break;
        char *str2 = strstr(str1 + 1, "/");
        // if it is the uppermost directory
        if (!str2)
        {
            str2 = fix_path + ivol;
        }

        char *begin = str1 + 1;
        char *end = str2;
        // filled the current pointer
        if (begin >= end)
        {
            (*result_list)[idx++] = clone_string("");
        }
        else // directly add
        {
            char buffer[end - begin + 1];
            buffer[end - begin] = 0;
            memcpy(buffer, begin, end - begin);
            (*result_list)[idx++] = clone_string(buffer);
        }

        // complete
        if (str2 == (fix_path + ivol - 1) || str2 == fix_path + ivol)
        {
            break;
        }
        search_start = str2;
    }

    assert(idx == i_count_word + 1);
    // checks for the lit elements
    if (strcmp((*result_list)[1], "") == 0)
    {
        // make the list start from the beginning
        for (int i = 0; i < i_count_word; i++)
        {
            (*result_list)[i] = (*result_list)[i + 1];
        }
        (*result_length)--; // shift is complete and last element is already shifted to correct position
    }
    else // if the string is correct
    {
        (*result_list)[0] = clone_string("");
    }
    free(fix_path); // free the variable

    return 0;
}

/*
    split using strtok
*/
/*int split_path(const char *path, char ***result_list, int *result_length)
{
    char* pathcpy = clone_string(path);
    //char* pathcpy = strdup(path);
    pathcpy = strtok(pathcpy, "/");
    while (pathcpy != NULL) {
        pathcpy = strtok(NULL, "/");
        result_length++;
    }
    printf("%i",*result_length);
    *result_list = (char **)malloc(sizeof(char *) * (*result_length));
    pathcpy = clone_string(path);
    for (int i = 0; i < *result_length; i++) {
        *result_list[i] = pathcpy;
        pathcpy = strtok(NULL,"/");
    }
    printf("%s",*result_list[0]);
    return 0;
}*/

/*
    opens the file in the required mode and return the pointer to this file
    and set the pointer of *error_code point to the error happens in getting this file
    for example: if we want to open volA, it is a file of volume
*/
FILE *get_file(const char *file_name, const char *mode, int *error_code)
{
    // if file name is not provided (or if is invalid)
    if (file_name == NULL)
    {
        *error_code = SIFS_EINVAL;
        return 0;
    }
    FILE *fp = fopen(file_name, mode);
    // checking if the fle open is successful
    if (fp == NULL)
    {
        *error_code = SIFS_ENOVOL;
        return 0;
    }
    return fp;
}

/*
    returns a pointer pointes to ith block (marked by idx) in the vh structure's block list,
    and read contents in volume to the block
*/
void *get_ith_block(VolumeHelper *vh, int block_idx)
{
    // ensure the ith block we get has valid block id
    assert(block_idx >= 0 && block_idx < vh->header->nblocks);
    
    // if the blocklist already has block for the idx
    if (vh->block_list[block_idx])
    {
        return vh->block_list[block_idx];
    }

    // if there is no such block, we create a block and allocate a blocksize memory to it
    vh->block_list[block_idx] = malloc(vh->header->blocksize);
    // set the file pointer point to this block(after header, bitmap, and blocks before it)
    fseek(vh->vol, sizeof(SIFS_VOLUME_HEADER) + vh->header->nblocks + vh->header->blocksize * block_idx, SEEK_SET);
    // read the block which is being currently pointed to by fseek
    fread(vh->block_list[block_idx], vh->header->blocksize, 1, vh->vol);
    return vh->block_list[block_idx];
}

/*
    checks childblock(child_name) of a parent directory(parent_name) exists
    then returns the pointer pointing to childblock in blocklist, based on the indicator 'child_is_dir',
    it returns 0 to indicate that the parent is not a directory
    it also leaves the blockids of the parent dirblock(to res_parent_block_idx), its child direblock/fileblock (to res_block_idx)
    and the child entryindex in parent directory(to res_entry_idx_in_parent_block)
*/
void *ensure_parent_child_dir_exist(
    VolumeHelper *vh,
    const char *parent_name,
    const char *child_name,
    int *res_block_idx,
    int *res_parent_block_idx,
    int *res_entry_idx_in_parent_block,
    int child_is_dir)
{
    // get the contents of the volume    
    SIFS_VOLUME_HEADER *header = vh->header;
    void **block_list = vh->block_list;
    char *bitmap = vh->bitmap;
    // look through every block in the volume
    for (int j = 0; j < header->nblocks; j++)
    {
        // when the block is a directory block
        if (bitmap[j] == SIFS_DIR)
        {
            SIFS_DIRBLOCK *blockj = (SIFS_DIRBLOCK *)(block_list[j]);
            // and this directory block exists
            if (blockj)
            {
                // when it is a higher level directory, i.e., parent directory
                if (strcmp(blockj->name, parent_name) == 0)
                {
                    // look through all entries in this directory block
                    for (int i = 0; i < blockj->nentries; i++)
                    {
                        // when the entry is a subdirectory, find the subdirectories dirblockid
                        int block_idx = blockj->entries[i].blockID;
                        if (child_is_dir == 1 && bitmap[block_idx] == SIFS_DIR)
                        {
                            SIFS_DIRBLOCK *block_dir = (SIFS_DIRBLOCK *)(get_ith_block(vh, block_idx));
                            // ensure this subdirectory exists
                            assert(block_dir);
                            // and compare the child directory name provided with it
                            if (strcmp(block_dir->name, child_name) == 0)
                            {
                                *res_block_idx = block_idx;
                                *res_parent_block_idx = j;
                                *res_entry_idx_in_parent_block = i;
                                return block_dir;
                            }
                        }
                        // when the entry is a file entry
                        else if (child_is_dir == 0 && bitmap[block_idx] == SIFS_FILE)
                        {
                            // get the pointer which points to the fileblock of this file entry and exits if it doesn't exists
                            SIFS_FILEBLOCK *block_file = (SIFS_FILEBLOCK *)(get_ith_block(vh, block_idx));
                            assert(block_file);
                            uint32_t file_index = blockj->entries[i].fileindex;
                            // ensure the fileblock of this file entry has more than 'fileindex' copy of files
                            assert(block_file->nfiles > file_index && file_index >= 0);
                            // when the fileblock's 'fileindex' th copy has the same name with parameter
                            if (strcmp(block_file->filenames[file_index], child_name) == 0)
                            {
                                *res_block_idx = block_idx; // record the child file blockid
                                *res_parent_block_idx = j; // record the parent directory blockid
                                *res_entry_idx_in_parent_block = i; // record the entry index of the child file
                                return block_file;// return the child fileblock
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/*
    this function searches from the root directory to the last 
*/
void *search_deep(
    VolumeHelper *vh,
    char **path_list,
    int path_length,
    int *res_block_idx,
    int *res_parent_block_idx,
    int *res_entry_idx_in_parent_block,
    int last_is_dir)
{
    void **block_list = vh->block_list;
    // the path should be valid, i.e., it should contain something and length should be greater than or equal 1
    assert(strcmp(path_list[0], "") == 0);
    assert(path_length >= 1);

    get_ith_block(vh, 0); // get the first block (all info of root dir)

    // getting root dir, returns standard answer
    if (path_length == 1)
    {
        *res_block_idx = 0; //not useful for root dir.
        *res_parent_block_idx = -1; //not useful for root dir.
        *res_entry_idx_in_parent_block = -1; //not useful for root dir.
        return block_list[0]; 
    }
    void *cur = 0;
    int res_block_idx_tmp = -1;
    int res_parent_block_idx_tmp = -1;
    int res_entry_idx_in_parent_block_tmp = -1;
    
    // go from upper most hierarchy to lowermost
    for (int i = 0; i + 1 < path_length; i++)
    {
        // if it is second last directory (based on hierarchy)
        if (i + 1 + 1 == path_length)
        {
            // check if the last thing passed is directory or file and perform accordingly
            if (last_is_dir == 1)
            {
                // check if the directory is parent or the next directory
                cur = ensure_parent_child_dir_exist(vh, path_list[i], path_list[i + 1], &res_block_idx_tmp, &res_parent_block_idx_tmp, &res_entry_idx_in_parent_block_tmp, 1);
            }
            else
            {
                // check if cotent is in given directory
                cur = ensure_parent_child_dir_exist(vh, path_list[i], path_list[i + 1], &res_block_idx_tmp, &res_parent_block_idx_tmp, &res_entry_idx_in_parent_block_tmp, 0);
            }
        }
        else
        {
            // if it is not last or second last entry in the hierarchy
            cur = ensure_parent_child_dir_exist(vh, path_list[i], path_list[i + 1], &res_block_idx_tmp, &res_parent_block_idx_tmp, &res_entry_idx_in_parent_block_tmp, 1);
        }
        // if directories cannot be found
        if (!cur)
        {
            return 0;
        }
    }
    // if the search is complete put everything local to this function to the variables passed as parameter
    if (cur)
    {
        *res_block_idx = res_block_idx_tmp;
        *res_parent_block_idx = res_parent_block_idx_tmp;
        *res_entry_idx_in_parent_block = res_entry_idx_in_parent_block_tmp;
    }
    // success
    return cur;
}

/*
    sets the value(u,d,f,b) of bitmap in the vh struct 
    for blocks including blockidx and the next blocks required
*/
void set_bitmap_value_2(VolumeHelper *vh, int block_idx, char *value, int value_size)
{
    if(value_size == 0) return;
    SIFS_VOLUME_HEADER *header = vh->header;
    FILE *vol = vh->vol;
    char *bitmap = vh->bitmap;
    // ensure the blockid is valid
    if (value_size <= 0)
        return;
    assert(0 <= block_idx && (block_idx + value_size <= header->nblocks));
    
    // it sets several blocks's bitmap in vh struct
    for (int i = 0; i < value_size; i++)
    {
        bitmap[i + block_idx] = value[i];
    }
    // set the pointer to the bitmap of begining block
    fseek(vol, sizeof(SIFS_VOLUME_HEADER) + block_idx, SEEK_SET);
    // write the bitmap info of total number of "value_size" blocks to the volume
    fwrite(&bitmap[block_idx], value_size, 1, vol);
}

/*
    sets the bitmap of the block with id = block_idx 
*/
void set_bitmap_value(VolumeHelper *vh, int block_idx, char value)
{
    set_bitmap_value_2(vh, block_idx, &value, 1);
}

/*
    clear the contents of several blocks after the block with id = block_idx(including this block)
*/
void empty_blocks_to_file(VolumeHelper *vh, int block_idx, int blocks_count)
{
    if(blocks_count==0) return;
    SIFS_VOLUME_HEADER *header = vh->header;
    FILE *vol = vh->vol;
    // put the pointer of volume to its blocks
    fseek(vol, sizeof(SIFS_VOLUME_HEADER) + header->nblocks + header->blocksize * block_idx, SEEK_SET);
    // calculate the whole memory of blocks and consider it an integrated block
    char one_block[header->blocksize * blocks_count];
    // empty all the contents of this whole block
    memset(one_block, 0, sizeof(one_block));
    // write this whole block to the volume
    fwrite(one_block, sizeof(one_block), 1, vol);
}

/*
    clears the contents of blocks as required and write the 'buffer' of 'size' to the empty blocks
*/
void write_buffer_to_file(VolumeHelper *vh, int block_idx, int blocks_count, char *buffer, int size)
{
    if(blocks_count==0) return;
    SIFS_VOLUME_HEADER *header = vh->header;
    FILE *vol = vh->vol;
    // empty contents of blocks in vol
    empty_blocks_to_file(vh, block_idx, blocks_count);
    int total_size = blocks_count * header->blocksize;
    fseek(vol, sizeof(SIFS_VOLUME_HEADER) + header->nblocks + header->blocksize * block_idx, SEEK_SET);
    //check what we want to write is less than the total size of cleared blocks
    assert(size <= total_size);
    //write the contents in the buffer to vol
    fwrite(buffer, size, 1, vol);
}

/*
    first empty the block with id = block_idx and write the contents to vol, the block could be fileblock, direblock or datablock
*/
void write_block_to_file(VolumeHelper *vh, int block_idx)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    FILE *vol = vh->vol;
    void **block_list = vh->block_list;
    char *bitmap = vh->bitmap;
    // empty the contents of this 'block_idx' block
    empty_blocks_to_file(vh, block_idx, 1);
    // the block should be valid
    assert(block_list[block_idx]);
    fseek(vol, sizeof(SIFS_VOLUME_HEADER) + header->nblocks + header->blocksize * block_idx, SEEK_SET);
    // write the contents of this block in blocklist to vol depending on the type of this block
    if (bitmap[block_idx] == SIFS_DIR)
    {
        fwrite(block_list[block_idx], sizeof(SIFS_DIRBLOCK), 1, vol);
    }
    else if (bitmap[block_idx] == SIFS_FILE)
    {
        fwrite(block_list[block_idx], sizeof(SIFS_FILEBLOCK), 1, vol);
    }
    else
    {
        fwrite(block_list[block_idx], header->blocksize, 1, vol);
    }
}

/*
    add a new directory to block with id = block_idx in vol and initialise the contents of this directory block
*/
void add_a_dir(VolumeHelper *vh, int block_idx, const char *dir_name)
{
    void **block_list = vh->block_list;
    char *bitmap = vh->bitmap;
    // ensure what we are adding is a directory
    assert(bitmap[block_idx] == SIFS_DIR);
    // allocates memory for dirblock
    SIFS_DIRBLOCK *dir_block = (SIFS_DIRBLOCK *)malloc(sizeof(SIFS_DIRBLOCK));
    // initialize it with 0 and then populate its modification time and entries and directory name...
    memset(dir_block, 0, sizeof(SIFS_DIRBLOCK));
    dir_block->modtime = time(0);
    dir_block->nentries = 0;
    // copies the directory name to what should be (passed as parameter to this function)
    strcpy(dir_block->name, dir_name);
    // first add this directory block to the blocklist of vh
    block_list[block_idx] = dir_block;
    // then writes it to volume
    write_block_to_file(vh, block_idx);
}

/*
    // compares all fileblocks and 
    // records the id of the block with the same md5 as parameter to where the res_block_idx pointer points to 
*/
void get_file_idx_by_md5(VolumeHelper *vh, const char *md5_str, int *res_block_idx)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    // loop through all blocks
    for (int i = 0; i < header->nblocks; i++)
    {
        // when the block is a fileblock
        if (bitmap[i] == SIFS_FILE)
        {
            // get the pointer of this block in blocklist of vh struct
            SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK *)get_ith_block(vh, i);
            // ensure the blocklist has this block
            assert(file);
            // compare the provided md5_str parameter with the fileblock's
            if (memcmp(file->md5, md5_str, MD5_BYTELEN) == 0)
            {
                // set the res_block_idx to the blockid
                *res_block_idx = i;
                return;
            }
        }
    }
    // when there is no such fileblock
    *res_block_idx = -1;
}

/*
    adds a new directory entry to the directory block in blocklist and then writes this block to the volume
*/
void add_dir_idx_entry(VolumeHelper *vh, int block_idx, int new_block_idx)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    // ensure the block_idx provided is valid
    assert(0 <= block_idx && block_idx < header->nblocks);
    // and it is a dirblock
    assert(bitmap[block_idx] == SIFS_DIR);
    // get the pointer points to 'block_idx' block in blocklist and read contents in volume to it
    SIFS_DIRBLOCK *block = (SIFS_DIRBLOCK *)get_ith_block(vh, block_idx);
    // ensure we have valid number of entries
    assert(block->nentries < SIFS_MAX_ENTRIES);
    int old_entry = block->nentries++;
    // set the new entry info of this dirblock in block list
    block->entries[old_entry].blockID = new_block_idx;
    block->entries[old_entry].fileindex = 0;
    // write the block in blocklist to volume
    write_block_to_file(vh, block_idx);
}

/*
    adds a new file entry to the directory block (in blocklist) with its blockid and file index populated 
    and then writes this block to the volume
*/
void add_file_idx_entery(VolumeHelper *vh, int block_idx, int new_block_idx, int file_index)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    assert(0 <= block_idx && block_idx < header->nblocks);
    // ensure the block we are adding the entry to is a dirblock
    assert(bitmap[block_idx] == SIFS_DIR);
    SIFS_DIRBLOCK *block = (SIFS_DIRBLOCK *)get_ith_block(vh, block_idx);
    // ensure the dirblock has valid  number of total entries
    assert(block->nentries < SIFS_MAX_ENTRIES);
    int old_entry = block->nentries++;
    // set the values of the new added file entry in the dirblock
    block->entries[old_entry].blockID = new_block_idx;
    block->entries[old_entry].fileindex = file_index;
    // write this direblock with new file entry added to the volume
    write_block_to_file(vh, block_idx);
}

/*
    sets the bitmap in the vh struct and of the actual volume as unused for the fileblock and its datablocks 
    it also clear the fileblock and its datablocks in the volume
*/
void delete_file_block(VolumeHelper *vh, int block_file_idx, int block_content_idx, int block_count)
{
    // ensure what we are going to delete is a file block
    assert(vh->bitmap[block_file_idx] == SIFS_FILE);
    // we first set the bitmap of the fileblock in vh struct as unused
    vh->bitmap[block_file_idx] = SIFS_UNUSED;
    // then set the  bitmap for blocks containing file data as unused too
    for (int i = 0; i < block_count; i++)
    {
        // ensure that what we are deleting is the datablock of the fileblock
        assert(vh->bitmap[i + block_content_idx] == SIFS_DATABLOCK);
        vh->bitmap[i + block_content_idx] = SIFS_UNUSED;
    }
    SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK *)get_ith_block(vh, block_file_idx);
    assert(file->nfiles == 0);
    // set the volume bitmap for the fileblock as unused
    set_bitmap_value(vh, block_file_idx, vh->bitmap[block_file_idx]);
    // set the volume bitmap for datablocks of this fileblock as unused
    set_bitmap_value_2(vh, block_content_idx, &vh->bitmap[block_content_idx], block_count);
    // clear the contents of the fileblock in the volume
    empty_blocks_to_file(vh, block_file_idx, 1);
    // clear the contents of datablocks of that fileblock in the volume
    empty_blocks_to_file(vh, block_content_idx, block_count);
}

/*
    populates the info provided as parameter to the new added fileblock in blocklist 
    then add this fileblock to volume and write the contents from buf of 'size' to datablocks in the volume
    it also sets up their bitmaps both in bitmap of vh struct and in the actual volume
*/
void add_file_block(VolumeHelper *vh, int block_file_idx, int block_content_idx, void *buf, const char *md5_str, int size,int block_count, const char *file_name)
{
    void **block_list = vh->block_list;
    char *bitmap = vh->bitmap;
    // ensure the fileblock we add a new file to is empty
    assert(bitmap[block_file_idx] == SIFS_UNUSED);
    // set this block as a fileblock
    bitmap[block_file_idx] = SIFS_FILE;
    // for every contents block
    for (int i = 0; i < block_count; i++)
    {
        // ensure the contents blocks required are empty
        if(bitmap[i + block_content_idx] != SIFS_UNUSED)
        {
            assert(0);
        }
        // set the bitmap in vh struct of the content blocks as 'd'
        bitmap[i + block_content_idx] = SIFS_DATABLOCK;
    }
    // set the volume's bitmap of the fileblock as 'f'
    set_bitmap_value(vh, block_file_idx, bitmap[block_file_idx]);
    // set the volume's bitmap  of the contents blocks as 'd'
    set_bitmap_value_2(vh, block_content_idx, &bitmap[block_content_idx], block_count);
    // set the info of the fileblock and its content blocks
    SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK *)malloc(sizeof(SIFS_FILEBLOCK));
    // pass the file name parameter as the new added fileblock's first filename
    strcpy(file->filenames[0], file_name);
    file->nfiles = 1;
    // pass the begining blockid of all datablocks (provided as the parameter) to the fileblock's firstblockID
    file->firstblockID = block_content_idx;
    file->length = size;
    // copy the parameter md5_str to the fileblock
    memcpy(file->md5, md5_str, MD5_BYTELEN);
    // set the fileblock's mod time
    file->modtime = time(0);
    // add the populated fileblock to the blocklist
    block_list[block_file_idx] = file;
    // write this fileblock in blocklist to the volume
    write_block_to_file(vh, block_file_idx);
    // write the 'size' of contents from file(stored in buf) to the the datablocks(begining with 'block_content_idx' block)
    write_buffer_to_file(vh, block_content_idx, block_count, buf, size);
}

/*
    adds the filename of the copy of file which already exists to its fileblock
*/
void add_file_index(VolumeHelper *vh, int block_idx, const char *file_name)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    assert(0 <= block_idx && block_idx < header->nblocks);
    // ensure we are adding the filename to is a fileblock
    assert(bitmap[block_idx] == SIFS_FILE);
    SIFS_FILEBLOCK *block = (SIFS_FILEBLOCK *)get_ith_block(vh, block_idx);
    // ensure that the number of files is less than maximum
    assert(block->nfiles < SIFS_MAX_ENTRIES);
    int old_entry = block->nfiles++;
    // copy the filename to the next filename entry in blocklist's fileblock
    strcpy(block->filenames[old_entry], file_name);
    // write this added filename fileblock to the volume
    write_block_to_file(vh, block_idx);
}

/*
    deletes a copy of file in the fileblock which has an index =  file_index
    both in blocklist and the volume
*/
void delete_file_index(VolumeHelper *vh, int file_block_idx, int file_index)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    assert(file_block_idx >= 0 && file_block_idx < header->nblocks);
    assert(bitmap[file_block_idx] == SIFS_FILE);
    SIFS_FILEBLOCK *block = (SIFS_FILEBLOCK *)get_ith_block(vh, file_block_idx);
    // ensure the copy of file we delete in the fileblock exists
    assert(file_index >= 0 && file_index < block->nfiles);
    // we move the following copies 1 copy ahead to delete the filename of the copy we want to delete
    for (int j = file_index; j + 1 < block->nfiles; j++)
    {
        // replace the former copy's filename with the latter copy's filename
        memcpy(block->filenames[j], block->filenames[j + 1], SIFS_MAX_NAME_LENGTH);
    }
    // set the last copy's filename as 0 since it has already been move ahead
    memset(block->filenames[block->nfiles - 1], 0, SIFS_MAX_NAME_LENGTH);
    // decrease the total copys in the fileblock
    block->nfiles--;
    // write the changed fileblock in blocklist to the volume
    write_block_to_file(vh, file_block_idx);
}

/*
    deletes the entry with index = delete_entry_idx in the directoryblock
    and moves all the following entries one entry ahead, and write the changed directory block to volume
*/
void delete_idx_entry(VolumeHelper *vh, int block_idx, int delete_entry_idx)
{
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    assert(0 <= block_idx && block_idx < header->nblocks);
    // ensure the block where we are deleting the entry is a direblock
    assert(bitmap[block_idx] == SIFS_DIR);
    SIFS_DIRBLOCK *block = get_ith_block(vh, block_idx);
    // ensure the entry we are deleting is a valid one
    assert(0 <= delete_entry_idx && delete_entry_idx < block->nentries);
    // loop through all the following entries
    for (int i = delete_entry_idx; i + 1 < block->nentries; i++)
    {
        // replace the deleted entry with its next entry 
        // move the following entries one entry ahead
        memcpy(&(block->entries[i]), &(block->entries[i + 1]), sizeof(block->entries[i + 1]));
    }
    // decrease the total number of entries in the direblock
    block->nentries--;
    // set the last entry to empty since it has already been moved ahead
    memset(&(block->entries[block->nentries]), 0, sizeof(block->entries[block->nentries]));
    // write the deleted dirblock to the volume
    write_block_to_file(vh, block_idx);
}

/*
    this function empty the dirblock both in the blocklist and the volume
    it also sets the bitmaps to 'u'
*/
void empty_a_dir(VolumeHelper *vh, int block_idx)
{
    void **block_list = vh->block_list;
    char *bitmap = vh->bitmap;
    //ensure the block we are emtry is a dirblock
    assert(bitmap[block_idx] == SIFS_DIR);
    SIFS_DIRBLOCK *dir = (SIFS_DIRBLOCK *)get_ith_block(vh, block_idx);
    // directory with data cannot be deleted
    assert(dir->nentries == 0);
    //set the bitmap of this dirblock to 'u'
    set_bitmap_value(vh, block_idx, SIFS_UNUSED);
    //free this dirblock in blocklist
    free(block_list[block_idx]);
    block_list[block_idx] = 0;
    //clear 1 block since the block with id = block_idx in the volume, i.e., empty the direblock in the volume
    empty_blocks_to_file(vh, block_idx, 1);
}

/*
    fail: return -1, success: return >= 0
    since we employ a first-fit contiguous data-block allocation
    this function checks whether we could get block_length number of continuous blocks 
    we return the number of blocks since the begin_search_idx block, if we could have enough unused contiuous blocks after that, 
    otherwise we return -1 to indicate we can't find continuous blocks as required in the volume 
*/
int request_blocks_2(int begin_search_idx, VolumeHelper *vh, int block_length)
{
    if(block_length == 0)
        return 0;
    SIFS_VOLUME_HEADER *header = vh->header;
    char *bitmap = vh->bitmap;
    // check the number of blocks we requried is valid
    if (block_length <= 0 || block_length > header->nblocks)
        return -1;
    // loop from the 'begin_search_idx' block to the first block of last possible continuous blocks
    for (int i = begin_search_idx; i + block_length < header->nblocks; i++)
    {
        int good = 1; // initialize the indicator as successful
        // loop through all the continuous block
        for (int j = 0; j < block_length; j++)
        {   
            // check whether the continuous blocks are all empty
            if (bitmap[j + i ] != SIFS_UNUSED)
            {
                good = 0; // indicator set as fail
                break;
            }
        }
        if (good == 0)
            continue; // continue to search for the next continous blocks since the next begin block
        else
            return i; // return how many blocks after begin_search_idx block could have required continous block following 
    }
    return -1;
}

/*
    finds all the indicators for different block length requirements and set the pointer *block_idx_list points to them
*/
int request_blocks(VolumeHelper *vh, int *block_length_list, int count, int *block_idx_list)
{
    int bi = 0;//we search from the first block
    for (int i = 0; i < count; i++)
    {
        block_idx_list[i] = request_blocks_2(bi, vh, block_length_list[i]);
        //if it is impossible to find required length of blocks
        if (block_idx_list[i] == -1)
            return 1;
        //if the request succeed, we set the next begining search block as the block after 
        bi = block_idx_list[i] + 1;
    }
    return 0;
}

/*
    checks whether the files in fileblock have reached the limited number
    if it isn't, i.e., we could still add an entry for the block file, it returns 0 to indicate the request for a new entry is valid
*/
int request_entry_in_file(VolumeHelper *vh, int block_idx)
{
    SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK *)get_ith_block(vh, block_idx);
    // we need to transform this code
    if (file->nfiles < SIFS_MAX_ENTRIES)
        return 0;
    else
        return 1;
}

/*
    checks whether the entries in dirblock have reached the limited number
    it returns 0 when the request is valid and 1 when invalid
*/
int request_entry(VolumeHelper *vh, int block_idx)
{
    SIFS_DIRBLOCK *dir = (SIFS_DIRBLOCK *)get_ith_block(vh, block_idx);
    // must be a directory
    assert(dir);
    if (dir->nentries < SIFS_MAX_ENTRIES)
        return 0;
    else
        return 1;
}

/*
    create a volumehelper and initialize it to 0s
*/
VolumeHelper *create_vh()
{
    VolumeHelper *res = malloc(sizeof(VolumeHelper));
    // initalize the vh to 0s
    memset(res, 0, sizeof(VolumeHelper));
    res->header = (SIFS_VOLUME_HEADER *)malloc(sizeof(SIFS_VOLUME_HEADER));
    // intialize the header in volumehelper as 0s
    memset(res->header, 0, sizeof(SIFS_VOLUME_HEADER));
    return res;
}

/*
    opens a file/volume, and populate the vh struct based on the opened file/volume
*/
void init_with_file(VolumeHelper *vh, const char *file_name, const char *file_mode, int *current_error)
{
    //open the file and pass the filepointer the volume
    vh->vol = get_file(file_name, file_mode, current_error);
    //check there is no error
    if (*current_error != SIFS_EOK)
        return;
    //set the pointer to the begining of the volume
    fseek(vh->vol, 0, SEEK_SET);
    //read the header from volume to the header of vh struct, after the read, the pointer of volume advanced to the position of bitmap
    fread(vh->header, sizeof(SIFS_VOLUME_HEADER), 1, vh->vol);
    //allocate 'nblocks' memory to bitmap of vh struct
    vh->bitmap = (char *)malloc(vh->header->nblocks);
    //read the bitmap of volume to the bitmap in struct
    fread(vh->bitmap, vh->header->nblocks, 1, vh->vol);
    vh->block_list = (void **)(malloc(sizeof(void *) * vh->header->nblocks));
    //set the blocklist as 0s
    memset(vh->block_list,0,sizeof(void *) *vh->header->nblocks );
}

/*
    deletes a diretoryblock and its entry in parent directory in blocklist and also make changes to volume
*/
void delete_a_dir(VolumeHelper *vh, int block_idx, int parent_block_idx, int entry_idx_in_parent)
{
    empty_a_dir(vh, block_idx);
    // delete its entry of higher level directory in blocklist and also change the volume
    delete_idx_entry(vh, parent_block_idx, entry_idx_in_parent);
}

/*
    find a valid block to create a child directory,initialise it 
    and make changes to parent directory
*/
void create_a_dir(VolumeHelper *vh, int parent_block_idx, const char *dir_name, int *cur_error)
{
    //if the parent directory's entries are full
    if (request_entry(vh, parent_block_idx) == 1)
    {
        *cur_error = SIFS_EMAXENTRY;
    }
    else
    {
        int using_block_idx = -1;//at the moment,we havn't find the blockid for the dirblock we request
        int using_block_length = 1;//the dir only occupied 1 directory block
        //serach from the first block to find an empty block
        if (request_blocks(vh, &using_block_length, 1, &using_block_idx) == 1)
        {
            *cur_error = SIFS_ENOSPC;//if we cannot find a empty block
        }
        else//when we could find the empty block, using_block_idx now will point to the id of that block
        {
            set_bitmap_value(vh, using_block_idx, SIFS_DIR);//set the bitmap of the created direblock
            add_dir_idx_entry(vh, parent_block_idx, using_block_idx);//add it as a new entry to its parent dirctory
            add_a_dir(vh, using_block_idx, dir_name);//add this new directory block to volume and blocklist, initialise its values
        }
    }
}

/*
    opens the file and read all its contents to buffer
    if this operation succeed, 0 will be returned as an indicator, the content will be stored in *buffer, the file length will be stored in *size  
*/
int get_file_content(const char *file_name, char **buffer, int *size)
{
    *buffer = 0;
    *size = 0;
    FILE *f = fopen(file_name, "rb");
    // when we successfully open the file
    if (f)
    {
        // set the file pointer to the end of file
        fseek(f, 0, SEEK_END);
        *size = ftell(f);
        // when it is not a empty file, i.e., it has contents
        if (*size != 0)
        {
            fseek(f, 0, SEEK_SET);
            *buffer = malloc(*size);
            fread(*buffer, 1, *size, f); // read the file content to the buffer
        }
        fclose(f);
        return 0; // return 0 to indicate success
    }
    return 1;
}

/*
    makes a file to the volume
    if it is already exists, we create a new entry in diretory and give it an fileindex
    if there is no copy of this file, we create a fileblock and its content blocks based on continuous allocation to the volume
    and make changes to where needed
*/
void create_a_file(VolumeHelper *vh, int parent_block_idx, const char *file_name, int *cur_error)
{
    // ensure currently no error
    assert(*cur_error == SIFS_EOK);
    // when the parent fileblock has reached its maximum entry number
    if (request_entry(vh, parent_block_idx) == 1)
    {
        *cur_error = SIFS_EMAXENTRY;
    }
    char *file_content = 0; // intialise the buffer
    int size = 0; // intialise the size of file
    // when we successfully get the file
    if (get_file_content(file_name, &file_content, &size) == 0)
    {
        char md5_str[MD5_BYTELEN] = {0}; // initialise the md5_str
        MD5_buffer(file_content, size, md5_str); // leave the md5 digest of file to md5_str
        int res_file_block_idx = -1; // initialise the fileblock id
        int file_idx = -1; // and fileindex
        get_file_idx_by_md5(vh, md5_str, &res_file_block_idx);
        if (res_file_block_idx == -1)
        {
            int need_blocks = ((size * 1.0)-1 / vh->header->blocksize)+1;
            int block_length_list[] = {1, need_blocks};
            int block_idx_list[] = {-1, -1}; // initialise the blockid
            // check whether we could successfully find both the fileblock and its contentblocks
            if (request_blocks(vh, block_length_list, 2, block_idx_list) == 1)
            {
                *cur_error = SIFS_ENOSPC;
            }
            else
            {
                // add the fileblock and its contentblocks and populate all info to these blocks
                add_file_block(vh, block_idx_list[0], block_idx_list[1], file_content, md5_str, size,need_blocks, file_name);
                file_idx = 0; // the file index in a new fileblock would be 0
                res_file_block_idx = block_idx_list[0]; // the fileblock id now is obtained
            }
        }
        else // when we already has the file in a fileblock, &res_file_block_idx will now point to its id 
        {
            // check whether we could add a new entry in the fileblock with same contents as our file
            if (request_entry_in_file(vh, res_file_block_idx) == 1)
            {
                *cur_error = SIFS_EMAXENTRY;
            }
            else
            {
                // return the pointer points to the block with id=res_file_block_idx
                SIFS_FILEBLOCK *file = (SIFS_FILEBLOCK *)get_ith_block(vh, res_file_block_idx);
                file_idx = file->nfiles; // the file index for this new copy would be the total number of entries originally
                add_file_index(vh, res_file_block_idx, file_name); // add this new name to the fileblock
            }
        }
        if (*cur_error == SIFS_EOK)
        {
            // add a new entry to the parent directoryblock of the fileblock with its id and fileindex
            add_file_idx_entery(vh, parent_block_idx, res_file_block_idx, file_idx);
        }
    }
    // when there is some error getting the file provided
    else
    {
        *cur_error = SIFS_EINVAL;
    }
    free(file_content);
}

/*
    frees all the contents in the volumehelper stuct
*/
void free_vh(VolumeHelper *vh)
{
    // free every block in the blocklist
    for (int i = 0; i < vh->header->nblocks; i++)
    {
        free(vh->block_list[i]);
    }
    free(vh->block_list);
    free(vh->bitmap);
    fclose(vh->vol);
    free(vh);
}

