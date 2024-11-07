#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

// Structure to hold information required for decoding
typedef struct _DecodeInfo {

    char *stego_image_fname;    // Name of the image file containing hidden data
    FILE *fptr_stego_image;     // File pointer for the stego image
    int magic_string_len;	// Length of the magic string that is embedded in the image
    char magic_string[10];	// Array to store the decoded magic string

    int secret_file_extsize;	// Size of the secret file extension
    char secret_file_ext[20];	// Array to store the decoded secret file extension

    int secret_file_size;	// Size of the secret file in bytes
    char secret_file[1000];	// Array to store the name or content of the decoded secret file

    long s_file_size;		// Size of the actual secret file data
    char s_file_data[1000];	// Array to store the decoded secret file data
    char output_fname[20];      // Name of the output file
    FILE *fptr_output;          // File pointer for the output file
} DecodeInfo;

// Function prototypes for decoding process
/* Perform the encoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo);

//1. Get File pointers for i/p and o/p files 
Status dopen_files(DecodeInfo *decInfo);

//2.decode the Magic String and its length
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

//3. Function to decode the secret file extension
Status decode_secret_file_ext_size(char *secret_file_ext,DecodeInfo*decInfo);

//4.Function to decode the size of the secret file
Status decode_secret_file_size(long s_file_size,DecodeInfo *decInfo);

//5.// Function to decode the secret file data
Status decode_secret_file_data(char *s_file_data,DecodeInfo*decInfo);

// Function to decode 32-bit integer and bytes from LSBs
int decode_lsb_to_int(char *image_buffer);
char decode_lsb_to_byte(char *image_buffer);

#endif
