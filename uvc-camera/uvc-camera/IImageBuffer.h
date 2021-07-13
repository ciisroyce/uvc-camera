#pragma once
#include "BmpInfo.h"
#ifndef BYTE
#define BYTE unsigned char
#endif

// IImageBuffer is an ImageBuffer Interface to manage access to frame buffers
// grabRead can provide multiple grabs on same buffer
// grabWrite should lock buffer for exclusive access until released
class IImageBuffer
{
public:
	virtual const BYTE * grabRead() = 0; 
	virtual BYTE * grabWrite() = 0;
	virtual int release(const BYTE *buffer) = 0; 
	virtual const BmpInfo & getInfo(void) const = 0;
	virtual void init() = 0;
};

