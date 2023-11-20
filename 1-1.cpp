#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
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
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����
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
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(100, TimerFunction, 1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		r = 0;
		g = 1;
		b = 1;
		break; //--- ������ û�ϻ����� ����
	case 'm': 
		r = 1;
		g = 0;
		b = 1;
		break; //--- ������ ��ȫ������ ����
	case 'y':
		r = 1;
		g = 1;
		b= 0;
		break; //--- ������ ��������� ����
	case 'a':
		srand(time(NULL));
		r = rand() % 2;
		g = rand() % 2;
		b = rand() % 2;
		break; //--- ������ �������� ����
	case 'w':
		r = 1;
		g = 1;
		b = 1;
		break; //--- ������ �Ͼ������ ����
	case 'b':
		r = 0;
		g = 0;
		b = 0;
		break; //--- ������ ���������� ����
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
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