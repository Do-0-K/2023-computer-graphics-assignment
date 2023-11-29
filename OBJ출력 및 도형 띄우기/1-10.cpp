#include <iostream>
#include <random>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include<math.h>
char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}
float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
float r = 1.0f, g = 1.0f, b = 1.0f;
int maxVertexSize;
int how_draw = 0;
int num = 0;
bool isON = false;
bool isButton = false;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLuint spiVbo;
GLint result;
GLchar errorLog[512];
float rotate_Angle = 0;
struct shape {
	glm::vec3 m_move;
	int count_Pos = 1;
	float rotate = 0;
};
struct shape shape_data[5];
enum TIMER_CASE {
	RENDER,
	POINT_ANI,
	POINT_ROTATE
};
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}
void InitBuffer() {
	glGenBuffers(1, &spiVbo);
	glBindBuffer(GL_ARRAY_BUFFER, spiVbo);
	vector<glm::vec3> vertexes;
	vertexes.reserve(1080);
	float radius = 0.3f;
	std::pair<float, float> center = make_pair(0, 0);
	for (int i = 0; i < 4; i++) {
		float currentTheta = 0.0f;
		if (i % 2 != 0) {
			currentTheta = 180.0f;
		}
		for (int theta = 0; theta < 62; theta++) {
			float x = radius * cos(currentTheta * 3.14f / 180.0f);
			float y = radius * sin(currentTheta * 3.14f / 180.0f);
			x += center.first;
			vertexes.emplace_back(x, y, 0);
			currentTheta += 3.0f;
		}
		if (i % 2 != 0) {
			center.first -= radius;
		}
		else {
			center.first += radius;
		}
		radius *= 2.0f;
	}
	auto backIdx = vertexes.back();
	for (auto& p : vertexes) {
		p.x -= backIdx.x;
	}
	vector<glm::vec3> reverseVertexes = vertexes;
	std::reverse(reverseVertexes.begin(), reverseVertexes.end());
	for (auto& p : reverseVertexes) {
		p.x *= -1.0f;
		p.y *= -1.0f;
		vertexes.push_back(p);
	}
	for (auto& p : vertexes) {
		p.x *= 0.03f;
		p.y *= 0.03f;
	}
	maxVertexSize = vertexes.size();
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * maxVertexSize, &vertexes[0], GL_STATIC_DRAW);//m_vertex�� size��ŭ ���� ���ε� �Ѵٴ� vbo�� ��Ī?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glPointSize(2.0f);
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

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	InitBuffer();
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(ClickFunc);
	glutTimerFunc(1, TimerFunction, RENDER);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
	int vRotateLocation = glGetUniformLocation(shaderProgramID, "vRotate");
	glBindVertexArray(spiVbo);
	for (int i = 0;i < num;i++) {
		glUniform3f(vMoveLocation, shape_data[i].m_move.x, shape_data[i].m_move.y, shape_data[i].m_move.z);
		glUniform1f(vRotateLocation, shape_data[i].rotate);
		if (how_draw == 0) {
			glEnable(GL_POINT_SMOOTH);
			glDrawArrays(GL_POINTS, 0, shape_data[i].count_Pos);
		}
		if (how_draw == 1) {
			for (int j = 0;j < shape_data[i].count_Pos - 1;j++) {
				glDrawArrays(GL_LINES, j, 2);
			}
		}
	}
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex.glsl");

	//vertex shader ���� ���ְ� ������ �����͸� vertexShader��� ������ ����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//�����ϸ� 0�� �ƴѰ�

	//���̴� ��ü, 1, ���̴� �ҽ� �ڵ�, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, ���ڿ����� ����, ���ڿ����� �迭, ���ڿ����� ���̸� ������ int�迭

	//������ �ؾ�����.
	glCompileShader(vertexShader);

	//���̴� ��� ��� �ƴ���? ���� Ȯ���ϴ� �ڵ�
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0�̸� ������ �ִ�.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram() {
	//vertex, frament shader�� �̹� �������� �� ��Ȳ
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� ����� - �� ���̴� �پ�ߵ�, vertex - fragment�� ¦�� �¾ƾߵ�
	glAttachShader(shaderID, spiVbo);
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
	return shaderID;
}
GLvoid ClickFunc(int button, int state, int x, int y) {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> Color(0, 1);
	uniform_int_distribution<int> xPos(200, 600);
	uniform_int_distribution<int> yPos(150, 450);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (num < 5) {
			if (num == 0) {
				r = Color(dre);
				g = Color(dre);
				b = Color(dre);
				auto center = ConvertWinToGL(x, y);
				shape_data[num].m_move.x = center.first;
				shape_data[num].m_move.y = center.second;
				num++;
				isON = true;
				glutTimerFunc(17, TimerFunction, POINT_ANI);
			}
			else {
				r = Color(dre);
				g = Color(dre);
				b = Color(dre);
				auto center = ConvertWinToGL(xPos(dre), yPos(dre));
				shape_data[num].m_move.x = center.first;
				shape_data[num].m_move.y = center.second;
				num++;
			}
		}
	}
}
void reset() {
	for (int i = 0;i < 5;i++) {
		shape_data[i].count_Pos = 1;
		shape_data[i].m_move.x = 0;
		shape_data[i].m_move.y = 0;
		shape_data[i].rotate = 0;
		num++;
	}
	num = 0;
	isON = false;
	isButton = false;
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> Color(0, 1);
	uniform_int_distribution<int> xPos(200, 600);
	uniform_int_distribution<int> yPos(150, 450);
	auto center = ConvertWinToGL(xPos(dre), yPos(dre));
	switch (key) {
	case 'p':
		how_draw = 0;
		break;
	case 'l':
		how_draw = 1;
		break;
	case 'q':
		reset();
		r = Color(dre);
		g = Color(dre);
		b = Color(dre);
		shape_data[num].m_move.x = center.first;
		shape_data[num].m_move.y = center.second;
		shape_data[num].count_Pos = 1;
		shape_data[num].rotate = 0;
		num++;
		if (!isON) {
			glutTimerFunc(17, TimerFunction, POINT_ANI);
			isON = true;
		}
		break;
	case 'w':
		reset();
		r = Color(dre);
		g = Color(dre);
		b = Color(dre);
		for (int i = 0;i < 2;i++) {
			center = ConvertWinToGL(xPos(dre), yPos(dre));
			shape_data[i].m_move.x = center.first;
			shape_data[i].m_move.y = center.second;
			shape_data[i].count_Pos = 1;
			shape_data[i].rotate = 0;
			num++;
		}
		if (!isON) {
			glutTimerFunc(17, TimerFunction, POINT_ANI);
			isON = true;
		}
		break;
	case 'e':
		reset();
		r = Color(dre);
		g = Color(dre);
		b = Color(dre);
		for (int i = 0;i < 3;i++) {
			center = ConvertWinToGL(xPos(dre), yPos(dre));
			shape_data[i].m_move.x = center.first;
			shape_data[i].m_move.y = center.second;
			shape_data[i].count_Pos = 1;
			shape_data[i].rotate = 0;
			num++;
		}
		if (!isON) {
			glutTimerFunc(17, TimerFunction, POINT_ANI);
			isON = true;
		}
		break;
	case 'r':
		reset();
		r = Color(dre);
		g = Color(dre);
		b = Color(dre);
		for (int i = 0;i < 4;i++) {
			center = ConvertWinToGL(xPos(dre), yPos(dre));
			shape_data[i].m_move.x = center.first;
			shape_data[i].m_move.y = center.second;
			shape_data[i].count_Pos = 1;
			shape_data[i].rotate = 0;
			num++;
		}
		if (!isON) {
			glutTimerFunc(17, TimerFunction, POINT_ANI);
			isON = true;
		}
		break;
	case 't':
		reset();
		r = Color(dre);
		g = Color(dre);
		b = Color(dre);
		for (int i = 0;i < 5;i++) {
			center = ConvertWinToGL(xPos(dre), yPos(dre));
			shape_data[i].m_move.x = center.first;
			shape_data[i].m_move.y = center.second;
			shape_data[i].count_Pos = 1;
			shape_data[i].rotate = 0;
			num++;
		}
		if (!isON) {
			glutTimerFunc(17, TimerFunction, POINT_ANI);
			isON = true;
		}
		break;
	case 'c':
		r = g = b = 1.0f;
		reset();
		break;
	case 'v':
		if (!isButton) {
			glutTimerFunc(10, TimerFunction, POINT_ROTATE);
			isButton = true;
		}
		break;
	}
	glutPostRedisplay();
}
GLvoid TimerFunction(int value) { //Ÿ�̸� �ȿ��� switch�� �̻ڴ� //1~5�� ��� + ������ ���
	switch (value) {
	case RENDER:
		glutPostRedisplay();
		glutTimerFunc(17, TimerFunction, RENDER);
		break;
	case POINT_ANI:
		if (isON) {
			for (int i = 0;i < num;i++) {
				if (shape_data[i].count_Pos != maxVertexSize) {
					shape_data[i].count_Pos++;
				}
			}
			glutTimerFunc(17, TimerFunction, POINT_ANI);
		}
		break;
	case POINT_ROTATE:
		if (isButton) {
			for (int i = 0;i < num;i++) {
				shape_data[i].rotate += 2.0f;
			}
			glutTimerFunc(10, TimerFunction, POINT_ROTATE);
		}
		break;
	default:
		break;
	}
}