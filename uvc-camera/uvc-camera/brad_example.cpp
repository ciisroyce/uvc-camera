#include "MyImage.h"

int main_not_used()
{

	MyImage im(640, 480);
	BYTE *pData = im.grabWrite();
	const BYTE yourimagebyte = 0x80;
	// manipulate bytes
	for (int j = 0; j < im.getInfo().size.h; j++) {  // rows from bottom
		for (int i = 0; i < im.getInfo().size.w; i++) { // columns from left
			for (int c = 0; c < im.getInfo().BPP; c++) { // R,G,B
				pData[j*im.getInfo().stride + i * im.getInfo().BPP + c] = yourimagebyte;
			}
		}
	}
	im.release(pData);
	im.Save("ex1.bmp");
    return 0;
}
