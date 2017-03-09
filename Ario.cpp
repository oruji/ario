#include <stdlib.h>
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <iostream>
#include <GL/glut.h>

#pragma comment(lib, "fmod.lib")

FSOUND_STREAM* g_mp3_stream = NULL;

GLuint myX;
GLuint myY;

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_LINES);
	glVertex2i(myX, myY);
	glVertex2i(myX, myY + 10);
	glEnd();

	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y) {
	if (state == 0) {
		myX = x;
		myY = 400 - y;
		g_mp3_stream = FSOUND_Stream_Open("mouse.mp3", FSOUND_2D, 0, 0);
		FSOUND_Stream_Play(0,g_mp3_stream);
	}

	glutPostRedisplay();
}

void myTimer(int value) {
	
	if (myY <= 400) {
		myY += 2;	
	}

	glutPostRedisplay();
	glutTimerFunc(33, myTimer, 1);
}

void main(int argc, char* argv[]) {
	if (FSOUND_Init(44000, 64, 0) == 0) {
		//Valid maxRate Range:    4000 to 65535   up to 1024
		std :: cout << "[ERROR] Could not initialise fmod\n";

		return;
	}

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(400, 400);
	glutCreateWindow("My First Practice");

	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutTimerFunc(33, myTimer, 1);

	gluOrtho2D(0, 400, 0, 400);

	glutMainLoop();

	FSOUND_Stream_Stop(g_mp3_stream);
	FSOUND_Stream_Close(g_mp3_stream);
	FSOUND_Close();
}
