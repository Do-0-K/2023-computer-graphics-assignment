#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
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
float mouse_x = 0;
float mouse_y = 0;
float r = 1.0, g = 1.0, b = 1.0;
bool drag_point = false, drag_line = false, drag_tri = false, drag_rect = false, drag_ogh = false;
int who_point = -1, who_line = -1, who_tri = -1, who_rect = -1, who_ogh = -1;
enum DRAW_CASE {
	D_SOLID,
	D_POINT,
	D_LINE
};
void make_vertexShaders();
void make_fragmentShaders();
void Timer(int val);
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}
struct Point {
	glm::vec3 m_vertex[1];
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	int Move = 0;
	int cnt = 0, cnt1 = 0;
	Point(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		m_color[0].r = r;
		m_color[0].g = g;
		m_color[0].b = b;
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[0].x = 0.0f;
		m_vertex[0].y = 0.0f;
		m_vertex[0].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw() {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUseProgram(shaderProgramID);
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 1);
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
};
vector<Point> g_points;
struct Triangle {
	glm::vec3 m_vertex[3];
	glm::vec3 m_color[3];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	int Move = 0;
	int cnt = 0, cnt1 = 0;
	Triangle(int x, int y) {
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
		m_vertex[2].y = 0.1f;
		m_vertex[0].x = -0.08f;
		m_vertex[0].y = -0.06f;
		m_vertex[1].x = 0.08f;
		m_vertex[1].y = -0.06f;
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
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw() {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUseProgram(shaderProgramID);
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
};
vector<Triangle> g_triangles;
struct Line {
	glm::vec3 m_vertex[2];
	glm::vec3 m_color[2];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	int Move = 0;
	int cnt = 0, cnt1 = 0;
	Line(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		for (int i = 0;i < 2;i++) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		center;
		m_move.x = center.first;
		m_move.y = center.second;
		m_vertex[0].x = -0.1f;
		m_vertex[0].y = 0.0f;
		m_vertex[1].x = +0.1f;
		m_vertex[1].y = 0.0f;
		m_vertex[0].z = m_vertex[1].z = 0.0f;
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
	void Draw() {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUseProgram(shaderProgramID);
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		glLineWidth(6.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);
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
};
vector<Line> g_lines;
struct Rect {
	glm::vec3 m_vertex[4];
	glm::vec3 m_color[4];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	int Move = 0;
	int cnt = 0, cnt1 = 0;
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
		m_vertex[1].x = -0.1f;
		m_vertex[1].y = -0.1f;
		m_vertex[0].x = -0.1f;
		m_vertex[0].y = +0.1f;
		m_vertex[2].x = +0.1f;
		m_vertex[2].y = +0.1f;
		m_vertex[3].x = +0.1f;
		m_vertex[3].y = -0.1f;
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
	void Draw() {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
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
};
vector<Rect> g_rects;
struct OGH {
	glm::vec3 m_vertex[5];
	glm::vec3 m_color[5];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	int Move = 0;
	int cnt = 0, cnt1 = 0;
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
		m_vertex[2].x = -0.08f;
		m_vertex[2].y = +0.06f;
		m_vertex[0].x = -0.06f;
		m_vertex[0].y = -0.08f;
		m_vertex[1].x = +0.06f;
		m_vertex[1].y = -0.08f;
		m_vertex[3].x = +0.08f;
		m_vertex[3].y = +0.06f;
		m_vertex[4].x = 0.0f;
		m_vertex[4].y = +0.1f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = m_vertex[4].z = 0.0f;
		SetVBO();
	}
	void SetVBO() {
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
};
vector<OGH> g_oghs;
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
	else {
		std::cout << "GLEW Initialized" << std::endl;
	}
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();
	for (int i = 0;i < 3; i++) {
		g_oghs.emplace_back(200 * (i + 1), 550);
		g_rects.emplace_back(200 * (i + 1), 400);
		g_triangles.emplace_back(200 * (i + 1), 250);
		g_lines.emplace_back(200 * (i + 1), 150);
		g_points.emplace_back(200 * (i + 1), 50);
	}
	glutTimerFunc(17, Timer, 0);
	glutTimerFunc(17, Timer, 1);
	glutTimerFunc(17, Timer, 2);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMotionFunc(Motion);
	glutMouseFunc(ClickFunc);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	if (g_points.size() > 0) {
		for (auto& p : g_points)p.Draw();
	}
	if (g_lines.size() > 0) {
		for (auto& s : g_lines)s.Draw();
	}
	if (g_triangles.size() > 0) {
		for (auto& t : g_triangles)t.Draw();
	}
	if (g_rects.size() > 0) {
		for (auto& v : g_rects)v.Draw();
	}
	if (g_oghs.size() > 0) {
		for (auto& f : g_oghs)f.Draw();
	}
	glutPostRedisplay();//system.cls ��� �ٽ� ������ִ� ����
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
bool is_in_Triangles(float x, float y, float r) {
	if (x * x + y * y <= r * r) {
		return true;
	}
	else { return false; }
}
void checking_connect(int x, int y, int kind ,int who) {
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> Move(1, 2);
	if (kind == 1) {
		for (int i = 0;i < g_points.size();i++) {
			if (g_points[i].m_move.x - 0.02f < g_points[who].m_move.x && g_points[who].m_move.x < g_points[i].m_move.x + 0.02f) {
				if (g_points[i].m_move.y - 0.02f < g_points[who].m_move.y && g_points[who].m_move.y < g_points[i].m_move.y + 0.02f) {
					if (i != who) {
						g_points.erase(g_points.begin() + i);
						g_points.erase(g_points.begin() + who -1);
						g_lines.emplace_back(x, y);
						g_lines[g_lines.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
		}
		for (int j = 0;j < g_lines.size();j++) {
			if (g_lines[j].m_move.x - 0.1f < g_points[who].m_move.x && g_points[who].m_move.x < g_lines[j].m_move.x + 0.1f) {
				if (g_lines[j].m_move.y - 0.1f < g_points[who].m_move.y && g_points[who].m_move.y < g_lines[j].m_move.y + 0.1f) {
					g_lines.erase(g_lines.begin() + j);
					g_points.erase(g_points.begin() + who);
					g_triangles.emplace_back(x, y);
					g_triangles[g_triangles.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int k = 0;k < g_triangles.size();k++) {
			if (is_in_Triangles(g_triangles[k].m_move.x - g_points[who].m_move.x, g_triangles[k].m_move.y - g_points[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_points.erase(g_points.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int l = 0;l < g_rects.size();l++) {
			if (g_rects[l].m_move.x - 0.1f < g_points[who].m_move.x && g_points[who].m_move.x < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_points[who].m_move.y && g_points[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + l);
					g_points.erase(g_points.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int h = 0;h < g_oghs.size();h++) {
			if (is_in_Triangles(g_oghs[h].m_move.x - g_points[who].m_move.x, g_oghs[h].m_move.y - g_points[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_points.erase(g_points.begin() + who);
				g_points.emplace_back(x, y);
				g_points[g_points.size() - 1].Move = Move(dre);
				return;
			}
		}
		return;
	}
	if (kind == 2) {
		for (int i = 0;i < g_points.size();i++) {
			if (g_lines[who].m_move.x - 0.1f < g_points[i].m_move.x && g_points[i].m_move.x < g_lines[who].m_move.x + 0.1f) {
				if (g_lines[who].m_move.y - 0.1f < g_points[i].m_move.y && g_points[i].m_move.y < g_lines[who].m_move.y + 0.1f) {
					g_points.erase(g_points.begin() + i);
					g_lines.erase(g_lines.begin() + who);
					g_triangles.emplace_back(x, y);
					g_triangles[g_triangles.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int j = 0;j < g_lines.size();j++) {
			if (g_lines[j].m_move.x - 0.1f < g_lines[who].m_move.x-0.1f && g_lines[who].m_move.x -0.1f< g_lines[j].m_move.x + 0.1f) {
				if (g_lines[j].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_lines[j].m_move.y + 0.1f) {
					if (j != who) {
						g_lines.erase(g_lines.begin() + j);
						g_lines.erase(g_lines.begin() + who);
						g_rects.emplace_back(x, y);
						g_rects[g_rects.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_lines[j].m_move.x - 0.1f < g_lines[who].m_move.x-0.05f && g_lines[who].m_move.x -0.05f< g_lines[j].m_move.x + 0.1f) {
				if (g_lines[j].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_lines[j].m_move.y + 0.1f) {
					if (j != who) {
						g_lines.erase(g_lines.begin() + j);
						g_lines.erase(g_lines.begin() + who);
						g_rects.emplace_back(x, y);
						g_rects[g_rects.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_lines[j].m_move.x - 0.1f < g_lines[who].m_move.x && g_lines[who].m_move.x < g_lines[j].m_move.x + 0.1f) {
				if (g_lines[j].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_lines[j].m_move.y + 0.1f) {
					if (j != who) {
						g_lines.erase(g_lines.begin() + j);
						g_lines.erase(g_lines.begin() + who);
						g_rects.emplace_back(x, y);
						g_rects[g_rects.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_lines[j].m_move.x - 0.1f < g_lines[who].m_move.x +0.05f&& g_lines[who].m_move.x +0.05f< g_lines[j].m_move.x + 0.1f) {
				if (g_lines[j].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_lines[j].m_move.y + 0.1f) {
					if (j != who) {
						g_lines.erase(g_lines.begin() + j);
						g_lines.erase(g_lines.begin() + who);
						g_rects.emplace_back(x, y);
						g_rects[g_rects.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_lines[j].m_move.x - 0.1f < g_lines[who].m_move.x +0.1f&& g_lines[who].m_move.x +0.1f< g_lines[j].m_move.x + 0.1f) {
				if (g_lines[j].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_lines[j].m_move.y + 0.1f) {
					if (j != who) {
						g_lines.erase(g_lines.begin() + j);
						g_lines.erase(g_lines.begin() + who);
						g_rects.emplace_back(x, y);
						g_rects[g_rects.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
		}
		for (int k = 0;k < g_triangles.size();k++) {
			if (is_in_Triangles(g_triangles[k].m_move.x - g_lines[who].m_move.x -0.1f, g_triangles[k].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.erase(g_lines.begin() + who);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_lines[who].m_move.x + 0.1f, g_triangles[k].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.erase(g_lines.begin() + who);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_lines[who].m_move.x - 0.05f, g_triangles[k].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.erase(g_lines.begin() + who);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_lines[who].m_move.x + 0.05f, g_triangles[k].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.erase(g_lines.begin() + who);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_lines[who].m_move.x, g_triangles[k].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.erase(g_lines.begin() + who);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int l = 0;l < g_rects.size();l++) {
			if (g_rects[l].m_move.x - 0.1f < g_lines[who].m_move.x && g_lines[who].m_move.x < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + l);
					g_lines.erase(g_lines.begin() + who);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_lines[who].m_move.x -0.1f && g_lines[who].m_move.x -0.1< g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + l);
					g_lines.erase(g_lines.begin() + who);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_lines[who].m_move.x +0.1f&& g_lines[who].m_move.x +0.1f< g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + l);
					g_lines.erase(g_lines.begin() + who);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_lines[who].m_move.x -0.05f&& g_lines[who].m_move.x -0.05f< g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + l);
					g_lines.erase(g_lines.begin() + who);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_lines[who].m_move.x +0.05f&& g_lines[who].m_move.x +0.05f< g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_lines[who].m_move.y && g_lines[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + l);
					g_lines.erase(g_lines.begin() + who);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int h = 0;h < g_oghs.size();h++) {
			if (is_in_Triangles(g_oghs[h].m_move.x - g_lines[who].m_move.x, g_oghs[h].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_lines.erase(g_lines.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_lines[who].m_move.x+0.1f, g_oghs[h].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_lines.erase(g_lines.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_lines[who].m_move.x+0.05f, g_oghs[h].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_lines.erase(g_lines.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_lines[who].m_move.x-0.05f, g_oghs[h].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_lines.erase(g_lines.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_lines[who].m_move.x-0.1f, g_oghs[h].m_move.y - g_lines[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_lines.erase(g_lines.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
		}
		return;
	}
	if (kind == 3) {
		for (int i = 0;i < g_points.size();i++) {
			if (is_in_Triangles(g_triangles[who].m_move.x - g_points[i].m_move.x, g_triangles[who].m_move.y - g_points[i].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_points.erase(g_points.begin() + i);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int j = 0;j < g_lines.size();j++) {
			if (is_in_Triangles(g_triangles[who].m_move.x - g_lines[j].m_move.x - 0.1f, g_triangles[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_lines[j].m_move.x + 0.1f, g_triangles[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_lines[j].m_move.x - 0.05f, g_triangles[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_lines[j].m_move.x + 0.05f, g_triangles[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_lines[j].m_move.x, g_triangles[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_oghs.emplace_back(x, y);
				g_oghs[g_oghs.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int k = 0;k < g_triangles.size();k++) {
			if (is_in_Triangles(g_triangles[k].m_move.x - g_triangles[who].m_move.x, g_triangles[k].m_move.y - g_triangles[who].m_move.y, 0.1f)) {
				if (k != who) {
					g_triangles.erase(g_triangles.begin() + k);
					g_triangles.erase(g_triangles.begin() + who);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int l = 0;l < g_rects.size();l++) {
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x, g_triangles[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x -0.1f, g_triangles[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x, g_triangles[who].m_move.y - g_rects[l].m_move.y-0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x-0.1f, g_triangles[who].m_move.y - g_rects[l].m_move.y-0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x+0.1f, g_triangles[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x, g_triangles[who].m_move.y - g_rects[l].m_move.y+0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x - g_rects[l].m_move.x+0.1f, g_triangles[who].m_move.y - g_rects[l].m_move.y+0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + l);
				g_triangles.erase(g_triangles.begin() + who);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int h = 0;h < g_oghs.size();h++) {
			if (is_in_Triangles(g_triangles[who].m_move.x - g_oghs[h].m_move.x, g_triangles[who].m_move.y+0.1f - g_oghs[h].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_oghs.erase(g_oghs.begin() + h);
				g_triangles.emplace_back(x, y);
				g_triangles[g_triangles.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x-0.08f - g_oghs[h].m_move.x, g_triangles[who].m_move.y - 0.06f - g_oghs[h].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_oghs.erase(g_oghs.begin() + h);
				g_triangles.emplace_back(x, y);
				g_triangles[g_triangles.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[who].m_move.x+0.08f - g_oghs[h].m_move.x, g_triangles[who].m_move.y - 0.06f - g_oghs[h].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + who);
				g_oghs.erase(g_oghs.begin() + h);
				g_triangles.emplace_back(x, y);
				g_triangles[g_triangles.size() - 1].Move = Move(dre);
				return;
			}
		}
		return;
	}
	if (kind == 4) {
		for (int i = 0;i < g_points.size();i++) {
			if (g_rects[who].m_move.x - 0.1f < g_points[i].m_move.x && g_points[i].m_move.x < g_rects[who].m_move.x + 0.1f) { //x
				if (g_rects[who].m_move.y - 0.1f < g_points[i].m_move.y && g_points[i].m_move.y < g_rects[who].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + who);
					g_points.erase(g_points.begin() + i);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int j = 0;j < g_lines.size();j++) {
			if (g_rects[who].m_move.x - 0.1f < g_lines[j].m_move.x && g_lines[j].m_move.x < g_rects[who].m_move.x + 0.1f) { //x
				if (g_rects[who].m_move.y - 0.1f < g_lines[j].m_move.y && g_lines[j].m_move.y < g_rects[who].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + who);
					g_lines.erase(g_lines.begin() + j);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[who].m_move.x - 0.1f < g_lines[j].m_move.x - 0.1f && g_lines[j].m_move.x - 0.1 < g_rects[who].m_move.x + 0.1f) { //x
				if (g_rects[who].m_move.y - 0.1f < g_lines[j].m_move.y && g_lines[j].m_move.y < g_rects[who].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + who);
					g_lines.erase(g_lines.begin() + j);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[who].m_move.x - 0.1f < g_lines[j].m_move.x + 0.1f && g_lines[j].m_move.x + 0.1f < g_rects[who].m_move.x + 0.1f) { //x
				if (g_rects[who].m_move.y - 0.1f < g_lines[j].m_move.y && g_lines[j].m_move.y < g_rects[who].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + who);
					g_lines.erase(g_lines.begin() + j);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[who].m_move.x - 0.1f < g_lines[j].m_move.x - 0.05f && g_lines[j].m_move.x - 0.05f < g_rects[who].m_move.x + 0.1f) { //x
				if (g_rects[who].m_move.y - 0.1f < g_lines[j].m_move.y && g_lines[j].m_move.y < g_rects[who].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + who);
					g_lines.erase(g_lines.begin() + j);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (g_rects[who].m_move.x - 0.1f < g_lines[j].m_move.x + 0.05f && g_lines[j].m_move.x + 0.05f < g_rects[who].m_move.x + 0.1f) { //x
				if (g_rects[who].m_move.y - 0.1f < g_lines[j].m_move.y && g_lines[j].m_move.y < g_rects[who].m_move.y + 0.1f) { //y
					g_rects.erase(g_rects.begin() + who);
					g_lines.erase(g_lines.begin() + j);
					g_points.emplace_back(x, y);
					g_points[g_points.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		for (int k = 0;k < g_triangles.size();k++) {
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x, g_triangles[k].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x - 0.1f, g_triangles[k].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x, g_triangles[k].m_move.y - g_rects[who].m_move.y - 0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x - 0.1f, g_triangles[k].m_move.y - g_rects[who].m_move.y - 0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x + 0.1f, g_triangles[k].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x, g_triangles[k].m_move.y - g_rects[who].m_move.y + 0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - g_rects[who].m_move.x + 0.1f, g_triangles[k].m_move.y - g_rects[who].m_move.y + 0.1f, 0.1f)) {
				g_rects.erase(g_rects.begin() + who);
				g_triangles.erase(g_triangles.begin() + k);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int l = 0;l < g_rects.size();l++) {
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x && g_rects[who].m_move.x < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y && g_rects[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x - 0.1f && g_rects[who].m_move.x - 0.1f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y && g_rects[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x && g_rects[who].m_move.x< g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y -0.1f&& g_rects[who].m_move.y -0.1f< g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x - 0.1f && g_rects[who].m_move.x - 0.1f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y - 0.1f && g_rects[who].m_move.y - 0.1f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x + 0.1f && g_rects[who].m_move.x + 0.1f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y && g_rects[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x  && g_rects[who].m_move.x  < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y + 0.1f&& g_rects[who].m_move.y + 0.1f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x + 0.1f && g_rects[who].m_move.x + 0.1f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y + 0.1f && g_rects[who].m_move.y + 0.1f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x - 0.1f && g_rects[who].m_move.x - 0.1f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y + 0.1f && g_rects[who].m_move.y + 0.1f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x + 0.1f && g_rects[who].m_move.x + 0.1f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y - 0.1f && g_rects[who].m_move.y - 0.1f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x - 0.05f && g_rects[who].m_move.x - 0.05f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y && g_rects[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x  && g_rects[who].m_move.x  < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y - 0.05f&& g_rects[who].m_move.y - 0.05f< g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x - 0.05f && g_rects[who].m_move.x - 0.05f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y - 0.05f && g_rects[who].m_move.y - 0.05f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x + 0.05f && g_rects[who].m_move.x + 0.05f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y && g_rects[who].m_move.y < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x  && g_rects[who].m_move.x  < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y + 0.05f&& g_rects[who].m_move.y + 0.05f< g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x + 0.05f && g_rects[who].m_move.x + 0.05f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y + 0.05f && g_rects[who].m_move.y + 0.05f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x - 0.05f && g_rects[who].m_move.x - 0.05f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y + 0.05f && g_rects[who].m_move.y + 0.05f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
			if (g_rects[l].m_move.x - 0.1f < g_rects[who].m_move.x + 0.05f && g_rects[who].m_move.x + 0.05f < g_rects[l].m_move.x + 0.1f) { //x
				if (g_rects[l].m_move.y - 0.1f < g_rects[who].m_move.y - 0.05f && g_rects[who].m_move.y - 0.05f < g_rects[l].m_move.y + 0.1f) { //y
					if (l != who) {
						g_rects.erase(g_rects.begin() + l);
						g_rects.erase(g_rects.begin() + who);
						g_triangles.emplace_back(x, y);
						g_triangles[g_triangles.size() - 1].Move = Move(dre);
						return;
					}
				}
			}
		}
		for (int h = 0;h < g_oghs.size();h++) {
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x, g_oghs[h].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y+0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y-0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y+0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y-0.05, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y+0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y+0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y-0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x + 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y-0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y + 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y + 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y - 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.05f, g_oghs[h].m_move.y - g_rects[who].m_move.y - 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y + 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y - 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x- 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y + 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_rects[who].m_move.x - 0.1f, g_oghs[h].m_move.y - g_rects[who].m_move.y - 0.05, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + h);
				g_rects.erase(g_rects.begin() + who);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
		}
		return;
	}
	if (kind == 5) {
		for (int i = 0;i < g_points.size();i++) {
			if (is_in_Triangles(g_oghs[who].m_move.x - g_points[i].m_move.x, g_oghs[who].m_move.y - g_points[i].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_points.erase(g_points.begin() + i);
				g_points.emplace_back(x, y);
				g_points[g_points.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int j = 0;j < g_lines.size();j++) {
			if (is_in_Triangles(g_oghs[who].m_move.x - g_lines[j].m_move.x, g_oghs[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_lines[j].m_move.x + 0.1f, g_oghs[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_lines[j].m_move.x + 0.05f, g_oghs[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_lines[j].m_move.x - 0.05f, g_oghs[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_lines[j].m_move.x - 0.1f, g_oghs[who].m_move.y - g_lines[j].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_lines.erase(g_lines.begin() + j);
				g_lines.emplace_back(x, y);
				g_lines[g_lines.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int k = 0;k < g_triangles.size();k++) {
			if (is_in_Triangles(g_triangles[k].m_move.x - g_oghs[who].m_move.x, g_triangles[k].m_move.y + 0.1f - g_oghs[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_oghs.erase(g_oghs.begin() + who);
				g_triangles.emplace_back(x, y);
				g_triangles[g_triangles.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x - 0.08f - g_oghs[who].m_move.x, g_triangles[k].m_move.y - 0.06f - g_oghs[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_oghs.erase(g_oghs.begin() + who);
				g_triangles.emplace_back(x, y);
				g_triangles[g_triangles.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_triangles[k].m_move.x + 0.08f - g_oghs[who].m_move.x, g_triangles[k].m_move.y - 0.06f - g_oghs[who].m_move.y, 0.1f)) {
				g_triangles.erase(g_triangles.begin() + k);
				g_oghs.erase(g_oghs.begin() + who);
				g_triangles.emplace_back(x, y);
				g_triangles[g_triangles.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int l = 0;l < g_rects.size();l++) {
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x, g_oghs[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.05, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x + 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.05f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.1f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y + 0.05f, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
			if (is_in_Triangles(g_oghs[who].m_move.x - g_rects[l].m_move.x - 0.1f, g_oghs[who].m_move.y - g_rects[l].m_move.y - 0.05, 0.1f)) {
				g_oghs.erase(g_oghs.begin() + who);
				g_rects.erase(g_rects.begin() + l);
				g_rects.emplace_back(x, y);
				g_rects[g_rects.size() - 1].Move = Move(dre);
				return;
			}
		}
		for (int h = 0;h < g_oghs.size();h++) {
			if (is_in_Triangles(g_oghs[h].m_move.x - g_oghs[who].m_move.x, g_oghs[h].m_move.y+0.1f - g_oghs[who].m_move.y, 0.1f)) {
				if (h != who) {
					g_oghs.erase(g_oghs.begin() + h);
					g_oghs.erase(g_oghs.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (is_in_Triangles(g_oghs[h].m_move.x-0.08f - g_oghs[who].m_move.x, g_oghs[h].m_move.y+0.06f - g_oghs[who].m_move.y, 0.1f)) {
				if (h != who) {
					g_oghs.erase(g_oghs.begin() + h);
					g_oghs.erase(g_oghs.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (is_in_Triangles(g_oghs[h].m_move.x+0.08f - g_oghs[who].m_move.x, g_oghs[h].m_move.y+0.06f - g_oghs[who].m_move.y, 0.1f)) {
				if (h != who) {
					g_oghs.erase(g_oghs.begin() + h);
					g_oghs.erase(g_oghs.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - 0.06f - g_oghs[who].m_move.x, g_oghs[h].m_move.y - 0.08f - g_oghs[who].m_move.y, 0.1f)) {
				if (h != who) {
					g_oghs.erase(g_oghs.begin() + h);
					g_oghs.erase(g_oghs.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (is_in_Triangles(g_oghs[h].m_move.x + 0.06f - g_oghs[who].m_move.x, g_oghs[h].m_move.y - 0.08f - g_oghs[who].m_move.y, 0.1f)) {
				if (h != who) {
					g_oghs.erase(g_oghs.begin() + h);
					g_oghs.erase(g_oghs.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
			if (is_in_Triangles(g_oghs[h].m_move.x - g_oghs[who].m_move.x, g_oghs[h].m_move.y - g_oghs[who].m_move.y, 0.1f)) {
				if (h != who) {
					g_oghs.erase(g_oghs.begin() + h);
					g_oghs.erase(g_oghs.begin() + who);
					g_oghs.emplace_back(x, y);
					g_oghs[g_oghs.size() - 1].Move = Move(dre);
					return;
				}
			}
		}
		return;
	}
}
GLvoid ClickFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //���� ��ư ���� ���
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
		mouse_x = mx;
		mouse_y = my;
		if (g_points.size() > 0) {
			for (int i = 0;i < g_points.size();i++) {
				if (g_points[i].m_move.x - 0.02f < mx && mx < g_points[i].m_move.x + 0.02f) { //x
					if (g_points[i].m_move.y - 0.02f < my && my < g_points[i].m_move.y + 0.02f) { //y
						who_point = i;
						drag_point = true;
						break;
					}
				}
			}
		}
		if (g_lines.size() > 0) {
			for (int i = 0;i < g_lines.size();i++) {
				if (g_lines[i].m_move.x - 0.1f < mx && mx < g_lines[i].m_move.x + 0.1f) { //x
					if (g_lines[i].m_move.y - 0.1f < my && my < g_lines[i].m_move.y + 0.1f) { //y
						who_line = i;
						drag_line = true;
						break;
					}
				}
			}
		}
		if (g_triangles.size() > 0) {
			for (int i = 0;i < g_triangles.size();i++) {
				if (is_in_Triangles(g_triangles[i].m_move.x - mx, g_triangles[i].m_move.y - my, 0.1f)) {
					who_tri = i;
					drag_tri = true;
					break;
				}
			}
		}
		if (g_rects.size() > 0) {
			for (int i = 0;i < g_rects.size();i++) {
				if (g_rects[i].m_move.x - 0.1f < mx && mx < g_rects[i].m_move.x + 0.1f) { //x
					if (g_rects[i].m_move.y - 0.1f < my && my < g_rects[i].m_move.y + 0.1f) { //y
						who_rect = i;
						drag_rect = true;
						break;
					}
				}
			}
		}
		if (g_oghs.size() > 0) {
			for (int i = 0;i < g_oghs.size();i++) {
				if (is_in_Triangles(g_oghs[i].m_move.x - mx, g_oghs[i].m_move.y - my, 0.1f)) {
					who_ogh = i;
					drag_ogh = true;
					break;
				}
			}
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (drag_point) {
			checking_connect(x, y, 1 , who_point);
		}
		if (drag_line) {
			checking_connect(x, y, 2, who_line);
		}
		if (drag_tri) {
			checking_connect(x, y, 3, who_tri);
		}
		if (drag_rect) {
			checking_connect(x, y, 4, who_rect);
		}
		if (drag_ogh) {
			checking_connect(x, y, 5, who_ogh);
		}
		drag_point = false;
		drag_line = false;
		drag_tri = false;
		drag_rect = false;
		drag_ogh = false;
	}
}
GLvoid Motion(int x, int y) {
	//���������� erase(who_???) �̷������� ���� -> �� ���� ���콺 ��ǥ�� ���ο� ���� emplace_back���ְ� timer �����ϸ� �� ��
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	float exmove_x = 0.0f, exmove_y = 0.0f;
	if (drag_point) {
		g_points[who_point].m_move.y = my;
		g_points[who_point].m_move.x = mx;
		g_points[who_point].Draw();
		mouse_x = mx;
		mouse_y = my;
	}
	if (drag_line) {
		g_lines[who_line].m_move.y = my;
		g_lines[who_line].m_move.x = mx;
		g_lines[who_line].Draw();
		mouse_x = mx;
		mouse_y = my;
	}
	if (drag_tri) {
		if (mouse_x > mx) {
			g_triangles[who_tri].m_move.x -= mouse_x - mx;
		}
		if (mouse_x < mx) {
			g_triangles[who_tri].m_move.x += mx - mouse_x;
		}
		if (mouse_y > my) {
			g_triangles[who_tri].m_move.y -= mouse_y - my;
		}
		if (mouse_y < my) {
			g_triangles[who_tri].m_move.y += my - mouse_y;
		}
		g_triangles[who_tri].Draw();
		mouse_x = mx;
		mouse_y = my;
	}
	if (drag_rect) {
		if (mouse_x > mx) {
			g_rects[who_rect].m_move.x -= mouse_x - mx;
		}
		if (mouse_x < mx) {
			g_rects[who_rect].m_move.x += mx - mouse_x;
		}
		if (mouse_y > my) {
			g_rects[who_rect].m_move.y -= mouse_y - my;
		}
		if (mouse_y < my) {
			g_rects[who_rect].m_move.y += my - mouse_y;
		}
		g_rects[who_rect].Draw();
		mouse_x = mx;
		mouse_y = my;
	}
	if (drag_ogh) {
		if (mouse_x > mx) {
			g_oghs[who_ogh].m_move.x -= mouse_x - mx;
		}
		if (mouse_x < mx) {
			g_oghs[who_ogh].m_move.x += mx - mouse_x;
		}
		if (mouse_y > my) {
			g_oghs[who_ogh].m_move.y -= mouse_y - my;
		}
		if (mouse_y < my) {
			g_oghs[who_ogh].m_move.y += my - mouse_y;
		}
		g_oghs[who_ogh].Draw();
		mouse_x = mx;
		mouse_y = my;
	}
	glutPostRedisplay();
}
void Timer(int val){
	switch (val){
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		for (int i = 0;i < g_points.size();i++) {
			if (g_points[i].Move == 1) {
				if (g_points[i].cnt % 2 == 0) {
					g_points[i].Move_right();
					if (g_points[i].m_move.x  + 0.02f>= 1) {
						if (g_points[i].cnt1 % 2 == 0) {
							g_points[i].Move_double_up();
							if (g_points[i].m_move.y + 0.02f >= 1) {
								g_points[i].cnt1++;
							}
						}
						else {
							g_points[i].Move_double_down();
							if (g_points[i].m_move.y - 0.02f <= -1) {
								g_points[i].cnt1++;
							}
						}
						g_points[i].cnt++;
					}
				}
				else {
					g_points[i].Move_left();
					if (g_points[i].m_move.x - 0.02f <= -1) {
						if (g_points[i].cnt1 % 2 == 0) {
							g_points[i].Move_double_up();
							if (g_points[i].m_move.y + 0.02f >= 1) {
								g_points[i].cnt1++;
							}
						}
						else {
							g_points[i].Move_double_down();
							if (g_points[i].m_move.y - 0.02f <= -1) {
								g_points[i].cnt1++;
							}
						}
						g_points[i].cnt++;
					}
				}
			}
		}
		for (int i = 0;i < g_lines.size();i++) {
			if (g_lines[i].Move == 1) {
				if (g_lines[i].cnt % 2 == 0) {
					g_lines[i].Move_right();
					if (g_lines[i].m_move.x + 0.1f >= 1) {
						if (g_lines[i].cnt1 % 2 == 0) {
							g_lines[i].Move_double_up();
							if (g_lines[i].m_move.y + 0.02f >= 1) {
								g_lines[i].cnt1++;
							}
						}
						else {
							g_lines[i].Move_double_down();
							if (g_lines[i].m_move.y - 0.02f<= -1) {
								g_lines[i].cnt1++;
							}
						}
						g_lines[i].cnt++;
					}
				}
				else {
					g_lines[i].Move_left();
					if (g_lines[i].m_move.x - 0.1f <= -1) {
						if (g_lines[i].cnt1 % 2 == 0) {
							g_lines[i].Move_double_up();
							if (g_lines[i].m_move.y + 0.02f>= 1) {
								g_lines[i].cnt1++;
							}
						}
						else {
							g_lines[i].Move_double_down();
							if (g_lines[i].m_move.y - 0.02f<= -1) {
								g_lines[i].cnt1++;
							}
						}
						g_lines[i].cnt++;
					}
				}
			}
		}
		for (int i = 0;i < g_triangles.size();i++) {
			if (g_triangles[i].Move == 1) {
				if (g_triangles[i].cnt % 2 == 0) {
					g_triangles[i].Move_right();
					if (g_triangles[i].m_move.x + 0.08f>= 1) {
						if (g_triangles[i].cnt1 % 2 == 0) {
							g_triangles[i].Move_double_up();
							if (g_triangles[i].m_move.y + 0.1f>= 1) {
								g_triangles[i].cnt1++;
							}
						}
						else {
							g_triangles[i].Move_double_down();
							if (g_triangles[i].m_move.y - 0.06f<= -1) {
								g_triangles[i].cnt1++;
							}
						}
						g_triangles[i].cnt++;
					}
				}
				else {
					g_triangles[i].Move_left();
					if (g_triangles[i].m_move.x - 0.08f<= -1) {
						if (g_triangles[i].cnt1 % 2 == 0) {
							g_triangles[i].Move_double_up();
							if (g_triangles[i].m_move.y + 0.1f >= 1) {
								g_triangles[i].cnt1++;
							}
						}
						else {
							g_triangles[i].Move_double_down();
							if (g_triangles[i].m_move.y - 0.06f <= -1) {
								g_triangles[i].cnt1++;
							}
						}
						g_triangles[i].cnt++;
					}
				}
			}
		}
		for (int i = 0;i < g_rects.size();i++) {
			if (g_rects[i].Move == 1) {
				if (g_rects[i].cnt % 2 == 0) {
					g_rects[i].Move_right();
					if (g_rects[i].m_move.x + 0.1f>= 1) {
						if (g_rects[i].cnt1 % 2 == 0) {
							g_rects[i].Move_double_up();
							if (g_rects[i].m_move.y + 0.1f>= 1) {
								g_rects[i].cnt1++;
							}
						}
						else {
							g_rects[i].Move_double_down();
							if (g_rects[i].m_move.y - 0.1f<= -1) {
								g_rects[i].cnt1++;
							}
						}
						g_rects[i].cnt++;
					}
				}
				else {
					g_rects[i].Move_left();
					if (g_rects[i].m_move.x - 0.1f<= -1) {
						if (g_rects[i].cnt1 % 2 == 0) {
							g_rects[i].Move_double_up();
							if (g_rects[i].m_move.y + 0.1f >= 1) {
								g_rects[i].cnt1++;
							}
						}
						else {
							g_rects[i].Move_double_down();
							if (g_rects[i].m_move.y - 0.1f <= -1) {
								g_rects[i].cnt1++;
							}
						}
						g_rects[i].cnt++;
					}
				}
			}
		}
		for (int i = 0;i < g_oghs.size();i++) {
			if (g_oghs[i].Move == 1) {
				if (g_oghs[i].cnt % 2 == 0) {
					g_oghs[i].Move_right();
					if (g_oghs[i].m_move.x +0.08f>= 1) {
						if (g_oghs[i].cnt1 % 2 == 0) {
							g_oghs[i].Move_double_up();
							if (g_oghs[i].m_move.y +0.1f>= 1) {
								g_oghs[i].cnt1++;
							}
						}
						else {
							g_oghs[i].Move_double_down();
							if (g_oghs[i].m_move.y -0.06f<= -1) {
								g_oghs[i].cnt1++;
							}
						}
						g_oghs[i].cnt++;
					}
				}
				else {
					g_oghs[i].Move_left();
					if (g_oghs[i].m_move.x - 0.08f<= -1) {
						if (g_oghs[i].cnt1 % 2 == 0) {
							g_oghs[i].Move_double_up();
							if (g_oghs[i].m_move.y + 0.1f >= 1) {
								g_oghs[i].cnt1++;
							}
						}
						else {
							g_oghs[i].Move_double_down();
							if (g_oghs[i].m_move.y - 0.06f <= -1) {
								g_oghs[i].cnt1++;
							}
						}
						g_oghs[i].cnt++;
					}
				}
			}
		}
		glutTimerFunc(17, Timer, 1);
		break;
	case 2:
		for (int i = 0;i < g_points.size();i++) {
			if (g_points[i].Move == 2) {
				if (g_points[i].cnt % 2 == 0) {
					g_points[i].Move_right();
					if (g_points[i].m_move.x + 0.02f >= 1) {
						g_points[i].cnt++;
					}
					if (g_points[i].cnt1 % 2 == 0) {
						g_points[i].Move_up();
						if (g_points[i].m_move.y + 0.02f >= 1) {
							g_points[i].cnt1++;
						}
					}
					else {
						g_points[i].Move_down();
						if (g_points[i].m_move.y - 0.02f <= -1) {
							g_points[i].cnt1++;
						}
					}
				}
				else {
					g_points[i].Move_left();
					if (g_points[i].m_move.x - 0.02f <= -1) {
						g_points[i].cnt++;
					}
					if (g_points[i].cnt1 % 2 == 0) {
						g_points[i].Move_up();
						if (g_points[i].m_move.y + 0.02f >= 1) {
							g_points[i].cnt1++;
						}
					}
					else {
						g_points[i].Move_down();
						if (g_points[i].m_move.y - 0.02f <= -1) {
							g_points[i].cnt1++;
						}
					}
				}
			}
		}
		for (int i = 0;i < g_lines.size();i++) {
			if (g_lines[i].Move == 2) {
				if (g_lines[i].cnt % 2 == 0) {
					g_lines[i].Move_right();
					if (g_lines[i].m_move.x + 0.1f >= 1) {
						g_lines[i].cnt++;
					}
					if (g_lines[i].cnt1 % 2 == 0) {
						g_lines[i].Move_up();
						if (g_lines[i].m_move.y + 0.02f >= 1) {
							g_lines[i].cnt1++;
						}
					}
					else {
						g_lines[i].Move_down();
						if (g_lines[i].m_move.y - 0.02f <= -1) {
							g_lines[i].cnt1++;
						}
					}
				}
				else {
					g_lines[i].Move_left();
					if (g_lines[i].m_move.x - 0.1f <= -1) {
						g_lines[i].cnt++;
					}
					if (g_lines[i].cnt1 % 2 == 0) {
						g_lines[i].Move_up();
						if (g_lines[i].m_move.y + 0.02f >= 1) {
							g_lines[i].cnt1++;
						}
					}
					else {
						g_lines[i].Move_down();
						if (g_lines[i].m_move.y - 0.02f <= -1) {
							g_lines[i].cnt1++;
						}
					}
				}
			}
		}
		for (int i = 0;i < g_triangles.size();i++) {
			if (g_triangles[i].Move == 2) {
				if (g_triangles[i].cnt % 2 == 0) {
					g_triangles[i].Move_right();
					if (g_triangles[i].m_move.x + 0.08f >= 1) {
						g_triangles[i].cnt++;
					}
					if (g_triangles[i].cnt1 % 2 == 0) {
						g_triangles[i].Move_up();
						if (g_triangles[i].m_move.y + 0.1f >= 1) {
							g_triangles[i].cnt1++;
						}
					}
					else {
						g_triangles[i].Move_down();
						if (g_triangles[i].m_move.y - 0.06f <= -1) {
							g_triangles[i].cnt1++;
						}
					}
				}
				else {
					g_triangles[i].Move_left();
					if (g_triangles[i].m_move.x - 0.08f <= -1) {
						g_triangles[i].cnt++;
					}
					if (g_triangles[i].cnt1 % 2 == 0) {
						g_triangles[i].Move_up();
						if (g_triangles[i].m_move.y + 0.1f >= 1) {
							g_triangles[i].cnt1++;
						}
					}
					else {
						g_triangles[i].Move_down();
						if (g_triangles[i].m_move.y - 0.06f <= -1) {
							g_triangles[i].cnt1++;
						}
					}
				}
			}
		}
		for (int i = 0;i < g_rects.size();i++) {
			if (g_rects[i].Move == 2) {
				if (g_rects[i].cnt % 2 == 0) {
					g_rects[i].Move_right();
					if (g_rects[i].m_move.x + 0.1f >= 1) {
						g_rects[i].cnt++;
					}
					if (g_rects[i].cnt1 % 2 == 0) {
						g_rects[i].Move_up();
						if (g_rects[i].m_move.y + 0.1f >= 1) {
							g_rects[i].cnt1++;
						}
					}
					else {
						g_rects[i].Move_down();
						if (g_rects[i].m_move.y - 0.1f <= -1) {
							g_rects[i].cnt1++;
						}
					}
				}
				else {
					g_rects[i].Move_left();
					if (g_rects[i].m_move.x - 0.1f <= -1) {
						g_rects[i].cnt++;
					}
					if (g_rects[i].cnt1 % 2 == 0) {
						g_rects[i].Move_up();
						if (g_rects[i].m_move.y + 0.1f >= 1) {
							g_rects[i].cnt1++;
						}
					}
					else {
						g_rects[i].Move_down();
						if (g_rects[i].m_move.y - 0.1f <= -1) {
							g_rects[i].cnt1++;
						}
					}
				}
			}
		}
		for (int i = 0;i < g_oghs.size();i++) {
			if (g_oghs[i].Move == 2) {
				if (g_oghs[i].cnt % 2 == 0) {
					g_oghs[i].Move_right();
					if (g_oghs[i].m_move.x + 0.08f >= 1) {
						g_oghs[i].cnt++;
					}
					if (g_oghs[i].cnt1 % 2 == 0) {
						g_oghs[i].Move_up();
						if (g_oghs[i].m_move.y + 0.1f >= 1) {
							g_oghs[i].cnt1++;
						}
					}
					else {
						g_oghs[i].Move_down();
						if (g_oghs[i].m_move.y - 0.06f <= -1) {
							g_oghs[i].cnt1++;
						}
					}
				}
				else {
					g_oghs[i].Move_left();
					if (g_oghs[i].m_move.x - 0.08f <= -1) {
						g_oghs[i].cnt++;
					}
					if (g_oghs[i].cnt1 % 2 == 0) {
						g_oghs[i].Move_up();
						if (g_oghs[i].m_move.y + 0.1f >= 1) {
							g_oghs[i].cnt1++;
						}
					}
					else {
						g_oghs[i].Move_down();
						if (g_oghs[i].m_move.y - 0.06f <= -1) {
							g_oghs[i].cnt1++;
						}
					}
				}
			}
		}
		glutTimerFunc(17, Timer, 2);
		break;
	default:
		break;
	}
}