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
void ReadDObj(const std::string objfilename, std::vector<glm::vec3>& vertex)
{
	std::vector<glm::vec4> vertexFace;
	std::vector<glm::vec4> vertexData; //���ؽ� �� ����� ����
	std::string line;
	std::ifstream inFile(objfilename);
	char head[3] = { 0,0,0 };
	int vertexFaceNum[3] = { 0,0,0 };
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
			head[1] = '\0';
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				char tempC; // "/" �޾��ִ� ����
				int tempi[6]; //������ �븻���Ͷ� �ؽ��� �Ⱦ��� ������ ������ó��
				inFile >> std::skipws >> vertexFaceNum[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[1];

				inFile >> std::skipws >> vertexFaceNum[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[3];

				inFile >> std::skipws >> vertexFaceNum[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[4];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> tempi[5];

				glm::vec4 vertexFaceTemp = { vertexFaceNum[0], vertexFaceNum[1], vertexFaceNum[2], 1 };
				vertexFace.push_back(vertexFaceTemp);
			}
			head[1] = '\0';
		}
	}
	for (auto iter = vertexFace.begin(); iter < vertexFace.end(); iter++) { //�� �����ϴ� �ε����� �ش��ϴ� ��ǥ�� �־��ش�. �������
		vertex.push_back(vertexData[(int)(iter->x) - 1]);
		vertex.push_back(vertexData[(int)(iter->y) - 1]);
		vertex.push_back(vertexData[(int)(iter->z) - 1]);
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
bool g_isOn = false, g_isOn1 = false, g_isOn2 = false, g_isOn3 = false,g_isOn4 = false;
int g_How_Rotate = 0;
int g_moveD = 0, g_moveT = 0;
int g_Show_Spyral = 0;
int g_num1 = 0, g_num = 0;
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

std::vector<glm::vec3> vertex; //��

std::vector<glm::vec3> p_vertex; //��

struct Dice { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	int m_Cnt_x = 0;
	int m_Cnt_y = 0;
	int m_over = 0;
	Dice() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		S_matrix = glm::scale(S_matrix, glm::vec3(0.8f, 0.8f, 0.8f));
		T_matrix = glm::translate(T_matrix, glm::vec3(-2.0f, 0.0f, -0.25f));
		SetVBO();
	}
	void SetVBO() {
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		unsigned int ArotateLocation = glGetUniformLocation(shaderProgramID, "uA_Rotate");
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(AR_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		for (int i = 0;i < 6;i++) {
			glUniform3f(vColorLocation, m_color[i].r, m_color[i].g, m_color[i].b);
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
		}
	}
};
vector<Dice> g_Dices;

struct pyramid { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	int m_Cnt_x = 0;
	int m_Cnt_y = 0;
	int m_over = 0;
	pyramid() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		S_matrix = glm::scale(S_matrix, glm::vec3(0.8f, 0.8f, 0.8f));
		T_matrix = glm::translate(T_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * p_vertex.size(), &p_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		unsigned int ArotateLocation = glGetUniformLocation(shaderProgramID, "uA_Rotate");
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(AR_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		for (int i = 0;i < 6;i++) {
			glUniform3f(vColorLocation, m_color[i].r, m_color[i].g, m_color[i].b);
			glDrawArrays(GL_TRIANGLES, i * 3, 3);
		}
	}
};
vector<pyramid> g_pyramids;

struct Line {
	glm::vec3 m_vertex[2];
	glm::vec3 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 A_R_matrix = glm::mat4(1.0f);
	Line(int linenum) {
		if (linenum == 0) {
			m_color[0].r = 1;
			m_color[0].g = 0;
			m_color[0].b = 0;
			m_vertex[0].x = +10.0f;
			m_vertex[0].y = 0.0f;
			m_vertex[1].x = -10.0f;
			m_vertex[1].y = 0.0f;
			m_vertex[0].z = m_vertex[1].z = 0.0f;
		}
		if (linenum == 1) {
			m_color[0].r = 0;
			m_color[0].g = 1;
			m_color[0].b = 0;
			m_vertex[0].x = 0.0f;
			m_vertex[0].y = -10.0f;
			m_vertex[1].x = 0.0f;
			m_vertex[1].y = 10.0f;
			m_vertex[0].z = m_vertex[1].z = 0.0f;
		}
		if (linenum == 2) {
			m_color[0].r = 0;
			m_color[0].g = 0;
			m_color[0].b = 1;
			m_vertex[0].x = m_vertex[1].x = 0.0f;
			m_vertex[0].y = m_vertex[1].y = 0.0f;
			m_vertex[0].z = -10.0f;
			m_vertex[1].z = 10.0f;
		}
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
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
		glLineWidth(5.0f);
		glDrawArrays(GL_LINES, 0, 2);
	}
};
vector<Line> g_lines;

struct Spyral {
	std::vector<glm::vec3> s_vertex; //��
	glm::vec3 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 A_R_matrix = glm::mat4(1.0f);
	Spyral(int type) {
		if (type == 0) {
			m_color[0].r = 1.0f;
			m_color[0].g = 1.0f;
			m_color[0].b = 0;
			float x = 0;
			float y = 0;
			float old_x = 0;
			float old_y = 0;
			float r = 0;
			float theta = 0.3f;
			s_vertex.emplace_back(x, 0, y);
			for (int i = 0;i < 500;i++) {
				r += 0.1f;
				theta += 0.26f;
				x = 0 + r * cos(theta);
				y = 0 + r * sin(theta);
				s_vertex.emplace_back(x, 0, y);
			}
		}
		if (type == 1) {
			m_color[0].r = 0;
			m_color[0].g = 1.0f;
			m_color[0].b = 1.0f;
			float x = 0;
			float y = 0;
			float old_x = 0;
			float old_y = 0;
			float r = 0;
			float theta = 0.3f;
			s_vertex.emplace_back(x, 0, y);
			for (int i = 0;i < 500;i++) {
				r -= 0.1f;
				theta += 0.26f;
				x = 0 + r * cos(theta);
				y = 0 + r * sin(theta);
				s_vertex.emplace_back(x, 0, y);
			}
		}
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * s_vertex.size(), &s_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
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
		glLineWidth(5.0f);
		for (int i = 0;i < 499;i++) {
			glDrawArrays(GL_LINES, i, 2);
		}
	}
};
vector<Spyral> g_Spyrals;

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
	ReadDObj("cube(1).obj", vertex);
	g_Dices.emplace_back();
	ReadDObj("pyramid.obj", p_vertex);
	g_pyramids.emplace_back();
	g_lines.emplace_back(0);
	g_lines.emplace_back(1);
	g_lines.emplace_back(2);
	g_Spyrals.emplace_back(0);
	g_Spyrals.emplace_back(1);
	glutTimerFunc(17, Timer, 0);
	glutDisplayFunc(DrawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid DrawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	glm::vec3 cameraPos = glm::vec3(3.0f, 4.0f, 3.0f); //--- ī�޶� ��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glEnable(GL_CULL_FACE);
	glEnable(GL_CCW);
	g_lines[0].Draw();
	g_lines[1].Draw();
	g_lines[2].Draw();
	if (g_Show_Spyral == 1) {
		g_Spyrals[0].Draw();
		g_Spyrals[1].Draw();
	}
	g_Dices[0].Draw();
	g_pyramids[0].Draw();
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
	vertexSource = FileToBuf("vertex.glsl");

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
	fragmentSource = FileToBuf("fragment.glsl"); // �����׼��̴� �о����
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

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.05f, 0.0f, 0.0f));
		g_moveD += 10;
		break;
	case 'A':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.05f, 0.0f, 0.0f));
		g_moveD -= 10;
		break;
	case 's':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, -0.05f, 0.0f));
		break;
	case 'S':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, 0.05f, 0.0f));
		break;
	case 'd':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, 0.0f, -0.05f));
		break;
	case 'D':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, 0.0f, 0.05f));
		break;
	case 'q':
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.05f, 0.0f, 0.0f));
		g_moveT += 10;
		break;
	case 'Q':
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.05f, 0.0f, 0.0f));
		g_moveT -= 10;
		break;
	case 'w':
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, -0.05f, 0.0f));
		break;
	case 'W':
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, 0.05f, 0.0f));
		break;
	case 'e':
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, 0.0f, -0.05f));
		break;
	case 'E':
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, 0.0f, 0.05f));
		break;
	case 'z':
		g_Dices[0].S_matrix = glm::scale(g_Dices[0].S_matrix, glm::vec3(0.3f, 0.3f, 0.3f));
		break;
	case 'Z':
		g_Dices[0].S_matrix = glm::scale(g_Dices[0].S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		break;
	case 'x':
		g_pyramids[0].S_matrix = glm::scale(g_pyramids[0].S_matrix, glm::vec3(0.3f, 0.3f, 0.3f));
		break;
	case 'X':
		g_pyramids[0].S_matrix = glm::scale(g_pyramids[0].S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		break;
	case 'c':
		g_Dices[0].S_matrix = glm::scale(g_Dices[0].S_matrix, glm::vec3(0.9f, 0.9f, 0.9f));
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.2f, 0.0f, 0.0f));
		g_moveD -= 60;
		break;
	case 'C':
		g_Dices[0].S_matrix = glm::scale(g_Dices[0].S_matrix, glm::vec3(1.1f, 1.1f, 1.1f));
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.2f, 0.0f, 0.0f));
		g_moveD += 60;
		break;
	case 'v':
		g_pyramids[0].S_matrix = glm::scale(g_pyramids[0].S_matrix, glm::vec3(0.9f, 0.9f, 0.9f));
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.2f, 0.0f, 0.0f));
		g_moveT -= 60;
		break;
	case 'V':
		g_pyramids[0].S_matrix = glm::scale(g_pyramids[0].S_matrix, glm::vec3(1.1f, 1.1f, 1.1f));
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.2f, 0.0f, 0.0f));
		g_moveT += 60;
		break;
	case 'b':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, -0.05f, 0.0f));
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, -0.05f, 0.0f));
		g_lines[0].T_matrix = glm::translate(g_lines[0].T_matrix, glm::vec3(0.0f, -0.05f, 0.0f));
		g_lines[2].T_matrix = glm::translate(g_lines[2].T_matrix, glm::vec3(0.0f, -0.05f, 0.0f));
		break;
	case 'B':
		g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, 0.05f, 0.0f));
		g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, 0.05f, 0.0f));
		g_lines[0].T_matrix = glm::translate(g_lines[0].T_matrix, glm::vec3(0.0f, 0.05f, 0.0f));
		g_lines[2].T_matrix = glm::translate(g_lines[2].T_matrix, glm::vec3(0.0f, 0.05f, 0.0f));
		break;
	case 't':
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		break;
	case 'r':
		if (!g_isOn4) {
			g_Show_Spyral = 1;
			g_Dices[0].T_matrix = glm::mat4(1.0f);
			g_pyramids[0].T_matrix = glm::mat4(1.0f);
			glutTimerFunc(17, Timer, 5);
			g_isOn4 = true;
		}
		else {
			g_num = 0, g_num1 = 0;
			g_Show_Spyral = 0;
			g_How_Rotate = 0;
			g_Dices[0].AR_matrix = glm::mat4(1.0f);
			g_pyramids[0].AR_matrix = glm::mat4(1.0f);
			g_Dices[0].T_matrix = glm::mat4(1.0f);
			g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-2.0f, 0.0f, -0.25f));
			g_pyramids[0].T_matrix = glm::mat4(1.0f);
			g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
			g_isOn4 = false;
		}
		break;
	case '1':
		if (!g_isOn2) {
			glutTimerFunc(17, Timer, 3);
			g_isOn2 = true;
		}
		break;
	case '2':
		if (!g_isOn3) {
			glutTimerFunc(17, Timer, 4);
			g_isOn3 = true;
		}
		break;
	case '3':
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
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
		if (g_isOn) {
			if (g_Dices[0].m_over == 0) {
				if (g_Dices[0].m_Cnt_y % 2 == 0) {
					if (g_Dices[0].m_Cnt_x < 200 + g_moveD) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					else if (g_Dices[0].m_Cnt_x < 2 * (200 + g_moveD)) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					if (g_Dices[0].m_Cnt_x == (200 + g_moveD) * 2) {
						g_Dices[0].m_over = 1;
						g_Dices[0].m_Cnt_x = 0;
					}
				}
				else if (g_Dices[0].m_Cnt_y % 2 == 1) {
					if (g_Dices[0].m_Cnt_x < 200 + g_moveD) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					else if (g_Dices[0].m_Cnt_x < 2 * (200 + g_moveD)) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					if (g_Dices[0].m_Cnt_x == 2 * (200 + g_moveD)) {
						g_isOn = false;
						g_Dices[0].m_Cnt_x = 0;
					}
				}
			}
			if (g_pyramids[0].m_over == 0) {
				if (g_pyramids[0].m_Cnt_y % 2 == 0) {
					if (g_pyramids[0].m_Cnt_x < 200 + g_moveT) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					else if (g_pyramids[0].m_Cnt_x < 2 * (200 + g_moveT)) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					if (g_pyramids[0].m_Cnt_x == 2 * (200 + g_moveT)) {
						g_pyramids[0].m_over = 1;
						g_pyramids[0].m_Cnt_x = 0;
					}
				}
				else if (g_pyramids[0].m_Cnt_y % 2 == 1) {
					if (g_pyramids[0].m_Cnt_x < 200 + g_moveT) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					else if (g_pyramids[0].m_Cnt_x < 2 * (200 + g_moveT)) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					if (g_pyramids[0].m_Cnt_x == 2 * (200 + g_moveT)) {
						g_isOn = false;
						g_pyramids[0].m_Cnt_x = 0;
					}
				}
			}
			if (g_Dices[0].m_over == 1 && g_pyramids[0].m_over == 1) {
				g_isOn = false;
				g_Dices[0].m_over = 0;
				g_pyramids[0].m_over = 0;
			}
			glutTimerFunc(17, Timer, 1);
		}
		break;
	case 2:
		if (g_isOn1) {
			if (g_Dices[0].m_over == 0) {
				if (g_Dices[0].m_Cnt_y % 2 == 0) {
					if (g_Dices[0].m_Cnt_x < 200 + g_moveD) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, 0.01f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					else if (g_Dices[0].m_Cnt_x < 2 * (200 + g_moveT)) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, -0.01f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					if (g_Dices[0].m_Cnt_x == 2 * (200 + g_moveT)) {
						g_Dices[0].m_over = 1;
						g_Dices[0].m_Cnt_x = 0;
						g_Dices[0].m_Cnt_y = 1;
					}
				}
				else if (g_Dices[0].m_Cnt_y % 2 == 1) {
					if (g_Dices[0].m_Cnt_x < 200 + g_moveT) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, -0.01f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					else if (g_Dices[0].m_Cnt_x < 2 * (200 + g_moveD)) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.0f, 0.01f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					if (g_Dices[0].m_Cnt_x == 2 * (200 + g_moveD)) {
						g_Dices[0].m_over = 1;
						g_Dices[0].m_Cnt_x = 0;
						g_Dices[0].m_Cnt_y = 0;
					}
				}
			}
			if (g_pyramids[0].m_over == 0) {
				if (g_pyramids[0].m_Cnt_y % 2 == 0) {
					if (g_pyramids[0].m_Cnt_x < 200 + g_moveT) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, -0.01f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					else if (g_pyramids[0].m_Cnt_x < 2 * (200 + g_moveD)) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, 0.01f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					if (g_pyramids[0].m_Cnt_x == 2 * (200 + g_moveD)) {
						g_pyramids[0].m_over = 1;
						g_pyramids[0].m_Cnt_x = 0;
						g_pyramids[0].m_Cnt_y = 1;
					}
				}
				else if (g_pyramids[0].m_Cnt_y % 2 == 1) {
					if (g_pyramids[0].m_Cnt_x < 200 + g_moveD) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, 0.01f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					else if (g_pyramids[0].m_Cnt_x < 2 * (200 + g_moveT)) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.0f, -0.01f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					if (g_pyramids[0].m_Cnt_x == 2 * (200 + g_moveT)) {
						g_pyramids[0].m_over = 1;
						g_pyramids[0].m_Cnt_x = 0;
						g_pyramids[0].m_Cnt_y = 0;
					}
				}
			}
			if (g_Dices[0].m_over == 1 && g_pyramids[0].m_over == 1) {
				g_isOn1 = false;
				g_Dices[0].m_over = 0;
				g_pyramids[0].m_over = 0;
			}
			glutTimerFunc(17, Timer, 2);
		}
		break;
	case 3:
		if (g_isOn2) {
			if (g_Dices[0].m_over == 0) {
				if (g_Dices[0].m_Cnt_y % 2 == 0) {
					if (g_Dices[0].m_Cnt_x < 400 + g_moveD + g_moveT) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					if (g_Dices[0].m_Cnt_x == 400 + g_moveD + g_moveT) {
						g_Dices[0].m_over = 1;
						g_Dices[0].m_Cnt_x = 0;
						g_Dices[0].m_Cnt_y++;
					}
				}
				else if (g_Dices[0].m_Cnt_y % 2 == 1) {
					if (g_Dices[0].m_Cnt_x < 400 + g_moveD + g_moveT) {
						g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_Dices[0].m_Cnt_x++;
					}
					if (g_Dices[0].m_Cnt_x == 400 + g_moveD + g_moveT) {
						g_Dices[0].m_over = 1;
						g_Dices[0].m_Cnt_x = 0;
						g_Dices[0].m_Cnt_y++;
					}
				}
			}
			if (g_pyramids[0].m_over == 0) {
				if (g_pyramids[0].m_Cnt_y % 2 == 0) {
					if (g_pyramids[0].m_Cnt_x < 400 + g_moveT + g_moveD) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(-0.01f, 0.0f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					if (g_pyramids[0].m_Cnt_x == 400 + g_moveT + g_moveD) {
						g_pyramids[0].m_over = 1;
						g_pyramids[0].m_Cnt_x = 0;
						g_pyramids[0].m_Cnt_y++;
					}
				}
				else if (g_pyramids[0].m_Cnt_y % 2 == 1) {
					if (g_pyramids[0].m_Cnt_x < 400 + g_moveT + g_moveD) {
						g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(0.01f, 0.0f, 0));
						g_pyramids[0].m_Cnt_x++;
					}
					if (g_pyramids[0].m_Cnt_x == 400 + g_moveT + g_moveD) {
						g_pyramids[0].m_over = 1;
						g_pyramids[0].m_Cnt_x = 0;
						g_pyramids[0].m_Cnt_y++;
					}
				}
			}
			if (g_Dices[0].m_over == 1 && g_pyramids[0].m_over == 1) {
				g_isOn2 = false;
				g_Dices[0].m_over = 0;
				g_pyramids[0].m_over = 0;
			}
			glutTimerFunc(17, Timer, 3);
		}
		break;
	case 4:
		if (g_isOn3) {
			g_Dices[0].AR_matrix = glm::rotate(g_Dices[0].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_pyramids[0].AR_matrix = glm::rotate(g_pyramids[0].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_How_Rotate++;
			if (g_How_Rotate % 60 == 0) {
				g_isOn3 = false;
				g_How_Rotate = 0;
			}
			glutTimerFunc(17, Timer, 4);
		}
		break;
	case 5:
		if (g_isOn4) {
			g_Dices[0].T_matrix = glm::translate(g_Dices[0].T_matrix, glm::vec3(g_Spyrals[0].s_vertex[g_num + 1].x - g_Spyrals[0].s_vertex[g_num].x, 0.0f, g_Spyrals[0].s_vertex[g_num + 1].z - g_Spyrals[0].s_vertex[g_num].z));
			g_pyramids[0].T_matrix = glm::translate(g_pyramids[0].T_matrix, glm::vec3(g_Spyrals[1].s_vertex[g_num1 + 1].x - g_Spyrals[1].s_vertex[g_num1].x, 0.0f, g_Spyrals[1].s_vertex[g_num1 + 1].z - g_Spyrals[1].s_vertex[g_num1].z));
			if (g_num < g_Spyrals[0].s_vertex.size() - 1) {
				g_num++;
			}
			if (g_num1 < g_Spyrals[1].s_vertex.size() - 1) {
				g_num1++;
			}
			glutTimerFunc(17, Timer, 5);
		}
		break;
	default:
		break;
	}
}