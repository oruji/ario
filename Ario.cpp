#include <stdlib.h>
#include <iostream>
#include <gl/glut.h>
#include "FMOD_API/inc/fmod.hpp"
#include "FMOD_API/inc/fmod_errors.h"
#include <chrono>

using namespace std;

#pragma comment(lib, "FMOD_API/lib/fmodex_vc.lib")

FMOD :: System *mySystem;
FMOD :: Sound *mySound;

const int width = 800;
const int height = 600;

int arioX;
int arioY;
GLboolean arioIsAlive = true;
bool arioShake = false;
int arioShakeCounter = 0;

bool arioInControl = true;
bool arioInvisible = false;
chrono :: steady_clock :: time_point arioDeadStart;
chrono :: steady_clock :: time_point arioInControlStart;
chrono :: steady_clock :: time_point arioInvisibleStart;

const int arioBulletLimit = 50;
int arioBulletsY[arioBulletLimit] = {};
int arioBulletsX[arioBulletLimit] = {};
GLboolean arioBulletsIsAlive[arioBulletLimit] = {false};

float enemyX = 400;
float enemyY = 500;
GLboolean enemyIsAlive = true;
bool isKeyUp = true;
bool enemyShake = false;
int enemyShakeCounter = 0;

const int enemyBulletLimit = 50;
int enemyBulletsY[enemyBulletLimit] = {};
int enemyBulletsX[enemyBulletLimit] = {};
GLboolean enemyBulletsIsAlive[enemyBulletLimit] = {false};

bool enemyInControl = true;
bool enemyInvisible = false;
chrono :: steady_clock :: time_point enemyDeadStart;
chrono :: steady_clock :: time_point enemyInControlStart;
chrono :: steady_clock :: time_point enemyInvisibleStart;

bool keyStates[256] = {0};

bool isElapsed(chrono :: steady_clock :: time_point start, int dur) {
	return chrono :: duration_cast<chrono :: milliseconds>(chrono :: steady_clock :: now() - start).count() > dur;
}

void myKeyboard(unsigned char key, int x, int y) {  
	keyStates[key] = true;
}  

void myKeyboardUp(unsigned char key, int x, int y) { 
	if (key == ' ')
		isKeyUp = true;

	keyStates[key] = false;
} 

void myIdle() {
	if((keyStates['a'] || keyStates['A']) && enemyInControl) {
		enemyX -= 0.1; }

	if((keyStates['d'] || keyStates['D']) && enemyInControl) {
		enemyX += 0.1; }

	if((keyStates['w'] || keyStates['W']) && enemyInControl) {
		enemyY += 0.1; }

	if((keyStates['s'] || keyStates['S']) && enemyInControl) {
		enemyY -= 0.1; }

	if(keyStates[' '] && isKeyUp && enemyIsAlive && enemyInControl) {
		for (int i = 0; i < enemyBulletLimit; i++) {
			if (!enemyBulletsIsAlive[i]) {
				enemyBulletsX[i] = enemyX;
				enemyBulletsY[i] = enemyY;
				enemyBulletsIsAlive[i] = true;
				isKeyUp = false;

				mySystem -> createSound("shot.mp3", FMOD_HARDWARE, 0, &mySound);
				mySystem -> playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

				enemyY += 3;
				enemyShake = true;

				break;
			}
		}	
	}

	glutPostRedisplay();
}

