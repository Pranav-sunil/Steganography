#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"


/* This file contains the function definitions required for 
   carrying out the encoding process                     */

uint get_image_size_for_bmp(FILE *fptr_image)
{
	/* Function is used to obtain the amount of RGB data
	   int the image file. ( Encoding is done only on the 
	   RGB data of the image */
	
	uint width, height;

	fseek(fptr_image, 18, SEEK_SET);

	fread(&width, sizeof(int), 1, fptr_image);

	fread(&height, sizeof(int), 1, fptr_image);

	return width * height * 3;
	
	/* A pixel is the basic unit of an image. Each pixel is made up
	   of 3 character data; R (Red), G (Green), B (Blue). The intensity of
	   each color will determinte the final color of that pixel. Hence the size 
	   is multiplied by 3. */
}

Status open_files(EncodeInfo *encInfo)
{
	/* Function is used to open the necessary files */

	//Opening the image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", encInfo->src_image_fname);

	// Opening the file containing the secret data
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", encInfo->secret_fname);

	// Opening the oputput image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);

	return e_success;
}

Status read_and_validate_encode_args( char *argv[], EncodeInfo *encinfo )
{
	/* Function is used to check the command line argument for proper input
	   and to assign a default filename if user has not input a name for the 
	   output file */

	//Checking if src file is .bmp
	if( strstr( argv[2], ".bmp") != NULL )
	{
		encinfo->src_image_fname = argv[2];
	}
	else
	{
		printf("ERROR: The source file passed should be a .bmp file\n");
	}

	//Obtaing the extions of the file containing the secret data
	if( strchr( argv[3], '.') != NULL )
	{
		encinfo->secret_fname = argv[3];
		strcpy(encinfo->extn_secret_file, strchr( argv[3], '.' ) );
	}
	else
	{
		printf("Error : The secret file does not have an extension\n");
		return e_failure;
	}

	//Checking if output filename has been input by the user
	if( argv[4] != NULL )
	{
		//Checking if file extension is .bmp 
		if( strstr ( argv[4] , ".bmp" ) != NULL )
		{
			encinfo->stego_image_fname = argv [4];
		}
		else
		{
			printf("Error : The output file is not a bmp file\n");
			return e_failure;
		}
	}
	else
	{
		//Assigns a default file name
		printf("INFO: Output File not mentioned. Creating stego.bmp as default\n");
		encinfo->stego_image_fname = "Output.bmp";
	}

	return e_success;
}

