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
int g_HowDrawNow = 0;
int g_Rotate = 0, g_Rotate1 = 0;
bool g_isOn = false, g_isOn1 = false, g_isOn2 = true, g_isOn3 = true;
int g_num = 0, g_num1 = 0, g_num2 = 0, g_num3 = 0, g_num4 = 0;
int g_proj = 1;
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

struct Shape {
	std::vector<glm::vec3> s_vertex; //��
	glm::vec3 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	Shape* m_parent = nullptr;
	glm::mat4 GetMatrix()
	{
		if (m_parent == nullptr) {
			return AR_matrix * T_matrix * R_matrix * S_matrix;
		}
		return m_parent->GetMatrix() * AR_matrix * T_matrix * R_matrix * S_matrix; //����� �߻�
	}
	int m_shape = 0;
	float theta = 0.0f;
	Shape(int type, int color, Shape* parent) {
		switch (type) {
		case 0:
			m_color[0].r = 0;
			m_color[0].g = 0;
			m_color[0].b = 1;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.7f, 0.7f, 0.7f));
			m_shape = 1;
			break;
		case 1:
			m_color[0].r = 1;
			m_color[0].g = 0;
			m_color[0].b = 0;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
			switch (color) {
			case 0:
				T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 55.0f));
				break;
			case 1:
				T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 55.0f));
				R_matrix = glm::rotate(R_matrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 2:
				T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 55.0f));
				R_matrix = glm::rotate(R_matrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			}
			m_shape = 1;
			break;
		case 2:
			m_color[0].r = 0;
			m_color[0].g = 1;
			m_color[0].b = 0;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.6f, 0.6f, 0.6f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 20.0f));
			m_shape = 1;
			break;
		case 3:
			while (theta <= 360) {
				s_vertex.emplace_back(55 * sin(glm::radians(theta)), 0, 55 * cos(glm::radians(theta)));
				theta += 10.0f;
			}
			switch (color) {
			case 0:
				m_color[0].r = 1;
				m_color[0].g = 0.0f;
				m_color[0].b = 1.0f;
				break;
			case 1:
				m_color[0].r = 1;
				m_color[0].g = 0.5f;
				m_color[0].b = 0.0f;
				R_matrix = glm::rotate(R_matrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 2:
				m_color[0].r = 0;
				m_color[0].g = 1.0f;
				m_color[0].b = 1.0f;
				R_matrix = glm::rotate(R_matrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			}
			m_shape = 2;
			break;
		case 4:
			while (theta <= 360) {
				s_vertex.emplace_back(20 * sin(glm::radians(theta)), 0, 20 * cos(glm::radians(theta)));
				theta += 10.0f;
			}
			m_color[0].r = 0.0f;
			m_color[0].g = 0.0f;
			m_color[0].b = 0.0f;
			m_shape = 2;
			break;
		}
		m_parent = parent;
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		if (m_shape == 1) {
			glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_vertex.size(), &g_vertex[0], GL_STATIC_DRAW);
			int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
			//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
			glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(positionAttrib);
		}
		if (m_shape == 2) {
			glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * s_vertex.size(), &s_vertex[0], GL_STATIC_DRAW);
			int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
			//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
			glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(positionAttrib);
		}
		glm::mat4 parentMatrix(1.0f);
		if (m_parent != nullptr) {
			parentMatrix = m_parent->GetMatrix(); //�θ� ��������
		}
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
		glLineWidth(5.0f);
		switch (m_shape) {
		case 1:
			switch (g_HowDrawNow) {
			case 0:
				glDrawArrays(GL_TRIANGLES, 0, g_vertex.size());
				break;
			case 1:
				glDrawArrays(GL_LINE_LOOP, 0, g_vertex.size());
				break;
			}
			break;
		case 2:
			for (int i = 0;i < s_vertex.size() - 1;i++) {
				glDrawArrays(GL_LINES, i, 2);
			}
			break;
		}
	}
};
vector<Shape>g_Shape;

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
	ReadDObj("Sphere.obj", g_vertex);
	g_Shape.reserve(50);
	g_Shape.emplace_back(0, 0, nullptr); //�¾�
	g_Shape.emplace_back(1, 0, &g_Shape[0]); //�༺
	g_Shape.emplace_back(1, 1, &g_Shape[0]);
	g_Shape.emplace_back(1, 2, &g_Shape[0]);
	g_Shape.emplace_back(3, 0, &g_Shape[0]); //�¾�˵�
	g_Shape.emplace_back(3, 1, &g_Shape[0]);
	g_Shape.emplace_back(3, 2, &g_Shape[0]);
	g_Shape.emplace_back(2, 0, &g_Shape[1]); //����
	g_Shape.emplace_back(2, 0, &g_Shape[2]);
	g_Shape.emplace_back(2, 0, &g_Shape[3]);
	g_Shape.emplace_back(4, 0, &g_Shape[1]); //�༺ �˵�
	g_Shape.emplace_back(4, 0, &g_Shape[2]);
	g_Shape.emplace_back(4, 0, &g_Shape[3]);

	glutTimerFunc(17, Timer, 0);
	glutTimerFunc(17, Timer, 3);
	glutTimerFunc(17, Timer, 4);
	glutTimerFunc(17, Timer, 5);
	glutDisplayFunc(DrawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid DrawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	if (g_proj == 0) {
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 100.0f); //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 500.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	if (g_proj == 1) {
		glm::vec3 cameraPos = glm::vec3(2.0f, 3.0f, 3.0f); //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f); //--- ���� ���� ����: [-100.0, 100.0]
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_CW);
	for (auto& t : g_Shape) {
		t.Draw();
	}
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
	case 'p':
		g_proj = 0;
		break;
	case 'P':
		g_proj = 1;
		break;
	case 'm':
		g_HowDrawNow = 0;
		break;
	case 'M':
		g_HowDrawNow = 1;
		break;
	case 'w':
		g_Shape[0].T_matrix = glm::translate(g_Shape[0].T_matrix, glm::vec3(0.0f, 0.3f, 0.0f));
		break;
	case 's':
		g_Shape[0].T_matrix = glm::translate(g_Shape[0].T_matrix, glm::vec3(0.0f, -0.3f, 0.0f));
		break;
	case 'a':
		g_Shape[0].T_matrix = glm::translate(g_Shape[0].T_matrix, glm::vec3(-0.3f, 0.0f, 0.0f));
		break;
	case 'd':
		g_Shape[0].T_matrix = glm::translate(g_Shape[0].T_matrix, glm::vec3(0.3f, 0.0f, 0.0f));
		break;
	case '+':
		g_Shape[0].T_matrix = glm::translate(g_Shape[0].T_matrix, glm::vec3(0.0f, 0.0f, 0.3f));
		break;
	case '-':
		g_Shape[0].T_matrix = glm::translate(g_Shape[0].T_matrix, glm::vec3(0.0f, 0.0f, -0.3f));
		break;
	case 'y':
		g_Rotate = 1;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		break;
	case 'Y':
		g_Rotate = 2;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		break;
	case 'z':
		g_Rotate1 = 1;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
		break;
	case 'Z':
		g_Rotate1 = 2;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
		break;
	case 'b':
		g_isOn = false;
		g_isOn1 = false;
		g_Shape[0].AR_matrix = glm::mat4(1.0f);
		break;
	}
}

