/*
 * JpegCompressor.h
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#ifndef JPEGCOMPRESSOR_H_
#define JPEGCOMPRESSOR_H_

#include <windows.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace Gdiplus;

class JpegCompressor {
public:
	// Constructor: Initialize GDI+
	JpegCompressor() {
		GdiplusStartupInput gdiplusStartupInput;
		if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr)
				!= Ok) {
			throw std::runtime_error("GDI+ initialization failed.");
		}
	}

	// Destructor: Shutdown GDI+
	~JpegCompressor() {
		GdiplusShutdown(gdiplusToken);
	}

	// Compress the provided HBITMAP into JPEG format.
	// quality: JPEG quality (0-100)
	// Returns a vector containing the compressed JPEG data.
	std::vector<BYTE> compress(HBITMAP hBitmap, ULONG quality = 80) {
		if (!hBitmap) {
			throw std::invalid_argument("Invalid HBITMAP.");
		}

		// Create a GDI+ Bitmap from the HBITMAP.
		Bitmap bmp(hBitmap, nullptr);
		if (bmp.GetLastStatus() != Ok) {
			throw std::runtime_error(
					"Failed to create GDI+ Bitmap from HBITMAP.");
		}

		// Get the CLSID of the JPEG encoder.
		CLSID clsidEncoder;
		if (GetEncoderClsid(L"image/jpeg", &clsidEncoder) == -1) {
			throw std::runtime_error("JPEG encoder not found.");
		}

		// Create an in-memory stream to hold the JPEG data.
		IStream *pStream = nullptr;
		HRESULT hr = CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
		if (FAILED(hr) || !pStream) {
			throw std::runtime_error("Failed to create IStream.");
		}

		// Set up encoder parameters (quality parameter)
		EncoderParameters encoderParams;
		encoderParams.Count = 1;
		encoderParams.Parameter[0].Guid = EncoderQuality;
		encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParams.Parameter[0].NumberOfValues = 1;
		encoderParams.Parameter[0].Value = &quality;

		// Save the bitmap as JPEG into the stream.
		if (bmp.Save(pStream, &clsidEncoder, &encoderParams) != Ok) {
			pStream->Release();
			throw std::runtime_error("Failed to save Bitmap to IStream.");
		}

		// Rewind the stream to the beginning.
		LARGE_INTEGER liZero = { };
		pStream->Seek(liZero, STREAM_SEEK_SET, nullptr);

		// Get the size of the stream.
		STATSTG statstg;
		hr = pStream->Stat(&statstg, STATFLAG_NONAME);
		if (FAILED(hr)) {
			pStream->Release();
			throw std::runtime_error("Failed to get stream statistics.");
		}
		ULONG streamSize = statstg.cbSize.LowPart;

		// Read the stream data into a vector.
		std::vector<BYTE> buffer(streamSize);
		ULONG bytesRead = 0;
		hr = pStream->Read(buffer.data(), streamSize, &bytesRead);
		pStream->Release();

		if (FAILED(hr) || bytesRead != streamSize) {
			throw std::runtime_error("Failed to read stream data.");
		}

		return buffer;
	}

private:
	// Helper function to retrieve the encoder CLSID for the specified format.
	int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
		UINT num = 0, size = 0;
		GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;

		ImageCodecInfo *pImageCodecInfo =
				reinterpret_cast<ImageCodecInfo*>(malloc(size));
		if (pImageCodecInfo == nullptr)
			return -1;

		GetImageEncoders(num, size, pImageCodecInfo);
		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;
			}
		}
		free(pImageCodecInfo);
		return -1;
	}

	ULONG_PTR gdiplusToken;
};

#endif /* JPEGCOMPRESSOR_H_ */
