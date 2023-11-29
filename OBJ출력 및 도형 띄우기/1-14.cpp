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
void ReadObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec2>& vertexTexture, std::vector<glm::vec3>& vertexNomal)
{
	std::vector<glm::vec4> vertexFace;
	std::vector<glm::vec4> vertexNomalFace;
	std::vector<glm::vec4> vertexTextureFace;
	std::string line;
	int vertexNum = 0;
	int vertexNomalNum = 0;
	int vertexTextureNum = 0;
	std::ifstream inFile(objfilename);
	while (std::getline(inFile, line)) {
		if (line[0] == 'v' && line[1] == ' ')
			vertexNum++;
		else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
			vertexNomalNum++;
		else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
			vertexTextureNum++;
	}
	glm::vec4* vertexData = new glm::vec4[vertexNum];
	glm::vec4* vertexNomalData = new glm::vec4[vertexNomalNum];
	glm::vec2* vertexTextureData = new glm::vec2[vertexTextureNum];
	inFile.clear();
	inFile.seekg(0, std::ios::beg); //seekg - �б�, beg - ó������
	vertexNum = 0;
	vertexNomalNum = 0;
	vertexTextureNum = 0;
	char head[3];
	int vertexFaceNum[3];
	int vertexNomalFaceNum[3];
	int vertexTextureFaceNum[3];
	std::string nt;
	while (inFile >> std::noskipws >> head[0]) { //noskipws = �Ⱥ�ĭ�� �ðڴ�,skipws = ��ĭ�� �ðڴ�
		if (head[0] == 'v') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> vertexData[vertexNum].x >> vertexData[vertexNum].y >> vertexData[vertexNum].z;
				vertexNum++;
			}
			else if (head[1] == 'n') {
				inFile >> std::noskipws >> head[2];
				if (head[2] == ' ') {
					inFile >> std::skipws >> vertexNomalData[vertexNomalNum].x >> vertexNomalData[vertexNomalNum].y >> vertexNomalData[vertexNomalNum].z;
					vertexNomalNum++;
				}
			}
			else if (head[1] == 't') {
				inFile >> std::noskipws >> head[2];
				if (head[2] == ' ') {
					float tr;
					inFile >> std::skipws >> vertexTextureData[vertexTextureNum].x >> vertexTextureData[vertexTextureNum].y >> tr;
					vertexTextureNum++;
				}
			}
			head[1] = '\0';
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				char tempC;
				int tempi;
				inFile >> std::skipws >> vertexFaceNum[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexTextureFaceNum[0];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexNomalFaceNum[0];

				inFile >> std::skipws >> vertexFaceNum[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexTextureFaceNum[1];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexNomalFaceNum[1];

				inFile >> std::skipws >> vertexFaceNum[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexTextureFaceNum[2];
				inFile >> std::skipws >> tempC;
				inFile >> std::skipws >> vertexNomalFaceNum[2];

				glm::vec4 vertexFaceTemp = { vertexFaceNum[0], vertexFaceNum[1], vertexFaceNum[2], 1 };
				glm::vec4 vertexNomalFaceTemp = { vertexNomalFaceNum[0], vertexNomalFaceNum[1], vertexNomalFaceNum[2],1 };
				glm::vec4 vertexTextureFaceTemp = { vertexTextureFaceNum[0], vertexTextureFaceNum[1], vertexTextureFaceNum[2],1 };
				vertexFace.push_back(vertexFaceTemp);
				vertexNomalFace.push_back(vertexNomalFaceTemp);
				vertexTextureFace.push_back(vertexTextureFaceTemp);
			}
			head[1] = '\0';
		}
	}
	for (auto iter = vertexFace.begin(); iter < vertexFace.end(); iter++) {
		vertex.push_back(vertexData[(int)(iter->x) - 1]);
		vertex.push_back(vertexData[(int)(iter->y) - 1]);
		vertex.push_back(vertexData[(int)(iter->z) - 1]);
	}
	for (auto iter = vertexNomalFace.begin(); iter < vertexNomalFace.end(); iter++) {
		vertexNomal.push_back(vertexNomalData[(int)(iter->x) - 1]);
		vertexNomal.push_back(vertexNomalData[(int)(iter->y) - 1]);
		vertexNomal.push_back(vertexNomalData[(int)(iter->z) - 1]);
	}
	for (auto iter = vertexTextureFace.begin(); iter < vertexTextureFace.end(); iter++) {
		vertexTexture.push_back(vertexTextureData[(int)(iter->x) - 1]);
		vertexTexture.push_back(vertexTextureData[(int)(iter->y) - 1]);
		vertexTexture.push_back(vertexTextureData[(int)(iter->z) - 1]);
	}
	delete[] vertexData; //��� �� ������ �����ڴ�
	delete[] vertexNomalData; //��� �� ������ �����ڴ�
	delete[] vertexTextureData; //��� �� ������ �����ڴ�
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
int g_HowToDrawDice = 0, g_HowToDrawFourTower = 0;
int g_RandomDrawDice1 = 0, g_RandomDrawDice2 = 0, g_RandomDrawFourTower1 = 0, g_RandomDrawFourTower2 = 0;
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
std::vector<glm::vec2> uv; //�ؽ���
std::vector<glm::vec3> normal; //��������

glm::mat4 A_R_matrix = glm::mat4(1.0f);

std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}

