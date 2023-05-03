#include <random>
#include "OverlayRender.h"

DWORD Overlay_::InitializePixelTexture()
{
	DWORD textureId = 7878;

	HDC dcHandle = CreateCompatibleDC(NULL);

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 1;
	bmi.bmiHeader.biHeight = 1;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	char* buffer;

	HBITMAP dibHandle = CreateDIBSection(dcHandle, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, (void**)&buffer, 0, 0);
	SelectObject(dcHandle, dibHandle);

	const RECT Rect = { 0, 0, 1, 1 };

	HBRUSH hBrush; 
	CreateSolidBrush(RGB(0, 0, 0)); 
	SelectObject(dcHandle, &hBrush); 
	FillRect(dcHandle, &Rect, hBrush);

	DWORD size = 4;

	for (int i = 0; i < size; i += 4)
	{
		char r = buffer[i];
		char g = buffer[i + 1];
		char b = buffer[i + 2];
		buffer[i + 3] = (char)((float)r * 0.34f + (float)g * 0.55f + (float)b * 0.11f);
	}

	renderStream->PutFront(buffer, size);

	struct
	{
		DWORD renderCommand;
		DWORD textureId;
		DWORD version;
		BOOL fullUpdate;
		DWORD size;
		DWORD width;
		DWORD height;
		DWORD x;
		DWORD y;
	} loadTexture =
	{
		1,
		textureId,
		1,
		1,
		size,
		1,
		1,
		0,
		0
	};

	// command
	renderStream->PutFront(&loadTexture, sizeof(loadTexture));

	DeleteObject(dibHandle);
	DeleteObject(hBrush);
	DeleteDC(dcHandle);

	return textureId;
}

void Overlay_::BeginRender()
{
	WaitForMultipleObjects(ARRAYSIZE(handles), handles, TRUE, INFINITE);
}

void Overlay_::EndRender()
{
	ReleaseMutex(renderMutex);
	ReleaseMutex(inputMutex);
}

void Overlay_::RenderPixel(Color color,int x, int y, int thickness)
{
	struct
	{
		DWORD renderCommand;
		int x0;
		int y0;
		int x1;
		int y1;
		float u0;
		float v0;
		float u1;
		float v1;
		float uk4;
		DWORD colorStart;
		DWORD colorEnd; 
		DWORD gradientDirection;
		DWORD textureId;
	} drawTexturedRect =
	{
		3, 
		x,
		y,
		x + thickness,
		y + thickness,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		(DWORD)color,
		0,
		3,
		pixelTextureId
	};
	
	renderStream->PutFront(&drawTexturedRect, sizeof(drawTexturedRect));

}

void Overlay_::RenderLine(Color color, int x0, int y0, int x1, int y1, int thickness) {
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	while (x0 != x1 || y0 != y1) {
		RenderPixel(color, x0, y0, thickness);
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
	RenderPixel(color, x1, y1, thickness);
}
void Overlay_::RenderLineOutlined(Color color, int x0, int y0, int x1, int y1)
{
	RenderLine(color, x0, y0, x1, y1, 1);
	RenderLine(BLACK, x0 + 1, y0 + 1, x1 + 1, y1 + 1, 1);
	RenderLine(BLACK, x0 - 1, y0 - 1, x1 - 1, y1 - 1, 1);
}

void Overlay_::DrawBoundingBox(Color color, int x, int y, int w, int h) {
	// Рисуем верхнюю грань
	RenderLine(color, x, y, x + w, y, 1);

	// Рисуем правую грань
	RenderLine(color, x + w, y, x + w, y + h, 1);

	// Рисуем нижнюю грань
	RenderLine(color, x + w, y + h, x, y + h, 1);

	// Рисуем левую грань
	RenderLine(color, x, y + h, x, y, 1);
	
	// Рисуем верхнюю грань
	RenderLine(BLACK, x + 1, y + 1, x + w + 1, y + 1, 1); // 
	RenderLine(BLACK, x - 1, y - 1, x + w, y - 1, 1); //

	// Рисуем правую грань
	RenderLine(BLACK, x + w + 1, y - 1, x + w + 1, y + h + 1, 1);
	RenderLine(BLACK, x + w - 1, y - 1, x + w - 1, y + h - 1, 1);

	// Рисуем левую грань
	RenderLine(BLACK, x + 1, y + h, x + 1, y, 1);
	RenderLine(BLACK, x - 1, y + h + 1, x - 1, y - 1, 1);

	// Рисуем нижнюю грань
	RenderLine(BLACK, x + w + 1, y + h + 1, x, y + h + 1, 1);
	RenderLine(BLACK, x + w - 1, y + h - 1, x - 1, y + h - 1, 1);
}