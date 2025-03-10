#include "ImageObject.h"
#include <opencv2/imgcodecs.hpp>
using namespace cv;

ImageObject::ImageObject() : width(0), height(0), buffer(nullptr) {

}

ImageObject::ImageObject(int width, int height) : width(width), height(height) {
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

Mat ImageObject::toMat() const
{
	return Mat(height, width, CV_8UC1, buffer).clone();
}

Mat ImageObject::operator=(const Mat& image)
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
	return image;
}