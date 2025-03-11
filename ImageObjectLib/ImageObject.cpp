#include "pch.h"
#include "ImageObject.h"

ImageObject::ImageObject() : width(0), height(0), buffer(){

}

ImageObject::ImageObject(int width, int height) : width(width), height(height) {
	this->buffer.resize(width * height);
}

ImageObject::ImageObject(const cv::Mat& image)
{
	this->width = image.cols;
	this->height = image.rows;
	this->buffer.assign(image.data, image.data + image.total());
}

ImageObject::~ImageObject() {}

cv::Mat ImageObject::toMat() const
{
	return cv::Mat(height, width, CV_8UC1, const_cast<uint8_t*>(buffer.data())).clone();
}

cv::Mat ImageObject::operator=(const cv::Mat& image) {
	this->width = image.cols;
	this->height = image.rows;
	this->buffer.assign(image.data, image.data + image.total());
	return image;
}