void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius){
	int i;
	int lineAmount = 100; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * 3.1415;

	glBegin(GL_LINE_LOOP);
	for(i = 0; i <= lineAmount;i++) { 
		glVertex2f(
			x + (radius * cos(i *  twicePi / lineAmount)), 
			y + (radius* sin(i * twicePi / lineAmount))
			);
	}
	glEnd();
}

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!arioInControl) {
		if (isElapsed(arioInControlStart, 500)) {
			arioInControl = true; } }

	if (arioInvisible) {
		if (isElapsed(arioInvisibleStart, 2000)) {
			arioInvisible = false; } }

	if (!enemyInControl) {
		if (isElapsed(enemyInControlStart, 500)) {
			enemyInControl = true; } }

	if (enemyInvisible) {
		if (isElapsed(enemyInvisibleStart, 2000)) {
			enemyInvisible = false; } }

	// Ario
	if (arioIsAlive) {
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
		glVertex2i(arioX - 20, arioY);
		glVertex2i(arioX + 20, arioY);
		glVertex2i(arioX, arioY + 20);
		glEnd(); } 

	else if (isElapsed(arioDeadStart, 2000)) {
		arioX = width / 2;
		arioY = -20;
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
		glVertex2i(arioX - 20, arioY);
		glVertex2i(arioX + 20, arioY);
		glVertex2i(arioX, arioY + 20);
		glEnd(); 

		arioIsAlive = true; 
		arioInControl = false; 
		arioInControlStart = chrono :: system_clock :: now(); 
		arioInvisible = true;
		arioInvisibleStart = chrono :: system_clock :: now(); }

	// Invisible Circle
	if (arioInvisible) {
		drawHollowCircle(arioX, arioY + 5, 50); }

	// Enemy
	if (enemyIsAlive) {
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2i(enemyX, enemyY - 20);
		glVertex2i(enemyX + 20, enemyY);
		glVertex2i(enemyX - 20, enemyY);
		glEnd(); }


	else if (isElapsed(enemyDeadStart, 2000)) {
		enemyX = width / 2;
		enemyY = height + 20;
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
		glVertex2i(enemyX, enemyY - 20);
		glVertex2i(enemyX + 20, enemyY);
		glVertex2i(enemyX - 20, enemyY);
		glEnd(); 

		enemyIsAlive = true; 
		enemyInControl = false; 
		enemyInControlStart = chrono :: system_clock :: now(); 
		enemyInvisible = true;
		enemyInvisibleStart = chrono :: system_clock :: now(); }

	// Invisible Circle
	if (enemyInvisible) {
		drawHollowCircle(enemyX, enemyY - 5, 50); }


	// ario's bullets strike to enemy
	if (enemyIsAlive && !enemyInvisible) {
		for (int i = 0; i < arioBulletLimit; i++) {
			if (arioBulletsIsAlive[i]) {
				if (arioBulletsX[i] > enemyX - 20 && arioBulletsX[i] < enemyX + 20) {
					if (arioBulletsY[i] > enemyY - 20 && arioBulletsY[i] < enemyY || 
						arioBulletsY[i] + 10 > enemyY - 20 && arioBulletsY[i] + 10 < enemyY) {
							mySystem -> createSound("explosion.mp3", FMOD_HARDWARE, 0, &mySound);
							mySystem -> playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

							enemyIsAlive = false;
							arioBulletsIsAlive[i] = false;
							arioBulletsX[i] = 0;
							arioBulletsY[i] = 0;

							enemyDeadStart = chrono :: steady_clock :: now();

							break;
					}
				}
			}
		}
	}

	// enemy's bullets strike to ario
	if (arioIsAlive && !arioInvisible) {
		for (int i = 0; i < enemyBulletLimit; i++) {
			if (enemyBulletsIsAlive[i]) {
				if (enemyBulletsX[i] > arioX - 20 && enemyBulletsX[i] < arioX + 20) {
					if (enemyBulletsY[i] > arioY && enemyBulletsY[i] < arioY + 20 || 
						enemyBulletsY[i] - 10 > arioY && enemyBulletsY[i] - 10 < arioY + 20) {
							mySystem->createSound("explosion.mp3", FMOD_HARDWARE, 0, &mySound);
							mySystem->playSound(FMOD_CHANNEL_FREE, mySound, false, 0);

							arioIsAlive = false;
							enemyBulletsIsAlive[i] = false;
							enemyBulletsX[i] = 0;
							enemyBulletsY[i] = 0;

							arioDeadStart = chrono :: steady_clock :: now();

							break;
					}
				}
			}
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

	mySystem -> update();

	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y) {
	// ario shot
	if (arioIsAlive && arioInControl) {
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
						arioShake = true;

						break;
					}
				}
			}
		}
	}

	glutPostRedisplay();
}

void myTimer(int value) {

	if (arioIsAlive && !arioInControl) {
		arioY += 4; }

	if (enemyIsAlive && !enemyInControl) {
		enemyY -= 4; }

	if (arioShake) {
		arioShakeCounter ++; }

	if (arioShakeCounter == 4) {
		arioY += 3; 
		arioShakeCounter = 0;
		arioShake = false; }


	if(enemyShake) {
		enemyShakeCounter ++; }

	if(enemyShakeCounter == 4) {
		enemyY -= 3;
		enemyShakeCounter = 0;
		enemyShake = false; }

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
	if (arioIsAlive && arioInControl) {
		arioX = x;
		arioY = (height - y);

		glutPostRedisplay(); }
}

void myMotion(int x, int y) {
	if (arioIsAlive && arioInControl) {
		arioX = x;
		arioY = (height - y);

		glutPostRedisplay(); }
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