struct Dice { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	Dice(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		m_move.x = center.first;
		m_move.y = center.second;
		m_move.z = 0.0f;
		m_color[0].r = r;
		m_color[0].g = g;
		m_color[0].b = b;
		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.3f, 0.3f));
		T_matrix = glm::translate(T_matrix, glm::vec3(-0.5f, 0.0f, 0.0f));
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
		glUniformMatrix4fv(ArotateLocation, 1, GL_FALSE, glm::value_ptr(A_R_matrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		switch (g_HowToDrawDice) {
		case 0:
			glDrawArrays(GL_TRIANGLES, 0, vertex.size());
			break;
		case 1:
			glDrawArrays(GL_TRIANGLES, 0, 6);
			break;
		case 2:
			glDrawArrays(GL_TRIANGLES, 6, 6);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 12, 6);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLES, 18, 6);
			break;
		case 5:
			glDrawArrays(GL_TRIANGLES, 24, 6);
			break;
		case 6:
			glDrawArrays(GL_TRIANGLES, 30, 6);
			break;
		default:
			break;
		}
		switch (g_RandomDrawDice1) {
		case 1:
			glDrawArrays(GL_TRIANGLES, 0, 6);
			break;
		case 2:
			glDrawArrays(GL_TRIANGLES, 6, 6);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 12, 6);
			break;
		default:
			break;
		}
		switch (g_RandomDrawDice2) {
		case 4:
			glDrawArrays(GL_TRIANGLES, 18, 6);
			break;
		case 5:
			glDrawArrays(GL_TRIANGLES, 24, 6);
			break;
		case 6:
			glDrawArrays(GL_TRIANGLES, 30, 6);
			break;
		default:
			break;
		}
	}
};
vector<Dice> g_Dices;

