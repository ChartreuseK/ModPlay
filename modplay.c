/* modplay
 *  Author: Hayden Kroepfl
 * 
 *  First attempt at a mod file player, and audio programming.
 * 
 ****************************/


#include "mod_file.h"

int main( int argc, char **argv )
{
	struct ModFile modfile;
	int i;
	
	if(argc < 2)
	{
		fprintf(stderr,"Usage %s <modfile>\n", argv[0]);
		return 1;
	}
	
	loadModFile( &modfile, argv[1] );

	for( i = 0; i < 31; i++ )
		debugDumpSample( &modfile, i );

	
	freeModFile( &modfile );
	
	return 0;
}
