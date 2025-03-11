#pragma once
#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <vector>
#include <opencv2/opencv.hpp>

class ImageObject
{
private:
	int width;
	int height;
	std::vector<uint8_t> buffer;
public:
	ImageObject();
	ImageObject(int width, int height);
	ImageObject(const cv::Mat& image);
	~ImageObject();

	int getWidth() const;
	int getHeight() const;

	uint8_t getPixel(int x, int y) const;
	void setPixel(int x, int y, uint8_t value);

	cv::Mat toMat() const;
	cv::Mat operator=(const cv::Mat& image);
};

inline int ImageObject::getWidth() const { return width; }
inline int ImageObject::getHeight() const { return height; }
inline uint8_t ImageObject::getPixel(int x, int y) const { return buffer[y * width + x]; }
inline void ImageObject::setPixel(int x, int y, uint8_t value) { buffer[y * width + x] = value; }

#endif // IMAGEOBJECT_H