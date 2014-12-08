#ifndef _CHANNELS_H
#define _CHANNELS_H

#include "pattern.h"
#include "mod_file.h"


struct Channel
{
	struct Pattern *patterns; // Array of patterns in the song
	int numPatterns;
	
};

int initChannel( struct Channel *channel, int numPatterns );
void freeChannelPatterns( struct Channel *channel );



#endif
