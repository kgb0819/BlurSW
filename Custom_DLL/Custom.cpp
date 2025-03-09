#include "pch.h"
#include "Custom.h"

bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize) {
	if (src == nullptr || dst == nullptr) {
		return false;
	}

	float kernelValue = 1.0f / (kernelSize * kernelSize);

	for (int y = 0; y < src->getHeight(); y++) {
		for (int x = 0; x < src->getWidth(); x++) {
			float sum = 0.0f;

			for (int ky = 0; ky < kernelSize; ky++) {
				for (int kx = 0; kx < kernelSize; kx++) {
					int sx = x + kx - kernelSize / 2;
					int sy = y + ky - kernelSize / 2;
					if (sx < 0 || sx >= src->getWidth() || sy < 0 || sy >= src->getHeight()) {
						continue;
					}
					sum += src->getPixel(sx, sy) * kernelValue;
				}
			}
			dst->setPixel(x, y, sum);
		}
	}

	return true;
}