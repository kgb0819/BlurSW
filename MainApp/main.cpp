#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
#include "ImageProcess.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

using json = nlohmann::json;
using namespace std;

auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("image_blur.log", true);

std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
auto fileLogger = std::make_shared<spdlog::logger>("multi_logger", sinks.begin(), sinks.end());

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
	ImageProcess imageProcess(index, config["kernel_size"]);
	if (!imageProcess.loadImage(filePath)) return;
	imageProcess.customBlurImage();
	imageProcess.opencvBlurImage();

	imageProcess.compareImage();
	imageProcess.makeReport();
	imageProcess.saveImages(config["result_path"]);
}


int main() {
	spdlog::register_logger(fileLogger);
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
		//processImage(fileList[i], config, i);
		threads.emplace_back(processImage,fileList[i], config, i);
	}

	for (auto& t : threads) {
		t.join();
	}

	return 0;
}

