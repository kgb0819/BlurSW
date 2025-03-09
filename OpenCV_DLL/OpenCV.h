#pragma once
#ifndef OPENCV_H
#define OPENCV_H

#ifdef OPENCV_EXPORTS
#define OPENCV_API __declspec(dllexport)
#else
#define OPENCV_API __declspec(dllimport)
#endif

class ImageObject;
extern "C" OPENCV_API bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize);

#endif // !OPENCV_H
