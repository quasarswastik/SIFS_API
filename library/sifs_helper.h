#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "sifs-internal.h"

/* CITS2002 Project 2 2019
   Names:             Swastik Raj Chauhan , Ziwei Li
   Student numbers:   22556239 , 22439925
*/
//  sifs_helper (header) (v1.1)

/*
    stores all the necessary information about volume along with the volume
*/
typedef struct
{
    SIFS_VOLUME_HEADER *header;
    SIFS_BIT *bitmap;
    FILE *vol;
    void **block_list;
} VolumeHelper;

/*
    split a string by '/'
*/
int split_path(const char *path, char ***result_list, int *result_length);
char *clone_string(const char *str);
char *cat_string(const char *s1, const char *s2);
void free_list(void **result_list, int result_length); // free the memory of result_list

// rb and rb+ are supported.
FILE *get_file(const char *file_name, const char *mode, int *error_code);
// search the path list
void *search_deep( VolumeHelper *vh, char **path_list, int path_length, int *res_block_idx, int *res_parent_block_idx, int *res_entry_idx_in_parent_block, int last_is_dir);

// deletes the entry with index = delete_entry_idx in the directoryblock and moves all the following entries one entry ahead, and write the changed directory block to volume
void delete_idx_entry(VolumeHelper *vh, int block_idx, int delete_entry_idx);
// deletes a file in the fileblock which has an index =  file_index both in blocklist and the volume
void delete_file_index(VolumeHelper *vh, int file_block_idx, int file_index);
// delete file block and sets the bitmap in the vh struct and of the actual volume as unused for the fileblock and its datablocks it also clear the fileblock and its datablocks in the volume
void delete_file_block(VolumeHelper *vh, int block_file_idx, int block_content_idx, int block_content_count);

// finds all the indicators for different block length requirements and set the pointer *block_idx_list points to them
int request_blocks(VolumeHelper *vh, int *block_length_list, int count, int *block_idx_list);
// checks whether the entries in dirblock have reached the limited number, it returns 0 when the request is valid and 1 when invalid
int request_entry(VolumeHelper *vh, int block_idx);

// clear the contents of several blocks after the block with id = block_idx(including this block) for the files
void empty_blocks_to_file(VolumeHelper *vh, int block_idx, int blocks_count);
// first empty the block with id = block_idx and write the contents to vol, the block could be fileblock, direblock or datablock
void write_block_to_file(VolumeHelper *vh, int block_idx);

VolumeHelper *create_vh();

// frees all the contents in the volumehelper stuct
void free_vh(VolumeHelper *vh);
// opens a file/volume, and populate the vh struct based on the opened file/volume
void init_with_file(VolumeHelper *vh, const char *file_name, const char *file_mode, int *current_error);
// returns a pointer pointing to ith block (marked by idx) in the vh structure's block list, and read contents in volume to the block
void *get_ith_block(VolumeHelper *vh, int block_idx);

// deletes a diretoryblock and its entry in parent directory in blocklist and also make changes to volume
void delete_a_dir(VolumeHelper *vh, int block_idx, int parent_block_idx, int entry_idx_in_parent);
// find a valid block to create a child directory,initialise it  and make changes to parent directory
void create_a_dir(VolumeHelper *vh, int parent_block_idx, const char *dir_name, int *current_code);
// makes a file to the volume if it is already exists, we create a new entry in diretory and give it an fileindex if there is no copy of this file, we create a fileblock and its content blocks based on continuous allocation to the volume and make changes to where needed
void create_a_file(VolumeHelper *vh, int parent_block_idx, const char *file_name, int *current_code);

