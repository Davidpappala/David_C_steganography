#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"


// Function to validate decoding arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	// Check if the second argument is a .bmp file
        char *bmp_ext = strchr(argv[2], '.');
        if (bmp_ext == NULL || strcmp(bmp_ext, ".bmp") != 0)
        {
                printf("%s: Decoding: %s -e <.bmp file> [output file]\n",argv[0],argv[0]);
                return e_failure;
        }
	// Set output filename or use default
        if ( argv[3] != NULL )
        {
                char str[10];	
                strcpy(str,argv[3]);		// Copy the output filename argument to a temporary string
                char *token=strtok(str,".");	// Extract the filename without extension
                strcpy(decInfo->output_fname,token);	// Set the output filename
    	}
        else
        {
		// Use a default output filename if none is provided
                printf("INFO: Output File not mentioned. Creating output as default\n");
                strcpy(decInfo->output_fname,"output");
        }
	
	// Check if decoding arguments are correct
        if (strcmp(argv[0], "./a.out") == 0 && strcmp(argv[1], "-d") == 0)
        {
                return e_success;
        }
        else
        {
		// Print error message if arguments are incorrect
                printf("Error: Invalid arguments\n"); 
                return e_failure;
        }

}

// Function to open necessary files
Status dopen_files(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started\n");
    printf("INFO: Opening required files\n");
    printf("INFO: Opened steged_img_.bmp\n");
    // open Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("The Stego image file is not opened\n");
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// Main decoding function
Status do_decoding(char *argv[],DecodeInfo *decInfo)
{

    if(read_and_validate_decode_args(argv, decInfo) != e_success)	// Validate arguments and open files
    {
        printf("file of read and validate not opened\n");
        return e_failure;
    }
    // Open the necessary files
    if (dopen_files(decInfo) != e_success)
    {
        printf("Failed to open files.\n");
        return e_failure;
    }
    
    // Decode 1.magic string
    if (decode_magic_string(decInfo->magic_string,decInfo) != e_success)
    {
        printf("Failed to decode magic string.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    //2. Decode file extension size
    if(decode_secret_file_ext_size(decInfo->secret_file_ext, decInfo)!= e_success)
    {
            printf("Failed to decode secret file ext\n");
            return e_failure;
    }
    printf("INFO: Done\n");
    //3. Decode secret file size
    if(decode_secret_file_size(decInfo->s_file_size,decInfo)!= e_success)
    {

            printf("Failed to decode secret file size\n");
            return e_failure;
    }
    printf("INFO: Done\n");
    //4. secret file data
    if(decode_secret_file_data(decInfo->s_file_data, decInfo)!=e_success)
    {

            printf("Failed to decode secret file data\n");
            return e_failure;
     }
    printf("INFO: Done\n");
    
    printf("INFO: ## Decoding Done Successfully\n");
    return e_success;
}

Status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);	// Seek to the position in the file where the magic string is encoded
    char buf[32];
    fread(buf, 32, 1, decInfo->fptr_stego_image);	  // Read 32 bytes from the stego image
    decInfo->magic_string_len = decode_lsb_to_int(buf);   // Decode the magic string length
    //printf("Magic length: %d\n",decInfo->magic_string_len);

    char buffer[8];
    for (int i = 0; i < decInfo->magic_string_len; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);		// Read 8 bytes for each character
        decInfo->magic_string[i] = decode_lsb_to_byte(buffer);	// Decode each byte to get the character
    }
    decInfo->magic_string[decInfo->magic_string_len] = '\0';	// Null-terminate the magic string
    //printf("Magic string: %s\n",decInfo->magic_string);
    if(strcmp(MAGIC_STRING,decInfo->magic_string) != 0)
    {
            printf("Magic string is not equal\n");
            return e_failure;
    }
    return e_success;
}
// Function to decode 32-bit integer from LSBs
int decode_lsb_to_int(char *image_buffer)
{
    int data = 0;
    for (int i = 0; i < 32; i++)
    {
      data <<= 1;			// Shift left to make room for the next bit
      data |= image_buffer[i] & 1;	// Get LSB and add to result

    }
    return data;
}
// Function to decode a byte from LSBs
char decode_lsb_to_byte(char *image_buffer)
{
    char ch=0;
    for (int i = 0; i <8; i++)
    {
        ch <<= 1 ;
        ch |= image_buffer[i] & 1;
    }
    return ch;
}

// Function to decode the secret file extension
Status decode_secret_file_ext_size(char * secret_file_ext,DecodeInfo*decInfo)
{
    printf("INFO: Decoding Output File Extension\n");
    char buf[32];
    fread(buf, 32, 1, decInfo->fptr_stego_image);
    decInfo->secret_file_extsize = decode_lsb_to_int(buf);
    //printf("Secret File extsize: %d\n",decInfo->secret_file_extsize);

    char buffer[8];
    for (int i = 0; i < decInfo->secret_file_extsize; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->secret_file_ext[i] = decode_lsb_to_byte(buffer);
    }
    decInfo->secret_file_ext[decInfo->secret_file_extsize] = '\0';
    strcat(decInfo->output_fname,secret_file_ext);
    printf("INFO: Opened %s\n",decInfo->output_fname);
    printf("INFO: Done.Opened all required files\n");
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    //printf("Secret_file_ext: %s\n",decInfo->secret_file_ext);
    return e_success;
}
// Function to decode the size of the secret file
Status decode_secret_file_size(long s_file_size, DecodeInfo *decInfo)
{
    printf("INFO: Decoding File Size\n");
    char buf[32];
    fread(buf, 32, 1, decInfo->fptr_stego_image);
    decInfo->s_file_size = (long)decode_lsb_to_int(buf);
    //printf("Secret_file_size : %ld\n",decInfo->s_file_size);
    return e_success;
}

// Function to decode the secret file data
Status decode_secret_file_data(char *secret_file_data, DecodeInfo *decInfo)
{
    printf("INFO: Decoding File Data\n");
    char buffer[8];
    //printf("%d\n",secret_data_len);
    for(int i = 0; i < decInfo->s_file_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->s_file_data[i] = decode_lsb_to_byte(buffer);		//decode each type
	putc(decInfo->s_file_data[i],decInfo->fptr_output);
    }
    //printf("%s\n",decInfo->s_file_data);
    decInfo->s_file_data[decInfo->s_file_size] = '\0';			//null- terminate the secret d
    return e_success;
}


