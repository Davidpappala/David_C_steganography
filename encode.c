#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"

// Global variables for file size and secret file extension
long file_size;		// Variable to store secret file size
char *sec_ext;		// Holds the extension of the secret file
			// // Holds the extension of the source image 
char *src_ext;

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

/* Get the size of the BMP image data */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte 
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // calculate and Return image data size
    return width * height * 3;
}


/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

/* Validate command-line arguments for encoding */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *src_ext=strchr(argv[2], '.');		// Extract file extension from source image filename
    sec_ext=strchr(argv[3], '.');		// Extract file extension from secret file

    // Validate that both source and secret files have valid extensions
    if(sec_ext==NULL || src_ext == NULL)
    {
	    // If no extension is found, print usage and return failure
            printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        return e_failure;
    }
    
    // Check if the user has provided a valid output file name
    if(argv[4] != NULL)
    {
    	if(strstr(argv[4],".bmp") != NULL)		// Check if output file has .bmp extension
    	{
        	char *token = strchr(argv[4],'.');	// Extract extension from output file
        	if (strcmp(token,".bmp")==0)		// Compare with .bmp
        	{
			// Allocate memory for the output file name and copy it
            		encInfo->stego_image_fname = (char *)malloc(strlen(argv[4]) + 1);
            		strcpy(encInfo->stego_image_fname,argv[4]);
        	}
        	else
        	{
			// Print usage if file is not .bmp
            		printf("%s: Encoding: %s <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
            		return e_failure;
        	}
    	}
    	else
    	{
		// Print usage if invalid output format is specified
        	printf("%s: Encoding: %s <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        	return e_failure;
    	}
  }
  else
  {
	  // If no output file specified, default to "output.bmp"
  	printf("INFO: Output File not mentioned. Creating output.bmp as default\n");
    	encInfo->stego_image_fname = "output.bmp";
  }
    
  // Validate the file extensions and command-line arguments
    if (strcmp(argv[0], "./a.out") == 0 &&
        strcmp(argv[1], "-e") == 0 &&
        strcmp(src_ext, ".bmp") == 0 &&    // Check for .bmp file extension
        (strcmp(sec_ext,".c") == 0 || strcmp(sec_ext, ".txt") == 0))  // Check for .c or .txt
    {
        return e_success;
    }
    else
    {
        printf("Error: Invalid arguments\n"); // Fixed missing semicolon
        return e_failure;
    }
}

//1. opening files 
Status open_files(EncodeInfo *encInfo)
{
    // Open the source image file
    printf("INFO: Opening required files\n");
    
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        printf("The Source file is not opened\n");
        return e_failure;
    }
    printf("INFO: Opened SkeletonCode/beautiful.bmp\n");

    // open the Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        printf("The Secret file is not opened\n");
        return e_failure;
    }
    printf("INFO: Opened secret.txt\n");

    // open the Stego Image file for writing
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        printf("The Stego image file is not opened\n");
        return e_failure;
    }
    printf("INFO: Opened steged_img.bmp\n");
    // No failure return e_success
    return e_success;
}

// perform the encoding process
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    if(read_and_validate_encode_args(argv, encInfo) != e_success)
    {
        printf("file of read and validate not opened\n");	// Validate command-line arguments
        return e_failure;
    }
    // 1. Open the required files
    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Failed to open files.\n");
        return e_failure;		//return failure if the file openening fails
    }

    printf("INFO: ## Encoding Procedure Started\n");
    
    //Check if the secret file is empty
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    file_size = secret_file_size(encInfo);
    if(file_size!=0)
    {
            printf("INFO: Done.Not Empty\n");
    }
    else
    {
            printf("ERROR: %s file is empty\n",encInfo->secret_fname);
            return e_failure;
    }

    // Check if the image has enough capacity to hide the secret file
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if(check_capacity(encInfo) != e_success)
    {
            printf("ERROR:%s does not have capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
            return e_failure;
    }

        printf("INFO: Done. Found OK\n");

    // Copy BMP header from the src image to the stego image
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to copy BMP header.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    
    // Encode the magic string to identify the stego image
    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
    {
        printf("Failed to encode magic string.\n");
        return e_failure;
    }
    printf("INFO: Done\n");

    // Encode file extension size and file extension
    if (encode_secret_file_extn(sec_ext,encInfo) !=e_success)
    //if (encode_secret_file_ext_size(encInfo) != e_success)
    {
        printf("Failed to encode file extension size.\n");
        return e_failure;
    }
    printf("INFO: Done\n");

    // check Encode the size of the secret file
    if(encode_secret_file_size(file_size,encInfo) !=e_success)
    {
            printf("Failed to secret file size.\n");
            return e_failure;
    }
     printf("INFO: Done\n");
    
     // Encode secret file data
    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf("Failed to encode secret file data.\n");
        return e_failure;
    }
     printf("INFO: Done\n");

     // Copy remaining image data
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to copy remaining image data.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    printf("INFO: ## Encoding Done Successfully\n");
    return e_success;
}

