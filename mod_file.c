#include "mod_file.h"




int allocateChannels( struct ModFile *mod );
int readPatterns( struct ModFile *mod, FILE *mfile );
uint16_t periodToNote( uint16_t period ) ;



int loadModFile(struct ModFile *mod, const char *filename )
{
	FILE *mfile = fopen( filename, "rb");
	int ele_read; int i;
	
	// First check if the signature is a traditional MOD file (M.K., 6CHN, 8CHN)
	fseek(mfile, MOD_SIGNATURE_OFFSET, SEEK_SET );
	ele_read = fread(mod->signature, MOD_SIGNATURE_SIZE, 1, mfile);
	if( ele_read != 1 )
	{
		// Error while reading the signature
		fprintf(stderr,"ERROR: An error occurred while trying to read in the signature\n");
		return 1;
	}
	// Force our signature to be a null terminated string
	mod->signature[4] = '\0';
	
	// Make sure our signature is one we are expecting
	if     (strcmp(mod->signature,"M.K.") == 0) { mod->numChannels = 4; }
	else if(strcmp(mod->signature,"4CHN") == 0) { mod->numChannels = 4; }
	else if(strcmp(mod->signature,"6CHN") == 0) { mod->numChannels = 6; }
	else if(strcmp(mod->signature,"8CHN") == 0) { mod->numChannels = 8; }
	else
	{
		// Invalid signature for MOD file
		fprintf(stderr,"ERROR: An invalid signature \"%s\" was found in the file\n", mod->signature);
		return 1;
	}
	printf("DEBUG: Signature: %s\n", mod->signature);
	
	// Now to load in the file
	// First the module name
	fseek(mfile, 0, SEEK_SET);
	ele_read = fread(mod->name, MOD_NAME_SIZE, 1, mfile);
	if( ele_read != 1 )
	{
		fprintf(stderr,"ERROR: Failed to read the mod name. How did we fail to read the start of the file but succeeded in reading the signature?!\n");
		return 1;
	}
	// Force the name to be a null terminated string
	mod->name[MOD_NAME_SIZE] = '\0';
	
	printf("DEBUG: Name: %s\n", mod->name);
	
	
	printf("DEBUG: Samples: \n");
	// Now we read in the 31 sample headers
	for( i = 0; i < MOD_NUM_SAMPLES; i++ )
	{
		// Have each sample load in its header data from the mod file
		if( readSampleHeader( &mod->samples[i], mfile ) != 0 )
		{
			// Error while reading in the sample
			fprintf(stderr, "ERROR: An error occurred while reading in sample header %d\n", i);
			return 1;
		}
		
		printSampleInfo( &mod->samples[i] );
	}
	
	// Next is the song length (in orders)
	ele_read = fread(&mod->songLength, MOD_SONG_LENGTH_SIZE, 1, mfile);
	if( ele_read != 1 )
	{
		fprintf(stderr,"ERROR: Failed to read the song length (in orders)\n");
		return 1;
	}
	// Skip the next unused byte
	fseek(mfile, 1, SEEK_CUR);
	
	// Now we read in the orders table
	ele_read = fread(mod->orders, MOD_ORDERS_SIZE, MOD_ORDERS_NUM, mfile);
	if( ele_read != MOD_ORDERS_NUM )
	{
		fprintf(stderr,"ERROR: Failed to read the orders table\n");
		return 1;
	}
	// Now we need to find the highest pattern number
	mod->numPatterns = 0;
	for( i = 0; i < MOD_ORDERS_NUM; i++ )
	{
		// Pattern numbering starts at 0, so our count is one higher
		if( (mod->orders[i] + 1) > mod->numPatterns ) 
			mod->numPatterns = (mod->orders[i] + 1);
	}
	
	// Now we're back to our signature, we can skip over it
	fseek(mfile, MOD_SIGNATURE_SIZE, SEEK_CUR);
	
	
	// Next is the pattern data, let's allocate our channels and patterns
	if( allocateChannels( mod ) != 0 )
	{
		fprintf(stderr, "ERROR: Failed to initialize channels\n");
		return 1;
	}
	
	// Now we read in the actual pattern data
	if( readPatterns( mod, mfile ) != 0 )
	{
		fprintf(stderr, "ERROR: Error reading in pattern data\n");
		return 1;
	}
	
	// Finally we're at the sample data, 8-bit signed
	for( i = 0; i < MOD_NUM_SAMPLES; i++ )
	{
		if( readSampleData( &mod->samples[i], mfile ) != 0 )
		{
			fprintf(stderr, "ERROR: Error reading sample data for sample %d\n", i);
			return 1;
		}
	}
	
	
	return 0;
	
}

