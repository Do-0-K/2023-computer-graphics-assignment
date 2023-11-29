#include <gl/glew.h> //--- 필요한 헤더파일 include
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
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
float r = 0.1, g = 0.1, b = 0.1;
int who = -1, r2 = 0;
struct rect {
	float x;
	float y;
	float r, g, b;
	float size = 0.2;
	int exist = 0;
	int div = -1;
	bool chosen;
};
struct rect re[5];

struct rect re11[4];
struct rect re12[4];
struct rect re13[8];

struct rect re21[4];
struct rect re22[4];
struct rect re23[8];

struct rect re31[4];
struct rect re32[4];
struct rect re33[8];

struct rect re41[4];
struct rect re42[4];
struct rect re43[8];

struct rect re51[4];
struct rect re52[4];
struct rect re53[8];

enum TIMER_CASE {
	RENDER,
	RECT_ANI
};

void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("OPENGL"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized" << std::endl;
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1, TimerFunction, RENDER);
	glutTimerFunc(1, TimerFunction, RECT_ANI);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	if (r2 == 1) {
		for (int i = 0;i < 5;i++) {
			if (re[i].exist == 0) {
				glColor3f(re[i].r, re[i].g, re[i].b);
				glRectf(re[i].x - re[i].size, re[i].y - re[i].size, re[i].x + re[i].size, re[i].y + re[i].size);
			}
		}
	}
	if (re[0].chosen) {
		if (re[0].div == 0) {
			for (int i = 0;i < 4;i++) {
				if (re11[i].exist == 0) {
					glColor3f(re11[i].r, re11[i].g, re11[i].b);
					glRectf(re11[i].x - re11[i].size, re11[i].y - re11[i].size, re11[i].x + re11[i].size, re11[i].y + re11[i].size);
				}
			}
		}
		if (re[0].div == 1) {
			for (int i = 0;i < 4;i++) {
				if (re12[i].exist == 0) {
					glColor3f(re12[i].r, re12[i].g, re12[i].b);
					glRectf(re12[i].x - re12[i].size, re12[i].y - re12[i].size, re12[i].x + re12[i].size, re12[i].y + re12[i].size);
				}
			}
		}
		if (re[0].div == 2) {
			for (int i = 0;i < 8;i++) {
				if (re13[i].exist == 0) {
					glColor3f(re13[i].r, re13[i].g, re13[i].b);
					glRectf(re13[i].x - re13[i].size, re13[i].y - re13[i].size, re13[i].x + re13[i].size, re13[i].y + re13[i].size);
				}
			}
		}
	}
	if (re[1].chosen) {
		if (re[1].div == 0) {
			for (int i = 0;i < 4;i++) {
				if (re21[i].exist == 0) {
					glColor3f(re21[i].r, re21[i].g, re21[i].b);
					glRectf(re21[i].x - re21[i].size, re21[i].y - re21[i].size, re21[i].x + re21[i].size, re21[i].y + re21[i].size);
				}
			}
		}
		if (re[1].div == 1) {
			for (int i = 0;i < 4;i++) {
				if (re22[i].exist == 0) {
					glColor3f(re22[i].r, re22[i].g, re22[i].b);
					glRectf(re22[i].x - re22[i].size, re22[i].y - re22[i].size, re22[i].x + re22[i].size, re22[i].y + re22[i].size);
				}
			}
		}
		if (re[1].div == 2) {
			for (int i = 0;i < 8;i++) {
				if (re23[i].exist == 0) {
					glColor3f(re23[i].r, re23[i].g, re23[i].b);
					glRectf(re23[i].x - re23[i].size, re23[i].y - re23[i].size, re23[i].x + re23[i].size, re23[i].y + re23[i].size);
				}
			}
		}
	}
	if (re[2].chosen) {
		if (re[2].div == 0) {
			for (int i = 0;i < 4;i++) {
				if (re31[i].exist == 0) {
					glColor3f(re31[i].r, re31[i].g, re31[i].b);
					glRectf(re31[i].x - re31[i].size, re31[i].y - re31[i].size, re31[i].x + re31[i].size, re31[i].y + re31[i].size);
				}
			}
		}
		if (re[2].div == 1) {
			for (int i = 0;i < 4;i++) {
				if (re32[i].exist == 0) {
					glColor3f(re32[i].r, re32[i].g, re32[i].b);
					glRectf(re32[i].x - re32[i].size, re32[i].y - re32[i].size, re32[i].x + re32[i].size, re32[i].y + re32[i].size);
				}
			}
		}
		if (re[2].div == 2) {
			for (int i = 0;i < 8;i++) {
				if (re33[i].exist == 0) {
					glColor3f(re33[i].r, re33[i].g, re33[i].b);
					glRectf(re33[i].x - re33[i].size, re33[i].y - re33[i].size, re33[i].x + re33[i].size, re33[i].y + re33[i].size);
				}
			}
		}
	}
	if (re[3].chosen) {
		if (re[3].div == 0) {
			for (int i = 0;i < 4;i++) {
				if (re41[i].exist == 0) {
					glColor3f(re41[i].r, re41[i].g, re41[i].b);
					glRectf(re41[i].x - re41[i].size, re41[i].y - re41[i].size, re41[i].x + re41[i].size, re41[i].y + re41[i].size);
				}
			}
		}
		if (re[3].div == 1) {
			for (int i = 0;i < 4;i++) {
				if (re42[i].exist == 0) {
					glColor3f(re42[i].r, re42[i].g, re42[i].b);
					glRectf(re42[i].x - re42[i].size, re42[i].y - re42[i].size, re42[i].x + re42[i].size, re42[i].y + re42[i].size);
				}
			}
		}
		if (re[3].div == 2) {
			for (int i = 0;i < 8;i++) {
				if (re43[i].exist == 0) {
					glColor3f(re43[i].r, re43[i].g, re43[i].b);
					glRectf(re43[i].x - re43[i].size, re43[i].y - re43[i].size, re43[i].x + re43[i].size, re43[i].y + re43[i].size);
				}
			}
		}
	}
	if (re[4].chosen) {
		if (re[4].div == 0) {
			for (int i = 0;i < 4;i++) {
				if (re51[i].exist == 0) {
					glColor3f(re51[i].r, re51[i].g, re51[i].b);
					glRectf(re51[i].x - re51[i].size, re51[i].y - re51[i].size, re51[i].x + re51[i].size, re51[i].y + re51[i].size);
				}
			}
		}
		if (re[4].div == 1) {
			for (int i = 0;i < 4;i++) {
				if (re52[i].exist == 0) {
					glColor3f(re52[i].r, re52[i].g, re52[i].b);
					glRectf(re52[i].x - re52[i].size, re52[i].y - re52[i].size, re52[i].x + re52[i].size, re52[i].y + re52[i].size);
				}
			}
		}
		if (re[4].div == 2) {
			for (int i = 0;i < 8;i++) {
				if (re53[i].exist == 0) {
					glColor3f(re53[i].r, re53[i].g, re53[i].b);
					glRectf(re53[i].x - re53[i].size, re53[i].y - re53[i].size, re53[i].x + re53[i].size, re53[i].y + re53[i].size);
				}
			}
		}
	}
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
GLvoid Mouse(int button, int state, int x, int y) {
	srand(time(NULL));
	random_device rd; //랜덤 장치 세팅
	default_random_engine dre(rd()); //세팅된 장치를 엔진으로 돌린다
	uniform_int_distribution<int> uDiv(0, 2); //주어진 범위 안에서 돌린다, 중복 안되게
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //왼쪽 버튼 누른 경우
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
		mouse_x = mx;
		mouse_y = my;
		for (int i = 4;i > -1;i--) {
			if (re[i].exist == 0) {
				if (re[i].x - 0.2 < mx && mx < re[i].x + 0.2) { //x
					if (re[i].y - 0.2 < my && my < re[i].y + 0.2) { //y
						who = i;
						re[i].exist = 1;
						re[i].chosen = true;
						break;
					}
				}
			}
		}
		re[who].div = uDiv(dre);
		switch (re[who].div) {
		case 0:
			switch (who) {
			case 0:
				re11[0].x = re[who].x - 0.05;
				re11[0].y = re[who].y - 0.05;
				re11[1].x = re[who].x + 0.05;
				re11[1].y = re[who].y - 0.05;
				re11[2].x = re[who].x - 0.05;
				re11[2].y = re[who].y + 0.05;
				re11[3].x = re[who].x + 0.05;
				re11[3].y = re[who].y + 0.05;
				for (int i = 0;i < 4;i++) {
					re11[i].exist = 0;
					re11[i].r = re[who].r;
					re11[i].g = re[who].g;
					re11[i].b = re[who].b;
					re11[i].size = 0.1;
				}
				break;
			case 1:
				re21[0].x = re[who].x - 0.05;
				re21[0].y = re[who].y - 0.05;
				re21[1].x = re[who].x + 0.05;
				re21[1].y = re[who].y - 0.05;
				re21[2].x = re[who].x - 0.05;
				re21[2].y = re[who].y + 0.05;
				re21[3].x = re[who].x + 0.05;
				re21[3].y = re[who].y + 0.05;
				for (int i = 0;i < 4;i++) {
					re21[i].exist = 0;
					re21[i].r = re[who].r;
					re21[i].g = re[who].g;
					re21[i].b = re[who].b;
					re21[i].size = 0.1;
				}
				break;
			case 2:
				re31[0].x = re[who].x - 0.05;
				re31[0].y = re[who].y - 0.05;
				re31[1].x = re[who].x + 0.05;
				re31[1].y = re[who].y - 0.05;
				re31[2].x = re[who].x - 0.05;
				re31[2].y = re[who].y + 0.05;
				re31[3].x = re[who].x + 0.05;
				re31[3].y = re[who].y + 0.05;
				for (int i = 0;i < 4;i++) {
					re31[i].exist = 0;
					re31[i].r = re[who].r;
					re31[i].g = re[who].g;
					re31[i].b = re[who].b;
					re31[i].size = 0.1;
				}
				break;
			case 3:
				re41[0].x = re[who].x - 0.05;
				re41[0].y = re[who].y - 0.05;
				re41[1].x = re[who].x + 0.05;
				re41[1].y = re[who].y - 0.05;
				re41[2].x = re[who].x - 0.05;
				re41[2].y = re[who].y + 0.05;
				re41[3].x = re[who].x + 0.05;
				re41[3].y = re[who].y + 0.05;
				for (int i = 0;i < 4;i++) {
					re41[i].exist = 0;
					re41[i].r = re[who].r;
					re41[i].g = re[who].g;
					re41[i].b = re[who].b;
					re41[i].size = 0.1;
				}
				break;
			case 4:
				re51[0].x = re[who].x - 0.05;
				re51[0].y = re[who].y - 0.05;
				re51[1].x = re[who].x + 0.05;
				re51[1].y = re[who].y - 0.05;
				re51[2].x = re[who].x - 0.05;
				re51[2].y = re[who].y + 0.05;
				re51[3].x = re[who].x + 0.05;
				re51[3].y = re[who].y + 0.05;
				for (int i = 0;i < 4;i++) {
					re51[i].exist = 0;
					re51[i].r = re[who].r;
					re51[i].g = re[who].g;
					re51[i].b = re[who].b;
					re51[i].size = 0.1;
				}
				break;
			}
			break;
		case 1:
			switch (who) {
			case 0:
				re12[0].x = re[who].x - 0.1;
				re12[0].y = re[who].y - 0.1;
				re12[1].x = re[who].x + 0.1;
				re12[1].y = re[who].y - 0.1;
				re12[2].x = re[who].x - 0.1;
				re12[2].y = re[who].y + 0.1;
				re12[3].x = re[who].x + 0.1;
				re12[3].y = re[who].y + 0.1;
				for (int i = 0;i < 4;i++) {
					re12[i].exist = 0;
					re12[i].r = re[who].r;
					re12[i].g = re[who].g;
					re12[i].b = re[who].b;
					re12[i].size = 0.1;
				}
				break;
			case 1:
				re22[0].x = re[who].x - 0.1;
				re22[0].y = re[who].y - 0.1;
				re22[1].x = re[who].x + 0.1;
				re22[1].y = re[who].y - 0.1;
				re22[2].x = re[who].x - 0.1;
				re22[2].y = re[who].y + 0.1;
				re22[3].x = re[who].x + 0.1;
				re22[3].y = re[who].y + 0.1;
				for (int i = 0;i < 4;i++) {
					re22[i].exist = 0;
					re22[i].r = re[who].r;
					re22[i].g = re[who].g;
					re22[i].b = re[who].b;
					re22[i].size = 0.1;
				}
				break;
			case 2:
				re32[0].x = re[who].x - 0.1;
				re32[0].y = re[who].y - 0.1;
				re32[1].x = re[who].x + 0.1;
				re32[1].y = re[who].y - 0.1;
				re32[2].x = re[who].x - 0.1;
				re32[2].y = re[who].y + 0.1;
				re32[3].x = re[who].x + 0.1;
				re32[3].y = re[who].y + 0.1;
				for (int i = 0;i < 4;i++) {
					re32[i].exist = 0;
					re32[i].r = re[who].r;
					re32[i].g = re[who].g;
					re32[i].b = re[who].b;
					re32[i].size = 0.1;
				}
				break;
			case 3:
				re42[0].x = re[who].x - 0.1;
				re42[0].y = re[who].y - 0.1;
				re42[1].x = re[who].x + 0.1;
				re42[1].y = re[who].y - 0.1;
				re42[2].x = re[who].x - 0.1;
				re42[2].y = re[who].y + 0.1;
				re42[3].x = re[who].x + 0.1;
				re42[3].y = re[who].y + 0.1;
				for (int i = 0;i < 4;i++) {
					re42[i].exist = 0;
					re42[i].r = re[who].r;
					re42[i].g = re[who].g;
					re42[i].b = re[who].b;
					re42[i].size = 0.1;
				}
				break;
			case 4:
				re52[0].x = re[who].x - 0.1;
				re52[0].y = re[who].y - 0.1;
				re52[1].x = re[who].x + 0.1;
				re52[1].y = re[who].y - 0.1;
				re52[2].x = re[who].x - 0.1;
				re52[2].y = re[who].y + 0.1;
				re52[3].x = re[who].x + 0.1;
				re52[3].y = re[who].y + 0.1;
				for (int i = 0;i < 4;i++) {
					re52[i].exist = 0;
					re52[i].r = re[who].r;
					re52[i].g = re[who].g;
					re52[i].b = re[who].b;
					re52[i].size = 0.1;
				}
				break;
			}
			break;
		case 2:
			switch (who) {
			case 0:
				re13[0].x = re[who].x - 0.1;
				re13[0].y = re[who].y + 0.134;
				re13[1].x = re[who].x - 0.1;
				re13[1].y = re[who].y;
				re13[2].x = re[who].x + 0.1;
				re13[2].y = re[who].y;
				re13[3].x = re[who].x + 0.1;
				re13[3].y = re[who].y - 0.134;
				re13[4].x = re[who].x - 0.15;
				re13[4].y = re[who].y - 0.134;
				re13[5].x = re[who].x - 0.1;
				re13[5].y = re[who].y - 0.134;
				re13[6].x = re[who].x + 0.1;
				re13[6].y = re[who].y + 0.134;
				re13[7].x = re[who].x + 0.15;
				re13[7].y = re[who].y + 0.134;
				for (int i = 0;i < 8;i++) {
					re13[i].exist = 0;
					re13[i].r = re[who].r;
					re13[i].g = re[who].g;
					re13[i].b = re[who].b;
					re13[i].size = 0.1;
				}
				break;
			case 1:
				re23[0].x = re[who].x - 0.1f;
				re23[0].y = re[who].y + 0.134f;
				re23[1].x = re[who].x - 0.1f;
				re23[1].y = re[who].y;
				re23[2].x = re[who].x + 0.1f;
				re23[2].y = re[who].y;
				re23[3].x = re[who].x + 0.1f;
				re23[3].y = re[who].y - 0.134f;
				re23[4].x = re[who].x - 0.15f;
				re23[4].y = re[who].y - 0.134f;
				re23[5].x = re[who].x - 0.1f;
				re23[5].y = re[who].y - 0.134f;
				re23[6].x = re[who].x + 0.1f;
				re23[6].y = re[who].y + 0.134f;
				re23[7].x = re[who].x + 0.15f;
				re23[7].y = re[who].y + 0.134f;
				for (int i = 0;i < 8;i++) {
					re23[i].exist = 0;
					re23[i].r = re[who].r;
					re23[i].g = re[who].g;
					re23[i].b = re[who].b;
					re23[i].size = 0.1;
				}
				break;
			case 2:
				re33[0].x = re[who].x - 0.1f;
				re33[0].y = re[who].y + 0.134f;
				re33[1].x = re[who].x - 0.1f;
				re33[1].y = re[who].y;
				re33[2].x = re[who].x + 0.1f;
				re33[2].y = re[who].y;
				re33[3].x = re[who].x + 0.1f;
				re33[3].y = re[who].y - 0.134f;
				re33[4].x = re[who].x - 0.15f;
				re33[4].y = re[who].y - 0.134f;
				re33[5].x = re[who].x - 0.1f;
				re33[5].y = re[who].y - 0.134f;
				re33[6].x = re[who].x + 0.1f;
				re33[6].y = re[who].y + 0.134f;
				re33[7].x = re[who].x + 0.15f;
				re33[7].y = re[who].y + 0.134f;
				for (int i = 0;i < 8;i++) {
					re33[i].exist = 0;
					re33[i].r = re[who].r;
					re33[i].g = re[who].g;
					re33[i].b = re[who].b;
					re33[i].size = 0.1;
				}
				break;
			case 3:
				re43[0].x = re[who].x - 0.1f;
				re43[0].y = re[who].y + 0.134f;
				re43[1].x = re[who].x - 0.1f;
				re43[1].y = re[who].y;
				re43[2].x = re[who].x + 0.1f;
				re43[2].y = re[who].y;
				re43[3].x = re[who].x + 0.1f;
				re43[3].y = re[who].y - 0.134f;
				re43[4].x = re[who].x - 0.15f;
				re43[4].y = re[who].y - 0.134f;
				re43[5].x = re[who].x - 0.1f;
				re43[5].y = re[who].y - 0.134f;
				re43[6].x = re[who].x + 0.1f;
				re43[6].y = re[who].y + 0.134f;
				re43[7].x = re[who].x + 0.15f;
				re43[7].y = re[who].y + 0.134f;
				for (int i = 0;i < 8;i++) {
					re43[i].exist = 0;
					re43[i].r = re[who].r;
					re43[i].g = re[who].g;
					re43[i].b = re[who].b;
					re43[i].size = 0.1;
				}
				break;
			case 4:
				re53[0].x = re[who].x - 0.1f;
				re53[0].y = re[who].y + 0.134f;
				re53[1].x = re[who].x - 0.1f;
				re53[1].y = re[who].y;
				re53[2].x = re[who].x + 0.1f;
				re53[2].y = re[who].y;
				re53[3].x = re[who].x + 0.1f;
				re53[3].y = re[who].y - 0.134f;
				re53[4].x = re[who].x - 0.15f;
				re53[4].y = re[who].y - 0.134f;
				re53[5].x = re[who].x - 0.1f;
				re53[5].y = re[who].y - 0.134f;
				re53[6].x = re[who].x + 0.1f;
				re53[6].y = re[who].y + 0.134f;
				re53[7].x = re[who].x + 0.15f;
				re53[7].y = re[who].y + 0.134f;
				for (int i = 0;i < 8;i++) {
					re53[i].exist = 0;
					re53[i].r = re[who].r;
					re53[i].g = re[who].g;
					re53[i].b = re[who].b;
					re53[i].size = 0.1;
				}
				break;
			}
			break;
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	srand(time(NULL));
	random_device rd; //랜덤 장치 세팅
	default_random_engine dre(rd()); //세팅된 장치를 엔진으로 돌린다
	uniform_real_distribution<float> uSize(-0.8, 0.8); //주어진 범위 안에서 돌린다, 중복 안되게
	uniform_real_distribution<float> uColor(0, 1); //주어진 범위 안에서 돌린다, 중복 안되게
	switch (key) {
	case 'r':
		r2 = 1;
		for (int i = 0;i < 5;i++) {
			re[i].x = uSize(dre);
			re[i].y = uSize(dre);
			re[i].r = uColor(dre);
			re[i].g = uColor(dre);
			re[i].b = uColor(dre);
			re[i].exist = 0;
		}
		break; //--- 랜덤 사각형 생성
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
GLvoid TimerFunction(int value) { //타이머 안에는 switch가 이쁘다
	switch (value){
	case RENDER:{
		glutPostRedisplay();
		glutTimerFunc(17, TimerFunction, RENDER);
	}
	break;
	case RECT_ANI:{
		if (re[0].chosen) {
			if (re[0].div == 0) {
				re11[0].x -= 0.02;
				re11[0].y -= 0.02;
				re11[0].size -= 0.007;
				if (re11[0].size <= 0) {
					re11[0].exist = 1;
				}
				re11[1].x += 0.02;
				re11[1].y -= 0.02;
				re11[1].size -= 0.007;
				if (re11[1].size <= 0) {
					re11[1].exist = 1;
				}
				re11[2].x -= 0.02;
				re11[2].y += 0.02;
				re11[2].size -= 0.007;
				if (re11[2].size <= 0) {
					re11[2].exist = 1;
				}
				re11[3].x += 0.02;
				re11[3].y += 0.02;
				re11[3].size -= 0.007;
				if (re11[3].size <= 0) {
					re11[3].exist = 1;
					re[0].chosen = false;
				}
			}
			else if (re[0].div == 1) {
				re12[0].y -= 0.02;
				re12[0].size -= 0.007;
				if (re12[0].size <= 0) {
					re12[0].exist = 1;
				}
				re12[1].x += 0.02;
				re12[1].size -= 0.007;
				if (re12[1].size <= 0) {
					re12[1].exist = 1;
				}
				re12[2].x -= 0.02;
				re12[2].size -= 0.007;
				if (re12[2].size <= 0) {
					re12[2].exist = 1;
				}
				re12[3].y += 0.02;
				re12[3].size -= 0.007;
				if (re12[3].size <= 0) {
					re12[3].exist = 1;
					re[0].chosen = false;
				}
			}
			else if (re[0].div == 2) {
				re13[0].x -= 0.02;
				re13[0].y += 0.02;
				re13[0].size -= 0.007;
				if ((re13[0].x - re13[0].size > re13[0].x + re13[0].size) || (re13[0].y - re13[0].size > re13[0].y + re13[0].size)) {
					re13[0].exist = 1;
				}
				re13[1].x -= 0.02;
				re13[1].size -= 0.007;
				if ((re13[1].x - re13[1].size > re13[1].x + re13[1].size) || (re13[1].y - re13[1].size > re13[1].y + re13[1].size)) {
					re13[1].exist = 1;
				}
				re13[2].x += 0.02;
				re13[2].size -= 0.007;
				if ((re13[2].x - re13[2].size > re13[2].x + re13[2].size) || (re13[2].y - re13[2].size > re13[2].y + re13[2].size)) {
					re13[2].exist = 1;
				}
				re13[3].x += 0.02;
				re13[3].y -= 0.02;
				re13[3].size -= 0.007;
				if ((re13[3].x - re13[3].size > re13[3].x + re13[3].size) || (re13[3].y - re13[3].size > re13[3].y + re13[2].size)) {
					re13[3].exist = 1;
					re[0].chosen = false;
				}
				re13[4].x -= 0.02;
				re13[4].y -= 0.02;
				re13[4].size -= 0.007;
				if (re13[4].size <= 0) {
					re13[4].exist = 1;
				}
				re13[5].y -= 0.02;
				re13[5].size -= 0.007;
				if (re13[5].size <= 0) {
					re13[5].exist = 1;
				}
				re13[6].y += 0.02;
				re13[6].size -= 0.007;
				if (re13[6].size <= 0) {
					re13[6].exist = 1;
				}
				re13[7].x += 0.02;
				re13[7].y += 0.02;
				re13[7].size -= 0.007;
				if (re13[7].size <= 0) {
					re13[7].exist = 1;
				}
			}
		}
		if (re[1].chosen) {
			if (re[1].div == 0) {
				re21[0].x -= 0.02;
				re21[0].y -= 0.02;
				re21[0].size -= 0.007;
				if (re21[0].size <= 0) {
					re21[0].exist = 1;
				}
				re21[1].x += 0.02;
				re21[1].y -= 0.02;
				re21[1].size -= 0.007;
				if (re21[1].size <= 0) {
					re21[1].exist = 1;
				}
				re21[2].x -= 0.02;
				re21[2].y += 0.02;
				re21[2].size -= 0.007;
				if (re21[2].size <= 0) {
					re21[2].exist = 1;
				}
				re21[3].x += 0.02;
				re21[3].y += 0.02;
				re21[3].size -= 0.007;
				if (re21[3].size <= 0) {
					re21[3].exist = 1;
					re[1].chosen = false;
				}
			}
			else if (re[1].div == 1) {
				re22[0].y -= 0.02;
				re22[0].size -= 0.007;
				if (re22[0].size <= 0) {
					re22[0].exist = 1;
				}
				re22[1].x += 0.02;
				re22[1].size -= 0.007;
				if (re22[1].size <= 0) {
					re22[1].exist = 1;
				}
				re22[2].x -= 0.02;
				re22[2].size -= 0.007;
				if (re22[2].size <= 0) {
					re22[2].exist = 1;
				}
				re22[3].y += 0.02;
				re22[3].size -= 0.007;
				if (re22[3].size <= 0) {
					re22[3].exist = 1;
					re[1].chosen = false;
				}
			}
			else if (re[1].div == 2) {
				re23[0].x -= 0.02;
				re23[0].y += 0.02;
				re23[0].size -= 0.007;
				if ((re23[0].x - re23[0].size > re23[0].x + re23[0].size) || (re23[0].y - re23[0].size > re23[0].y + re23[0].size)) {
					re23[0].exist = 1;
				}
				re23[1].x -= 0.02;
				re23[1].size -= 0.007;
				if ((re23[1].x - re23[1].size > re23[1].x + re23[1].size) || (re23[1].y - re23[1].size > re23[1].y + re23[1].size)) {
					re23[1].exist = 1;
				}
				re23[2].x += 0.02;
				re23[2].size -= 0.007;
				if ((re23[2].x - re23[2].size > re23[2].x + re23[2].size) || (re23[2].y - re23[2].size > re23[2].y + re23[2].size)) {
					re23[2].exist = 1;
				}
				re23[3].x += 0.02;
				re23[3].y -= 0.02;
				re23[3].size -= 0.007;
				if ((re23[3].x - re23[3].size > re23[3].x + re23[3].size) || (re23[3].y - re23[3].size > re23[3].y + re23[2].size)) {
					re23[3].exist = 1;
					re[1].chosen = false;
				}
				re23[4].x -= 0.02;
				re23[4].y -= 0.02;
				re23[4].size -= 0.007;
				if (re23[4].size <= 0) {
					re23[4].exist = 1;
				}
				re23[5].y -= 0.02;
				re23[5].size -= 0.007;
				if (re23[5].size <= 0) {
					re23[5].exist = 1;
				}
				re23[6].y += 0.02;
				re23[6].size -= 0.007;
				if (re23[6].size <= 0) {
					re23[6].exist = 1;
				}
				re23[7].x += 0.02;
				re23[7].y += 0.02;
				re23[7].size -= 0.007;
				if (re23[7].size <= 0) {
					re23[7].exist = 1;
				}
			}
		}
		if (re[2].chosen) {
			if (re[2].div == 0) {
				re31[0].x -= 0.02;
				re31[0].y -= 0.02;
				re31[0].size -= 0.007;
				if (re31[0].size <= 0) {
					re31[0].exist = 1;
				}
				re31[1].x += 0.02;
				re31[1].y -= 0.02;
				re31[1].size -= 0.007;
				if (re31[1].size <= 0) {
					re31[1].exist = 1;
				}
				re31[2].x -= 0.02;
				re31[2].y += 0.02;
				re31[2].size -= 0.007;
				if (re31[2].size <= 0) {
					re31[2].exist = 1;
				}
				re31[3].x += 0.02;
				re31[3].y += 0.02;
				re31[3].size -= 0.007;
				if (re31[3].size <= 0) {
					re31[3].exist = 1;
					re[2].chosen = false;
				}
			}
			else if (re[2].div == 1) {
				re32[0].y -= 0.02;
				re32[0].size -= 0.007;
				if (re32[0].size <= 0) {
					re32[0].exist = 1;
				}
				re32[1].x += 0.02;
				re32[1].size -= 0.007;
				if (re32[1].size <= 0) {
					re32[1].exist = 1;
				}
				re32[2].x -= 0.02;
				re32[2].size -= 0.007;
				if (re32[2].size <= 0) {
					re32[2].exist = 1;
				}
				re32[3].y += 0.02;
				re32[3].size -= 0.007;
				if (re32[3].size <= 0) {
					re32[3].exist = 1;
					re[2].chosen = false;
				}
			}
			else if (re[2].div == 2) {
				re33[0].x -= 0.02;
				re33[0].y += 0.02;
				re33[0].size -= 0.007;
				if ((re33[0].x - re33[0].size > re33[0].x + re33[0].size) || (re33[0].y - re33[0].size > re33[0].y + re33[0].size)) {
					re33[0].exist = 1;
				}
				re33[1].x -= 0.02;
				re33[1].size -= 0.007;
				if ((re33[1].x - re33[1].size > re33[1].x + re33[1].size) || (re33[1].y - re33[1].size > re33[1].y + re33[1].size)) {
					re33[1].exist = 1;
				}
				re33[2].x += 0.02;
				re33[2].size -= 0.007;
				if ((re33[2].x - re33[2].size > re33[2].x + re33[2].size) || (re33[2].y - re33[2].size > re33[2].y + re33[2].size)) {
					re33[2].exist = 1;
				}
				re33[3].x += 0.02;
				re33[3].y -= 0.02;
				re33[3].size -= 0.007;
				if ((re33[3].x - re33[3].size > re33[3].x + re33[3].size) || (re33[3].y - re33[3].size > re33[3].y + re33[2].size)) {
					re33[3].exist = 1;
					re[2].chosen = false;
				}
				re33[4].x -= 0.02;
				re33[4].y -= 0.02;
				re33[4].size -= 0.007;
				if (re33[4].size <= 0) {
					re33[4].exist = 1;
				}
				re33[5].y -= 0.02;
				re33[5].size -= 0.007;
				if (re33[5].size <= 0) {
					re33[5].exist = 1;
				}
				re33[6].y += 0.02;
				re33[6].size -= 0.007;
				if (re33[6].size <= 0) {
					re33[6].exist = 1;
				}
				re33[7].x += 0.02;
				re33[7].y += 0.02;
				re33[7].size -= 0.007;
				if (re33[7].size <= 0) {
					re33[7].exist = 1;
				}
			}
		}
		if (re[3].chosen) {
			if (re[3].div == 0) {
				re41[0].x -= 0.02;
				re41[0].y -= 0.02;
				re41[0].size -= 0.007;
				if (re41[0].size <= 0) {
					re41[0].exist = 1;
				}
				re41[1].x += 0.02;
				re41[1].y -= 0.02;
				re41[1].size -= 0.007;
				if (re41[1].size <= 0) {
					re41[1].exist = 1;
				}
				re41[2].x -= 0.02;
				re41[2].y += 0.02;
				re41[2].size -= 0.007;
				if (re41[2].size <= 0) {
					re41[2].exist = 1;
				}
				re41[3].x += 0.02;
				re41[3].y += 0.02;
				re41[3].size -= 0.007;
				if (re41[3].size <= 0) {
					re41[3].exist = 1;
					re[3].chosen = false;
				}
			}
			else if (re[3].div == 1) {
				re42[0].y -= 0.02;
				re42[0].size -= 0.007;
				if (re42[0].size <= 0) {
					re42[0].exist = 1;
				}
				re42[1].x += 0.02;
				re42[1].size -= 0.007;
				if (re42[1].size <= 0) {
					re42[1].exist = 1;
				}
				re42[2].x -= 0.02;
				re42[2].size -= 0.007;
				if (re42[2].size <= 0) {
					re42[2].exist = 1;
				}
				re42[3].y += 0.02;
				re42[3].size -= 0.007;
				if (re42[3].size <= 0) {
					re42[3].exist = 1;
					re[3].chosen = false;
				}
			}
			else if (re[3].div == 2) {
				re43[0].x -= 0.02;
				re43[0].y += 0.02;
				re43[0].size -= 0.007;
				if ((re43[0].x - re43[0].size > re43[0].x + re43[0].size) || (re43[0].y - re43[0].size > re43[0].y + re43[0].size)) {
					re43[0].exist = 1;
				}
				re43[1].x -= 0.02;
				re43[1].size -= 0.007;
				if ((re43[1].x - re43[1].size > re43[1].x + re43[1].size) || (re43[1].y - re43[1].size > re43[1].y + re43[1].size)) {
					re43[1].exist = 1;
				}
				re43[2].x += 0.02;
				re43[2].size -= 0.007;
				if ((re43[2].x - re43[2].size > re43[2].x + re43[2].size) || (re43[2].y - re43[2].size > re43[2].y + re43[2].size)) {
					re43[2].exist = 1;
				}
				re43[3].x += 0.02;
				re43[3].y -= 0.02;
				re43[3].size -= 0.007;
				if ((re43[3].x - re43[3].size > re43[3].x + re43[3].size) || (re43[3].y - re43[3].size > re43[3].y + re43[2].size)) {
					re43[3].exist = 1;
					re[3].chosen = false;
				}
				re43[4].x -= 0.02;
				re43[4].y -= 0.02;
				re43[4].size -= 0.007;
				if (re43[4].size <= 0) {
					re43[4].exist = 1;
				}
				re43[5].y -= 0.02;
				re43[5].size -= 0.007;
				if (re43[5].size <= 0) {
					re43[5].exist = 1;
				}
				re43[6].y += 0.02;
				re43[6].size -= 0.007;
				if (re43[6].size <= 0) {
					re43[6].exist = 1;
				}
				re43[7].x += 0.02;
				re43[7].y += 0.02;
				re43[7].size -= 0.007;
				if (re43[7].size <= 0) {
					re43[7].exist = 1;
				}
			}
		}
		if (re[4].chosen) {
			if (re[4].div == 0) {
				re51[0].x -= 0.02;
				re51[0].y -= 0.02;
				re51[0].size -= 0.007;
				if (re51[0].size <= 0) {
					re51[0].exist = 1;
				}
				re51[1].x += 0.02;
				re51[1].y -= 0.02;
				re51[1].size -= 0.007;
				if (re51[1].size <= 0) {
					re51[1].exist = 1;
				}
				re51[2].x -= 0.02;
				re51[2].y += 0.02;
				re51[2].size -= 0.007;
				if (re51[2].size <= 0) {
					re51[2].exist = 1;
				}
				re51[3].x += 0.02;
				re51[3].y += 0.02;
				re51[3].size -= 0.007;
				if (re51[3].size <= 0) {
					re51[3].exist = 1;
					re[4].chosen = false;
				}
			}
			else if (re[4].div == 1) {
				re52[0].y -= 0.02;
				re52[0].size -= 0.007;
				if (re52[0].size <= 0) {
					re52[0].exist = 1;
				}
				re52[1].x += 0.02;
				re52[1].size -= 0.007;
				if (re52[1].size <= 0) {
					re52[1].exist = 1;
				}
				re52[2].x -= 0.02;
				re52[2].size -= 0.007;
				if (re52[2].size <= 0) {
					re52[2].exist = 1;
				}
				re52[3].y += 0.02;
				re52[3].size -= 0.007;
				if (re52[3].size <= 0) {
					re52[3].exist = 1;
					re[4].chosen = false;
				}
			}
			else if (re[4].div == 2) {
				re53[0].x -= 0.02;
				re53[0].y += 0.02;
				re53[0].size -= 0.007;
				if ((re53[0].x - re53[0].size > re53[0].x + re53[0].size) || (re53[0].y - re53[0].size > re53[0].y + re53[0].size)) {
					re53[0].exist = 1;
				}
				re53[1].x -= 0.02;
				re53[1].size -= 0.007;
				if ((re53[1].x - re53[1].size > re53[1].x + re53[1].size) || (re53[1].y - re53[1].size > re53[1].y + re53[1].size)) {
					re53[1].exist = 1;
				}
				re53[2].x += 0.02;
				re53[2].size -= 0.007;
				if ((re53[2].x - re53[2].size > re53[2].x + re53[2].size) || (re53[2].y - re53[2].size > re53[2].y + re53[2].size)) {
					re53[2].exist = 1;
				}
				re53[3].x += 0.02;
				re53[3].y -= 0.02;
				re53[3].size -= 0.007;
				if ((re53[3].x - re53[3].size > re53[3].x + re53[3].size) || (re53[3].y - re53[3].size > re53[3].y + re53[2].size)) {
					re53[3].exist = 1;
					re[4].chosen = false;
				}
				re53[4].x -= 0.02;
				re53[4].y -= 0.02;
				re53[4].size -= 0.007;
				if (re53[4].size <= 0) {
					re53[4].exist = 1;
				}
				re53[5].y -= 0.02;
				re53[5].size -= 0.007;
				if (re53[5].size <= 0) {
					re53[5].exist = 1;
				}
				re53[6].y += 0.02;
				re53[6].size -= 0.007;
				if (re53[6].size <= 0) {
					re53[6].exist = 1;
				}
				re53[7].x += 0.02;
				re53[7].y += 0.02;
				re53[7].size -= 0.007;
				if (re53[7].size <= 0) {
					re53[7].exist = 1;
				}
			}
		}
		glutTimerFunc(75, TimerFunction, RECT_ANI);
	}
	break;
	default:
		break;
	}
}