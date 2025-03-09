#include <iostream>
#include <fstream>
#include "ImageObject.h"
#include "nlohmann/json.hpp"
#include "opencv2/opencv.hpp"

using json = nlohmann::json;
using namespace std;

json loadConfig(const string path)
{
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
	return data;
}

int main()
{
	// 셋팅 값 load
	json config = loadConfig("../config.json");
	if (config.is_null())
	{
		return -1;
	}

	// 입력 영상 로드
	vector<string> fileList;
	cv::glob(config["image_path"], fileList, false);

	if (fileList.size() == 0)
	{
		cout << "No image files found!" << endl;
		return -1;
	}

	for (int i = 0; i < fileList.size(); i++)
	{
		cv::Mat src = cv::imread(fileList[i]);
		if (src.empty()) {
			cout << "Image file not found!" << endl;
			return -1;
		}
		if (src.channels() != 1) {
			cout << fileList[i] << endl;
			cout << "Image file should be grayscale!" << endl;
			return -1;
		}
	}

	// 입력 영상 Blur 처리

	// Blur 한 영상들 비교
	
	// 결과 영상 저장
	return 0;
}