/* allocateChannels( struct ModFile * )
 *  Allocates our array of channels and for each initializes them
 * 
 **********************************************************************/
int allocateChannels( struct ModFile *mod )
{
	int i;
	// Allocate how many channels we need
	mod->channels = malloc( sizeof(struct Channel) * mod->numChannels );
	for( i = 0; i < mod->numChannels; i++ )
	{
		// Initialize each channel, and allocate enough space for the patterns
		if( initChannel( &mod->channels[i], mod->numPatterns ) != 0 )
		{
			fprintf(stderr, "ERROR: Failed to initialize channel %d\n", i);
			return 1;
		}
	}
	
	return 0;
}


/* readPatterns( struct ModFile *, FILE * )
 *  Reads in the pattern data from the file into our structures
 **********************************************************************/
int readPatterns( struct ModFile *mod, FILE *mfile )
{
	int row, chan, pattern, ele_read;
	uint8_t b1,b2,b3,b4;
	uint16_t amigaPeriod;
	
	
	/* Each row of the pattern is stored as  Chan1, Chan2, ..., ChanN
	 *                                       Chan1, Chan2, ..., ChanN
	 * Each pattern has 64 rows, and each pattern is stored sequentially
	 ***********/
	for( pattern = 0; pattern < mod->numPatterns; pattern++ )
	{
		for( row = 0; row < MOD_NUM_PATTERN_ROWS; row++ )
		{
			for( chan = 0; chan < mod->numChannels; chan++ )
			{
				// Read in the 4 bytes that make up a channel entry in a row
				ele_read = fread( &b1, 1, 1, mfile );
				ele_read += fread( &b2, 1, 1, mfile );
				ele_read += fread( &b3, 1, 1, mfile );
				ele_read += fread( &b4, 1, 1, mfile );
				
				if(ele_read != 4)
				{
					fprintf(stderr, "ERROR: Failed to read in row entry! Pattern: %d, Channel: %d, Row: %d\n", pattern, chan, row);
					return 1;
				}
				
				amigaPeriod = ((b1 & 0x0F) << 8) | b2;
				
				// Now we translate that to our values
				mod->channels[chan].patterns[pattern].rows[row].note   = periodToNote( amigaPeriod );
				mod->channels[chan].patterns[pattern].rows[row].sampleNum = b1 | (b3 >> 4);
				mod->channels[chan].patterns[pattern].rows[row].effect = b3 & 0x0F;
				mod->channels[chan].patterns[pattern].rows[row].eparam = b4;
			}
		}
	}
	
	return 0;
}

/* periodToNote( uint16_t )
 *  Converts an amiga timing period to a note value
 **********************************************************************/
uint16_t periodToNote( uint16_t period ) 
{
	
	
	return 0;
}





void freeModFile(struct ModFile *mod)
{
	int i;
	
	// First clean up the samples
	for( i = 0; i < MOD_NUM_SAMPLES; i++ )
	{
		freeSampleData( &mod->samples[i] );
	}
	
	// Now the channels and patterns
	for( i = 0; i < mod->numChannels; i++ )
	{
		freeChannelPatterns( &mod->channels[i] );
	}
	free(mod->channels);
	mod->channels = NULL;
	
}








void debugDumpSample(struct ModFile *mod, int num)
{
	int i,j; 
	int8_t *samp;
	
	samp = mod->samples[num].sample;
	for( i = 0; i < mod->samples[num].length; i+= 1 )
	{
		putchar( *(samp) ^ 0x80);
		
		
		samp += 1;
	}
}
