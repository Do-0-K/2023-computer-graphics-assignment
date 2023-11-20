#include <iostream>
#include <random>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
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
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
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
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * maxVertexSize, &vertexes[0], GL_STATIC_DRAW);//m_vertex를 size만큼 위에 바인딩 한다는 vbo에 매칭?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glPointSize(2.0f);
}
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

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	InitBuffer();
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(ClickFunc);
	glutTimerFunc(1, TimerFunction, RENDER);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
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
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex.glsl");

	//vertex shader 생성 해주고 생성한 데이터를 vertexShader라는 변수에 저장
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//성공하면 0이 아닌값

	//세이더 객체, 1, 세이더 소스 코드, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, 문자열들의 갯수, 문자열들의 배열, 문자열들의 길이를 저장할 int배열

	//컴파일 해야지요.
	glCompileShader(vertexShader);

	//세이더 결과 어떻게 됐는지? 상태 확인하는 코드
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram() {
	//vertex, frament shader가 이미 컴파일은 된 상황
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, spiVbo);
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
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
GLvoid TimerFunction(int value) { //타이머 안에는 switch가 이쁘다 //1~5개 출력 + 선으로 출력
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