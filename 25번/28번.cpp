#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/gtc/type_ptr.hpp>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
using namespace std;
void ReadDObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& vertexNomal)
{
	std::vector<glm::vec4> vertexFace;
	std::vector<glm::vec4> vertexNomalFace;
	std::vector<glm::vec4> vertexData; //���ؽ� �� ����� ����
	std::vector<glm::vec4> vertexNomalData; //���ؽ� �� ����� ����
	std::string line;
	std::ifstream inFile(objfilename);
	char head[3] = { 0,0,0 };
	int vertexFaceNum[3] = { 0,0,0 };
	int vertexnomalFaceNum[3] = { 0,0,0 };
	std::string nt;
	while (inFile >> std::noskipws >> head[0]) { //noskipws = �Ⱥ�ĭ�� �ðڴ�,skipws = ��ĭ�� �ðڴ�
		if (head[0] == 'v') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				float x = 0, y = 0, z = 0; //�� �������� ����
				inFile >> std::skipws >> x >> y >> z; //��������
				glm::vec4 vertexD = { x,y,z,1 }; //���Ϳ� �����ϰ�
				vertexData.push_back(vertexD); //push_back���� �־��ֱ�
			}
			else if (head[1] == 'n') {
				inFile >> std::noskipws >> head[2];
				if (head[2] == ' ') {
					float x = 0, y = 0, z = 0; //�� �������� ����
					inFile >> std::skipws >> x >> y >> z; //��������
					glm::vec4 vertexN = { x,y,z,1 }; //���Ϳ� �����ϰ�
					vertexNomalData.push_back(vertexN); //push_back���� �־��ֱ�
				}
			}
			else {
				head[1] = '\0';
			}
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				char tempC; // "/" �޾��ִ� ����
				int tempi[3]; //������ �븻���Ͷ� �ؽ��� �Ⱦ��� ������ ������ó��
				inFile >> std::skipws >> vertexFaceNum[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexnomalFaceNum[0];

				inFile >> std::skipws >> vertexFaceNum[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexnomalFaceNum[1];

				inFile >> std::skipws >> vertexFaceNum[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexnomalFaceNum[2];

				glm::vec4 vertexFaceTemp = { vertexFaceNum[0], vertexFaceNum[1], vertexFaceNum[2], 1 };
				glm::vec4 vertexnomalFaceTemp = { vertexnomalFaceNum[0], vertexnomalFaceNum[1], vertexnomalFaceNum[2], 1 };
				vertexFace.push_back(vertexFaceTemp);
				vertexNomalFace.push_back(vertexnomalFaceTemp);
			}
			head[1] = '\0';
		}
	}
	for (auto iter = vertexFace.begin(); iter < vertexFace.end(); iter++) { //�� �����ϴ� �ε����� �ش��ϴ� ��ǥ�� �־��ش�. �������
		vertex.push_back(vertexData[(int)(iter->x) - 1]);
		vertex.push_back(vertexData[(int)(iter->y) - 1]);
		vertex.push_back(vertexData[(int)(iter->z) - 1]);
	}
	for (auto iter = vertexNomalFace.begin(); iter < vertexNomalFace.end(); iter++) {
		vertexNomal.push_back(vertexNomalData[(int)(iter->x) - 1]);
		vertexNomal.push_back(vertexNomalData[(int)(iter->y) - 1]);
		vertexNomal.push_back(vertexNomalData[(int)(iter->z) - 1]);
	}
	inFile.close();
}
char* FileToBuf(const char* file) {
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
int Coc = 0, g_num = 0, g_Shapecnt = 0, g_witch = 0;
float distane = 3.0f;
bool g_falling = false;
void MakeVertexShaders();
void MakeFragmentShaders();
void Timer(int val);
GLuint MakeShaderProgram();
GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

std::vector<glm::vec3> g_vertex; //��
std::vector<glm::vec3> g_normal; //��

std::vector<glm::vec3> p_vertex; //��
std::vector<glm::vec3> p_normal; //��

std::vector<glm::vec3> l_vertex; //��
std::vector<glm::vec3> l_normal; //��

glm::vec3 cameraPos = glm::vec3(15.0f, 10.0f, 6.0f); //--- ī�޶� ��ġ
glm::mat4 A_R_matrix = glm::mat4(1.0f);
glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f); //�븻����

