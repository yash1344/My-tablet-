/*
 * CaptureScreen.h
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#ifndef CAPTURESCREEN_H_
#define CAPTURESCREEN_H_
#include <minwindef.h>
#include <windef.h>
#include <winuser.h>
#include <wingdi.h>
#include <vector>

#include "JpegCompressor.h"
#include "Utility/MyRectangle.h"

using std::vector;

class CaptureScreen {
private:
	float window_scale_setting;
	HDC hScreenDC;
	HDC hMemoryDC;
	JpegCompressor compressor;

public:
	CaptureScreen(float window_scale_setting = 1);
	virtual ~CaptureScreen();

	void initialize();
	HBITMAP captureCustomScreen_Bitmap(MyRectangle rect, bool copyImg = false);
	vector<BYTE> captureCustomScreen_jpeg(MyRectangle &rect, int quality = 80);
};

#endif /* CAPTURESCREEN_H_ */
