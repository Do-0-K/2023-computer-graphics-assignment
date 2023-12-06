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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
using namespace std;
void ReadDObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& vertexNomal, std::vector<glm::vec2>& vertexTexture)
{
	std::vector<glm::vec4> vertexFace;
	std::vector<glm::vec4> vertexNomalFace;
	std::vector<glm::vec4> vertexTextFace;
	std::vector<glm::vec4> vertexData; //���ؽ� �� ����� ����
	std::vector<glm::vec4> vertexNomalData; //���ؽ� �� ����� ����
	std::vector<glm::vec4> vertexTextData; //���ؽ� �� ����� ����
	std::string line;
	std::ifstream inFile(objfilename);
	char head[3] = { 0,0,0 };
	int vertexFaceNum[3] = { 0,0,0 };
	int vertexnomalFaceNum[3] = { 0,0,0 };
	int vertextextureFaceNum[3] = { 0,0,0 };
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
			else if (head[1] == 't') {
				inFile >> std::noskipws >> head[2];
				if (head[2] == ' ') {
					float x = 0, y = 0, z = 0; //�� �������� ����
					inFile >> std::skipws >> x >> y >> z; //��������
					glm::vec4 vertexT = { x,y,z,1 }; //���Ϳ� �����ϰ�
					vertexTextData.push_back(vertexT); //push_back���� �־��ֱ�
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
				inFile >> std::skipws >> vertexFaceNum[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertextextureFaceNum[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexnomalFaceNum[0];

				inFile >> std::skipws >> vertexFaceNum[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertextextureFaceNum[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexnomalFaceNum[1];

				inFile >> std::skipws >> vertexFaceNum[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertextextureFaceNum[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexnomalFaceNum[2];

				glm::vec4 vertexFaceTemp = { vertexFaceNum[0], vertexFaceNum[1], vertexFaceNum[2], 1 };
				glm::vec4 vertexnomalFaceTemp = { vertexnomalFaceNum[0], vertexnomalFaceNum[1], vertexnomalFaceNum[2], 1 };
				glm::vec4 vertexTextFaceTemp = { vertextextureFaceNum[0], vertextextureFaceNum[1], vertextextureFaceNum[2], 1 };
				vertexFace.push_back(vertexFaceTemp);
				vertexNomalFace.push_back(vertexnomalFaceTemp);
				vertexTextFace.push_back(vertexTextFaceTemp);
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
	for (auto iter = vertexTextFace.begin(); iter < vertexTextFace.end(); iter++) {
		vertexTexture.push_back(vertexTextData[(int)(iter->x) - 1]);
		vertexTexture.push_back(vertexTextData[(int)(iter->y) - 1]);
		vertexTexture.push_back(vertexTextData[(int)(iter->z) - 1]);
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
int g_WhoDrawNow = 0;
int g_Rotate = 0, g_Rotate1 = 0;
bool g_isOn = false, g_isOn1 = false;
unsigned int textures[7];
void MakeVertexShaders();
void MakeFragmentShaders();
void Timer(int val);
void initText();
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
std::vector<glm::vec3> normal; //�븻
std::vector<glm::vec2> texture; //�ؽ���

std::vector<glm::vec3> p_vertex; //��
std::vector<glm::vec3> p_normal; //�븻
std::vector<glm::vec2> p_texture; //�ؽ���

std::vector<glm::vec3> t_vertex; //��
std::vector<glm::vec3> t_normal; //�븻
std::vector<glm::vec2> t_texture; //�ؽ���

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f); //--- ī�޶� ��ġ
glm::mat4 A_R_matrix = glm::mat4(1.0f);
glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f); //�븻����

struct Dice {
	glm::vec4 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOText;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	Dice() {
		m_color[0].r = 1.7f;
		m_color[0].g = 1.0f;
		m_color[0].b = 1.0f;
		m_color[0].a = 1.0f;
		S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
		glGenBuffers(1, &m_VBOText);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normal.size(), &normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBOText);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texture.size(), &texture[0], GL_STATIC_DRAW);
		int textureAttrib = glGetAttribLocation(shaderProgramID, "vertexTexture");
		glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(textureAttrib);

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
		glUniform4f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b, m_color[0].a);
		int texloc = glGetUniformLocation(shaderProgramID, "is_text");
		glUniform1i(texloc, 1);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glBindTexture(GL_TEXTURE_2D, textures[5]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 30, 6);
	}
};
vector<Dice> g_Dices;

struct pyramid { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec4 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOText;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	pyramid() {
		m_color[0].r = 1.0f;
		m_color[0].g = 1.0f;
		m_color[0].b = 1.0f;
		m_color[0].a = 1.0f;
		S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
		glGenBuffers(1, &m_VBOText);
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

		glBindBuffer(GL_ARRAY_BUFFER, m_VBOText);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * p_texture.size(), &p_texture[0], GL_STATIC_DRAW);
		int textureAttrib = glGetAttribLocation(shaderProgramID, "vertexTexture");
		glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(textureAttrib);

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
		glUniform4f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b, m_color[0].a);
		int texloc = glGetUniformLocation(shaderProgramID, "is_text");
		glUniform1i(texloc, 1);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 6, 3);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 9, 3);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 12, 6);
	}
};
vector<pyramid> g_pyramids;

