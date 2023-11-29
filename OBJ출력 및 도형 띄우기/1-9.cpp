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
int num = 0;
int linenum = 0;
int what = 0;
float ct = 0;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
float r = 1.0, g = 1.0, b = 1.0;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid TimerFunction(int value);
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
enum DRAW_CASE {
	D_SOLID,
	D_LINE
};
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
struct Triangle {
	glm::vec3 m_vertex[3];
	glm::vec3 m_color[3];
	glm::vec3 m_move = { 0,0,0 };
	glm::vec3 m_rotate = { 0,0,0 };
	int cnt = 0, cnt1 = 0;
	double angle = 0;
	float r = 0.01f;
	float rotate = 0;
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Triangle(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		m_color[0].r = r;
		m_color[1].r = r;
		m_color[2].r = r;
		m_color[0].g = g;
		m_color[1].g = g;
		m_color[2].g = g;
		m_color[0].b = b;
		m_color[1].b = b;
		m_color[2].b = b;
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[0].x = 0;
		m_vertex[0].y =  + 0.1f;
		m_vertex[1].x = - 0.05f;
		m_vertex[1].y =  - 0.05f;
		m_vertex[2].x = + 0.05f;
		m_vertex[2].y = - 0.05f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
		SetVAO();
	}
	void NewTri(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[0].x = 0;
		m_vertex[0].y =  + 0.1f;
		m_vertex[1].x = - 0.05f;
		m_vertex[1].y =  - 0.05f;
		m_vertex[2].x = + 0.05f;
		m_vertex[2].y =  - 0.05f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw(int type) {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		int vRotateLocation = glGetUniformLocation(shaderProgramID, "vRotate");
		glUniform1f(vRotateLocation, rotate);
		glBindVertexArray(m_VAO);
		switch (type) {
		case D_SOLID:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		default:
			break;
		}
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_double_up() {
		m_move.y += 0.1f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_double_down() {
		m_move.y -= 0.1f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
	void Move_rotate() {
		m_rotate.x = r * sin(angle + M_PI + (M_PI * 1 / 4));
		m_rotate.y = r * cos(angle + M_PI + (M_PI * 1 / 4));
		m_move.x += m_rotate.x;
		m_move.y += m_rotate.y;
	}
};
vector<Triangle> g_triangles;
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
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(ClickFunc);
	g_triangles.emplace_back(200, 150);
	g_triangles.emplace_back(600, 150);
	g_triangles.emplace_back(200, 450);
	g_triangles.emplace_back(600, 450);
	glutTimerFunc(17, TimerFunction, 1);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	for (auto& t : g_triangles)t.Draw(D_SOLID);
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x < 400 && y < 300) {
			g_triangles[0].NewTri(x, y);
		}
		if (x > 400 && y < 300) {
			g_triangles[1].NewTri(x, y);
		}
		if (x < 400 && y > 300) {
			g_triangles[2].NewTri(x, y);
		}
		if (x > 400 && y > 300) {
			g_triangles[3].NewTri(x, y);
		}
		for (int i = 0;i < 4;i++) {
			g_triangles[i].cnt = 0;
			g_triangles[i].cnt1 = 0;
			g_triangles[i].rotate = 0;
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		for (int i = 0;i < 4;i++) {
			g_triangles[i].cnt = 0;
			g_triangles[i].cnt1 = 0;
			g_triangles[i].rotate = 0;
		}
		what = 1;
		break;
	case 'w':
		for (int i = 0;i < 4;i++) {
			g_triangles[i].cnt = 0;
			g_triangles[i].cnt1 = 0;
			g_triangles[i].rotate = 0;
		}
		what = 2;
		break;
	case 'e':
		g_triangles[0].NewTri(100, 75);
		g_triangles[1].NewTri(700, 75);
		g_triangles[2].NewTri(100, 525);
		g_triangles[3].NewTri(700, 525);
		for (int i = 0;i < 4;i++) {
			g_triangles[i].cnt = 0;
			g_triangles[i].cnt1 = 0;
			g_triangles[i].rotate = 0;
		}
		what = 3;
		break;
	case 'r':
		for (int i = 0;i < 4;i++) {
			g_triangles[i].cnt = 0;
			g_triangles[i].cnt1 = 0;
			g_triangles[i].r = 0.01f;
			g_triangles[i].angle = 0;
			g_triangles[i].rotate = 0;
		}
		what = 4;
		break;
	}
	glutPostRedisplay();
}
GLvoid TimerFunction(int value) { //타이머 안에는 switch가 이쁘다
	switch (what) {
	case 1: //대각선 튕기기
		for (int i = 0;i < g_triangles.size();i++) {
			if (g_triangles[i].cnt % 2 == 0) {
				g_triangles[i].Move_right();
				if (0.05f + g_triangles[i].m_move.x >= 1) {
					g_triangles[i].cnt++;
					g_triangles[i].rotate += 90.0f;
				}
				if (g_triangles[i].cnt1 % 2 == 0) {
					g_triangles[i].Move_up();
					if (0.1f + g_triangles[i].m_move.y >= 1) {
						g_triangles[i].cnt1++;
						g_triangles[i].rotate += 90.0f;
					}
				}
				else {
					g_triangles[i].Move_down();
					if (g_triangles[i].m_move.y - 0.05f <= -1) {
						g_triangles[i].cnt1++;
						g_triangles[i].rotate += 90.0f;
					}
				}
			}
			else {
				g_triangles[i].Move_left();
				if (g_triangles[i].m_move.x - 0.05f <= -1) {
					g_triangles[i].cnt++;
					g_triangles[i].rotate += 90.0f;
				}
				if (g_triangles[i].cnt1 % 2 == 0) {
					g_triangles[i].Move_up();
					if (0.1f + g_triangles[i].m_move.y >= 1) {
						g_triangles[i].cnt1++;
						g_triangles[i].rotate += 90.0f;
					}
				}
				else {
					g_triangles[i].Move_down();
					if (g_triangles[i].m_move.y - 0.05f <= -1) {
						g_triangles[i].cnt1++;
						g_triangles[i].rotate += 90.0f;
					}
				}
			}
		}
		break;
	case 2: //지그재그
		for (int i = 0;i < g_triangles.size();i++) {
			if (g_triangles[i].cnt % 2 == 0) {
				g_triangles[i].Move_right();
				if (g_triangles[i].m_vertex->x + g_triangles[i].m_move.x >= 1) {
					if (g_triangles[i].cnt1 % 2 == 0) {
						g_triangles[i].Move_double_up();
						g_triangles[i].rotate += 90.0f;
						if (g_triangles[i].m_vertex->y + g_triangles[i].m_move.y >= 1) {
							g_triangles[i].cnt1++;
							g_triangles[i].rotate += 90.0f;
						}
					}
					else {
						g_triangles[i].Move_double_down();
						g_triangles[i].rotate += 90.0f;
						if (g_triangles[i].m_vertex->y + g_triangles[i].m_move.y <= -1) {
							g_triangles[i].cnt1++;
							g_triangles[i].rotate -= 90.0f;
						}
					}
					g_triangles[i].cnt++;
				}
			}
			else {
				g_triangles[i].Move_left();
				if (g_triangles[i].m_vertex->x + g_triangles[i].m_move.x <= -1) {
					if (g_triangles[i].cnt1 % 2 == 0) {
						g_triangles[i].Move_double_up();
						g_triangles[i].rotate -= 90.0f;
						if (g_triangles[i].m_vertex->y + g_triangles[i].m_move.y >= 1) {
							g_triangles[i].cnt1++;
							g_triangles[i].rotate -= 90.0f;
						}
					}
					else {
						g_triangles[i].Move_double_down();
						g_triangles[i].rotate -= 90.0f;
						if (g_triangles[i].m_vertex->y + g_triangles[i].m_move.y <= -1) {
							g_triangles[i].cnt1++;
							g_triangles[i].rotate -= 90.0f;
						}
					}
					g_triangles[i].cnt++;
				}
			}
		}
		break;
	case 3: //사각 스파이럴
		for (int i = 0;i < g_triangles.size();i++) {
			if (g_triangles[i].m_move.x >= 0.75 - ct) {
				g_triangles[i].rotate = 90.0f;
				g_triangles[i].cnt = 1;
			}
			if (g_triangles[i].m_move.y >= 0.75 - ct) {
				g_triangles[i].rotate = 180.0f;
				g_triangles[i].cnt = 2;
			}
			if (g_triangles[i].m_move.x <= -0.75 + ct) {
				g_triangles[i].rotate = 270.0f;
				g_triangles[i].cnt = 3;
			}
			if (g_triangles[i].m_move.y <= -0.75 + ct) {
				g_triangles[i].rotate = 360.0f;
				g_triangles[i].cnt = 0;
			}
			if (g_triangles[i].m_move.x >= 0.75 - ct && g_triangles[i].m_move.y <= -0.75 + ct) {
				g_triangles[i].rotate = 90.0f;
				g_triangles[i].cnt = 1;
			}
			if (g_triangles[i].cnt == 0) {
				g_triangles[i].Move_right();
			}
			if (g_triangles[i].cnt == 1) {
				g_triangles[i].Move_up();
			}
			if (g_triangles[i].cnt == 2) {
				g_triangles[i].Move_left();
			}
			if (g_triangles[i].cnt == 3) {
				g_triangles[i].Move_down();
			}
		}
		linenum++;
		if (linenum % 20 == 0) {
			ct += 0.01;
		}
		if (0.75 - ct <= 0.05) {
			ct = 0;
			g_triangles[0].NewTri(100, 75);
			g_triangles[1].NewTri(700, 75);
			g_triangles[2].NewTri(100, 525);
			g_triangles[3].NewTri(700, 525);
			for (int i = 0;i < 4;i++) {
				g_triangles[i].cnt = 0;
				g_triangles[i].cnt1 = 0;
				g_triangles[i].rotate = 0;
			}
			linenum = 0;
		}
		break;
	case 4: //원 스파이럴
		for (int i = 0;i < g_triangles.size();i++) {
			g_triangles[i].r += 0.0002f;
			g_triangles[i].rotate += 8.0f;
			g_triangles[i].angle -= (M_PI / 15);
			g_triangles[i].Move_rotate();
		}
		break;
	}
	glutPostRedisplay();
	glutTimerFunc(17, TimerFunction, 1);
}