struct Shape {
	glm::vec3 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	Shape(int type) {
		switch (type) {
		case 0:
			m_color[0].r = 0.3f;
			m_color[0].g = 0.0f;
			m_color[0].b = 0.3f;
			S_matrix = glm::scale(S_matrix, glm::vec3(4.0f, 4.0f, 4.0f));
			break;
		case 1:
			m_color[0].r = 0.3f;
			m_color[0].g = 0.0f;
			m_color[0].b = 0.3f;
			S_matrix = glm::mat4(1.0f);
			break;
		}
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * p_vertex.size(), &p_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * p_normal.size(), &p_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);
		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //�θ� �־��ֱ�
		unsigned int ArotateLocation = glGetUniformLocation(shaderProgramID, "uA_Rotate");
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(AR_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 0, p_vertex.size());
		glUniform3f(vColorLocation, 0, 0, 0);
		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, p_vertex.size());
	}
};
vector<Shape>g_Shape;

struct Ground { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	Ground() {
		m_color[0].r = 0.2;
		m_color[0].g = 0.2;
		m_color[0].b = 0.2;
		S_matrix = glm::scale(S_matrix, glm::vec3(10.0f, 1.0f, 10.0f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, -0.5f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_vertex.size(), &g_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_normal.size(), &g_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);

		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //�θ� �־��ֱ�
		unsigned int ArotateLocation = glGetUniformLocation(shaderProgramID, "uA_Rotate");
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(AR_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 6, 6);
	}
};
vector<Ground> g_Grounds;

struct light { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_color[1];
	glm::vec4 m_move = { 0,0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 A_R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	light() {
		m_color[0].r = 1.0f;
		m_color[0].g = 1.0f;
		m_color[0].b = 0.0f;
		m_move.x = 0.0f;
		m_move.y = 3.0f;
		m_move.z = 3.0f;
		m_move.w = 1;
		S_matrix = glm::scale(S_matrix, glm::vec3(0.1f, 0.1f, 0.1f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 3.0f, 3.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_vertex.size(), &l_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_normal.size(), &l_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);
		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //�θ� �־��ֱ�
		unsigned int ArotateLocation = glGetUniformLocation(shaderProgramID, "uA_Rotate");
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(A_R_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 0, l_vertex.size());
	}
};
vector<light> g_lights;

struct snow { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_color[1];
	glm::vec4 m_move = { 0,0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 A_R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	float recycle_y = 0.0f;
	snow() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> x_pos(-8.0f, 8.0f);
		uniform_real_distribution<float> y_pos(10.0f, 15.0f);
		uniform_real_distribution<float> z_pos(-8.0f, 8.0f);
		m_color[0].r = 1.0f;
		m_color[0].g = 1.0f;
		m_color[0].b = 1.0f;
		recycle_y = y_pos(dre);
		m_move.x = x_pos(dre);
		m_move.y = y_pos(dre);
		m_move.z = z_pos(dre);
		m_move.w = 1;
		S_matrix = glm::scale(S_matrix, glm::vec3(0.03f, 0.03f, 0.03f));
		T_matrix = glm::translate(T_matrix, glm::vec3(x_pos(dre), y_pos(dre), z_pos(dre)));
		SetVBO();
	}
	void SetVBO() {
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_vertex.size(), &l_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_normal.size(), &l_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);
		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //�θ� �־��ֱ�
		unsigned int ArotateLocation = glGetUniformLocation(shaderProgramID, "uA_Rotate");
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(A_R_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 0, l_vertex.size());
	}
};
vector<snow> g_snows;

void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
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
	MakeVertexShaders(); //--- ���ؽ� ���̴� �����
	MakeFragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = MakeShaderProgram();
	ReadDObj("cube(1).obj", g_vertex, g_normal);
	g_Grounds.emplace_back();
	ReadDObj("pyramid.obj", p_vertex, p_normal);
	g_Shape.emplace_back(0);
	ReadDObj("sphere.obj", l_vertex, l_normal);
	g_lights.emplace_back();
	for (int i = 0;i < 30;i++) {
		g_snows.emplace_back();
	}
	glutTimerFunc(17, Timer, 0);
	glutDisplayFunc(DrawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid DrawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderProgramID);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 1000.0f);
	projection *= A_R_matrix;
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	g_lights[0].m_move = glm::vec4(1.0f);
	g_lights[0].m_move = g_lights[0].parentMatrix * g_lights[0].A_R_matrix * g_lights[0].T_matrix * g_lights[0].R_matrix * g_lights[0].S_matrix * g_lights[0].m_move;
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, g_lights[0].m_move.x, g_lights[0].m_move.y, g_lights[0].m_move.z);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light_color.x, light_color.y, light_color.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
	glm::vec3 dist = glm::vec3(distane, distane, distane);
	unsigned int disPosLocation = glGetUniformLocation(shaderProgramID, "dis");
	glUniform3f(disPosLocation, dist.x, dist.y, dist.z);
	glEnable(GL_CULL_FACE);
	glEnable(GL_CW);
	g_Grounds[0].Draw();
	glEnable(GL_CCW);
	for (auto& t : g_Shape) {
		t.Draw();
	}
	for (auto& a : g_snows) {
		a.Draw();
	}
	g_lights[0].Draw();
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}