struct FourTower { //VB0 = ���� �ϳ�, VAO = �迭 -> �䷱ ����
	glm::vec3 m_vertex[6];
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	FourTower(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		float r = color(dre);
		float g = color(dre);
		float b = color(dre);
		m_move.x = center.first;
		m_move.y = center.second;
		m_move.z = 0.0f;
		m_color[0].r = r;
		m_color[0].g = g;
		m_color[0].b = b;
		m_vertex[0] = { -0.8f, 0.0f, -0.6f };
		m_vertex[1] = { 0.0f, 1.0f, 0.0f };
		m_vertex[2] = { 0.0f, 0.0f, 1.0f };
		m_vertex[3] = { 0.8f, 0.0f, -0.6f };
		m_vertex[4] = { -0.8f, 0.0f, -0.6f };
		m_vertex[5] = { 0.0f, 1.0f, 0.0f };
		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.3f, 0.3f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.5f, 0.0f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, m_vertex, GL_STATIC_DRAW);
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
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glPointSize(4.0f);
		switch (g_HowToDrawFourTower) {
		case 0:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawArrays(GL_TRIANGLES, 1, 3);
			glDrawArrays(GL_TRIANGLES, 2, 3);
			glDrawArrays(GL_TRIANGLES, 3, 3);
			break;
		case 1:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 2:
			glDrawArrays(GL_TRIANGLES, 1, 3);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 2, 3);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLES, 3, 3);
			break;
		default:
			break;
		}
		switch (g_RandomDrawFourTower1) {
		case 1:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 2:
			glDrawArrays(GL_TRIANGLES, 1, 3);
			break;
		default:
			break;
		}
		switch (g_RandomDrawFourTower2) {
		case 3:
			glDrawArrays(GL_TRIANGLES, 2, 3);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLES, 3, 3);
			break;
		default:
			break;
		}
	}
};
vector<FourTower> g_FourTowers;

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
	MakeVertexShaders(); //--- ���ؽ� ���̴� �����
	MakeFragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = MakeShaderProgram();
	ReadObj("cube(1).obj", vertex, uv, normal);
	g_Dices.emplace_back(400, 300);
	g_FourTowers.emplace_back(400, 300);
	glutTimerFunc(17, Timer, 0);
	glutDisplayFunc(DrawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid DrawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	g_Dices[0].Draw();
	g_FourTowers[0].Draw();
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
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> Rand_D1(1, 3);
	uniform_int_distribution<int> Rand_D2(4, 6);
	uniform_int_distribution<int> Rand_P1(1, 2);
	uniform_int_distribution<int> Rand_P2(3, 4);
	switch (key) {
	case '1':
		g_HowToDrawDice = 1;
		g_RandomDrawDice1 = -1;
		g_RandomDrawDice2 = -1;
		break;
	case '2':
		g_HowToDrawDice = 2;
		g_RandomDrawDice1 = -1;
		g_RandomDrawDice2 = -1;
		break;
	case '3':
		g_HowToDrawDice = 3;
		g_RandomDrawDice1 = -1;
		g_RandomDrawDice2 = -1;
		break;
	case '4':
		g_HowToDrawDice = 4;
		g_RandomDrawDice1 = -1;
		g_RandomDrawDice2 = -1;
		break;
	case '5':
		g_HowToDrawDice = 5;
		g_RandomDrawDice1 = -1;
		g_RandomDrawDice2 = -1;
		break;
	case '6':
		g_HowToDrawDice = 6;
		g_RandomDrawDice1 = -1;
		g_RandomDrawDice2 = -1;
		break;
	case '7':
		g_HowToDrawFourTower = 1;
		g_RandomDrawFourTower1 = -1;
		g_RandomDrawFourTower2 = -1;
		break;
	case '8':
		g_HowToDrawFourTower = 2;
		g_RandomDrawFourTower1 = -1;
		g_RandomDrawFourTower2 = -1;
		break;
	case '9':
		g_HowToDrawFourTower = 3;
		g_RandomDrawFourTower1 = -1;
		g_RandomDrawFourTower2 = -1;
		break;
	case '0':
		g_HowToDrawFourTower = 4;
		g_RandomDrawFourTower1 = -1;
		g_RandomDrawFourTower2 = -1;
		break;
	case 'q':
		g_HowToDrawDice = -1;
		g_RandomDrawDice1 = Rand_D1(dre);
		g_RandomDrawDice2 = Rand_D2(dre);
		break;
	case 'w':
		g_HowToDrawFourTower = -1;
		g_RandomDrawFourTower1 = Rand_P1(dre);
		g_RandomDrawFourTower2 = Rand_P2(dre);
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
	default:
		break;
	}
}