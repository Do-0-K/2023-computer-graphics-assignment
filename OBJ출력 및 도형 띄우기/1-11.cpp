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
float r = 1.0, g = 1.0, b = 1.0;
int linenum = 0;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
GLuint m_VBOVertex;
GLuint m_VBOColor;
GLuint m_VAO;
bool ischange = false, ischange1 = false, ischange2 = false, ischange3 = false;
bool isshow = true, isshow1 = true, isshow2 = true, isshow3 = true;
enum DRAW_CASE {
	D_TRIANGLE,
	D_LINE,
	D_RECT,
	D_OGH,
	D_POINT
};
enum TIMER_CASE {
	RENDERING,
	BE_TRIANGLE,
	BE_RECT,
	BE_OGH,
	BE_POINT
};
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
struct Triangle {
	glm::vec3 m_vertex[4];
	glm::vec3 m_color[4];
	glm::vec3 m_move = { 0,0,0 };
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
		for (int i = 0;i < 4;i++) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[2].x = 0.0f;
		m_vertex[2].y = 0.2f;
		m_vertex[0].x = - 0.2f;
		m_vertex[0].y = - 0.2f;
		m_vertex[1].x = 0.2f;
		m_vertex[1].y = - 0.2f;
		m_vertex[3].x = 0.0f;
		m_vertex[3].y = 0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = 0.0f;
		SetVAO();
	}
	void NewTri(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[2].x = 0.0f;
		m_vertex[2].y = 0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = 0.2f;
		m_vertex[1].y = -0.2f;
		m_vertex[3].x = 0.0f;
		m_vertex[3].y = 0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = 0.0f;
		SetVAO();
	}
	void New() {
		m_vertex[2].x -= 0.01f;
		m_vertex[3].x += 0.01f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = 0.0f;
		SetVAO();
	}
	void reset() {
		m_vertex[2].x = 0.0f;
		m_vertex[2].y = 0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = 0.2f;
		m_vertex[1].y = -0.2f;
		m_vertex[3].x = 0.0f;
		m_vertex[3].y = 0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw(int type) {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUseProgram(shaderProgramID);
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		switch (type) {
		case D_TRIANGLE:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case D_RECT:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawArrays(GL_TRIANGLES, 1, 3);
			break;
		default:
			break;
		}
	}
};
vector<Triangle> g_triangles;
struct Line {
	glm::vec3 m_vertex[3];
	glm::vec3 m_color[3];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Line(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		for (int i = 0;i < 3;i++) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[2].x = 0.0f;
		m_vertex[2].y = -0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = +0.2f;
		m_vertex[1].y = -0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
		SetVAO();
	}
	void NewLine(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[2].x = 0.0f;
		m_vertex[2].y = -0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = +0.2f;
		m_vertex[1].y = -0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
		SetVAO();
	}
	void New() {
		m_vertex[2].x = 0.0f;
		m_vertex[2].y += 0.01f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
		SetVAO();
	}
	void reset() {
		m_vertex[2].x = 0.0f;
		m_vertex[2].y = -0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = +0.2f;
		m_vertex[1].y = -0.2f;
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
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw(int type) {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUseProgram(shaderProgramID);
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		switch (type) {
		case D_TRIANGLE:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case D_LINE:
			glLineWidth(6.0f);
			glDrawArrays(GL_LINE_LOOP, 0, 2);
			break;
		}
	}
};
vector<Line> g_lines;
struct Rect {
	glm::vec3 m_vertex[5];
	glm::vec3 m_color[5];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Rect(int x, int y) {
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
		m_color[3].r = r;
		m_color[0].g = g;
		m_color[1].g = g;
		m_color[2].g = g;
		m_color[3].g = g;
		m_color[0].b = b;
		m_color[1].b = b;
		m_color[2].b = b;
		m_color[3].b = b;
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[1].x = - 0.2f;
		m_vertex[1].y =  - 0.2f;
		m_vertex[0].x = - 0.2f;
		m_vertex[0].y =  + 0.2f;
		m_vertex[2].x = + 0.0f;
		m_vertex[2].y =  + 0.2f;
		m_vertex[3].x = + 0.2f;
		m_vertex[3].y =  - 0.2f;
		m_vertex[4].x = +0.2f;
		m_vertex[4].y = +0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = m_vertex[4].z = 0.0f;
		SetVAO();
	}
	void NewRect(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[1].x = -0.2f;
		m_vertex[1].y = -0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = +0.2f;
		m_vertex[2].x = +0.0f;
		m_vertex[2].y = +0.2f;
		m_vertex[3].x = +0.2f;
		m_vertex[3].y = -0.2f;
		m_vertex[4].x = +0.2f;
		m_vertex[4].y = +0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = m_vertex[4].z = 0.0f;
		SetVAO();
	}
	void New() {
		m_vertex[2].y += 0.015f;
		m_vertex[1].x += 0.01f;
		m_vertex[3].x -= 0.01f;
		m_vertex[0].y -= 0.005f;
		m_vertex[4].y -= 0.005f;
		SetVAO();
	}
	void reset() {
		m_vertex[1].x = -0.2f;
		m_vertex[1].y = -0.2f;
		m_vertex[0].x = -0.2f;
		m_vertex[0].y = +0.2f;
		m_vertex[2].x = +0.0f;
		m_vertex[2].y = +0.2f;
		m_vertex[3].x = +0.2f;
		m_vertex[3].y = -0.2f;
		m_vertex[4].x = +0.2f;
		m_vertex[4].y = +0.2f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = m_vertex[4].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw() {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
		glDrawArrays(GL_TRIANGLES, 2, 3);
	}
};
vector<Rect> g_rects;
struct OGH {
	glm::vec3 m_vertex[5];
	glm::vec3 m_color[5];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	OGH(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		for (int i = 0;i < 5;i++) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[2].x = -0.2f;
		m_vertex[2].y = +0.2f;
		m_vertex[0].x = -0.13f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = +0.13f;
		m_vertex[1].y = -0.2f;
		m_vertex[3].x = +0.2f;
		m_vertex[3].y = +0.2f;
		m_vertex[4].x = 0.0f;
		m_vertex[4].y = +0.4f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z= m_vertex[4].z = 0.0f;
		SetVAO();
	}
	void NewOgh(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[2].x = -0.2f;
		m_vertex[2].y = +0.2f;
		m_vertex[0].x = -0.13f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = +0.13f;
		m_vertex[1].y = -0.2f;
		m_vertex[3].x = +0.2f;
		m_vertex[3].y = +0.2f;
		m_vertex[4].x = 0.0f;
		m_vertex[4].y = +0.4f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = m_vertex[4].z = 0.0f;
		SetVAO();
	}
	void New() {
		m_vertex[2].x += 0.002f;
		m_vertex[2].y -= 0.002f;
		m_vertex[0].x += 0.002f;
		m_vertex[0].y += 0.002f;
		m_vertex[1].x -= 0.002f;
		m_vertex[1].y += 0.002f;
		m_vertex[3].x -= 0.002f;
		m_vertex[3].y -= 0.002f;
		m_vertex[4].x = 0.0f;
		m_vertex[4].y -= 0.002f;
		if (m_vertex[0].x > 0) {
			m_vertex[0].x = -0.002f;
		}
		if (m_vertex[0].y > 0) {
			m_vertex[0].y = -0.002f;
		}
		if (m_vertex[1].x < 0) {
			m_vertex[1].x = 0.002f;
		}
		if (m_vertex[1].y > 0) {
			m_vertex[1].y = -0.002f;
		}
		if (m_vertex[2].x > 0) {
			m_vertex[2].x = -0.002f;
		}
		if (m_vertex[2].y < 0) {
			m_vertex[2].y = 0.002f;
		}
		if (m_vertex[3].x < 0) {
			m_vertex[3].x = 0.002f;
		}
		if (m_vertex[3].y < 0) {
			m_vertex[3].y = 0.002f;
		}
		if (m_vertex[4].y < 0) {
			m_vertex[4].y = 0.002f;
		}
		SetVAO();
	}
	void reset() {
		m_vertex[2].x = -0.2f;
		m_vertex[2].y = +0.2f;
		m_vertex[0].x = -0.13f;
		m_vertex[0].y = -0.2f;
		m_vertex[1].x = +0.13f;
		m_vertex[1].y = -0.2f;
		m_vertex[3].x = +0.2f;
		m_vertex[3].y = +0.2f;
		m_vertex[4].x = 0.0f;
		m_vertex[4].y = +0.4f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = m_vertex[4].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw(int type) {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		switch (type) {
		case D_OGH:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawArrays(GL_TRIANGLES, 1, 3);
			glDrawArrays(GL_TRIANGLES, 2, 3);
			break;
		case D_POINT:
			glPointSize(15.0f);
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		}
	}
};
vector<OGH> g_oghs;
int num = 0, num1 = 0, num2 = 0, num3 = 0;
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
	g_lines.emplace_back(200, 150);
	g_triangles.emplace_back(600, 150);
	g_rects.emplace_back(200, 450);
	g_oghs.emplace_back(600, 450);
	glutTimerFunc(1, TimerFunction, RENDERING);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	if (isshow3) {
		for (auto& p : g_oghs) {
			if (num3 >= 108) {
				p.Draw(D_POINT);
			}
			else {
				p.Draw(D_OGH);
			}
		}
	}
	if (isshow) {
		for (auto& s : g_lines) {
			if (ischange) {
				s.Draw(D_TRIANGLE);
			}
			else {
				s.Draw(D_LINE);
			}
		}
	}
	if (isshow1) {
		for (auto& t : g_triangles) {
			if (ischange1) {
				t.Draw(D_RECT);
			}
			else {
				t.Draw(D_TRIANGLE);
			}
		}
	}
	if (isshow2) {
		for (auto& v : g_rects)v.Draw();
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
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'l':
		if (!ischange) {
			g_lines[0].NewLine(400, 300);
			g_triangles[0].NewTri(600, 150);
			g_rects[0].NewRect(200, 450);
			g_oghs[0].NewOgh(600, 450);
			ischange = true;
			isshow = true, isshow1 = false, isshow2 = false, isshow3 = false;
			glutTimerFunc(17, TimerFunction, BE_TRIANGLE);
		}
		else if(ischange){
			num = 0;
			g_lines[0].NewLine(200, 150);
			g_lines[0].reset();
			ischange = false;
			isshow = true, isshow1 = true, isshow2 = true, isshow3 = true;
		}
		break;
	case 't':
		if (!ischange1) {
			g_lines[0].NewLine(200, 150);
			g_triangles[0].NewTri(400, 300);
			g_rects[0].NewRect(200, 450);
			g_oghs[0].NewOgh(600, 450);
			ischange1 = true;
			isshow = false, isshow1 = true, isshow2 = false, isshow3 = false;
			glutTimerFunc(17, TimerFunction, BE_RECT);
		}
		else if (ischange1) {
			num1 = 0;
			g_triangles[0].NewTri(600, 150);
			g_triangles[0].reset();
			ischange1 = false;
			isshow = true, isshow1 = true, isshow2 = true, isshow3 = true;
		}
		break;
	case 'r':
		if (!ischange2) {
			g_rects[0].NewRect(400, 300);
			g_lines[0].NewLine(200, 150);
			g_triangles[0].NewTri(600, 150);
			g_oghs[0].NewOgh(600, 450);
			ischange2 = true;
			isshow = false, isshow1 = false, isshow2 = true, isshow3 = false;
			glutTimerFunc(17, TimerFunction, BE_OGH);
		}
		else if (ischange2) {
			num2 = 0;
			g_rects[0].NewRect(200,450);
			g_rects[0].reset();
			ischange2 = false;
			isshow = true, isshow1 = true, isshow2 = true, isshow3 = true;
		}
		break;
	case 'p':
		if (!ischange3) {
			g_rects[0].NewRect(200, 450);
			g_lines[0].NewLine(200, 150);
			g_triangles[0].NewTri(600, 150);
			g_oghs[0].NewOgh(400, 300);
			ischange3 = true;
			isshow = false, isshow1 = false, isshow2 = false, isshow3 = true;
			glutTimerFunc(17, TimerFunction, BE_POINT);
		}
		else if (ischange3) {
			num3 = 0;
			g_oghs[0].NewOgh(600,450);
			g_oghs[0].reset();
			ischange3 = false;
			isshow = true, isshow1 = true, isshow2 = true, isshow3 = true;
		}
		break;
	case 'a':
		if (!ischange) {
			ischange = true;
			glutTimerFunc(17, TimerFunction, BE_TRIANGLE);
		}
		else if (ischange) {
			num = 0;
			g_lines[0].reset();
			ischange = false;
		}
		if (!ischange1) {
			ischange1 = true;
			glutTimerFunc(17, TimerFunction, BE_RECT);
		}
		else if (ischange1) {
			num1 = 0;
			g_triangles[0].reset();
			ischange1 = false;
		}
		if (!ischange2) {
			ischange2 = true;
			glutTimerFunc(17, TimerFunction, BE_OGH);
		}
		else if (ischange2) {
			num2 = 0;
			g_rects[0].reset();
			ischange2 = false;
		}
		if (!ischange3) {
			ischange3 = true;
			glutTimerFunc(17, TimerFunction, BE_POINT);
		}
		else if (ischange3) {
			num3 = 0;
			g_oghs[0].reset();
			ischange3 = false;
		}
		break;
	}
	glutPostRedisplay();
}
GLvoid TimerFunction(int value) { //타이머 안에는 switch가 이쁘다 //1~5개 출력 + 선으로 출력
	switch (value) {
	case RENDERING:
		glutPostRedisplay();
		glutTimerFunc(17, TimerFunction, RENDERING);
		break;
	case BE_TRIANGLE:
		if (ischange) {
			num++;
			if (num > 30) {
				break;
			}
			else {
				g_lines[0].New();
				glutTimerFunc(17, TimerFunction, BE_TRIANGLE);
			}
		}
		break;
	case BE_RECT:
		if (ischange1) {
			num1++;
			if (num1 > 20) {
				break;
			}
			else {
				g_triangles[0].New();
				glutTimerFunc(17, TimerFunction, BE_RECT);
			}
		}
		break;
	case BE_OGH:
		if (ischange2) {
			num2++;
			if (num2 > 10) {
				break;
			}
			else {
				g_rects[0].New();
				glutTimerFunc(17, TimerFunction, BE_OGH);
			}
		}
		break;
	case BE_POINT:
		if (ischange3) {
			num3++;
			if (num3 > 108) {
				break;
			}
			else {
				g_oghs[0].New();
				glutTimerFunc(17, TimerFunction, BE_POINT);
			}
		}
		break;
	default:
		break;
	}
}