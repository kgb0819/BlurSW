#include <windows.h>
#include <psapi.h>

#include <fstream>
#include <filesystem>

#include "ImageProcess.h"

#include "Custom.h"
#include "OpenCV.h"
#pragma comment(lib, "Custom.lib")
#pragma comment(lib, "OpenCV.lib")


ImageProcess::ImageProcess(int index, int kernelSize):
	index(index), 
	kernelSize(kernelSize), 
	memoryUsageCustom(0),
	processTimeCustom(0), 
	memoryUsageOpenCV(0), 
	processTimeOpenCV(0),
	isSame(false)
{
	fileLogger = spdlog::get("multi_logger");
}

ImageProcess::~ImageProcess() {

}

bool ImageProcess::loadImage(const std::string& filePath)
{
	cv::Mat src = cv::imread(filePath, cv::IMREAD_UNCHANGED);
	std::filesystem::path fsFilePath = filePath;
	fileName = fsFilePath.filename().string();
	if (src.empty()) {
		fileLogger->error("Image file not found!");
		return false;
	}
	if (src.channels() != 1) {
		fileLogger->error("Image file should be grayscale!");
		return false;
	}
	if (kernelSize > src.cols || kernelSize > src.rows) {
		fileLogger->error("Kernel size must be smaller than or equal to the image dimensions.");
		return false;
	}

	srcImage = ImageObject(src);
	dstImage = ImageObject(src.cols, src.rows);
	dstImageOpenCV = ImageObject(src.cols, src.rows);
	return true;
}

size_t ImageProcess::GetMemoryUsage() {
	std::lock_guard<std::mutex> lock(memoryMutex);
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
		return static_cast<size_t>(pmc.PrivateUsage) / 1024;
	}
	else {
		fileLogger->warn("Failed to retrieve memory info! Error code: {}", GetLastError());
		return 0;
	}
}

void ImageProcess::customBlurImage() {
	size_t memoryBeforeCustom = this->GetMemoryUsage();
	int64_t startTime = cv::getTickCount();
	Custom::ImageBlur(&srcImage, &dstImage, kernelSize);
	int64_t endTime = cv::getTickCount();
	size_t memoryAfterCustom = this->GetMemoryUsage();

	memoryUsageCustom = memoryAfterCustom - memoryBeforeCustom;
	processTimeCustom = endTime - startTime;

	fileLogger->info("index:{} Custom ImageBlur success!", index);
}

void ImageProcess::opencvBlurImage() {
	size_t memoryBeforeOpencv = this->GetMemoryUsage();
	int64_t startTime = cv::getTickCount();
	OpenCV::ImageBlur(&srcImage, &dstImageOpenCV, kernelSize);
	int64_t endTime = cv::getTickCount();
	size_t memoryAfterOpencv = this->GetMemoryUsage();

	memoryUsageOpenCV = memoryAfterOpencv - memoryBeforeOpencv;
	processTimeOpenCV = endTime - startTime;

	fileLogger->info("index:{} OpenCV ImageBlur success!", index);
}

void ImageProcess::compareImage() {
	// Blur 결과 비교
	cv::Mat diff;
	cv::absdiff(dstImage.toMat(), dstImageOpenCV.toMat(), diff);
	isSame = cv::countNonZero(diff) == 0;
}

void ImageProcess::makeReport() {
	// 결과 Report 출력
	std::lock_guard<std::mutex> lock(fileMutex);
	std::fstream fs;
	fs.open("TestReport.txt", std::ios::app);
	fs << "date:" << __TIMESTAMP__ << std::endl;
	fs << "FileName: " << fileName << std::endl;
	fs << "index:" << index << " Custom::ImageBlur elapsed time: " << processTimeCustom / cv::getTickFrequency() << " sec" << std::endl;
	fs << "index:" << index << " OpenCV::ImageBlur elapsed time: " << processTimeOpenCV/ cv::getTickFrequency() << " sec" << std::endl;
	fs << "index:" << index << " Custom::ImageBlur memory usage: " << memoryUsageCustom << " KB" << std::endl;
	fs << "index:" << index << " OpenCV::ImageBlur memory usage: " << memoryUsageOpenCV << " KB" << std::endl;
	fs << "index:" << index << " Blur Image is same? " << isSame << std::endl << std::endl;
	fs.close();
	fileLogger->info("index:{} Make Test Report", index);
}

void ImageProcess::saveImages(const std::string& resultPath) {
	if (!std::filesystem::exists(resultPath)) {
		std::lock_guard<std::mutex> lock(fileMutex);
		fileLogger->warn("Result path not found! Creating new directory: {}", resultPath);
		std::filesystem::create_directories(resultPath);
	}
	cv::imwrite(resultPath + "/" + fileName + "_opencv_blur.png", dstImageOpenCV.toMat());
	cv::imwrite(resultPath + "/" + fileName + "_custom_blur.png", dstImage.toMat());
	//cv::imwrite(resultPath + "/" + to_string(index) + "_diff.png", diff);
	fileLogger->info("index:{} Result images saved successfully!", index);
}

