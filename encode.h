#ifndef ENCODE_H
#define ENCODE_H

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

// Structure to store information needed for encoding
typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;		// Filename of the source image
    FILE *fptr_src_image;		// File pointer to the source image

    /* Secret File Info */
    char *secret_fname;			// Filename of the secret file
    FILE *fptr_secret;			// File pointer to the secret file

    /* Stego Image Info */
    char *stego_image_fname;		// Filename for the stego image (output)
    FILE *fptr_stego_image;		// File pointer to the stego image

} EncodeInfo;

/* Function prototypes for encoding */

/* Check the operation type based on command-line arguments */
OperationType check_operation_type(char *argv[]);

/* Read and validate command-line arguments for encoding */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

// Perform the encoding process
Status do_encoding(char *argv[], EncodeInfo *encInfo);

//1. Open files for reading/writing 
Status open_files(EncodeInfo *encInfo);

/* Check if the image has enough capacity to hide the secret data */
Status check_capacity(EncodeInfo *encInfo);

/* Get the size of the BMP image data (excluding header) */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

//2.Copy the BMP header from source to destination image 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

//3.Store and encode Magic String and length
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

//4. Encode secret file extenstion 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);
Status encode_secret_file_ext_size(EncodeInfo *encInfo);

//5. Encode secret file size/length 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);
Status secret_file_size(EncodeInfo *encInfo);

//6. Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo);

//7.Encode a block of data into the image's LSB  
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte and int into the LSB of an image buffer */
Status encode_byte_to_lsb(char data, char *image_buffer);
Status encode_int_to_lsb(int data, char *image_buffer);

//8.Copy remaining image bytes from src to stego 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
