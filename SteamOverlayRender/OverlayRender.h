#pragma once
#include "SharedMemoryStream.h"
#include "ColorEnum.h"
#include "Logs.h"

class Overlay_
{
private:
	SharedMemoryStream* renderStream;
	SharedMemoryStream* inputStream;
	HANDLE renderMutex;
	HANDLE inputMutex;
	HANDLE handles[3];

	DWORD textureVersion = 0;

	DWORD InitializePixelTexture();

	DWORD pixelTextureId;
public:

	DWORD processId;


	void BeginRender();

	void EndRender();

	void RenderPixel(Color color,int x,int y, int thickness);
	void RenderLine(Color color, int x0, int y0, int x1, int y1, int thickness);
	void RenderLineOutlined(Color color, int x0, int y0, int x1, int y1);
	void DrawBoundingBox(Color color, int x, int y, int w, int h);

	Overlay_(std::string className)
	{
		GetWindowThreadProcessId(FindWindowA(className.c_str(), NULL),&processId);

		if (!processId)
		{
			L.ELog("Game not found");
			return;
		}

		wchar_t nameFormat[MAX_PATH];

		swprintf(nameFormat, ARRAYSIZE(nameFormat), L"GameOverlayRender_PaintCmdStream_%d_%%s-IPCWrapper", processId);
		renderStream = new SharedMemoryStream(nameFormat);

		swprintf(nameFormat, ARRAYSIZE(nameFormat), L"GameOverlay_InputEventStream_%d_%%s-IPCWrapper", processId);
		inputStream = new SharedMemoryStream(nameFormat);

		renderMutex = renderStream->GetMutexHandle();
		inputMutex = inputStream->GetMutexHandle();

		handles[0] = renderMutex;
		handles[1] = inputMutex;
		handles[2] = inputStream->GetAvailableHandle();

		pixelTextureId = InitializePixelTexture();

		L.Log("Successful overlay initialization");
	}
};inline Overlay_* Overlay;