#include "channels.h"

#include <stdlib.h>


int initChannel( struct Channel *channel, int numPatterns )
{
	channel->patterns = malloc( sizeof(struct Pattern) * numPatterns );
	channel->numPatterns = numPatterns;
	
	if( channel->patterns == NULL ) 
	{
		fprintf(stderr, "ERROR: Failed to allocate memory for a channels patterns\n");
		return 1;
	}
	
	return 0;
}

void freeChannelPatterns( struct Channel *channel )
{
	free( channel->patterns );
	channel->patterns = NULL;
	
	// Make numPatterns negative to indicate we have no allocated patterns here
	channel->numPatterns = -1;
}
