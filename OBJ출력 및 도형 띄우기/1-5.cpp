#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#include<random>
float WIDTH = 800;
float HEIGHT = 600;
float mouse_x = 0;
float mouse_y = 0;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLvoid glColor3f(GLfloat r, GLfloat g, GLfloat b);
GLvoid Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
float r = 1, g = 1, b = 1;
bool drag;
int r2 = 0;
int who = -1;
struct rect {
	float x;
	float y;
	float r, g, b;
	float size = 0.04;
	int exist = 0;
};
struct rect re[30];
struct rect eraser[1];
bool Rectin(float lx, float rx, float ly,float ry, float x, float y,float x1,float y1)
{
	if ((lx < x && x < rx) && (ly < y && y < ry)) {
		return true;
	}
	else if ((lx < x1 && x1 < rx) && (ly < y && y < ry)) {
		return true;
	}
	else if ((lx < x1 && x1 < rx) && (ly < y1 && y1 < ry)) {
		return true;
	}
	else if ((lx < x && x < rx) && (ly < y1 && y1 < ry)) {
		return true;
	}

	else if ((x < lx && lx < x1) && (y < ly && ly < y1)) {
		return true;
	}
	else if ((x < lx && lx < x1) && (y < ry && ry < y1)) {
		return true;
	}
	else if ((x < rx && rx < x1) && (y < ly && ly < y1)) {
		return true;
	}
	else if ((x < rx && rx < x1) && (y < ry && ry < y1)) {
		return true;
	}
	else return false;
}
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(WIDTH, HEIGHT); // �������� ũ�� ����
	glutCreateWindow("OPENGL"); // ������ ����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized" << std::endl;
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	if (r2 == 1) {
		for (int i = 0;i < 30;i++) {
			if (re[i].exist == 0) {
				glColor3f(re[i].r, re[i].g, re[i].b);
				glRectf(re[i].x - 0.02, re[i].y - 0.02, re[i].x + 0.02, re[i].y + 0.02);
			}
		}
	}
	if (drag) {
		glColor3f(eraser[0].r, eraser[0].g, eraser[0].b);
		glRectf(eraser[0].x - eraser[0].size, eraser[0].y - eraser[0].size, eraser[0].x + eraser[0].size, eraser[0].y + eraser[0].size);
	}
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //���� ��ư ���� ���

		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
		mouse_x = mx;
		mouse_y = my;
		eraser[0].x = mx;
		eraser[0].y = my;
		eraser[0].size = 0.04;
		drag = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		eraser[0].size = 0.04;
		eraser[0].r = eraser[0].g = eraser[0].b = 0;
		drag = false;
	}
}
GLvoid Motion(int x, int y) {
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	if (drag) {
		if (mouse_x > mx) {
			eraser[0].x -= mouse_x - mx;
		}
		if (mouse_x < mx) {
			eraser[0].x += mx - mouse_x;
		}
		if (mouse_y > my) {
			eraser[0].y -= mouse_y - my;
		}
		if (mouse_y < my) {
			eraser[0].y += my - mouse_y;
		}
		mouse_x = mx;
		mouse_y = my;
		glutPostRedisplay();
	}
	for (int i = 0;i < 30;i++) {
		if (re[i].exist == 0) {
			if (Rectin(re[i].x - 0.02, re[i].x + 0.02, re[i].y - 0.02, re[i].y + 0.02, eraser[0].x - eraser[0].size, eraser[0].y - eraser[0].size, eraser[0].x + eraser[0].size, eraser[0].y + eraser[0].size)) {
				eraser[0].size += 0.01;
				eraser[0].r = eraser[0].r + re[i].r;
				eraser[0].g = eraser[0].g + re[i].g;
				eraser[0].b = eraser[0].b + re[i].b;
				if (eraser[0].r >= 1) {
					eraser[0].r = 0.9f;
				}
				if (eraser[0].g >= 1) {
					eraser[0].g = 0.9f;
				}
				if (eraser[0].b >= 1) {
					eraser[0].b = 0.9f;
				}
				re[i].exist = 1;
				break;
			}
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		r2 = 1;
		srand(time(NULL));
		random_device rd; //���� ��ġ ����
		default_random_engine dre(rd()); //���õ� ��ġ�� �������� ������
		for (int i = 0;i < 30;i++) {
			uniform_real_distribution<float> uColor(0, 1); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
			re[i].r = uColor(dre);
			re[i].g = uColor(dre);
			re[i].b = uColor(dre);
			uniform_real_distribution<float> uPos(-0.9, 0.9); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
			re[i].x = uPos(dre);
			re[i].y = uPos(dre);
			re[i].exist = 0;
		}
		break; //--- ���� �簢�� ����
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}