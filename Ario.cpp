#include <stdlib.h>
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <map>

using namespace std;

#pragma comment(lib, "fmod.lib")

FSOUND_STREAM* g_mp3_stream = NULL;

GLuint ys[200] = {};
GLuint xs[200] = {};
int shotLim = 200;

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_LINES);

	for (int i = 0; i < shotLim; i++) {
		if (xs[i] > 0 && ys[i] > 0) {
			glVertex2i(xs[i], ys[i]);
			glVertex2i(xs[i], ys[i] + 10);
		}
	}

	glEnd();
	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y) {
	if (state == 0) {
		for (int i = 0; i < shotLim; i++) {
			if (xs[i] == 0 && ys[i] == 0) {
				xs[i] = x;
				ys[i] = 400 - y;

				break;
			}
		}

		g_mp3_stream = FSOUND_Stream_Open("mouse.mp3", FSOUND_2D, 0, 0);
		FSOUND_Stream_Play(0,g_mp3_stream);
	}

	glutPostRedisplay();
}

void myTimer(int value) {
	for (int i = 0; i < shotLim; i++) {
		if (ys[i] > 400) {
			ys[i] = 0;
			xs[i] = 0;
		}

		if (ys[i] > 0)
			ys[i] += 6;
	}


	glutPostRedisplay();
	glutTimerFunc(20, myTimer, 1);
}

void main(int argc, char* argv[]) {
	//cin.get();

	if (FSOUND_Init(44000, 64, 0) == 0) {
		//Valid maxRate Range:    4000 to 65535   up to 1024
		cout << "[ERROR] Could not initialise fmod\n";

		return;
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(400, 400);
	glutCreateWindow("My First Practice");

	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutTimerFunc(20, myTimer, 1);

	gluOrtho2D(0, 400, 0, 400);

	glutMainLoop();

	FSOUND_Stream_Stop(g_mp3_stream);
	FSOUND_Stream_Close(g_mp3_stream);
	FSOUND_Close();
}
