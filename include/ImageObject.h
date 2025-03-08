#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

class ImageObject
{
private:
	int width;
	int height;
	int* pixels;
public:
	ImageObject(int width, int height);
	~ImageObject();
	void setPixel(int x, int y, int value);
	int getPixel(int x, int y);
	int getWidth();
	int getHeight();
};

ImageObject::ImageObject(int width, int height)
{
	this->width = width;
	this->height = height;
	this->pixels = new int[width * height];
}

ImageObject::~ImageObject()
{
	delete[] pixels;
}

inline void ImageObject::setPixel(int x, int y, int value)
{
	pixels[y * width + x] = value;
}

inline int ImageObject::getPixel(int x, int y)
{
	return pixels[y * width + x];
}

inline int ImageObject::getWidth()
{
	return width;
}

inline int ImageObject::getHeight()
{
	return height;
}


#endif // IMAGEOBJECT_H