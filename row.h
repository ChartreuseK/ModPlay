#ifndef _ROW_H
#define _ROW_H


struct Row
{
	uint16_t note;      // We could use bit fields here to save 1 byte per row
	uint8_t sampleNum;  // Which would be up to 64kb at max. Not sure if worth it (note:11, sampleNum:5)
	uint8_t effect;
	uint8_t eparam;
};


#endif