Status do_encoding( EncodeInfo *encInfo )
{
	/* This function carries out the encoding process by calling 
	   other functions to perform the encoding process */

	printf("INFO: Opening required files\n");
	if ( open_files( encInfo ) == e_success )
	{
		printf("INFO: Done\n");
		printf("INFO: ## Encoding Procedure Started ##\n");
		printf("INFO: Checking for %s size\n", encInfo->secret_fname);
		if( get_file_size( encInfo->fptr_secret ) > 0 )
		{
			printf("INFO: Done. Not Empty\n");
			printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
			if ( check_capacity( encInfo ) == e_success )
			{
				printf("INFO: Done. Found OK\n");
				printf("INFO: Copying Image Header\n");
				if( copy_bmp_header( encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success )
				{
					printf("INFO: Done\n");
					printf("INFO: Encoding Magic String Length\n");
					if( encode_magic_string_length( encInfo ) == e_success )
					{
						printf("INFO: Done\n");
						printf("INFO: Encoding Magic String Signature\n");
						if (encode_magic_string(encInfo->magic_string_sig, encInfo) == e_success )
						{
							printf("INFO: Done\n");
							printf("INFO: Encoding %s Extension Size\n", encInfo->secret_fname);
							if( encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success )
							{
								printf("INFO: Done\n");
								printf("INFO: Encoding %s Extension\n", encInfo->secret_fname);
								if( encode_secret_file_extn( encInfo->extn_secret_file, encInfo) == e_success)
								{
									printf("INFO: Done\n");
									printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
									if( encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
									{
										printf("INFO: Done\n");
										printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
										if( encode_secret_file_data( encInfo) == e_success )
										{
											printf("INFO: Done\n");
											printf("INFO: Copying Left Over Data\n");
											if( copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure )
											{
												printf("ERROR: Unable to copy left over data to output image file\n");
												return e_failure;
											}
											else
											{
												printf("INFO: Done\n");
												return e_success;
											}
										}
										else
										{
											printf("ERROR: Unable to encode secret file data\n");
											return e_failure;
										}
									}	
									else
									{
										printf("ERROR: Unable to encode secret file size\n");
										return e_failure;
									}
								}
								else
								{
									printf("ERROR: Unable to encode secret file extension\n");
									return e_failure;
								}
							}
							else
							{
								printf("ERROR: Unable to encode secret file extension size\n");
								return e_failure;
							}
						}	
						else
						{
							printf("ERROR: Unable to encode magic string\n");
							return e_failure;
						}
					}
					else
					{
						printf("ERROR: Unable to encode magic string length\n");
						return e_failure;
					}
				}
				else
				{
					printf("ERROR: Unable to copy header data of input file\n");
					return e_failure;
				}
			}
			else
			{
				printf("ERROR: File %s does not have the capacity to store the data\n", encInfo->src_image_fname);
				return e_failure;
			}
		}
		else
		{
			printf("ERROR: ");
			return e_failure;
		}
	}
	else
	{
		printf("ERROR: Unable to open files\n");
		return e_failure;
	}
	return e_success;
}

Status check_capacity( EncodeInfo *encInfo )
{
	//Function to check if the source image file has enough space to store data to be encoded

	//Obtains image size
	encInfo->image_capacity = get_image_size_for_bmp( encInfo->fptr_src_image );

	//Obtains size of secret file
	encInfo->size_secret_file = get_file_size( encInfo->fptr_secret );

	//Checking for capacity of the source image file to store the data 

	/* The image should have the capacity to store the following data :
	   1. Size of datatype to store the magic string length : 4 byte
	   2. Magic string data : (string_length of magic string) byte
	   3. Size of data type to store extension of secret file : 4 byte
	   4. Extension of the secret file : (string length of extension ) byte
	   5. Size of datatype to store number of characters in secret file : 4 byte
	   6. Secret file data : (Number of characters in secret file ) byte
	 
	   To encode each byte of data to the image, 8 byte of image data (RGB Data) is required */
	if( ((encInfo->image_capacity)) > (( 4 + strlen(MAGIC_STRING) + strlen(encInfo->extn_secret_file) + 4 + 4 + encInfo->size_secret_file )*8) )
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
} 

uint get_file_size( FILE *fptr )
{
	//Function to get the number of characters in the secret file

	fseek(fptr, 0, SEEK_END); //Moves to the end of the file
	return ftell(fptr); //Returns the number of index to which file pointer points
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image )
{
	//Function reads 54 byte header data from source file and write to stego file directly
	//The header data is not to be modifed nor can be encoded with data

	rewind( fptr_src_image); //Source image file pointer should be reset after get image file size fn is called
	char buffer[54]; //Buffer to store the header data from source file
	if( fread( buffer, 1, 54, fptr_src_image) != 54 )
	{
		return e_failure;
	}
	if ( fwrite( buffer, 1, 54, fptr_dest_image) != 54 ) //Stores the data obtained from source file to destination file
	{
		return e_failure;
	}

	return e_success;
}

Status encode_magic_string_length( EncodeInfo *encInfo )
{
	/* Function encodes the length of the magic string */

	char buffer[32];
	if( fread( buffer, 1, 32, encInfo->fptr_src_image) != 32 )
	{
		return e_failure;
	}
	if( encode_size_to_lsb( encInfo->magic_str_length, buffer) == e_failure )
	{
		return e_failure;
	}
	if( fwrite( buffer, 1, 32, encInfo->fptr_stego_image) != 32 )
	{
		return e_failure;
	}
	return e_success;
}


Status encode_magic_string( const char *magic_string, EncodeInfo *encInfo )
{
	//This function encodes the magic string data to the output image file
	//Here instead of using loops, encode_data_to_image fn is called to write the data to the image
	if( encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure )
	{
		return e_failure;
	}
	return e_success;
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Use a loop for size times and call the folling function to incorporate magic string data to the destination file
	//Store 8 bytes of RGB data from soruce file to buffer
	char buffer[8];
	for( int i = 0 ; i < size ; i++ )
	{
		//To get 8 bytes of data from source file and store it to the buffer to store 1 byte of magic string data
		if(  fread( buffer, 1, 8, fptr_src_image)  != 8 )	
		{
			return e_failure;
		}
		if( encode_byte_to_lsb( data[i], buffer) == e_failure )
		{
			return e_failure;
		}
		if( fwrite( buffer, 1, 8, fptr_stego_image) != 8 ) //To write the data obtained to the output file
		{
			return e_failure;
		}
	}
	return e_success;
}

Status encode_byte_to_lsb( char data, char *image_buffer )
{
	// Logic to encode the data to of magic string to the image buffer
	//Loop is used to store each bit from MSB of the 1 byte data to the LSB of each character of buffer

	int i=0, k=0;
	for( i = 7 ; i >= 0 ; i-- )
	{
		unsigned char temp = (data & ( 1 << i ) );
		temp = temp >> i;
		image_buffer[k] = ( image_buffer[k] & ( ~ ( 0x01 ) ) ) | temp;
		k++;
	}
	return e_success;
}

Status encode_size_to_lsb( long size, char *buffer )
{
	//Function to encode the size data ( 4 byte data ) to the buffer of size 32 byte
	//Loop is used to store each bit from MSB of size to LSB of each element of buffer

	int i = 0 , k = 0 ;
	for( i = 31 ; i >= 0 ; i-- )
	{
		unsigned char temp = (size & ( 1 << i ) );
		temp = temp >> i;
		buffer[k] = ( buffer[k] & ( ~ ( 0x01 ) ) ) | temp;
		k++;
	}
	return e_success;
}

Status encode_secret_file_extn_size( long secret_file_extn_size, EncodeInfo *encInfo )
{
	//Function to encode the extn size of secret file to the output image

	char buffer[32];
	if( fread( buffer, 1, 32, encInfo->fptr_src_image) != 32 )
	{
		return e_failure;
	}
	if( encode_size_to_lsb( secret_file_extn_size, buffer) == e_failure )
	{
		return e_failure;
	}
	if( fwrite( buffer, 1, 32, encInfo->fptr_stego_image) != 32 )
	{
		return e_failure;
	}
	return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo )
{
	//The function encodes the extension of the secret file to the output image
	//encInfo->image_data is the buffer declared from structre

	for( int i = 0 ; i < strlen( file_extn ) ; i++ )
	{
		if( fread( encInfo->image_data, 1, 8, encInfo->fptr_src_image) != 8 )
		{
			//If fread does not read 8 chars from source image, process fails
			return e_failure;
		}
		if( encode_byte_to_lsb( file_extn[i], encInfo->image_data) == e_failure )
		{
			return e_failure;
		}
		if( fwrite( encInfo->image_data, 1, 8, encInfo->fptr_stego_image) != 8 )//To write the data obtained to the output file
		{
			//If fwrite does not return 8 after writing data to output file, process fails
			return e_failure;
		}
	}
	return e_success;
}

Status encode_secret_file_size( long file_size, EncodeInfo *encInfo)
{
	//Logic to write secret file size to the output file
	
	char buffer[32];
	if( fread( buffer, 1, 32, encInfo->fptr_src_image ) != 32 )
	{
		//Fread should return 32 after reading 32 bytes from source file and writing to buffer 
		return e_failure;
	}
	if( encode_size_to_lsb( file_size, buffer ) == e_failure )
	{
		return e_failure;
	}
	if( fwrite( buffer, 1, 32, encInfo->fptr_stego_image ) != 32 )
	{
		//fwrite should return 32 after writing 32 bytes from source file and writing to output file
		return e_failure;
	}

	return e_success;
}

Status encode_secret_file_data( EncodeInfo *encInfo )
{
	//Function is used to encode the secret file data to the image file

	char secret_data[encInfo->size_secret_file]; //Declares buffer to store secret file data for encoding
	rewind( encInfo->fptr_secret);
	fread( secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret );

	for( int i = 0 ; i < encInfo->size_secret_file ; i++ )
	{
		if( fread( &encInfo->image_data, 1, 8, encInfo->fptr_src_image) != 8 )	//To get RGB data from BMP file
		{
			//If fread does not sucessfully read from src file and write to buffer and retruns 8, the process fails
			return e_failure;
		}
		if( encode_byte_to_lsb( secret_data[i], encInfo->image_data) == e_failure ) //To store each character to the bmp file buffer
		{
			return e_failure;
		}
		if( fwrite( &encInfo->image_data, 1, 8, encInfo->fptr_stego_image) != 8 )	//To write the encoded buffer to the output file
		{
			//If fwtite does not sucessfully write to output file from buffer and return 8 ( number of characters written to file ), then process fails
			return e_failure;
		}
	}

	return e_success;
}
Status copy_remaining_img_data( FILE *fptr_src, FILE *fptr_dest)
{
	//Function copies the remaining data that was not used for encoding to the output file
	
	char ch; //Read each byte from the file and store to the variable
	while( fread(&ch, 1, 1, fptr_src) != 0 ) //Use while loop until return fread returns 0 ( which occurs when EOF is encoutered )
	{
		if( fwrite(&ch, 1, 1, fptr_dest) != 1 )
		{
			//If character is not sucessfully read to ch, the process fails
			return e_failure;
		}
	}
	return e_success;
}
