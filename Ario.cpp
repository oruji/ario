#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>
#include <stdlib.h>
#include <chrono>
#include <string>
#include <ctime>
#include <math.h>

using namespace std;

bool pause = false;

FMOD_SYSTEM *mySystem;
FMOD_SOUND *mySound;

const int width = 1366;
const int height = 768;

float arioX = width / 2;
float arioY = -20;
GLboolean arioIsAlive = true;
bool arioShake = false;
int arioShakeCounter = 0;
int arioScore = 0;

bool arioInControl = false;
bool arioInvisible = true;
int arioDeadCounter;
int arioInControlCounter = 0;
int arioInvisibleCounter = 0;

const int arioBulletLimit = 50;
int arioBulletsY[arioBulletLimit] = { };
int arioBulletsX[arioBulletLimit] = { };
GLboolean arioBulletsIsAlive[arioBulletLimit] = { false };

float enemyX = width / 2;
float enemyY = height + 20;
GLboolean enemyIsAlive = true;
bool isKeyUp = true;
bool isPKeyUp = true;
bool enemyShake = false;
int enemyShakeCounter = 0;
int enemyScore = 0;

const int enemyBulletLimit = 50;
int enemyBulletsY[enemyBulletLimit] = { };
int enemyBulletsX[enemyBulletLimit] = { };
GLboolean enemyBulletsIsAlive[enemyBulletLimit] = { false };

bool enemyInControl = false;
bool enemyInvisible = true;
int enemyDeadCounter;
int enemyInControlCounter = 0;
int enemyInvisibleCounter = 0;

bool keyStates[256] = { 0 };

#define FONT GLUT_BITMAP_8_BY_13
#define CHAR_W 8

void emitString(char *s, int tx, int ty) {
	int x = tx;
	int y = ty;

	while (*s) {
		glRasterPos2i(x, y);
		glutBitmapCharacter(FONT, *s);
		x += CHAR_W;
		++s;
	}
}

bool isElapsed(chrono::steady_clock::time_point start, int dur) {
	return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() > dur;
}

void myKeyboard(unsigned char key, int x, int y) {
	keyStates[key] = true;
}

void myKeyboardUp(unsigned char key, int x, int y) {
	if (key == ' ') {
		isKeyUp = true;
	}

	if (key == 'p' || key == 'P') {
		isPKeyUp = true;
	}

	keyStates[key] = false;
}

float movementSpeed = 0.2;

void myIdle() {
	if ((keyStates['p'] || keyStates['P']) && isPKeyUp) {
		pause = !pause;
		isPKeyUp = false;
	}

	else {
		if ((keyStates['a'] || keyStates['A']) && enemyInControl) {
			if (enemyX > 0)
				enemyX -= movementSpeed;
		}

		if ((keyStates['d'] || keyStates['D']) && enemyInControl) {
			if (enemyX < width)
				enemyX += movementSpeed;
		}

		if ((keyStates['w'] || keyStates['W']) && enemyInControl) {
			if (enemyY < height)
				enemyY += movementSpeed;
		}

		if ((keyStates['s'] || keyStates['S']) && enemyInControl) {
			if (enemyY > 20)
				enemyY -= movementSpeed;
		}

		if (keyStates[' '] && isKeyUp && enemyIsAlive && enemyInControl) {
			for (int i = 0; i < enemyBulletLimit; i++) {
				if (!enemyBulletsIsAlive[i]) {
					enemyBulletsX[i] = enemyX;
					enemyBulletsY[i] = enemyY;
					enemyBulletsIsAlive[i] = true;
					isKeyUp = false;

					FMOD_System_CreateSound(mySystem, "shot.mp3", FMOD_HARDWARE, 0, &mySound);
					FMOD_System_PlaySound(mySystem, FMOD_CHANNEL_FREE, mySound, false, 0);

					enemyY += 3;
					enemyShake = true;

					break;
				}
			}
		}
	}

	glutPostRedisplay();
}

