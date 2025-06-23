/*
 * Rectangle.cpp
 *
 *  Created on: Feb 27, 2025
 *      Author: vagha
 */

#include "MyRectangle.h"

using namespace std;

MyRectangle::MyRectangle() :
		centerX(0), centerY(0), width(0), height(0), left(0), right(0), top(0), bottom(
				0) {
}

MyRectangle* MyRectangle::setBoundery(int maxX, int maxY) {
	this->bounderyX = maxX;
	this->bounderyY = maxY;

	return this;
}

MyRectangle::~MyRectangle() {
}

unsigned int MyRectangle::getHeight() {
	return height;
}

unsigned int MyRectangle::getWidth() {
	return width;
}

void MyRectangle::setRect(int centerX, int centerY, int width, int height) {

	this->centerX = centerX;
	this->centerY = centerY;
	this->width = width;
	this->height = height;

	if (bounderyX > 0) {
		int cuttedLeft, cuttedRight;
		cuttedLeft = max((width / 2) - centerX, 0);
		cuttedRight = max((width / 2) - (bounderyX - centerX), 0);
		this->left = max(centerX - (width / 2) - cuttedRight, 0);
		this->right = min(centerX + (width / 2) + cuttedLeft, bounderyX);
	}

	if (bounderyY > 0) {
		int cuttedTop, cuttedBottom;
		cuttedTop = max((height / 2) - centerY, 0);
		cuttedBottom = max((height / 2) - (bounderyY - centerY), 0);
		this->top = max(centerY - (height / 2) - cuttedBottom, 0);
		this->bottom = min(centerY + (height / 2) + cuttedTop, bounderyY);
	}
}

int MyRectangle::getBounderyX() const {
	return bounderyX;
}

int MyRectangle::getBounderyY() const {
	return bounderyY;
}

MyRectangle* MyRectangle::reloadRect() {
	this->setRect(centerX, centerY, width, height);
	return this;
}
