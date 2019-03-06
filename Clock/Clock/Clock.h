#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <ctime>

class Clock {
public:
	Clock() {};
	Clock(int width, int height);

	virtual void drawClock(HDC hDC) = 0;
};