#include <windows.h>
#include <psapi.h>

#include <iostream>
#include <fstream>

#include "ImageObject.h"
#include "nlohmann/json.hpp"
#include "Custom.h"
#include "OpenCV.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#pragma comment(lib, "Custom.lib")
#pragma comment(lib, "OpenCV.lib")


using json = nlohmann::json;
using namespace std;

auto fileLogger = spdlog::basic_logger_mt("file_logger", "image_blur.log");

size_t GetMemoryUsage() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
		return pmc.PrivateUsage; // 현재 프로세스의 메모리 사용량 (바이트 단위)
	}
	return 0;
}

json loadConfig(const string& path) {
	ifstream ifs(path);
	if (!ifs.is_open()) {
		fileLogger->error("config.json file not found!");
		return json();
	}
	json data = json::parse(ifs);

	if (data["image_path"].is_null()) {
		fileLogger->error("image_path not found in config.json!");
		return json();
	}
	if (data["kernel_size"].is_null()) {
		fileLogger->error("kernel_size not found in config.json!");
		return json();
	}
	if (data["result_path"].is_null()) {
		fileLogger->error("result_path not found in config.json!");
		return json();
	}
	if (data["kernel_size"] < 21) {
		fileLogger->error("kernel_size should be greater than 21!");
		return json();
	}
	if (data["kernel_size"] % 2 == 0) {
		fileLogger->error("kernel_size should be odd number!");
		return json();
	}
	fileLogger->info("config.json loaded successfully!");
	return data;
}

vector<string> getImageList(const string& imagePath) {
	vector<string> fileList;
	cv::glob(imagePath, fileList, false);

	if (fileList.empty()) {
		fileLogger->error("No image files found in: {}", imagePath);
	}
	fileLogger->info("Found {} image files in: {}", fileList.size(), imagePath);
	return fileList;
}

// 이미지 처리 함수
void processImage(const string& filePath, const json& config, int index) {
	cv::Mat src = cv::imread(filePath, cv::IMREAD_UNCHANGED);
	if (src.empty()) {
		fileLogger->error("Image file not found!");
		return;
	}
	if (src.channels() != 1) {
		fileLogger->error("Image file should be grayscale!");
		return;
	}

	ImageObject srcImage(src);
	ImageObject dstImage(src.cols, src.rows);
	ImageObject dstImageOpenCV(src.cols, src.rows);


	// Custom Blur
	size_t memoryBeforeCustom = GetMemoryUsage();
	int64_t startTime1 = cv::getTickCount();
	Custom::ImageBlur(&srcImage, &dstImage, config["kernel_size"]);
	int64_t endTime1 = cv::getTickCount();
	size_t memoryAfterCustom = GetMemoryUsage();
	size_t memoryUsageCustom = memoryAfterCustom - memoryBeforeCustom;
	fileLogger->info("index:{} Custom ImageBlur success!", index);

	// OpenCV Blur
	size_t memoryBeforeOpencv = GetMemoryUsage();
	int64_t startTime2 = cv::getTickCount();
	OpenCV::ImageBlur(&srcImage, &dstImageOpenCV, config["kernel_size"]);
	int64_t endTime2 = cv::getTickCount();
	size_t memoryAfterOpencv = GetMemoryUsage();
	size_t memoryUsageOpencv = memoryAfterOpencv - memoryBeforeOpencv;
	fileLogger->info("index:{}  OpenCV ImageBlur success!", index);

	// Blur 결과 비교
	cv::Mat diff;
	cv::absdiff(dstImage.toMat(), dstImageOpenCV.toMat(), diff);
	double diffScore = cv::sum(diff)[0] / (diff.rows * diff.cols);

	// 결과 Report 출력
	fstream fs;
	fs.open("outputReport.txt", ios::app);
	fs << "index:" << index << " Custom::ImageBlur elapsed time: " << (endTime1 - startTime1) / cv::getTickFrequency() << " sec" << endl;
	fs << "index:" << index << " OpenCV::ImageBlur elapsed time: " << (endTime2 - startTime2) / cv::getTickFrequency() << " sec" << endl;
	fs << "index:" << index << " Custom::ImageBlur memory usage: " << memoryUsageCustom << " bytes" << endl;
	fs << "index:" << index << " OpenCV::ImageBlur memory usage: " << memoryUsageOpencv << " bytes" << endl;
	fs << "index:" << index << " Image Difference Score: " << diffScore << endl << endl;
	fs.close();

	// 결과 저장
	string resultPath = config["result_path"];
	cv::imwrite(resultPath + "/" + to_string(index) + "_opencv_blur.png", dstImageOpenCV.toMat());
	cv::imwrite(resultPath + "/" + to_string(index) + "_custom_blur.png", dstImage.toMat());
	fileLogger->info("index:{} Result images saved successfully!", index);
}


int main() {
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

	// 셋팅 값 load
	json config = loadConfig("config.json");
	if (config.is_null()) return -1;

	// 입력 영상 로드
	vector<string> fileList = getImageList(config["image_path"]);
	if (fileList.empty()) return -1;


	std::vector<std::thread> threads;
	for (int i = 0; i < fileList.size(); i++)
	{
		threads.emplace_back(processImage,fileList[i], config, i);
	}

	for (auto& t : threads) {
		t.join();
	}

	return 0;
}

