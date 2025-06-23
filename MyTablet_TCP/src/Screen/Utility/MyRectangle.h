/*
 * Rectangle.h
 *
 *  Created on: Feb 27, 2025
 *      Author: vagha
 */

#ifndef MYRECTANGLE_H_
#define MYRECTANGLE_H_
#include <algorithm>
#include <iostream>

class MyRectangle {
private:
	int bounderyX = -1, bounderyY = -1;
	unsigned int centerX, centerY;
	int width, height;

public:
	int left, right, top, bottom;
	MyRectangle();
	void setRect(int centerX, int centerY, int width, int height);
	MyRectangle* reloadRect();
	MyRectangle* setBoundery(int maxX, int maxY);
	unsigned int getHeight();
	unsigned int getWidth();
	virtual ~MyRectangle();
	int getBounderyX() const;
	int getBounderyY() const;
};

#endif /* MYRECTANGLE_H_ */
