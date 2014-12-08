#include "sample.h"



void printSampleInfo( struct Sample *sample )
{
	printf(" Name: %22s, Length: %5d, Finetune: %3d, Volume: %3d, Loop Start: %5d, Loop Length: %5d\n",
		sample->name, sample->length, sample->finetune, sample->volume, sample->loopStart, sample->loopLength);	
}


int readSampleHeader( struct Sample *sample, FILE *fp )
{
	uint8_t bytes[2]; // For reading in the 16 bit values in amiga's big endian
	int ele_read;
	
	// First the name of the sample
	ele_read = fread( sample->name, SAMPLE_NAME_SIZE, 1, fp );
	sample->name[SAMPLE_NAME_SIZE] = '\0';
	
	if(ele_read != 1) 
	{ 
		fprintf(stderr, "ERROR: An error occurred while reading in the sample name\n");
		return -1; 
	}

	// Read in the length, and handle the big endian file format
	ele_read = fread( bytes, SAMPLE_LENGTH_SIZE, 1, fp );
	sample->length = ((bytes[0] << 8) | bytes[1]) << 1;
	
	if(ele_read != 1) 
	{ 
		fprintf(stderr, "ERROR: An error occurred while reading in the sample length\n");
		return -1; 
	}
	
	// Finetune is stored as a weird unsigned version of a signed value, 0-7 is normal, 8-15 is -8 through -1
	ele_read = fread( &(sample->finetune), SAMPLE_FINETUNE_SIZE, 1, fp );
	if(ele_read != 1) 
	{ 
		fprintf(stderr, "ERROR: An error occurred while reading in the sample finetune\n");
		return -1; 
	}
	if(sample->finetune > 7) sample->finetune -= 16;
	
	ele_read = fread( &(sample->volume), SAMPLE_VOLUME_SIZE, 1, fp );
	if(ele_read != 1) 
	{ 
		fprintf(stderr, "ERROR: An error occurred while reading in the sample volume\n");
		return -1; 
	}
	
	// Read in the loop start, and handle the big endian file format
	ele_read = fread( bytes, SAMPLE_LOOP_START_SIZE, 1, fp );
	if(ele_read != 1) 
	{ 
		fprintf(stderr, "ERROR: An error occurred while reading in the sample loop start\n");
		return -1; 
	}
	sample->loopStart = ((bytes[0] << 4) | bytes[1]) << 1;
	
	// Read in the loop length, and handle the big endian file format
	ele_read = fread( bytes, SAMPLE_LOOP_LENGTH_SIZE, 1, fp );
	if(ele_read != 1) 
	{ 
		fprintf(stderr, "ERROR: An error occurred while reading in the sample loop length\n");
		return -1; 
	}
	sample->loopLength = ((bytes[0] << 4) | bytes[1]) << 1;
	
	
	return 0;
}
	

int readSampleData( struct Sample *sample, FILE *fp )
{
	// Check if this is an unused sample
	if( sample->length == 0 )
	{
		sample->sample = NULL;
		return 0;
	}
	
	sample->sample = malloc( sample->length );
	
	if( sample->sample == NULL )
	{
		fprintf(stderr, "ERROR: Failed to allocate memory to store sample of %d bytes\n", sample->length);
		return 1;
	}
	
	if( fread(sample->sample, sample->length, 1, fp) != 1 )
	{
		fprintf(stderr, "ERROR: Failed to read sample data of length %d from file\n", sample->length);
		return 1;
	}
		
	return 0;
}


void freeSampleData( struct Sample *sample )
{
	free( sample->sample );
	sample->sample = NULL;
}