struct stage {
	glm::vec4 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOText;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	stage() {
		m_color[0].r = 1.0f;
		m_color[0].g = 1.0f;
		m_color[0].b = 1.0f;
		m_color[0].a = 1.0f;
		S_matrix = glm::scale(S_matrix, glm::vec3(1024.0f, 1.0f, 1024.0f));
		R_matrix = glm::rotate(R_matrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, -1000.0f));
		SetVBO();
	}
	void SetVBO() {
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
		glGenBuffers(1, &m_VBOText);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * t_vertex.size(), &t_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * t_normal.size(), &t_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBOText);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * t_texture.size(), &t_texture[0], GL_STATIC_DRAW);
		int textureAttrib = glGetAttribLocation(shaderProgramID, "vertexTexture");
		glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(textureAttrib);

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
		glUniform4f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b, m_color[0].a);
		int texloc = glGetUniformLocation(shaderProgramID, "is_text");
		glUniform1i(texloc, 1);
		glBindTexture(GL_TEXTURE_2D, textures[6]);
		glUniform1i(glGetUniformLocation(shaderProgramID, "outText"), 0);
		glDrawArrays(GL_TRIANGLES, 0, t_vertex.size());
	}
};
vector<stage> g_stages;

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
	initText();
	shaderProgramID = MakeShaderProgram();
	ReadDObj("cube.obj", vertex, normal, texture);
	g_Dices.emplace_back();
	ReadDObj("pyramid.obj", p_vertex, p_normal, p_texture);
	g_pyramids.emplace_back();
	ReadDObj("plane3.obj", t_vertex, t_normal, t_texture);
	g_stages.emplace_back();
	glutTimerFunc(17, Timer, 0);
	glutDisplayFunc(DrawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid DrawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 1024.0f);
	projection *= A_R_matrix;
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, 0.0f, 20.0f, 0.0f);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light_color.x, light_color.y, light_color.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
	glEnable(GL_CULL_FACE);
	glEnable(GL_CCW);
	glActiveTexture(GL_TEXTURE0);
	g_stages[0].Draw();
	if (g_WhoDrawNow == 1) {
		glActiveTexture(GL_TEXTURE0);
		g_Dices[0].Draw();
	}
	if (g_WhoDrawNow == 2) {
		glActiveTexture(GL_TEXTURE0);
		g_pyramids[0].Draw();
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

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		g_WhoDrawNow = 1;
		break;
	case 'p':
		g_WhoDrawNow = 2;
		break;
	case 'x':
		g_Rotate = 1;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		break;
	case 'X':
		g_Rotate = 2;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		break;
	case 'y':
		g_Rotate1 = 1;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
		break;
	case 'Y':
		g_Rotate1 = 2;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
		break;
	case 's':
		g_isOn = false;
		g_isOn1 = false;
		g_Dices[0].R_matrix = glm::mat4(1.0f);
		g_pyramids[0].R_matrix = glm::mat4(1.0f);
		break;
	case 'h':
		cameraPos.x += 1.0f;
		break;
	case 'H':
		cameraPos.x -= 1.0f;
		break;
	case 'j':
		cameraPos.y += 1.0f;
		break;
	case 'J':
		cameraPos.y -= 1.0f;
		break;
	case 'k':
		cameraPos.z += 1.0f;
		break;
	case 'K':
		cameraPos.z -= 1.0f;
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
			if (g_Rotate == 1) {
				g_Dices[0].R_matrix = glm::rotate(g_Dices[0].R_matrix, glm::radians(-3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				g_pyramids[0].R_matrix = glm::rotate(g_pyramids[0].R_matrix, glm::radians(-3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			if (g_Rotate == 2) {
				g_Dices[0].R_matrix = glm::rotate(g_Dices[0].R_matrix, glm::radians(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				g_pyramids[0].R_matrix = glm::rotate(g_pyramids[0].R_matrix, glm::radians(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			glutTimerFunc(17, Timer, 1);
		}
		break;
	case 2:
		if (g_isOn1) {
			if (g_Rotate1 == 1) {
				g_Dices[0].R_matrix = glm::rotate(g_Dices[0].R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				g_pyramids[0].R_matrix = glm::rotate(g_pyramids[0].R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (g_Rotate1 == 2) {
				g_Dices[0].R_matrix = glm::rotate(g_Dices[0].R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				g_pyramids[0].R_matrix = glm::rotate(g_pyramids[0].R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			glutTimerFunc(17, Timer, 2);
		}
		break;
	default:
		break;
	}
}

void initText()
{
	BITMAP* bmp;
	glGenTextures(7, &textures[0]); //--- �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, textures[0]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage1 = 512, cubeheightImage1 = 512, cubenumberOfChannel1;
	unsigned char* cubedata1 = stbi_load("BIT_A.bmp", &cubewidthImage1, &cubeheightImage1, &cubenumberOfChannel1, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage1, cubeheightImage1, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata1);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata1);

	glBindTexture(GL_TEXTURE_2D, textures[1]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage2 = 512, cubeheightImage2 = 512, cubenumberOfChannel2;
	unsigned char* cubedata2 = stbi_load("BIT_B.bmp", &cubewidthImage2, &cubeheightImage2, &cubenumberOfChannel2, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage2, cubeheightImage2, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata2);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata2);

	glBindTexture(GL_TEXTURE_2D, textures[2]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage3 = 512, cubeheightImage3 = 512, cubenumberOfChannel3;
	unsigned char* cubedata3 = stbi_load("BIT_C.bmp", &cubewidthImage3, &cubeheightImage3, &cubenumberOfChannel3, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage3, cubeheightImage3, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata3);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata3);

	glBindTexture(GL_TEXTURE_2D, textures[3]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage4 = 512, cubeheightImage4 = 512, cubenumberOfChannel4;
	unsigned char* cubedata4 = stbi_load("BIT_D.bmp", &cubewidthImage4, &cubeheightImage4, &cubenumberOfChannel4, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage4, cubeheightImage4, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata4);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata4);

	glBindTexture(GL_TEXTURE_2D, textures[4]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage5 = 512, cubeheightImage5 = 512, cubenumberOfChannel5;
	unsigned char* cubedata5 = stbi_load("BIT_E.bmp", &cubewidthImage5, &cubeheightImage5, &cubenumberOfChannel5, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage5, cubeheightImage5, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata5);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata5);

	glBindTexture(GL_TEXTURE_2D, textures[5]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage6 = 512, cubeheightImage6 = 512, cubenumberOfChannel6;
	unsigned char* cubedata6 = stbi_load("BIT_F.bmp", &cubewidthImage6, &cubeheightImage6, &cubenumberOfChannel6, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage6, cubeheightImage6, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata6);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata6);

	glBindTexture(GL_TEXTURE_2D, textures[6]); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage7 = 512, cubeheightImage7 = 512, cubenumberOfChannel7;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* cubedata7 = stbi_load("sky.bmp", &cubewidthImage7, &cubeheightImage7, &cubenumberOfChannel7, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage7, cubeheightImage7, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata7);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata7);
}
