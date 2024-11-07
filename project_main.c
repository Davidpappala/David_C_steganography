/*DOCUMENTATION:
 1.Name: David Pappala
 2.Date: 18/09/24
 3.Title: "Programme to encode secret file data to source image file by creating new image file with option '-e' and decode data from new image file to new text file"
 4.Project : "Steganography"
 5.Input:	"For option 'e'
		a) Source image file name
		b) Secret text file name
		c) Output image file name
		For option 'd'
		a) Stego image file name
		b) Output text file name"
6. Output:"Data in secret file copied to Output text file"*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

// Main function that handles encoding and decoding based on user input
int main(int argc, char*argv[])
{
    // Check if the arguments passed are less than or equal to 1
    // This means no operation is specified by the user
    if(argc<=1)
    {
	    // Print usage information for both encoding and decoding
            printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
            printf("%s: Decoding: %s -e <.bmp file> [output file]\n",argv[0],argv[0]);
            return e_failure;
    }
    // If the user input matches the encoding operation "-e"
    if(strcmp(argv[1],"-e")==0)
    {
	// Declare and initialize an EncodeInfo structure
        EncodeInfo encInfo;
        encInfo.src_image_fname = argv[2];
        encInfo.secret_fname = argv[3];

	// If the user provided an output file name,it will create the user one,otherwise it will"output.bmp"
        if(argv[4] != NULL)
        {
                encInfo.stego_image_fname = argv[4];
        }
        else
        {
                printf("INFO: Output File not mentioned. Creating output.bmp as default\n");
                encInfo.stego_image_fname = "output.bmp";
        }

	// If fewer than 4 arguments are passed, print usage information and return failure
        if(argc<4)
        {
            printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
            return e_failure;
        }

        do_encoding(argv,&encInfo);	// Call the encoding function, passing the arguments and encode info structure
    }

    // If the user input matches the decoding operation "-d"
    else if(strcmp(argv[1],"-d")==0)
    {
        DecodeInfo decInfo;				// Declare and initialize a DecodeInfo structure
        decInfo.stego_image_fname = argv[2];		// Store the stego image file name from user input
        
	// If fewer than 3 arguments are passed, print usage information and return failure
	if(argc<3)
        {
            printf("%s: Decoding: %s -e <.bmp file> [output file]\n",argv[0],argv[0]);
                return e_failure;
        }
        do_decoding(argv, &decInfo);
        //decoding
    }
    // If the user input does not match "-e" or "-d", print unsupported operation message
    else
    {
            printf("unsupported operation\n");
            return e_failure;
    }

    return 0;
}