void MakeVertexShaders() {
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�

	//vertex shader read, save vertexSource
	vertexSource = FileToBuf("vertex1.glsl");

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

void MakeFragmentShaders() {
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = FileToBuf("fragment1.glsl"); // �����׼��̴� �о����
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

GLuint MakeShaderProgram() {
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

void fractal_triangles(int level, float high, float wide , float x,float y,float z) {
	if (g_num == 0) {
		g_Shape.clear();
		g_Shapecnt = 0;
		g_num++;
	}
	if (level == 0) {
		g_Shape.emplace_back(1);
		g_Shape[g_Shapecnt].T_matrix = glm::translate(g_Shape[g_Shapecnt].T_matrix, glm::vec3(x, y, z));
		g_Shapecnt++;
	}
	else {
		fractal_triangles(level-1, high/2, wide/2, x + wide/2, y, z + wide/2);
		fractal_triangles(level-1, high/2, wide/2, x - wide/2, y, z + wide/2);
		fractal_triangles(level-1, high/2, wide/2, x + wide/2, y, z - wide/2);
		fractal_triangles(level-1, high/2, wide/2, x - wide/2, y, z - wide/2);
		fractal_triangles(level-1, high/2, wide/2, x, y + high/2, z);
	}
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> color(0, 2);
	uniform_int_distribution<int> pos(0, 3);
	switch (key) {
	case 's':
		if (!g_falling) {
			g_falling = true;
			glutTimerFunc(17, Timer, 1);
		}
		else {
			g_falling = false;
		}
		break;
	case '+':
		light_color.x += 0.1f;
		light_color.y += 0.1f;
		light_color.z += 0.1f;
		if (light_color.x > 1.0f) {
			light_color.x = 1.0f;
		}
		if (light_color.y > 1.0f) {
			light_color.y = 1.0f;
		}
		if (light_color.z > 1.0f) {
			light_color.z = 1.0f;
		}
		break;
	case '-':
		light_color.x -= 0.1f;
		light_color.y -= 0.1f;
		light_color.z -= 0.1f;
		if (light_color.x < 0.0f) {
			light_color.x = 0.0f;
		}
		if (light_color.y < 0.0f) {
			light_color.y = 0.0f;
		}
		if (light_color.z < 0.0f) {
			light_color.z = 0.0f;
		}
		break;
	case 'n':
		g_lights[0].A_R_matrix = glm::mat4(1.0f);
		g_lights[0].m_move.z -= 0.1f;
		g_lights[0].T_matrix = glm::translate(g_lights[0].T_matrix, glm::vec3(0.0f, 0.0f, -0.1f));
		distane = glm::distance(glm::vec3(g_lights[0].m_move), glm::vec3(0, 0, 0));
		break;
	case 'f':
		g_lights[0].A_R_matrix = glm::mat4(1.0f);
		g_lights[0].m_move.z += 0.1f;
		g_lights[0].T_matrix = glm::translate(g_lights[0].T_matrix, glm::vec3(0.0f, 0.0f, 0.1f));
		distane = glm::distance(glm::vec3(g_lights[0].m_move), glm::vec3(0, 0, 0));
		break;
	case 'r':
		g_lights[0].A_R_matrix = glm::rotate(g_lights[0].A_R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 'R':
		g_lights[0].A_R_matrix = glm::rotate(g_lights[0].A_R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case '1':
		g_num = 0;
		fractal_triangles(0, 3.2, 1.6, 0, 0, 0);
		for (auto& a : g_Shape) {
			a.S_matrix = glm::scale(a.S_matrix, glm::vec3(4.0f, 4.0f, 4.0f));
		}
		break;
	case '2':
		g_num = 0;
		fractal_triangles(1, 3.2, 1.6, 0, 0, 0);
		for (auto& a : g_Shape) {
			a.S_matrix = glm::scale(a.S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		}
		break;
	case '3':
		g_num = 0;
		fractal_triangles(2, 3.2, 1.6, 0, 0, 0);
		for (auto& a : g_Shape) {
			a.S_matrix = glm::scale(a.S_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
		}
		break;
	case '4':
		g_num = 0;
		fractal_triangles(3, 3.2, 1.6, 0, 0, 0);
		for (auto& a : g_Shape) {
			a.S_matrix = glm::scale(a.S_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
		}
		break;
	case '5':
		g_num = 0;
		fractal_triangles(4, 3.2, 1.6, 0, 0, 0);
		for (auto& a : g_Shape) {
			a.S_matrix = glm::scale(a.S_matrix, glm::vec3(0.25f, 0.25f, 0.25f));
		}
		break;
	case '6':
		g_num = 0;
		fractal_triangles(5, 3.2, 1.6, 0, 0, 0);
		for (auto& a : g_Shape) {
			a.S_matrix = glm::scale(a.S_matrix, glm::vec3(0.125f, 0.125f, 0.125f));
		}
		break;
	case 'x':
		cameraPos.x += 0.1f;
		break;
	case 'X':
		cameraPos.x -= 0.1f;
		break;
	case 'y':
		cameraPos.y += 0.1f;
		break;
	case 'Y':
		cameraPos.y -= 0.1f;
		break;
	case 'Z':
		cameraPos.z -= 0.1f;
		break;
	case 'z':
		cameraPos.z += 0.1f;
		break;
	case 'm':
		light_color.x = 1.0f;
		light_color.y = 1.0f;
		light_color.z = 1.0f;
		break;
	case 'M':
		light_color.x = 0.0f;
		light_color.y = 0.0f;
		light_color.z = 0.0f;
		break;
	case 'c':
	case 'C':
		Coc = color(dre);
		switch (Coc) {
		case 0:
			light_color.x = 1.0f;
			light_color.y = 1.0f;
			light_color.z = 0.0f;
			break;
		case 1:
			light_color.x = 0.0f;
			light_color.y = 1.0f;
			light_color.z = 1.0f;
			break;
		case 2:
			light_color.x = 1.0f;
			light_color.y = 0.0f;
			light_color.z = 1.0f;
			break;
		}
		break;
	case 'p':
	case 'P':
		g_witch = pos(dre);
		switch (g_witch) {
		case 0:
			g_lights[0].T_matrix = glm::mat4(1.0f);
			g_lights[0].m_move.x = 0.0f;
			g_lights[0].m_move.y = 3.0f;
			g_lights[0].m_move.z = 3.0f;
			g_lights[0].T_matrix = glm::translate(g_lights[0].T_matrix, glm::vec3(0.0f, 3.0f, 3.0f));
			break;
		case 1:
			g_lights[0].T_matrix = glm::mat4(1.0f);
			g_lights[0].m_move.x = 0.0f;
			g_lights[0].m_move.y = 3.0f;
			g_lights[0].m_move.z = -3.0f;
			g_lights[0].T_matrix = glm::translate(g_lights[0].T_matrix, glm::vec3(0.0f, 3.0f, -3.0f));
			break;
		case 2:
			g_lights[0].T_matrix = glm::mat4(1.0f);
			g_lights[0].m_move.x = 3.0f;
			g_lights[0].m_move.y = 3.0f;
			g_lights[0].m_move.z = 0.0f;
			g_lights[0].T_matrix = glm::translate(g_lights[0].T_matrix, glm::vec3(3.0f, 3.0f, 0.0f));
			break;
		case 3:
			g_lights[0].T_matrix = glm::mat4(1.0f);
			g_lights[0].m_move.x = -3.0f;
			g_lights[0].m_move.y = 3.0f;
			g_lights[0].m_move.z = 0.0f;
			g_lights[0].T_matrix = glm::translate(g_lights[0].T_matrix, glm::vec3(-3.0f, 3.0f, 0.0f));
			break;
		}
		break;
	case 'q':
	case 'Q':
		exit(1);
		break;
	}
}

void Timer(int val)
{
	switch (val)
	{
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		if (g_falling) {
			for (auto& a : g_snows) {
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, -0.1f, 0.0f));
				a.m_move.y -= 0.1f;
				if (a.m_move.y <= 0.0f) {
					a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, a.recycle_y, 0.0f));
					a.m_move.y = a.recycle_y;
				}
			}
			glutTimerFunc(17, Timer, 1);
		}
		break;
	default:
		break;
	}
}