void drawCircle(GLfloat x, GLfloat y, GLfloat radius) {
	int i;
	int lineAmount = 100; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * 3.1415;

	glBegin(GL_LINE_LOOP);
	for (i = 0; i <= lineAmount; i++) {
		glVertex2f(x + (radius * cos(i * twicePi / lineAmount)), y + (radius * sin(i * twicePi / lineAmount)));
	}
	glEnd();
}

long mycounter = 0;
chrono::steady_clock::time_point mytime = chrono::steady_clock::now();
void myDisplay(void) {
	mycounter++;

	if (isElapsed(mytime, 1000)) {
		cout << "myDisplay(): " << mycounter << endl;
		mycounter = 0;
		mytime = chrono::steady_clock::now();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (pause) {
		emitString((char *)"pause", width / 2, height / 2);
	}

	else {
		// ario scores
		glColor3f(0, 1, 0);
		emitString(strdup(("Win: " + to_string(arioScore)).c_str()), 10, 10);

		// enemy scores
		glColor3f(1, 0, 0);
		emitString(strdup(("Win: " + to_string(enemyScore)).c_str()), width - 70, height - 15);

		if (!arioInControl) {
			if (arioInControlCounter >= 30) {
				arioInControl = true;
			}
		}

		if (arioInvisible) {
			if (arioInvisibleCounter >= 100) {
				arioInvisible = false;
			}
		}

		if (!enemyInControl) {
			if (enemyInControlCounter >= 30) {
				enemyInControl = true;
			}
		}

		if (enemyInvisible) {
			if (enemyInvisibleCounter >= 100) {
				enemyInvisible = false;
			}
		}

		// Ario
		if (arioIsAlive) {
			glColor3f(0, 1, 0);
			glBegin(GL_TRIANGLES);
			glVertex2i(arioX - 20, arioY);
			glVertex2i(arioX + 20, arioY);
			glVertex2i(arioX, arioY + 20);
			glEnd();
		}

		else if (arioDeadCounter >= 100) {
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
			arioInControlCounter = 0;
			arioInvisible = true;
			arioInvisibleCounter = 0;
		}

		// Invisible Circle
		if (arioInvisible) {
			drawCircle(arioX, arioY + 5, 50);
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

		else if (enemyDeadCounter >= 100) {
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
			enemyInControlCounter = 0;
			enemyInvisible = true;
			enemyInvisibleCounter = 0;
		}

		// Invisible Circle
		if (enemyInvisible) {
			drawCircle(enemyX, enemyY - 5, 50);
		}

		// ario's bullets strike to enemy
		if (enemyIsAlive && !enemyInvisible) {
			for (int i = 0; i < arioBulletLimit; i++) {
				if (arioBulletsIsAlive[i]) {
					if (arioBulletsX[i] > enemyX - 20 && arioBulletsX[i] < enemyX + 20) {
						if ((arioBulletsY[i] > enemyY - 20 && arioBulletsY[i] < enemyY) || (arioBulletsY[i] + 10 > enemyY - 20 && arioBulletsY[i] + 10 < enemyY)) {
							FMOD_System_CreateSound(mySystem, "explosion.mp3",
							FMOD_HARDWARE, 0, &mySound);
							FMOD_System_PlaySound(mySystem, FMOD_CHANNEL_FREE, mySound, false, 0);

							enemyIsAlive = false;
							arioBulletsIsAlive[i] = false;
							arioBulletsX[i] = 0;
							arioBulletsY[i] = 0;

							enemyDeadCounter = 0;

							arioScore++;

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
						if ((enemyBulletsY[i] > arioY && enemyBulletsY[i] < arioY + 20) || (enemyBulletsY[i] - 10 > arioY && enemyBulletsY[i] - 10 < arioY + 20)) {
							FMOD_System_CreateSound(mySystem, "explosion.mp3",
							FMOD_HARDWARE, 0, &mySound);
							FMOD_System_PlaySound(mySystem, FMOD_CHANNEL_FREE, mySound, false, 0);

							arioIsAlive = false;
							enemyBulletsIsAlive[i] = false;
							enemyBulletsX[i] = 0;
							enemyBulletsY[i] = 0;

							arioDeadCounter = 0;
							enemyScore++;

							break;
						}
					}
				}
			}
		}

		// ario bullet
		glColor3f(0, 1, 0);
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

		FMOD_System_Update(mySystem);
	}

	glutSwapBuffers();
}

void myMouse(int button, int state, int x, int y) {
	if (!pause) {
		// ario shot
		if (arioIsAlive && arioInControl) {
			if (GLUT_LEFT_BUTTON == button) {
				if (state == 0) {
					for (int i = 0; i < arioBulletLimit; i++) {
						if (!arioBulletsIsAlive[i]) {
							arioBulletsX[i] = x;
							arioBulletsY[i] = height - y;
							arioBulletsIsAlive[i] = true;

							FMOD_System_CreateSound(mySystem, "shot.mp3",
							FMOD_HARDWARE, 0, &mySound);
							FMOD_System_PlaySound(mySystem, FMOD_CHANNEL_FREE, mySound, false, 0);

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
}

long mycountertimer = 0;
chrono::steady_clock::time_point mytimetimer = chrono::steady_clock::now();

void myTimer(int value) {
	mycountertimer++;
	if (isElapsed(mytimetimer, 1000)) {
		cout << "myTimer(): " << mycountertimer << endl;
		mycountertimer = 0;
		mytimetimer = chrono::steady_clock::now();
	}

	if (!pause) {
		if (!arioIsAlive) {
			arioDeadCounter++;
		}

		if (!enemyIsAlive) {
			enemyDeadCounter++;
		}

		if (!arioInControl) {
			arioInControlCounter++;
		}

		if (arioInvisible) {
			arioInvisibleCounter++;
		}

		if (!enemyInControl) {
			enemyInControlCounter++;
		}

		if (enemyInvisible) {
			enemyInvisibleCounter++;
		}

		if (arioIsAlive && !arioInControl) {
			arioY += 4;
		}

		if (enemyIsAlive && !enemyInControl) {
			enemyY -= 4;
		}

		// ario shake
		if (arioShake) {
			arioShakeCounter++;
		}
		if (arioShakeCounter == 4) {
			arioY += 3;
			arioShakeCounter = 0;
			arioShake = false;
		}

		// enemy shake
		if (enemyShake) {
			enemyShakeCounter++;
		}
		if (enemyShakeCounter == 4) {
			enemyY -= 3;
			enemyShakeCounter = 0;
			enemyShake = false;
		}

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
	}

	glutTimerFunc(20, myTimer, 1);
}

void myPassiveMotion(int x, int y) {
	if (arioIsAlive && arioInControl) {
		arioX = x;
		arioY = (height - y);

		glutPostRedisplay();
	}
}

void myMotion(int x, int y) {
	if (arioIsAlive && arioInControl) {
		arioX = x;
		arioY = (height - y);

		glutPostRedisplay();
	}
}

void myReshape(int w, int h) {
	GLfloat aspectRatio;

	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	aspectRatio = (GLfloat) w / (GLfloat) h;

	if (w <= h)
		glOrtho(0, width, 0 / aspectRatio, height / aspectRatio, 0, 100);

	else
		glOrtho(0 * aspectRatio, width * aspectRatio, 0, height, 0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_F4) {
		exit(0);
	}
}

int main(int argc, char* argv[]) {

	//cin.get();

	FMOD_System_Create(&mySystem);
	FMOD_System_Init(mySystem, 32, FMOD_INIT_NORMAL, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	glutCreateWindow("Ario 0.0.1");
	glutSetCursor(GLUT_CURSOR_NONE);

	glutFullScreen();
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutTimerFunc(20, myTimer, 1);
	glutPassiveMotionFunc(myPassiveMotion);
	glutMotionFunc(myMotion);
	//glutReshapeFunc(myReshape);
	glutKeyboardFunc(myKeyboard);
	glutKeyboardUpFunc(myKeyboardUp);
	glutIdleFunc(myIdle);
	glOrtho(0, width, 0, height, 0, 100);

	glutSpecialFunc(mySpecial);

	glutMainLoop();
	return 0;
}
