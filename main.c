#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

/*                       ### Documentation ###

          Name : Pranav S
          Project Name : LSB Steganography
Description : The project is to encode data in a secret file to a
              .bmp file using LSB steganography. The user can input
			  a magic string, i.e a key to lock the data. Upon decoding 
			  with the correct magic sting, the encoded file will be 
			  obtained.

Sample execution for encoding : ./a.out -e bmp_file.bmp secret_datafile.extension* output_bmp_filename.bmp(optional)

Sample execution for decoding : ./a.out -d encoded_image.bmp output_file_name.extension(optional)**

* File of any extension can be passed.
** Output file extension will be obtained when decoding the data from the image.
 */

/*		This file contains the main function to call upon other functions to do
		the process required by the user.											*/

int main(int argc, char *argv[])
{
	EncodeInfo encInfo;
	DecodeInfo decInfo;
	int ret;
	if( argc > 2 )
	{
		ret = check_operation_type(argv);
	}
	else
	{
		printf("ERROR: Insufficient Number of Command Line Arguments\n");
		return 0;
	}
	if( ret == e_encode )
	{
		/* Encoding Operation */
		printf("Selected Encoding\n");

		if( argc >= 4 ) //To check if sufficient number of CLA has been passed
		{
			if( read_and_validate_encode_args(argv, &encInfo) == e_success )
			{
				printf("Enter Magic String Signature : ");
				scanf("%s", encInfo.magic_string_sig);
				encInfo.magic_str_length=strlen(encInfo.magic_string_sig);
				if( do_encoding(&encInfo) == e_success )
				{
					printf("## Encoding Done Sucessfully ##\n");
				}
				else
				{
					printf("ERROR: Encoding Unsucessful\n");
				}
			}
		}
		else
		{
			printf("ERROR : Insufficent number of Command Line Arguments\n");
		}
	}
	else if( ret == e_decode )
	{
		/* Decoding Operation */
		printf("Selected Decoding\n");

		if( argc >= 3 )
		{
			if( read_and_validate_decode_args( argv, &decInfo) == e_success )
			{
				printf("Enter The Magic String Signature : ");
				scanf("%s", decInfo.magic_string_sig_user);
				if( do_decoding( &decInfo) == e_success )
				{
					printf("## Decoding Done Sucessfully ##\n");
				}
				else
				{
					printf("ERROR: Decoding Unsucessful\n");
				}
			}
		}
		else
		{
			printf("ERROR: Insufficient number of Command Line Arguments\n");
		}
	}
	else
	{
		printf("ERROR: Unsupported format\n");
	}
	return 0;
}

OperationType check_operation_type( char *argv[])
{
	/* Function determines the operation based on user input */
	if( strcmp(argv[1], "-e") == 0 )
	{
		return e_encode;
	}
	else if( strcmp(argv[1], "-d") == 0 )
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}
