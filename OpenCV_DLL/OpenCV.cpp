#include "pch.h"
#include "OpenCV.h"

bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize) {
	if (src == nullptr || dst == nullptr) {
		return false;
	}

	cv::blur(src->toMat(), dst->toMat(), cv::Size(kernelSize, kernelSize));
	return true;
}