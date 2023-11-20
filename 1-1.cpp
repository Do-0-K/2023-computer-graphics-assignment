#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include<stdlib.h>
#include<time.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
int r = 0, g = 0, b = 0;
bool togle = false;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성
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
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(100, TimerFunction, 1);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		r = 0;
		g = 1;
		b = 1;
		break; //--- 배경색을 청록색으로 설정
	case 'm': 
		r = 1;
		g = 0;
		b = 1;
		break; //--- 배경색을 자홍색으로 설정
	case 'y':
		r = 1;
		g = 1;
		b= 0;
		break; //--- 배경색을 노랑색으로 설정
	case 'a':
		srand(time(NULL));
		r = rand() % 2;
		g = rand() % 2;
		b = rand() % 2;
		break; //--- 배경색을 랜덤으로 설정
	case 'w':
		r = 1;
		g = 1;
		b = 1;
		break; //--- 배경색을 하얀색으로 설정
	case 'b':
		r = 0;
		g = 0;
		b = 0;
		break; //--- 배경색을 검정색으로 설정
	case 't':
		togle = true;
		break;
	case 's':
		togle = false;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
GLvoid TimerFunction(int value) {
	if (togle) {
		r = rand() % 2;
		g = rand() % 2;
		b = rand() % 2;
	}
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}