#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#include<random>
float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLvoid glColor3f(GLfloat r, GLfloat g, GLfloat b);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
float r = 0.1, g = 0.1, b = 0.1;
int h = 0, r2 = 0;
int  cnt_move2 = 0, cnt_move = 0, cnt_color = 0, cnt_size = 0;
bool togle = false, togle2 = false,togle_size=false,togle_color = false;
struct rect {
	float x;
	float y;
	float r, g, b;
	float first_x;
	float first_y;
	float first_r, first_g, first_b;
	float size_x;
	float size_y;
	float first_size_x;
	float first_size_y;
	int cnt = 0, cnt1 = 0;
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
	glutTimerFunc(17, TimerFunction, 1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	if (h > 0) {
		for (int i = 0;i < h;i++) {
			glColor3f(re[i].r, re[i].g, re[i].b);
			glRectf(re[i].x - re[i].size_x, re[i].y - re[i].size_y, re[i].x + re[i].size_x, re[i].y + re[i].size_y);
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
	srand(time(NULL));
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //���� ��ư ���� ���
		if (h > 4) {
			return;
		}
		random_device rd; //���� ��ġ ����
		default_random_engine dre(rd()); //���õ� ��ġ�� �������� ������
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
		re[h].x = re[h].first_x = mx;
		re[h].y = re[h].first_y = my;
		uniform_real_distribution<float> uSize(0, 0.5); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
		re[h].size_x = re[h].first_size_x = uSize(dre);
		re[h].size_y = re[h].first_size_y = uSize(dre);
		uniform_real_distribution<float> uColor(0, 1); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
		re[h].r = re[h].first_r = uColor(dre);
		re[h].g = re[h].first_g = uColor(dre);
		re[h].b = re[h].first_b = uColor(dre);
		h++;
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	srand(time(NULL));
	random_device rd; //���� ��ġ ����
	default_random_engine dre(rd()); //���õ� ��ġ�� �������� ������
	uniform_real_distribution<float> uSize(0, 0.5); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
	uniform_real_distribution<float> uColor(0, 1); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
	switch (key) {
	case 'c':
		cnt_size++;
		if (cnt_size % 2 == 1) {
			togle_size = true;
		}
		else {
			togle_size = false;
		}
		break; //--- ���� �簢�� ����
	case 'o':
		cnt_color++;
		if (cnt_color % 2 == 1) {
			togle_color = true;
		}
		else {
			togle_color = false;
		}
		break;
	case 'a':
		cnt_move++;
		if (cnt_move % 2 == 1) {
			togle2 = true;
		}
		else {
			togle2 = false;
		}
		break;
	case 'i':
		cnt_move2++;
		if (cnt_move2 % 2 == 1) {
			togle = true;
		}
		else {
			togle = false;
			for (int i = 0;i < 5;i++) {
				re[i].cnt = 0;
				re[i].cnt1 = 0;
			}
		}
		break;
	case 'm':
		for (int i = 0;i < 5;i++) {
			re[i].x = re[i].first_x;
			re[i].y = re[i].first_y;
		}
		break;
	case 's':
		togle = false;
		togle2 = false;
		togle_size = false;
		togle_color = false;
		for (int i = 0;i < 5;i++) {
			re[i].cnt = 0;
			re[i].cnt1 = 0;
		}
		break;
	case 'r':
		h = 0;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}
GLvoid TimerFunction(int value) { //Ÿ�̸� �ȿ��� switch�� �̻ڴ�
	random_device rd; //���� ��ġ ����
	default_random_engine dre(rd()); //���õ� ��ġ�� �������� ������
	uniform_real_distribution<float> uSize(0, 0.5); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
	uniform_real_distribution<float> uColor(0, 1); //�־��� ���� �ȿ��� ������, �ߺ� �ȵǰ�
	if (togle) {
		for (int i = 0;i < 5;i++) {
			if (re[i].cnt % 2 == 0) {
				re[i].x += 0.1f;
				if (re[i].x + re[i].size_x >= 1) {
					if (re[i].cnt1 % 2 == 0) {
						re[i].y -= 0.1f;
						if (re[i].y - re[i].size_y <= -1) {
							re[i].cnt1++;
						}
					}
					else {
						re[i].y += 0.1f;
						if (re[i].y + re[i].size_y >= +1) {
							re[i].cnt1++;
						}
					}
					re[i].cnt++;
				}
			}
			else {
				re[i].x -= 0.1f;
				if (re[i].x - re[i].size_x <= -1) {
					if (re[i].cnt1 % 2 == 0) {
						re[i].y -= 0.1f;
						if (re[i].y - re[i].size_y <= -1) {
							re[i].cnt1++;
						}
					}
					else {
						re[i].y += 0.1f;
						if (re[i].y + re[i].size_y >= +1) {
							re[i].cnt1++;
						}
					}
					re[i].cnt++;
				}
			}
		}
	}
	if (togle2) {
		for (int i = 0;i < 5;i++) {
			if (re[i].cnt % 2 == 0) {
				re[i].x += 0.1f;
				if (re[i].x + re[i].size_x >= 1) {
					re[i].cnt++;
				}
				if (re[i].cnt1 % 2 == 0) {
					re[i].y -= 0.1f;
					if (re[i].y - re[i].size_y <= -1) {
						re[i].cnt1++;
					}
				}
				else {
					re[i].y += 0.1f;
					if (re[i].y + re[i].size_y >= +1) {
						re[i].cnt1++;
					}
				}
			}
			else {
				re[i].x -= 0.1f;
				if (re[i].x - re[i].size_x <= -1) {
					re[i].cnt++;
				}
				if (re[i].cnt1 % 2 == 0) {
					re[i].y -= 0.1f;
					if (re[i].y - re[i].size_y <= -1) {
						re[i].cnt1++;
					}
				}
				else {
					re[i].y += 0.1f;
					if (re[i].y + re[i].size_y >= +1) {
						re[i].cnt1++;
					}
				}
			}
		}
	}
	if (togle_color) {
		for (int i = 0;i < 5;i++) {
			re[i].r = uColor(dre);
			re[i].g = uColor(dre);
			re[i].b = uColor(dre);
		}
	}
	if (togle_size) {
		for (int i = 0;i < 5;i++) {
			re[i].size_x = uSize(dre);
			re[i].size_y = uSize(dre);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(17, TimerFunction, 1);
}
//enum ����
enum day { //Ÿ�̸ӿ��� 
	sunday=-2,
	monday,
	tuesday,
	saturday
};