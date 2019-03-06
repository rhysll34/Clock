#include <windows.h>
#include <scrnsave.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <ctime>
#include "BroadcasterClock.h"

//globals used by the function below to hold the screen size
int Width;      
int Height;

//define a Windows timer 
#define TIMER 1 

static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC);
static void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC);

Clock *currentClock = NULL;

LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT message, WPARAM wParam, 
	LPARAM lParam)
{
	static HDC hDC;
	static HGLRC hRC;
	static RECT rect;

	switch( message )
	{
		case WM_CREATE: 
			// get window dimensions
			GetClientRect( hwnd, &rect );
			Width = rect.right;         
			Height = rect.bottom;

			//get configuration from registry if applicable

			//set up OpenGL
			InitGL( hwnd, hDC, hRC );

			currentClock = new BroadcasterClock(Width, Height);
			
			//create a timer that ticks every 10 milliseconds
			SetTimer( hwnd, TIMER, 10, NULL ); 
			return 0;

		case WM_DESTROY:
			KillTimer( hwnd, TIMER );

			delete currentClock;

			CloseGL( hwnd, hDC, hRC );
			return 0;

		case WM_TIMER:
			//call some function to advance your animation  
			currentClock->drawClock(hDC);            
			return 0;                           
	}
	//let the screensaver library take care of any
	//other messages

	return DefScreenSaverProc(hwnd, message, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, 
	LPARAM lParam)
{
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}


static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof pfd );
	pfd.nSize = sizeof pfd;
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	hDC = GetDC( hWnd );

	int i = ChoosePixelFormat( hDC, &pfd );  
	SetPixelFormat( hDC, i, &pfd );

	hRC = wglCreateContext( hDC );
	wglMakeCurrent( hDC, hRC );
}


static void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}


