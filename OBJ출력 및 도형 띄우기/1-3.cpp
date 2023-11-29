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
bool drag;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLvoid glColor3f(GLfloat r, GLfloat g, GLfloat b);
GLvoid Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
float r = 0.2, g = 0.4, b = 0.6;
int h = 0;
int r2 = 0;
int who = -1;
struct rect {
	float x;
	float y;
	float r, g, b;
};
struct rect re[5];
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
	if (h > 0) {
		for (int i = 0;i < h;i++) {
			glColor3f(re[i].r, re[i].g, re[i].b);
			glRectf(re[i].x - 0.1, re[i].y - 0.1, re[i].x + 0.1, re[i].y + 0.1);
		}
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
		float my = - (y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
		mouse_x = mx;
		mouse_y = my;
		for (int i = 0;i < h;i++) {
			if (re[i].x - 0.1f < mx && mx < re[i].x + 0.1f) { //x
				if (re[i].y - 0.1f < my && my < re[i].y + 0.1f) { //y
					who = i;
					drag = true;
				}
			}
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		drag = false;
	}
}
GLvoid Motion(int x, int y) {
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	if (drag) {
		if (mouse_x > mx) {
			re[who].x -= mouse_x - mx;
		}
		if (mouse_x < mx) {
			re[who].x += mx - mouse_x;
		}
		if (mouse_y > my) {
			re[who].y -= mouse_y - my;
		}
		if (mouse_y < my) {
			re[who].y += my - mouse_y;
		}
		mouse_x = mx;
		mouse_y = my;
		glutPostRedisplay();
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		srand(time(NULL));
		if (h > 4) {
			break;
		}
		random_device rd; //���� ��ġ ����
		default_random_engine dre(rd()); //���õ� ��ġ�� �������� ������
		uniform_real_distribution<float> uColor(0, 1); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
		re[h].r = uColor(dre);
		re[h].g = uColor(dre);
		re[h].b = uColor(dre);
		uniform_real_distribution<float> uPos(-1, 1); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
		re[h].x = uPos(dre);
		re[h].y = uPos(dre);
		h++;
		break; //--- ���� �簢�� ����
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}