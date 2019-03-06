#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "BroadcasterClock.h"

const float DEG2RAD = 3.14159 / 180;

// Determine how big various clock parts should be
float widthByTwo;

int smallest;

// An index for the clock markings
GLuint index;

BroadcasterClock::BroadcasterClock(int width, int height)
{
	Width = width;
	Height = height;
	smallest = min(Width, Height);
	buildClock();
}

BroadcasterClock::~BroadcasterClock()
{
	//delete any objects created during animation
	//and close down OpenGL nicely
	glDeleteLists(index, 1);
}

void BroadcasterClock::drawCircle(float radius)
{
	glBegin(GL_POLYGON);
	for(int i = 0; i < 360; i++)
	{
		float degInRad = i * DEG2RAD;
		glVertex2f(cos(degInRad) * radius, sin(degInRad) * radius);
	}
	glEnd();
}

void BroadcasterClock::drawClock(HDC hDC)
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
	glBegin(GL_QUADS);
	glVertex2f(-widthByTwo, 4 * widthByTwo);
	glVertex2f(-widthByTwo, inner - 2 * widthByTwo);
	glVertex2f(widthByTwo, inner - 2 * widthByTwo);
	glVertex2f(widthByTwo, 4 * widthByTwo);
	glEnd();
	glPopMatrix();

	SwapBuffers(hDC);
}

void BroadcasterClock::buildClock()
{
	//window resizing stuff
	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);

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