void Timer(int val) {
	switch (val) {
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		if (g_isOn) {
			if (g_Rotate == 1) {
				g_Shape[0].AR_matrix = glm::rotate(g_Shape[0].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (g_Rotate == 2) {
				g_Shape[0].AR_matrix = glm::rotate(g_Shape[0].AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			glutTimerFunc(17, Timer, 1);
		}
		break;
	case 2:
		if (g_isOn1) {
			if (g_Rotate1 == 1) {
				g_Shape[0].AR_matrix = glm::rotate(g_Shape[0].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			if (g_Rotate1 == 2) {
				g_Shape[0].AR_matrix = glm::rotate(g_Shape[0].AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			glutTimerFunc(17, Timer, 2);
		}
		break;
	case 3: //���� 1
		if (g_isOn2) {
			g_Shape[1].AR_matrix = glm::rotate(g_Shape[1].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[7].T_matrix = glm::translate(g_Shape[7].T_matrix, glm::vec3(g_Shape[10].s_vertex[g_num2 + 1].x - g_Shape[10].s_vertex[g_num2].x, g_Shape[10].s_vertex[g_num2 + 1].y - g_Shape[10].s_vertex[g_num2].y, g_Shape[10].s_vertex[g_num2 + 1].z - g_Shape[10].s_vertex[g_num2].z));
			g_num2++;
			if (g_num2 >= g_Shape[10].s_vertex.size() - 1) {
				g_num2 = 0;
			}
			glutTimerFunc(17, Timer, 3);
		}
		break;
	case 4://���� 2 g_Shape[2] = �༺(-45)�� �����ִ�), g_Shape[11] = ���� �˵�, g_Shape[8] = ����
		if (g_isOn2) {
			if (g_num == 0) {
				g_Shape[2].R_matrix = glm::rotate(g_Shape[2].R_matrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			g_Shape[2].AR_matrix = glm::rotate(g_Shape[2].AR_matrix, glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[2].AR_matrix = glm::rotate(g_Shape[2].AR_matrix, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			g_Shape[11].AR_matrix = glm::rotate(g_Shape[11].AR_matrix, glm::radians(-2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[11].AR_matrix = glm::rotate(g_Shape[11].AR_matrix, glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			g_Shape[8].AR_matrix = glm::rotate(g_Shape[8].AR_matrix, glm::radians(-2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[8].AR_matrix = glm::rotate(g_Shape[8].AR_matrix, glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			g_Shape[8].T_matrix = glm::translate(g_Shape[8].T_matrix, glm::vec3(g_Shape[11].s_vertex[g_num3 + 1].x - g_Shape[11].s_vertex[g_num3].x, g_Shape[11].s_vertex[g_num3 + 1].y - g_Shape[11].s_vertex[g_num3].y, g_Shape[11].s_vertex[g_num3 + 1].z - g_Shape[11].s_vertex[g_num3].z));
				g_num3++;
			if (g_num3 >= g_Shape[11].s_vertex.size() - 1) {
				g_num3 = 0;
			}
			g_num = 1;
			glutTimerFunc(17, Timer, 4);
		}
		break;
	case 5: //���� 3
		if (g_isOn2) {
			if (g_num1 == 0) {
				g_Shape[3].R_matrix = glm::rotate(g_Shape[3].R_matrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				g_Shape[9].R_matrix = glm::rotate(g_Shape[9].R_matrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			g_Shape[3].AR_matrix = glm::rotate(g_Shape[3].AR_matrix, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[3].AR_matrix = glm::rotate(g_Shape[3].AR_matrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			g_Shape[12].AR_matrix = glm::rotate(g_Shape[12].AR_matrix, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[12].AR_matrix = glm::rotate(g_Shape[12].AR_matrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			g_Shape[9].AR_matrix = glm::rotate(g_Shape[9].AR_matrix, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_Shape[9].AR_matrix = glm::rotate(g_Shape[9].AR_matrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			g_Shape[9].T_matrix = glm::translate(g_Shape[9].T_matrix, glm::vec3(g_Shape[12].s_vertex[g_num4 + 1].x - g_Shape[12].s_vertex[g_num4].x, g_Shape[12].s_vertex[g_num4 + 1].y - g_Shape[12].s_vertex[g_num4].y, g_Shape[12].s_vertex[g_num4 + 1].z - g_Shape[12].s_vertex[g_num4].z));
			g_num4++;
			if (g_num4 >= g_Shape[12].s_vertex.size() - 1) {
				g_num4 = 0;
			}
			g_num1 = 1;
			glutTimerFunc(17, Timer, 5);
		}
		break;
	default:
		break;
	}
}