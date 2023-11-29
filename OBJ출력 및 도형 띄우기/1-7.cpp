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
float r = 1.0, g = 1.0, b = 1.0;
int shape = -1;
int num = 0;
enum DRAW_CASE {
	D_SOLID,
	D_POINT,
	D_LINE
};
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
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
struct Triangle {
	glm::vec3 m_vertex[3];
	glm::vec3 m_color[3];
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
		m_vertex[0].x = center.first;
		m_vertex[0].y = center.second + 0.15f;
		m_vertex[1].x = center.first - 0.11f;
		m_vertex[1].y = center.second - 0.11f;
		m_vertex[2].x = center.first + 0.11f;
		m_vertex[2].y = center.second - 0.11f;
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
	void Move_down() {
		m_move.y -= 0.01f;
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
	Line(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		m_color[0].r = r;
		m_color[1].r = r;
		m_color[0].g = g;
		m_color[1].g = g;
		m_color[0].b = b;
		m_color[1].b = b;
		center;
		m_vertex[0].x = center.first + 0.11f;
		m_vertex[0].y = center.second;
		m_vertex[1].x = center.first - 0.11f;
		m_vertex[1].y = center.second;
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
		glLineWidth(15.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};
vector<Line> g_lines;
struct Point {
	glm::vec3 m_vertex[1];
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
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
		m_vertex[0].x = center.first;
		m_vertex[0].y = center.second;
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
		glPointSize(15.0);
		glDrawArrays(GL_POINTS, 0, 1);
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};
vector<Point> g_points;
struct Rect {
	glm::vec3 m_vertex[4];
	glm::vec3 m_color[4];
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
		m_vertex[1].x = center.first - 0.11f;
		m_vertex[1].y = center.second + 0.11f;
		m_vertex[0].x = center.first - 0.11f;
		m_vertex[0].y = center.second - 0.11f;
		m_vertex[2].x = center.first + 0.11f;
		m_vertex[2].y = center.second - 0.11f;
		m_vertex[3].x = center.first + 0.11f;
		m_vertex[3].y = center.second + 0.11f;
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
		glUniform3f(vColorLocation, 0.0f, 0.0f, 0.0f);
		glLineWidth(6.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
		glDrawArrays(GL_LINE_LOOP, 1, 3);
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};
vector<Rect> g_rects;
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
	glutMouseFunc(ClickFunc);
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	for (auto& p : g_points)p.Draw();
	for (auto& s : g_lines)s.Draw();
	for (auto& t : g_triangles)t.Draw();
	for (auto& v : g_rects)v.Draw();
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
GLvoid ClickFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		num++;
		if (num < 11) {
			if (shape == 0) {
				g_points.emplace_back(x, y);
			}
			if (shape == 1) {
				g_lines.emplace_back(x, y);
			}
			if (shape == 2) {
				g_triangles.emplace_back(x, y);
			}
			if (shape == 3) {
				g_rects.emplace_back(x, y);
			}
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
		shape = 0;
		break;
	case 'l':
		shape = 1;
		break;
	case 't':
		shape = 2;
		break;
	case 'r':
		shape = 3;
		break;
	case 'w'://��
		for (int i = 0;i < g_triangles.size();i++) {
			g_triangles[i].Move_up();
		}
		for (int i = 0;i < g_lines.size();i++) {
			g_lines[i].Move_up();
		}
		for (int i = 0;i < g_points.size();i++) {
			g_points[i].Move_up();
		}
		for (int i = 0;i < g_rects.size();i++) {
			g_rects[i].Move_up();
		}
		glutPostRedisplay();
		break;
	case 'a'://��
		for (int i = 0;i < g_triangles.size();i++) {
			g_triangles[i].Move_left();
		}
		for (int i = 0;i < g_lines.size();i++) {
			g_lines[i].Move_left();
		}
		for (int i = 0;i < g_points.size();i++) {
			g_points[i].Move_left();
		}
		for (int i = 0;i < g_rects.size();i++) {
			g_rects[i].Move_left();
		}
		glutPostRedisplay();
		break;
	case 's'://�Ʒ�
		for (int i = 0;i < g_triangles.size();i++) {
			g_triangles[i].Move_down();
		}
		for (int i = 0;i < g_lines.size();i++) {
			g_lines[i].Move_down();
		}
		for (int i = 0;i < g_points.size();i++) {
			g_points[i].Move_down();
		}
		for (int i = 0;i < g_rects.size();i++) {
			g_rects[i].Move_down();
		}
		glutPostRedisplay();
		break;
	case 'd'://��
		for (int i = 0;i < g_triangles.size();i++) {
			g_triangles[i].Move_right();
		}
		for (int i = 0;i < g_lines.size();i++) {
			g_lines[i].Move_right();
		}
		for (int i = 0;i < g_points.size();i++) {
			g_points[i].Move_right();
		}
		for (int i = 0;i < g_rects.size();i++) {
			g_rects[i].Move_right();
		}
		glutPostRedisplay();
		break;
	case 'c':
		g_triangles.clear();
		g_lines.clear();
		g_points.clear();
		g_rects.clear();
		num = 0;
		glutPostRedisplay();
		break;
	}
}