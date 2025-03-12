#pragma once
#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>
#include "ImageObject.h"

class ImageProcess
{
private:
	cv::Mat src;
	std::shared_ptr<spdlog::logger> fileLogger;
	ImageObject srcImage;
	ImageObject dstImage;
	ImageObject dstImageOpenCV;

	std::string fileName;
	int index;
	int kernelSize;

	size_t memoryUsageCustom;
	size_t memoryUsageOpenCV;
	int64_t processTimeCustom;
	int64_t processTimeOpenCV;

	bool isSame;

	std::mutex memoryMutex;
	std::mutex fileMutex;

	size_t GetMemoryUsage();

public:
	ImageProcess(int index, int kernelSize);
	~ImageProcess();

	bool loadImage(const std::string& filePath);
	void customBlurImage();
	void opencvBlurImage();
	void compareImage();
	void makeReport();
	void saveImages(const std::string& resultPath);
};

#endif // IMAGEPROCESS_H