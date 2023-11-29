#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include<stdlib.h>
#include<time.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLvoid glColor3f(GLfloat r, GLfloat g, GLfloat b);
int r = 1, g = 1, b = 1;
float r1 = 0.5, g1 = 0, b1 = 0.5;
float r2 = 1, g2 = 1, b2 = 0;
float r3 = 0.5, g3 = 0.5, b3 = 0.5;
float r4 = 1, g4 = 0.2, b4 = 0.4;
float r5 = 1, g5 = 0.2, b5 = 0.4;
float r6 = 0.5, g6 = 0.4, b6 = 0.3;
float r7 = 1, g7 = 1, b7 = 0;
float r8 = 1, g8 = 0, b8 = 1;

float lex1 = -0.5, ley1 = 0.5, rix1 = -0.5, riy1 = 0.5;
float lex2 = 0.5, ley2 = 0.5, rix2 = 0.5, riy2 = 0.5;
float lex3 = -0.5, ley3 = -0.5, rix3 = -0.5, riy3 = -0.5;
float lex4 = 0.5, ley4 = -0.5, rix4 = 0.5, riy4 = -0.5;

float p1 = 0, p2 = 0, p3 = 0, p4 = 0;
int nump1 = 0, nump2 = 0, nump3 = 0, nump4 = 0;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
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
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glColor3f(r1, g1, b1);
	glRectf(-1.0f, 1.0f, 0.0f, 0.0f);
	glColor3f(r2, g2, b2);
	glRectf(0.0f, 1.0f, 1.0f, 0.0f);
	glColor3f(r3, g3, b3);
	glRectf(-1.0f, 0.0f, 0.0f, -1.0f);
	glColor3f(r4, g4, b4);
	glRectf(0.0f, 0.0f, 1.0f, -1.0f);
	glColor3f(r5, g5, b5);
	glRectf(lex1 - p1, ley1 - p1, rix1 + p1, riy1 + p1);
	glColor3f(r6, g6, b6);
	glRectf(lex2 - p2, ley2 - p2, rix2 + p2, riy2 + p2);
	glColor3f(r7, g7, b7);
	glRectf(lex3 - p3, ley3 - p3, rix3 + p3, riy3 + p3);
	glColor3f(r8, g8, b8);
	glRectf(lex4 - p4, ley4 - p4, rix4 + p4, riy4 + p4);
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}
GLvoid Mouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {//왼쪽 버튼 누른 경우
		if (x < 400 && y < 300) {
			if (200 - (20 * nump1) < x && x < 200 + (20 * nump1)) {
				if (150 - (15 * nump1) < y && y < 150 + (15 * nump1)) {
					r5 = rand() % 2;
					g5 = rand() % 2;
					b5 = rand() % 2;
				}
				else {
					r1 = rand() % 2;
					g1 = rand() % 2;
					b1 = rand() % 2;
				}
			}
			else {
				r1 = rand() % 2;
				g1 = rand() % 2;
				b1 = rand() % 2;
			}
		}
		if (x > 400 && y < 300) {
			if (600 - (20 * nump2) < x && x < 600 + (20 * nump2)) {
				if (150 - (15 * nump2) < y && y < 150 + (15 * nump2)) {
					r6 = rand() % 2;
					g6 = rand() % 2;
					b6 = rand() % 2;
				}
				else {
					r2 = rand() % 2;
					g2 = rand() % 2;
					b2 = rand() % 2;
				}
			}
			else {
				r2 = rand() % 2;
				g2 = rand() % 2;
				b2 = rand() % 2;
			}
		}
		if (x < 400 && y > 300) {
			if (200 - (20 * nump3) < x && x < 200 + (20 * nump3)) {
				if (450 - (15 * nump3) < y && y < 450 + (15 * nump3)) {
					r7 = rand() % 2;
					g7 = rand() % 2;
					b7 = rand() % 2;
				}
				else {
					r3 = rand() % 2;
					g3 = rand() % 2;
					b3 = rand() % 2;
				}
			}
			else {
				r3 = rand() % 2;
				g3 = rand() % 2;
				b3 = rand() % 2;
			}
		}
		if (x > 400 && y > 300) {
			if (600 - (20 * nump4) < x && x < 600 + (20 * nump4)) {
				if (450 - (15 * nump4) < y && y < 450 + (15 * nump4)) {
					r8 = rand() % 2;
					g8 = rand() % 2;
					b8 = rand() % 2;
				}
				else {
					r4 = rand() % 2;
					g4 = rand() % 2;
					b4 = rand() % 2;
				}
			}
			else {
				r4 = rand() % 2;
				g4 = rand() % 2;
				b4 = rand() % 2;
			}
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {//오른쪽 버튼 누른 경우
		if (x < 400 && y < 300) {
			if (200 - (20 * nump1) < x && x < 200 + (20 * nump1)) {
				if (150 - (15 * nump1) < y && y < 150 + (15 * nump1)) {
					p1 -= 0.05;
					nump1--;
				}
				else {
					p1 += 0.05;
					nump1++;
				}
			}
			else {
				p1 += 0.05;
				nump1++;
			}
		}
		if (x > 400 && y < 300) {
			if (600 - (20 * nump2) < x && x < 600 + (20 * nump2)) {
				if (150 - (15 * nump2) < y && y < 150 + (15 * nump2)) {
					p2 -= 0.05;
					nump2--;
				}
				else {
					p2 += 0.05;
					nump2++;
				}
			}
			else {
				p2 += 0.05;
				nump2++;
			}
		}
		if (x < 400 && y > 300) {
			if (200 - (20 * nump3) < x && x < 200 + (20 * nump3)) {
				if (450 - (15 * nump3) < y && y < 450 + (15 * nump3)) {
					p3 -= 0.05;
					nump3--;
				}
				else {
					p3 += 0.05;
					nump3++;
				}
			}
			else {
				p3 += 0.05;
				nump3++;
			}
		}
		if (x > 400 && y > 300) {
			if (600 - (20 * nump4) < x && x < 600 + (20 * nump4)) {
				if (450 - (15 * nump4) < y && y < 450 + (15 * nump4)) {
					p4 -= 0.05;
					nump4--;
				}
				else {
					p4 += 0.05;
					nump4++;
				}
			}
			else {
				p4 += 0.05;
				nump4++;
			}
		}
	}
}