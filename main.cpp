#include <windows.h>
#include <scrnsave.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <ctime>

//#pragma comment(lib, "libscrnsave.a")
//#pragma comment(lib, "libopengl32.a")
//#pragma comment(lib, "libglu32.a")

//globals used by the function below to hold the screen size
int Width;      
int Height;
int smallest;

// An index for the clock markings
GLuint index;

// Determine how big various clock parts should be
float widthByTwo;
float outer;
float inner;

int timeOffset;

//define a Windows timer 
#define TIMER 1 

static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC);
static void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC);
static void buildClock();
static void drawClock(HDC hDC);
static void drawCircle(float radius);


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
			smallest = min(Width, Height);

			//get configuration from registry if applicable

			//set up OpenGL
			InitGL( hwnd, hDC, hRC );			
			buildClock();
			
			// Add an hour if DST is in effect
//			tzset(); 
			_tzset();
			if(_get_daylight)
			{
				timeOffset = 3600;
			}
			else
			{
				timeOffset = 0;
			}

			//Initialize perspective, viewpoint, and
			//any objects you wish to animate

			//create a timer that ticks every 10 milliseconds
			SetTimer( hwnd, TIMER, 10, NULL ); 
			return 0;

		case WM_DESTROY:
			KillTimer( hwnd, TIMER );

			//delete any objects created during animation
			//and close down OpenGL nicely
			glDeleteLists(index, 1);

			CloseGL( hwnd, hDC, hRC );
			return 0;

		case WM_TIMER:
			//call some function to advance your animation  
			drawClock(hDC);            
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

static void buildClock()
{
	//window resizing stuff
    glViewport(0, 0, (GLsizei) Width, (GLsizei) Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
        
	glOrtho(-Width / 2, Width / 2, -Height / 2, Height / 2, -1.0, 1.0);  
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
	//camera xyz, the xyz to look at, and the up vector (+y is up)
        
	//background
	glClearColor(0.0, 0.0, 0.55, 0.0); //0.0s is black

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
	glShadeModel(GL_SMOOTH); 

	glColor3f(1.0, 1.0, 0.0);
	
	index = glGenLists(1);
	
	widthByTwo = smallest / 100;
	inner = smallest * 0.325;
	outer = (2 * smallest) / 5;

	glNewList(index, GL_COMPILE);
		glColor3f(1.0, 1.0, 0.0);
		glLineWidth(0.2 * smallest);
		// Every three hour ticks
		glBegin(GL_QUADS);
			// Left Tick
			glVertex2f(-3 * widthByTwo, inner);
			glVertex2f(-3 * widthByTwo, outer);
			glVertex2f(-widthByTwo, outer);
			glVertex2f(-widthByTwo, inner);
			// Right Tick	
			glVertex2f(widthByTwo, inner);
			glVertex2f(widthByTwo, outer);
			glVertex2f(3 * widthByTwo, outer);
			glVertex2f(3 * widthByTwo, inner);
		glEnd();
		glPushMatrix();
		glRotatef(30, 0.0, 0.0, 1.0);
		// One o'clock tick
		glBegin(GL_POLYGON);
			glVertex2f(-widthByTwo, inner);
			glVertex2f(-widthByTwo, outer);
			glVertex2f(widthByTwo, outer);
			glVertex2f(widthByTwo, inner);
		glEnd();
		glPopMatrix();
		glPushMatrix();
		glRotatef(60, 0.0, 0.0, 1.0);
		// Two o'clock tick
		glBegin(GL_POLYGON);
			glVertex2f(-widthByTwo, inner);
			glVertex2f(-widthByTwo, outer);
			glVertex2f(widthByTwo, outer);
			glVertex2f(widthByTwo, inner);
		glEnd();
		glPopMatrix();
	glEndList();
}

static void drawClock(HDC hDC)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw the clock face
	for(int i = 0; i <= 360; i += 90)
	{
		glPushMatrix();
		glRotatef(i, 0.0, 0.0, 1.0);
		glCallList(index);
		glPopMatrix();
	}
	drawCircle(3 * widthByTwo);
	
	// Get the current time
	time_t cTime;
	cTime = time(NULL);
	
	// 86400 seconds in a day, but we are using a 12 hour clock
	int currentTime = cTime % 43200;
	currentTime += timeOffset;
	// See if we are in DST (assumes DST lasts from April 1st to October 31st)
/*	int dayOfYear = currentTime % (86400 * 365);
	if (dayOfYear > 80 && dayOfYear < 115)
	{
		// Add an hour to the time
		currentTime += 86400;
	}
	else
	{
		currentTime -= 86400;
	}*/
	int seconds = currentTime % 60;
	int minuteSeconds = currentTime % 3600;

	// Second Hand
	glLineWidth(5);
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRotatef(6 * seconds, 0.0, 0.0, -1.0);
	glBegin(GL_QUADS);
		glVertex2f(-widthByTwo / 3, 4 * widthByTwo);
		glVertex2f(-widthByTwo / 3, outer - widthByTwo);
		glVertex2f(widthByTwo / 3, outer - widthByTwo);
		glVertex2f(widthByTwo / 3, 4 * widthByTwo);
	glEnd();
	glPopMatrix();
	
	// Minute Hand
	glPushMatrix();
	glRotatef(0.1 * minuteSeconds, 0.0, 0.0, -1.0);
	glBegin(GL_QUADS);
		glVertex2f(-widthByTwo / 2, 4 * widthByTwo);
		glVertex2f(-widthByTwo / 2, outer - widthByTwo);
		glVertex2f(widthByTwo / 2, outer - widthByTwo);
		glVertex2d(widthByTwo / 2, 4 * widthByTwo);
	glEnd();
	glPopMatrix();
	
	// Hour Hand
	glLineWidth(10);
	glPushMatrix();
	glRotatef((float)currentTime * (360.0 / 43200.0), 0.0, 0.0, -1.0);
/*	glBegin(GL_LINES);
		glVertex2f(0.0, smallest / 40);
		glVertex2f(0.0, smallest / 4);
	glEnd();*/
	glBegin(GL_QUADS);
		glVertex2f(-widthByTwo, 4 * widthByTwo);
		glVertex2f(-widthByTwo, inner - 2 * widthByTwo);
		glVertex2f(widthByTwo, inner - 2 * widthByTwo);
		glVertex2f(widthByTwo, 4 * widthByTwo);
	glEnd();
	glPopMatrix();
	
	SwapBuffers(hDC);	
}

const float DEG2RAD = 3.14159 / 180;
 
void drawCircle(float radius)
{
   glBegin(GL_POLYGON);
   for (int i = 0; i < 360; i++)
   {
      float degInRad = i * DEG2RAD;
      glVertex2f(cos(degInRad) * radius, sin(degInRad) * radius);
   }
   glEnd();
}

