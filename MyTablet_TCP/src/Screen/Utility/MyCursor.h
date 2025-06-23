/*
 * MyCursor.h
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#ifndef MYCURSOR_H_
#define MYCURSOR_H_
#include <windef.h>
#include <winuser.h>

class MyCursor {
private:
	POINT point;
	float scale;

public:
	unsigned int x, y;
	MyCursor(float scale = 1);
	virtual ~MyCursor();
	void upadteCursor();
};

#endif /* MYCURSOR_H_ */
