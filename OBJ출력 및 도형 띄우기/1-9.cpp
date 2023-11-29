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
	glutTimerFunc(17, TimerFunction, 1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	for (auto& t : g_triangles)t.Draw(D_SOLID);
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
GLvoid TimerFunction(int value) { //Ÿ�̸� �ȿ��� switch�� �̻ڴ�
	switch (what) {
	case 1: //�밢�� ƨ���
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
	case 2: //�������
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
	case 3: //�簢 �����̷�
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
	case 4: //�� �����̷�
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