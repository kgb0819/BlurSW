#include "pch.h"
#include "Custom.h"

namespace Custom {
	int reflect101(int index, int limit) {
		if (index < 0) return -index;
		if (index >= limit) return 2 * limit - index - 2;
		return index;
	}

	bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize) {
		if (src == nullptr || dst == nullptr) {
			return false;
		}

		int halfSize = kernelSize / 2;
		int kernelArea = kernelSize * kernelSize;

		for (int y = 0; y < src->getHeight(); y++) {
			for (int x = 0; x < src->getWidth(); x++) {
				float sum = 0.0f;

				for (int ky = -halfSize; ky <= halfSize; ky++) {
					for (int kx = -halfSize; kx <= halfSize; kx++) {
						int yy = reflect101(y + ky, src->getHeight());
						int xx = reflect101(x + kx, src->getWidth());
						sum += src->getPixel(xx, yy);
					}
				}
				dst->setPixel(x, y, cv::saturate_cast<uchar>(sum / kernelArea));
			}
		}

		return true;
	}
}