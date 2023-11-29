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
#include <math.h>
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
float mouse_x = 0;
float mouse_y = 0;
int who = -1;
bool drag=false , drag1 = false;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid drawScene(GLvoid);
GLvoid Motion(int x, int y);
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
		m_move.x = center.first ;
		m_move.y = center.second;
		m_vertex[0].x = -0.5f;
		m_vertex[0].y = -0.5f;
		m_vertex[1].x = -0.5f;
		m_vertex[1].y = +0.5f;
		m_vertex[2].x = +0.5f;
		m_vertex[2].y = -0.5f;
		m_vertex[3].x = +0.5f;
		m_vertex[3].y = +0.5f;
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
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glBindVertexArray(m_VAO);
		glLineWidth(6.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
		glDrawArrays(GL_LINE_LOOP, 1, 3);
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
	glutMotionFunc(Motion);
	g_rects.emplace_back(400, 300);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
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
bool isPointInsideRect(float Ax, float Bx, float Cx, float Dx,float Px, float Ay, float By, float Cy, float Dy,float Py) {
	float area = 0.0f;
	float width = 0.0f, height = 0.0f;
	if (fabs(Cx - Ax) > fabs(Dx - Bx)) {
		width = fabs((Cx - Ax));
	}
	else {
		width = fabs(Dx - Bx);
	}
	if (fabs(By - Ay) > fabs(Dy - Cy)) {
		height = fabs(By - Ay);
	}
	else {
		height = fabs(Dy - Cy);
	}
	area = width * height;
	float area1 = fabs((Ax - Px) * (Ay - Py));
	float area2 = fabs((Bx - Px) * (By - Py));
	float area3 = fabs((Cx - Px) * (Cy - Py));
	float area4 = fabs((Dx - Px) * (Dy - Py));
	if (area >= area1 + area2 + area3 + area4 - 0.005f) {
		return true;
	}
	else {
		return false;
	}
}
GLvoid ClickFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //���� ��ư ���� ���
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
		mouse_x = mx;
		mouse_y = my;
		for (int i = 0;i < 4;i++) {
			if (g_rects[0].m_vertex[i].x - 0.1f < mx && mx < g_rects[0].m_vertex[i].x + 0.1f) { //x
				if (g_rects[0].m_vertex[i].y - 0.1f < my && my < g_rects[0].m_vertex[i].y + 0.1f) { //y
					who = i;
					drag = true;
				}
			}
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		drag = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
		mouse_x = mx;
		mouse_y = my;
		if (isPointInsideRect(g_rects[0].m_vertex[0].x, g_rects[0].m_vertex[1].x, g_rects[0].m_vertex[2].x, g_rects[0].m_vertex[3].x, mx, g_rects[0].m_vertex[0].y, g_rects[0].m_vertex[1].y, g_rects[0].m_vertex[2].y, g_rects[0].m_vertex[3].y, my)) {
			drag1 = true;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		drag1 = false;
	}
}
GLvoid Motion(int x, int y) {
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	float exmove_x = 0.0f,exmove_y = 0.0f;
	if (drag) {
		g_rects[0].m_vertex[who].x = mx;
		g_rects[0].m_vertex[who].y = my;
		g_rects[0].Draw();
		mouse_x = mx;
		mouse_y = my;
	}
	if (drag1) {
		g_rects[0].m_move.x = exmove_x;
		g_rects[0].m_move.y = exmove_y;
		if (mouse_x > mx) {
			g_rects[0].m_move.x -= mouse_x - mx;
		}
		if (mouse_x < mx) {
			g_rects[0].m_move.x += mx - mouse_x;
		}
		if (mouse_y > my) {
			g_rects[0].m_move.y -= mouse_y - my;
		}
		if (mouse_y < my) {
			g_rects[0].m_move.y += my - mouse_y;
		}
		g_rects[0].Draw();
		for (int i = 0;i < 4;i++) {
			g_rects[0].m_vertex[i].x -= exmove_x - g_rects[0].m_move.x;
			g_rects[0].m_vertex[i].y -= exmove_y - g_rects[0].m_move.y;
		}
		mouse_x = mx;
		mouse_y = my;
	}
	glutPostRedisplay();
}