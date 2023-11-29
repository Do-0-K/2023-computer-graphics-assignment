#include <gl/glew.h> //--- 필요한 헤더파일 include
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
	glutTimerFunc(17, TimerFunction, 1);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	if (h > 0) {
		for (int i = 0;i < h;i++) {
			glColor3f(re[i].r, re[i].g, re[i].b);
			glRectf(re[i].x - re[i].size_x, re[i].y - re[i].size_y, re[i].x + re[i].size_x, re[i].y + re[i].size_y);
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //왼쪽 버튼 누른 경우
		if (h > 4) {
			return;
		}
		random_device rd; //랜덤 장치 세팅
		default_random_engine dre(rd()); //세팅된 장치를 엔진으로 돌린다
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
		re[h].x = re[h].first_x = mx;
		re[h].y = re[h].first_y = my;
		uniform_real_distribution<float> uSize(0, 0.5); //주어진 범위 안에서 돌린다, 중복 안되게
		re[h].size_x = re[h].first_size_x = uSize(dre);
		re[h].size_y = re[h].first_size_y = uSize(dre);
		uniform_real_distribution<float> uColor(0, 1); //주어진 범위 안에서 돌린다, 중복 안되게
		re[h].r = re[h].first_r = uColor(dre);
		re[h].g = re[h].first_g = uColor(dre);
		re[h].b = re[h].first_b = uColor(dre);
		h++;
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	srand(time(NULL));
	random_device rd; //랜덤 장치 세팅
	default_random_engine dre(rd()); //세팅된 장치를 엔진으로 돌린다
	uniform_real_distribution<float> uSize(0, 0.5); //주어진 범위 안에서 돌린다, 중복 안되게
	uniform_real_distribution<float> uColor(0, 1); //주어진 범위 안에서 돌린다, 중복 안되게
	switch (key) {
	case 'c':
		cnt_size++;
		if (cnt_size % 2 == 1) {
			togle_size = true;
		}
		else {
			togle_size = false;
		}
		break; //--- 랜덤 사각형 생성
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
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
GLvoid TimerFunction(int value) { //타이머 안에는 switch가 이쁘다
	random_device rd; //랜덤 장치 세팅
	default_random_engine dre(rd()); //세팅된 장치를 엔진으로 돌린다
	uniform_real_distribution<float> uSize(0, 0.5); //주어진 범위 안에서 돌린다, 중복 안되게
	uniform_real_distribution<float> uColor(0, 1); //주어진 범위 안에서 돌린다, 중복 안되게
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
//enum 열거
enum day { //타이머에서 
	sunday=-2,
	monday,
	tuesday,
	saturday
};