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
int num = 0;
int linenum = 0;
int what = 1;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
float r = 1.0, g = 1.0, b = 1.0;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
enum DRAW_CASE {
	D_SOLID,
	D_LINE
};
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}
struct Triangle {
	glm::vec3 m_vertex[3];
	glm::vec3 m_color[3];
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
		m_vertex[0].y = center.second + 0.5f;
		m_vertex[1].x = center.first - 0.3f;
		m_vertex[1].y = center.second - 0.3f;
		m_vertex[2].x = center.first + 0.3f;
		m_vertex[2].y = center.second - 0.3f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
		SetVAO();
	}
	void NewTri(int x,int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		uniform_real_distribution<float> size(0, 0.5);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		float si = size(dre);
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
		m_vertex[0].y = center.second + 0.5f;
		m_vertex[1].x = center.first - si;
		m_vertex[1].y = center.second - si;
		m_vertex[2].x = center.first + si;
		m_vertex[2].y = center.second - si;
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
	void Draw(int type) {
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glUseProgram(shaderProgramID);
		glBindVertexArray(m_VAO);
		switch (type){
		case D_SOLID:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case D_LINE:
			glLineWidth(6.0f);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
			break;
		default:
			break;
		}
	}
};
vector<Triangle> g_triangles;
struct Line {
	glm::vec3 m_vertex[2];
	glm::vec3 m_color[2];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Line(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		center;
		if (linenum == 0) {
			m_vertex[0].x = center.first + 1.0f;
			m_vertex[0].y = center.second;
			m_vertex[1].x = center.first - 1.0f;
			m_vertex[1].y = center.second;
			linenum++;
		}
		else if (linenum == 1) {
			m_vertex[0].x = center.first;
			m_vertex[0].y = center.second + 1.0f;
			m_vertex[1].x = center.first;
			m_vertex[1].y = center.second - 1.0f;
			linenum++;
		}
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
		glUniform3f(vColorLocation, 0.0f, 0.0f, 0.0f);
		glBindVertexArray(m_VAO);
		glLineWidth(5.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);
	}
};
vector<Line> g_lines;
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
	glutMouseFunc(ClickFunc);
	g_triangles.emplace_back(200, 150);
	g_triangles.emplace_back(600, 150);
	g_triangles.emplace_back(200, 450);
	g_triangles.emplace_back(600, 450);
	g_lines.emplace_back(400, 300);
	g_lines.emplace_back(400, 300);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	if (what == 1) {
		for (auto& t : g_triangles)t.Draw(D_SOLID);
	}
	if (what == 2) {
		for (auto& t : g_triangles)t.Draw(D_LINE);
	}
	for (auto& t : g_lines)t.Draw();
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
		if (x < 400 && y < 300) {
			g_triangles[0].NewTri(x,y);
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
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		what = 1;
		break;
	case 'b':
		what = 2;
		break;
	}
	glutPostRedisplay();
}