#include <windows.h>
#include <thread>
#include "Globals.h"


int main()
{
	Overlay = new Overlay_("UnityWndClass");
	

	for (;;)
	{
		Overlay->BeginRender();
		
		Overlay->DrawBoundingBox(WHITE,100,100,300,500);

		Overlay->EndRender();
	}

}