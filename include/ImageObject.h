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

	Mat toMat();
};

ImageObject::ImageObject()
{
	this->width = 0;
	this->height = 0;
	this->buffer = nullptr;
}

ImageObject::ImageObject(int width, int height)
{
	this->width = width;
	this->height = height;
	this->buffer = new uchar[width * height];
}

ImageObject::ImageObject(const Mat& image)
{
	this->width = image.cols;
	this->height = image.rows;
	this->buffer = new uchar[width * height];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			buffer[i * width + j] = image.at<uchar>(i, j);
		}
	}
}

ImageObject::~ImageObject()
{
	delete[] buffer;
}

int ImageObject::getWidth() const { return width; }
int ImageObject::getHeight() const { return height; }

Mat ImageObject::toMat()
{
	Mat image(height, width, CV_8UC1);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			image.at<uchar>(i, j) = buffer[i * width + j];
		}
	}
	return image;
}
#endif // IMAGEOBJECT_H