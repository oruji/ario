#include <stdlib.h>
#include <iostream>
#include <gl/glut.h>
#include "FMOD_API/inc/fmod.hpp"
#include "FMOD_API/inc/fmod_errors.h"

using namespace std;

#pragma comment(lib, "FMOD_API/lib/fmodex_vc.lib")

FMOD::System *mySystem;
FMOD::Sound *mySound;

const int width = 800;
const int height = 600;

int arioX;
int arioY;
GLboolean arioIsAlive = true;

const int arioBulletLimit = 50;
int arioBulletsY[arioBulletLimit] = {};
int arioBulletsX[arioBulletLimit] = {};
GLboolean arioBulletsIsAlive[arioBulletLimit] = {false};

float enemyX = 400;
float enemyY = 500;
GLboolean enemyIsAlive = true;
bool isKeyUp = true;

const int enemyBulletLimit = 50;
int enemyBulletsY[enemyBulletLimit] = {};
int enemyBulletsX[enemyBulletLimit] = {};
GLboolean enemyBulletsIsAlive[enemyBulletLimit] = {false};

bool keyStates[256] = {0};

void myKeyboard(unsigned char key, int x, int y) {  
	keyStates[key] = true;
}   

void myKeyboardUp(unsigned char key, int x, int y) { 
	if (key == ' ')
		isKeyUp = true;

	keyStates[key] = false;
} 

void myIdle() {
	if(keyStates['a']) {
		enemyX -= 0.1; }

	if(keyStates['d']) {
		enemyX += 0.1; }

	if(keyStates['w']) {
		enemyY += 0.1; }

	if(keyStates['s']) {
		enemyY -= 0.1; }

	if(keyStates[' '] && isKeyUp && enemyIsAlive) {
		for (int i = 0; i < enemyBulletLimit; i++) {
			if (!enemyBulletsIsAlive[i]) {
				enemyBulletsX[i] = enemyX;
				enemyBulletsY[i] = enemyY;
				enemyBulletsIsAlive[i] = true;
				isKeyUp = false;

				mySystem -> createSound("shot.mp3", FMOD_HARDWARE, 0, &mySound);
				mySystem -> playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

				break;
			}
		}	
	}

	glutPostRedisplay();
}

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Ario
	if (arioIsAlive) {
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
		glVertex2i(arioX - 20, arioY);
		glVertex2i(arioX + 20, arioY);
		glVertex2i(arioX, arioY + 20);
		glEnd();
	}

	// Enemy
	if (enemyIsAlive) {
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2i(enemyX, enemyY - 20);
		glVertex2i(enemyX + 20, enemyY);
		glVertex2i(enemyX - 20, enemyY);
		glEnd();
	}

	// ario's bullets strike to enemy
	for (int i = 0; i < arioBulletLimit; i++) {
		if (arioBulletsIsAlive[i] && enemyIsAlive)
			if (arioBulletsX[i] > enemyX - 20 && arioBulletsX[i] < enemyX + 20)
				if (arioBulletsY[i] > enemyY - 20 && arioBulletsY[i] < enemyY || 
					arioBulletsY[i] + 10 > enemyY - 20 && arioBulletsY[i] + 10 < enemyY) {
					mySystem -> createSound("explosion.mp3", FMOD_HARDWARE, 0, &mySound);
					mySystem -> playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

					enemyIsAlive = false;
					arioBulletsIsAlive[i] = false;
					arioBulletsX[i] = 0;
					arioBulletsY[i] = 0;

					break;
				}
	}

	// enemy's bullets strike to ario
	for (int i = 0; i < enemyBulletLimit; i++) {
		if (enemyBulletsIsAlive[i] && arioIsAlive)
			if (enemyBulletsX[i] > arioX - 20 && enemyBulletsX[i] < arioX + 20)
				if (enemyBulletsY[i] > arioY && enemyBulletsY[i] < arioY + 20 || 
					enemyBulletsY[i] - 10 > arioY && enemyBulletsY[i] - 10 < arioY + 20) {
					mySystem->createSound("explosion.mp3", FMOD_HARDWARE, 0, &mySound);
					mySystem->playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

					arioIsAlive = false;
					enemyBulletsIsAlive[i] = false;
					enemyBulletsX[i] = 0;
					enemyBulletsY[i] = 0;

					break;
				}
	}

	// ario bullet
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for (int i = 0; i < arioBulletLimit; i++) {
		if (arioBulletsIsAlive[i]) {
			glVertex2i(arioBulletsX[i], arioBulletsY[i]);
			glVertex2i(arioBulletsX[i], arioBulletsY[i] + 10);
		}
	}
	glEnd();

	// enemy bullet
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < enemyBulletLimit; i++) {
		if (enemyBulletsIsAlive[i]) {
			glVertex2i(enemyBulletsX[i], enemyBulletsY[i]);
			glVertex2i(enemyBulletsX[i], enemyBulletsY[i] - 10);
		}
	}
	glEnd();

	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y) {
	// ario shot
	if (arioIsAlive) {
		if (GLUT_LEFT_BUTTON == button) {
			if (state == 0) {
				for (int i = 0; i < arioBulletLimit; i++) {
					if (!arioBulletsIsAlive[i]) {
						arioBulletsX[i] = x;
						arioBulletsY[i] = height - y;
						arioBulletsIsAlive[i] = true;

						mySystem -> createSound("shot.mp3", FMOD_HARDWARE, 0, &mySound);
						mySystem -> playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

						arioY -= 3;

						break;
					}
				}
			}
		}
	}

	glutPostRedisplay();
}

void myTimer(int value) {

	// ario bullets movement
	for (int i = 0; i < arioBulletLimit; i++) {
		if (arioBulletsIsAlive[i])
			if (arioBulletsY[i] > height) {
				arioBulletsY[i] = 0;
				arioBulletsX[i] = 0;
				arioBulletsIsAlive[i] = false;
			}

			if (arioBulletsIsAlive[i])
				arioBulletsY[i] += 6;
	}

	// enemy bullets movement
	for (int i = 0; i < enemyBulletLimit; i++) {
		if (enemyBulletsIsAlive[i])
			if (enemyBulletsY[i] < 0) {
				enemyBulletsY[i] = 0;
				enemyBulletsX[i] = 0;
				enemyBulletsIsAlive[i] = false;
			}

			if (enemyBulletsIsAlive[i])
				enemyBulletsY[i] -= 6;
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

	mySystem -> init(32, FMOD_INIT_NORMAL, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	glutCreateWindow("Ario 0.0.1");
	glutPositionWindow(300, 100);
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutTimerFunc(20, myTimer, 1);
	glutPassiveMotionFunc(myPassiveMotion);
	glutMotionFunc(myMotion);
	//glutReshapeFunc(myReshape);
	glutKeyboardFunc(myKeyboard);
	glutKeyboardUpFunc(myKeyboardUp);
	glutIdleFunc(myIdle);
	gluOrtho2D(0, 800, 0, 600);

	glutMainLoop();
}
