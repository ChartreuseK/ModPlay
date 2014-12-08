
#ifndef _SAMPLE_H
#define _SAMPLE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define big_to_little16(n)  	( ((n&0x0F)<<4) | ((n&0xF0)>>4) )
#define SAMPLE_BUFFER_SIZE 			64


#define SAMPLE_NAME_SIZE		    22
#define SAMPLE_LENGTH_SIZE       	 2
#define SAMPLE_FINETUNE_SIZE     	 1
#define SAMPLE_VOLUME_SIZE       	 1
#define SAMPLE_LOOP_START_SIZE       2
#define SAMPLE_LOOP_LENGTH_SIZE      2

struct Sample
{
	char     name[SAMPLE_NAME_SIZE+1];
	uint32_t length;		
	int8_t   finetune;
	uint8_t  volume;
	uint32_t loopStart;
	uint32_t loopLength;
	
	int8_t *sample;		// Two's compliment signed sample data
};

int readSampleHeader( struct Sample *sample, FILE *fp );
void printSampleInfo( struct Sample *sample );
int readSampleData( struct Sample *sample, FILE *fp );
void freeSampleData( struct Sample *sample );

#endif
