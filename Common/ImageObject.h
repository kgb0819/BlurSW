#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <opencv2/opencv.hpp>
using namespace cv;

class ImageObject
{
private:
	int width;
	int height;
	uchar* buffer;
public:
	ImageObject();
	ImageObject(int width, int height);
	ImageObject(const Mat& image);
	~ImageObject();

	int getWidth() const;
	int getHeight() const;

	uchar getPixel(int x, int y) const { return buffer[y * width + x]; }
	void setPixel(int x, int y, uchar value) { buffer[y * width + x] = value; }

	Mat toMat() const;
	Mat operator=(const Mat& image);
};


inline int ImageObject::getWidth() const { return width; }
inline int ImageObject::getHeight() const { return height; }
#endif // IMAGEOBJECT_H