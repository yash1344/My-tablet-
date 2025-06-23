/*
 * CaptureScreen.cpp
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#include "CaptureScreen.h"

using namespace std;

CaptureScreen::CaptureScreen(float window_scale_setting) :
		window_scale_setting(window_scale_setting) {
	initialize();
}

CaptureScreen::~CaptureScreen() {
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
}

HBITMAP CaptureScreen::captureCustomScreen_Bitmap(MyRectangle rect,
		bool copyImg) {

	rect.setBoundery(rect.getBounderyX() * window_scale_setting,
			rect.getBounderyY() * window_scale_setting)->reloadRect();

// Validate dimensions
	int width = rect.getWidth();
	int height = rect.getHeight();
	if (width <= 0 || height <= 0) {
		throw std::runtime_error("Invalid capture dimensions");
	}

	// Create temporary DC for this capture
	HDC tempDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

	if (!hBitmap || !tempDC) {
		if (tempDC)
			DeleteDC(tempDC);
		if (hBitmap)
			DeleteObject(hBitmap);
		throw std::runtime_error("GDI resource creation failed");
	}

	// Select bitmap into temporary DC
	HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(tempDC, hBitmap));

	// Perform capture
	if (!BitBlt(tempDC, 0, 0, width, height, hScreenDC, rect.left, rect.top,
	SRCCOPY)) {
		SelectObject(tempDC, oldBitmap);
		DeleteDC(tempDC);
		DeleteObject(hBitmap);
		throw std::runtime_error("Screen capture failed");
	}

	// Cleanup temporary DC
	SelectObject(tempDC, oldBitmap);
	DeleteDC(tempDC);

	// Clipboard handling
	if (copyImg) {
		if (OpenClipboard(nullptr)) {
			EmptyClipboard();
			SetClipboardData(CF_BITMAP, hBitmap);
			CloseClipboard();
			// Don't delete - clipboard now owns the bitmap
			return hBitmap;
		}
	}

	return hBitmap;
}

vector<BYTE> CaptureScreen::captureCustomScreen_jpeg(MyRectangle &rect,
		int quality) {
	HBITMAP bitmap = captureCustomScreen_Bitmap(rect, false);
	vector<BYTE> jpeg = compressor.compress(bitmap, quality);
//	cout << endl << "i was here";
	DeleteObject(bitmap);
	return jpeg;
}

void CaptureScreen::initialize() {
	hScreenDC = GetDC(NULL);
	hMemoryDC = CreateCompatibleDC(hScreenDC);
}
