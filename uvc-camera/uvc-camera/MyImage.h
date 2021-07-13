#pragma once

#include "IImageBuffer.h"
#include <stdio.h>
#include <string.h> // RS
#include <iostream> // RS
#include <cstdio> // RS

const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

// this is a dummy image following IImageBuffer interface that creates a test pattern and provides a save functionality
class MyImage : public IImageBuffer
{
public:
	BYTE *pData;
	BmpInfo info;

	MyImage(unsigned int w, unsigned int h) : info(w*3,3,Size(w,h)) {
		pData = new BYTE[info.size.Area()*3];
        std::cout << "created pData size is " + std::to_string(info.size.Area() * 3) << std::endl ;
        for (unsigned int i = 0; i < info.size.w; i++) {
			for (unsigned int j = 0; j < info.size.h; j++) {
				unsigned int k = j * info.size.w + i;
                // pData[k * 3] = unsigned char(255.0*i / (info.size.w - 1));
                // pData[k * 3 + 1] = unsigned char(255.0*(j) / (info.size.h - 1));
                // pData[k * 3 + 2] = unsigned char(255.0*(info.size.Area() -1-k) / (info.size.Area() - 1));
                pData[k * 3] = (unsigned char)(255.0*i / (info.size.w - 1));
                pData[k * 3 + 1] = (unsigned char)(255.0*(j) / (info.size.h - 1));
                pData[k * 3 + 2] = (unsigned char)(255.0*(info.size.Area() -1-k) / (info.size.Area() - 1));
			}
		}
	}

	~MyImage() {
		if (pData) {
			delete[] pData;
		}
	}

	virtual void init() {
		memset(pData, 0, info.size.Area() * 3);
	}

	virtual const BYTE * grabRead() {
		return pData;
	}
	virtual BYTE * grabWrite() {
		return pData;
	}
	virtual int release(const BYTE *buffer) {
		return 0;
	};
	virtual const BmpInfo & getInfo(void) const {
		return info;
	}
	virtual unsigned int getStride() const {
		return info.size.w*info.BPP;
	}
	//virtual unsigned int getBPP() const {
	//	return 3;
	//}
	//virtual unsigned int getHeight() const {
	//	return info.size.h;
	//}
	//virtual unsigned int getWidth() const {
	//	return info.size.w;
	//}

	void Save(const char *imageFileName) {
		unsigned int height = info.size.h;
		unsigned int width = info.size.w;
		unsigned int bpp = info.BPP;

		generateBitmapImage(pData, height, width, imageFileName);
		std::cout << imageFileName << "  Image generated!!\n";
	}

	void generateBitmapImage(unsigned char* image, int height, int width, const char* imageFileName)
	{
		int widthInBytes = width * info.BPP;

		unsigned char padding[3] = { 0, 0, 0 };
		int paddingSize = (4 - (widthInBytes) % 4) % 4;

		int stride = (widthInBytes)+paddingSize;

		FILE* imageFile;
        //fopen_s(&imageFile, imageFileName, "wb");
        imageFile = fopen(imageFileName, "wb");

		unsigned char* fileHeader = createBitmapFileHeader(height, stride);
		fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

		unsigned char* infoHeader = createBitmapInfoHeader(height, width);
		fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

		int i;
		for (i = 0; i < height; i++) {
			fwrite(image + (i*widthInBytes), info.BPP, width, imageFile);
			fwrite(padding, 1, paddingSize, imageFile);
		}
		fclose(imageFile);
	}

	unsigned char* createBitmapFileHeader(int height, int stride)
	{
		int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

		static unsigned char fileHeader[] = {
			0,0,     /// signature
			0,0,0,0, /// image file size in bytes
			0,0,0,0, /// reserved
			0,0,0,0, /// start of pixel array
		};

		fileHeader[0] = (unsigned char)('B');
		fileHeader[1] = (unsigned char)('M');
		fileHeader[2] = (unsigned char)(fileSize);
		fileHeader[3] = (unsigned char)(fileSize >> 8);
		fileHeader[4] = (unsigned char)(fileSize >> 16);
		fileHeader[5] = (unsigned char)(fileSize >> 24);
		fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

		return fileHeader;
	}

	unsigned char* createBitmapInfoHeader(int height, int width)
	{
		static unsigned char infoHeader[] = {
			0,0,0,0, /// header size
			0,0,0,0, /// image width
			0,0,0,0, /// image height
			0,0,     /// number of color planes
			0,0,     /// bits per pixel
			0,0,0,0, /// compression
			0,0,0,0, /// image size
			0,0,0,0, /// horizontal resolution
			0,0,0,0, /// vertical resolution
			0,0,0,0, /// colors in color table
			0,0,0,0, /// important color count
		};

		infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
		infoHeader[4] = (unsigned char)(width);
		infoHeader[5] = (unsigned char)(width >> 8);
		infoHeader[6] = (unsigned char)(width >> 16);
		infoHeader[7] = (unsigned char)(width >> 24);
		infoHeader[8] = (unsigned char)(height);
		infoHeader[9] = (unsigned char)(height >> 8);
		infoHeader[10] = (unsigned char)(height >> 16);
		infoHeader[11] = (unsigned char)(height >> 24);
		infoHeader[12] = (unsigned char)(1);
		infoHeader[14] = (unsigned char)(info.BPP * 8);

		return infoHeader;
	}
};