/* Check if the image has enough capacity to hide the secret data */
Status check_capacity(EncodeInfo *encInfo)
{
        //checking Secret file size
        int image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
        //printf("Image capacity %d\n",image_capacity);
        
	// Calculate the total (encoding things)required capacity for encoding
        int encoding_things = 54+(4+(strlen(MAGIC_STRING))+4+(strlen(sec_ext))+4+(file_size))*8;
        //printf("Encoding_things %d\n",encoding_things);
        if(image_capacity > encoding_things)
                return e_success;
        else
                return e_failure;

}

//2. cpoy the bmp header
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    printf("INFO: Copying Image Header\n");
    char buf[54];				// Array to store the BMP header (54 bytes)
    fread(buf, 54, 1, fptr_src_image);		// Read the BMP header from the source image
    fwrite(buf, 54, 1, fptr_dest_image);	// Write the BMP header to the destination image
    return e_success;
}

//Encode a byte into the LSB of an image buffer 
Status encode_int_to_lsb(int data, char *image_buffer)
{
    int index = 0;			// Initialize index for image buffer
    for (int i = 31; i >= 0; i--)		// Loop through all 32 bits of the integer
    {
        if (data & (1 << i))		// Set LSB of image byte to 1
        {
            image_buffer[index] = image_buffer[index] | 1;
        }
        else				// If the current bit of data is 0
        {
            image_buffer[index] = image_buffer[index] & (~1);		// Set LSB of image byte to 0
        }
        index++;			// Move to the next byte in the image buffer
    }
    return e_success;
}
// Encode an integer into the LSB of an image buffer 
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int index = 0;
    for (int i = 7; i >= 0; i--)
    {
        if (data & (1 << i))		// If the current bit of data is 1
        {
            image_buffer[index] = image_buffer[index] | 1;	// Set LSB of image byte to 1
        }
        else							// If the current bit of data is 0
        {
            image_buffer[index] = image_buffer[index] & (~1);	// Set LSB of image byte to 0
        }
        index++;
    }
    return e_success;
}

//3. encode magic string and length of the str
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen(magic_string);				// Get length of the magic string
    //printf("Encoding magic string: %s\n", MAGIC_STRING);
    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);			//read 32 bytes from the source img
    encode_int_to_lsb(len, buf);				//function call
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);		//write buffer to stego image
    printf("INFO: Encoding Magic String Signature\n");
    char buffer[8];						// Buffer to encode each byte of the magic string
    for (int i = 0; i < len; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);		// Encode each byte of the magic string
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}


//4. enocoding the secret file extension
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int ext_size=strlen(file_extn);				// Get the length of the file extension
    //printf("Secret ext size: %d bytes\n", ext_size);
    char buf[32];						// Buffer to read and encode the extension size
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(ext_size, buf);				// Encode the size of the extension into the buffer
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);

    printf("INFO: Encoding secret.txt File Extension\n");
    char buffer[8];
    for (int i = 0; i < ext_size; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);		// Encode each byte of the extension
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

//5. Encode the size of the secret file
Status secret_file_size(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_END);		// Move file pointer to the end(offest)
    file_size = ftell(encInfo->fptr_secret);		// Get the file size
    rewind(encInfo->fptr_secret);			// get back to the beginning of the file
    return file_size;

}
// 6.Encode the size of the secret file into the image
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Size\n");
    //printf("Secret file size: %ld bytes\n", file_size);
    int index = 0;
    char buf[32];					// Buffer to read and encode the file size
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, buf);			// Encode the file size into the buffer
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

//7.Encode the secret file data into the image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Data\n");
    char ch;						// Variable to hold each byte of the secret file
    char buffer[8];
    while((ch=fgetc(encInfo->fptr_secret))!=EOF)	// Loop until the end of the file
    {
        //printf("%c",ch);
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, buffer);			// Encode each byte of secret file data
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);

    }
    return e_success;
}

// 8. Copy remaining image data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    int c;						// Variable to hold each byte of the image
    while((c=fgetc(fptr_src))!=EOF)			// Loop until the end of the source image
    {
        fputc(c,(fptr_dest));				// Write each byte to the destination (stego) image
    }
        //printf("copying done\n");
    return e_success;
 }
