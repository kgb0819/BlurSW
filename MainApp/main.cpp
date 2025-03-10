#include <iostream>
#include <fstream>
#include "ImageObject.h"
#include "nlohmann/json.hpp"
#include "opencv2/opencv.hpp"

#include "Custom.h"
#include "OpenCV.h"

#pragma comment(lib, "Custom_DLL.lib")
#pragma comment(lib, "OpenCV_DLL.lib")

using json = nlohmann::json;
using namespace std;

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


int main()
{
	// 셋팅 값 load
	json config = loadConfig("../config.json");
	if (config.is_null()) return -1;


	// 입력 영상 로드
	vector<string> fileList = getImageList(config["image_path"]);
	if (fileList.empty()) return -1;

	for (int i = 0; i < fileList.size(); i++)
	{
		cv::Mat src = cv::imread(fileList[i], IMREAD_UNCHANGED);
		if (src.empty()) {
			cout << "Image file not found!" << endl;
			return -1;
		}
		if (src.channels() != 1) {
			cout << src.channels() << endl;
			cout << fileList[i] << endl;
			cout << "Image file should be grayscale!" << endl;
			return -1;
		}

		// 입력 영상 Blur 처리
		ImageObject srcImage2(src);
		ImageObject dstImage2(src.cols, src.rows);

		int startTime2 = cv::getTickCount();
		OpenCV::ImageBlur(&srcImage2, &dstImage2, config["kernel_size"]);
		int endTime2 = cv::getTickCount();
		cout << "OpenCV ImageBlur: " << (endTime2 - startTime2) / cv::getTickFrequency() << "s" << endl;

		ImageObject srcImage(src);
		ImageObject dstImage(src.cols, src.rows);

		int startTime = cv::getTickCount();
		Custom::ImageBlur(&srcImage, &dstImage, config["kernel_size"]);
		int endTime = cv::getTickCount();
		cout << "Custom ImageBlur: " << (endTime - startTime) / cv::getTickFrequency() << "s" << endl;

		// Blur 한 영상들 비교
		cv::Mat diff;
		cv::absdiff(dstImage.toMat(), dstImage2.toMat(), diff);

		cout << "SUM:" << cv::sum(diff)[0] << endl;

		cv::imwrite(string(config["result_path"]) + "/" + to_string(i) + "_opencv_blur.png", dstImage2.toMat());
		cv::imwrite(string(config["result_path"]) + "/" + to_string(i) + "_custom_blur.png", dstImage.toMat());
		cv::imwrite(string(config["result_path"]) + "/" + to_string(i) + "_diff.png", diff);
	}

	
	// 결과 영상 저장
	return 0;
}

