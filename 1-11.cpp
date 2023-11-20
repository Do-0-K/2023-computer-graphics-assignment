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
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
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
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
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
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
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
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
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
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
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
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
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
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	g_lines.emplace_back(200, 150);
	g_triangles.emplace_back(600, 150);
	g_rects.emplace_back(200, 450);
	g_oghs.emplace_back(600, 450);
	glutTimerFunc(1, TimerFunction, RENDERING);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
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
GLvoid TimerFunction(int value) { //Ÿ�̸� �ȿ��� switch�� �̻ڴ� //1~5�� ��� + ������ ���
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