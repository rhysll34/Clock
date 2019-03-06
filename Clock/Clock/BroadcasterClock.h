#include <GL/gl.h>
#include <GL/glu.h>
#include "Clock.h"

class BroadcasterClock : public Clock {
private:
	void drawCircle(float radius);

	void buildClock();

	float outer;
	float inner;
	int Width;
	int Height;

public:
	BroadcasterClock(int width, int height);
	~BroadcasterClock();
	void drawClock(HDC hDC);
};