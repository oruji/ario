#include <stdlib.h>
#include <iostream>
#include <gl/glut.h>
#include <ctime>
#include "FMOD_API/inc/fmod.hpp"
#include "FMOD_API/inc/fmod_errors.h"
using namespace std;

#pragma comment(lib, "FMOD_API/lib/fmodex_vc.lib")

FMOD::System *mySystem;
FMOD::Sound *mySound;

GLuint width = 400;
GLuint height = 400;

GLuint bulletYArray[100] = {};
GLuint bulletXArray[100] = {};
GLboolean bulletIsAlive[100] = {false};

GLuint bulletLimit = 100;

GLuint arioX;
GLuint arioY;

GLuint enemyX = 200;
GLuint enemyY = 200;
GLboolean enemyIsAlive = true;

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Ario
	glBegin(GL_TRIANGLES);
	glVertex2i(arioX - 20, arioY);
	glVertex2i(arioX + 20, arioY);
	glVertex2i(arioX, arioY + 20);
	glEnd();

	// bullet strike to enemy
	for (int i = 0; i < bulletLimit; i++) {
		if (bulletIsAlive[i] && enemyIsAlive)
			if (bulletXArray[i] > enemyX - 20 && bulletXArray[i] < enemyX + 20)
				if (bulletYArray[i] > enemyY - 20 && bulletYArray[i] < enemyY) {
					mySystem->createSound("explosion.mp3", FMOD_HARDWARE, 0, &mySound);
					mySystem->playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

					enemyIsAlive = false;
					bulletIsAlive[i] = false;
					bulletXArray[i] = 0;
					bulletYArray[i] = 0;

					break;
				}
	}

	// Enemy
	if (enemyIsAlive) {
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2i(enemyX, enemyY - 20);
		glVertex2i(enemyX + 20, enemyY);
		glVertex2i(enemyX - 20, enemyY);
		glEnd();
		glColor3f(1, 1, 1);
	}

	// bullet
	glBegin(GL_LINES);
	for (int i = 0; i < bulletLimit; i++) {
		if (bulletIsAlive) {
			glVertex2i(bulletXArray[i], bulletYArray[i]);
			glVertex2i(bulletXArray[i], bulletYArray[i] + 10);
		}
	}
	glEnd();

	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y) {
	if (state == 0) {
		for (int i = 0; i < bulletLimit; i++) {
			if (!bulletIsAlive[i]) {
				bulletXArray[i] = x;
				bulletYArray[i] = height - y;
				bulletIsAlive[i] = true;

				break;
			}
		}

		mySystem->createSound("shot.mp3", FMOD_HARDWARE, 0, &mySound);
		mySystem->playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

		arioY -= 3;
	}

	glutPostRedisplay();
}

void myTimer(int value) {
	for (int i = 0; i < bulletLimit; i++) {
		if (bulletIsAlive[i])
			if (bulletYArray[i] > 400) {
				bulletYArray[i] = 0;
				bulletXArray[i] = 0;
				bulletIsAlive[i] = false;
			}

			if (bulletYArray[i] > 0)
				bulletYArray[i] += 6;
	}

	glutPostRedisplay();
	glutTimerFunc(20, myTimer, 1);
}

void myPassiveMotion(int x, int y) {
	arioX = x;
	arioY = (height - y);

	glutPostRedisplay();
}

void myMotion(int x, int y) {
	arioX = x;
	arioY = (height - y);

	glutPostRedisplay();
}

void myReshape(int w, int h) {
	GLfloat aspectRatio;

	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	aspectRatio = (GLfloat) w / (GLfloat) h;

	if (w <= h) 
		gluOrtho2D(0, width, 0 / aspectRatio, height / aspectRatio);

	else 
		gluOrtho2D(0 * aspectRatio, width * aspectRatio, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main(int argc, char* argv[]) {
	//cin.get();

	FMOD::System_Create(&mySystem);

	mySystem->init(32, FMOD_INIT_NORMAL, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Ario 0.0.1");
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutTimerFunc(20, myTimer, 1);
	glutPassiveMotionFunc(myPassiveMotion);
	glutMotionFunc(myMotion);
	glutReshapeFunc(myReshape);

	glutMainLoop();
}
