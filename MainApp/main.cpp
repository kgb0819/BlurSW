#include <windows.h>
#include <psapi.h>

#include <iostream>
#include <fstream>

#include "ImageObject.h"
#include "nlohmann/json.hpp"
#include "Custom.h"
#include "OpenCV.h"

#pragma comment(lib, "Custom.lib")
#pragma comment(lib, "OpenCV.lib")

using json = nlohmann::json;
using namespace std;

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
		cout << "config.json file not found!" << endl;
		return json();
	}
	json data = json::parse(ifs);

	if (data["image_path"].is_null()) {
		cout << "image_path not found in config.json!" << endl;
		return json();
	}
	if (data["kernel_size"].is_null()) {
		cout << "kernel_size not found in config.json!" << endl;
		return json();
	}
	if (data["result_path"].is_null()) {
		cout << "result_path not found in config.json!" << endl;
		return json();
	}
	if (data["kernel_size"] < 21) {
		cout << "kernel_size should be greater than 21!" << endl;
		return json();
	}
	if (data["kernel_size"] % 2 == 0) {
		cout << "kernel_size should be odd number!" << endl;
		return json();
	}
	return data;
}

vector<string> getImageList(const string& imagePath) {
	vector<string> fileList;
	cv::glob(imagePath, fileList, false);

	if (fileList.empty()) {
		cerr << "No image files found in: " << imagePath << endl;
	}

	return fileList;
}

// 이미지 처리 함수
void processImage(const string& filePath, const json& config, int index) {
	cv::Mat src = cv::imread(filePath, cv::IMREAD_UNCHANGED);
	if (src.empty()) {
		cerr << "Error: Image file not found!" << endl;
		return;
	}
	if (src.channels() != 1) {
		cerr << "Error: Image file should be grayscale!" << endl;
		return;
	}

	// Blur 처리
	ImageObject srcImage(src);
	ImageObject dstImage(src.cols, src.rows);
	ImageObject dstImageOpenCV(src.cols, src.rows);

	size_t memoryBeforeCustom = GetMemoryUsage();
	int64_t startTime1 = cv::getTickCount();
	Custom::ImageBlur(&srcImage, &dstImage, config["kernel_size"]);
	int64_t endTime1 = cv::getTickCount();
	size_t memoryAfterCustom = GetMemoryUsage();
	size_t memoryUsageCustom = memoryAfterCustom - memoryBeforeCustom;

	size_t memoryBeforeOpencv = GetMemoryUsage();
	int64_t startTime2 = cv::getTickCount();
	OpenCV::ImageBlur(&srcImage, &dstImageOpenCV, config["kernel_size"]);
	int64_t endTime2 = cv::getTickCount();
	size_t memoryAfterOpencv = GetMemoryUsage();
	size_t memoryUsageOpencv = memoryAfterOpencv - memoryBeforeOpencv;

	// Blur 결과 비교
	cv::Mat diff;
	cv::absdiff(dstImage.toMat(), dstImageOpenCV.toMat(), diff);
	double diffScore = cv::sum(diff)[0] / (diff.rows * diff.cols);

	std::cout << "===== Performance Test Results =====\n";
	std::cout << "Image Size: " << src.cols << "x" << src.rows << "\n";
	std::cout << "Kernel Size: " << config["kernel_size"] << "x" << config["kernel_size"] << "\n";
	cout << "Custom::ImageBlur elapsed time: " << (endTime1 - startTime1) / cv::getTickFrequency() << " sec" << endl;
	cout << "OpenCV::ImageBlur elapsed time: " << (endTime2 - startTime2) / cv::getTickFrequency() << " sec" << endl;
	cout << "Custom::ImageBlur memory usage: " << memoryUsageCustom << " bytes" << endl;
	cout << "OpenCV::ImageBlur memory usage: " << memoryUsageOpencv << " bytes" << endl;
	std::cout << "Image Difference Score: " << diffScore << "\n";
	std::cout << "===================================\n";

	// 결과 저장
	string resultPath = config["result_path"];
	cv::imwrite(resultPath + "/" + to_string(index) + "_opencv_blur.png", dstImageOpenCV.toMat());
	cv::imwrite(resultPath + "/" + to_string(index) + "_custom_blur.png", dstImage.toMat());
}


int main()
{
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

