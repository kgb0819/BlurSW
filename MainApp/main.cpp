#include <iostream>
#include <fstream>
#include "ImageObject.h"
#include "nlohmann/json.hpp"
#include "opencv2/opencv.hpp"

using json = nlohmann::json;
using namespace std;

int main()
{
	ifstream ifs("../config.json");
	if (!ifs.is_open())
	{
		cout << "config.json file not found!" << endl;
		return -1;
	}
	json data = json::parse(ifs);

	vector<string> fileList;
	cv::glob(data["image_path"], fileList, false);

	cout << fileList.size() << " files found in " << data["image_path"] << endl;
	for (string file : fileList)
	{
		cout << file << endl;
	}

	return 0;
}