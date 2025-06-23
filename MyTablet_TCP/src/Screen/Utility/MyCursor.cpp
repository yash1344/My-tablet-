/*
 * MyCursor.cpp
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#include "MyCursor.h"

MyCursor::MyCursor(float scale) :
		scale(scale), x(0), y(0) {

}

MyCursor::~MyCursor() {
}

void MyCursor::upadteCursor() {
	GetCursorPos(&point);
	x = point.x * scale;
	y = point.y * scale;
}
