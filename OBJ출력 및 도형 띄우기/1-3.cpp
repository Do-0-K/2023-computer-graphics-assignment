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
	glutMotionFunc(Motion);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	if (h > 0) {
		for (int i = 0;i < h;i++) {
			glColor3f(re[i].r, re[i].g, re[i].b);
			glRectf(re[i].x - 0.1, re[i].y - 0.1, re[i].x + 0.1, re[i].y + 0.1);
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //왼쪽 버튼 누른 경우

		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
		float my = - (y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
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
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
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
		random_device rd; //랜덤 장치 세팅
		default_random_engine dre(rd()); //세팅된 장치를 엔진으로 돌린다
		uniform_real_distribution<float> uColor(0, 1); //주어진 범위 안에서 돌린다, 중복 안되게
		re[h].r = uColor(dre);
		re[h].g = uColor(dre);
		re[h].b = uColor(dre);
		uniform_real_distribution<float> uPos(-1, 1); //주어진 범위 안에서 돌린다, 중복 안되게
		re[h].x = uPos(dre);
		re[h].y = uPos(dre);
		h++;
		break; //--- 랜덤 사각형 생성
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}