#pragma once
#include <cstdio>

// 2d unit size (w,h)
class Size {
public:
	Size()
	{
		this->w = 0;
		this->h = 0;
	}
	Size(unsigned int w, unsigned int h)
	{
		this->w = w;
		this->h = h;
	}
	Size(const Size &that) {
		this->w = that.w;
		this->h = that.h;
	}
	unsigned long Area() const {
        //return unsigned long(w) * h;
        return (unsigned long)((w) * h);
	}
	unsigned int w;
	unsigned int h;
};

// bitmap info class containing bitmap size, stride, and bytes-per-pixel
class BmpInfo {
public:
	unsigned int stride;
	unsigned int BPP;
	Size size;
	BmpInfo() {
		this->stride = 0;
		this->BPP = 0;
		this->size = Size(0, 0);
	}
	BmpInfo(unsigned int stride, unsigned int BPP, const Size &size) {
		this->stride = stride;
		this->BPP = BPP;
		this->size = size;
	}
	BmpInfo(const BmpInfo &that)
	{
		this->stride = that.stride;
		this->BPP = that.BPP;
		this->size = that.size;
	}
	unsigned int step(unsigned int axis) const {
		if (axis == 0) {
			return this->BPP;
		}
		else {
			return this->stride;
		}
	}
    bool isValidPixel(size_t index) const {
    //bool isValidPixel(unsigned long index) const {
		if (index >= 0 && index + BPP - 1 < stride*size.h)
			return true;
		else
			return false;
	}
};
