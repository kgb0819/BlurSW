#include "pch.h"
#include "OpenCV.h"

namespace OpenCV {
	bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize) {
		if (src == nullptr || dst == nullptr) {
			return false;
		}

		cv::Mat dstMat;
		cv::blur(src->toMat(), dstMat, cv::Size(kernelSize, kernelSize));
		*dst=dstMat;
		return true;
	}
}