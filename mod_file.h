#ifndef _MOD_FILE_H
#define _MOD_FILE_H

#define MOD_BUFFER_SIZE              256
#define MOD_SIGNATURE_OFFSET		1080
#define MOD_SIGNATURE_SIZE			   4
#define MOD_NAME_SIZE				  20
#define MOD_SONG_LENGTH_SIZE           1
#define MOD_ORDERS_SIZE				   1
#define MOD_ORDERS_NUM				 128

#define MOD_NUM_SAMPLES				  31
#define MOD_NUM_PATTERNS			 256
#define MOD_NUM_PATTERN_ROWS          64

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sample.h"
#include "channels.h"


struct ModFile
{
	char name[MOD_NAME_SIZE + 1];
	char signature[MOD_SIGNATURE_SIZE + 1];
	int numChannels;			// 4 for M.K., 6 for 6CHN, 8 for 8CHN

	uint8_t songLength;    // Number of orders in the song
	uint8_t numPatterns;
	uint8_t orders[MOD_ORDERS_NUM];

	struct Sample samples[MOD_NUM_SAMPLES];
	
	// Array of our channels
	struct Channel *channels;
};


int loadModFile(struct ModFile *mod, const char *filename );
void freeModFile(struct ModFile *mod);


void debugDumpSample(struct ModFile *mod, int num);

#endif
