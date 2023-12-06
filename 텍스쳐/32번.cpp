#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h> //--- 필요한 헤더파일 include
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
	std::vector<glm::vec4> vertexData; //버텍스 값 담아줄 벡터
	std::vector<glm::vec4> vertexNomalData; //버텍스 값 담아줄 벡터
	std::vector<glm::vec4> vertexTextData; //버텍스 값 담아줄 벡터
	std::string line;
	std::ifstream inFile(objfilename);
	char head[3] = { 0,0,0 };
	int vertexFaceNum[3] = { 0,0,0 };
	int vertexnomalFaceNum[3] = { 0,0,0 };
	int vertextextureFaceNum[3] = { 0,0,0 };
	std::string nt;
	while (inFile >> std::noskipws >> head[0]) { //noskipws = 안빈칸을 씹겠다,skipws = 빈칸을 씹겠다
		if (head[0] == 'v') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				float x = 0, y = 0, z = 0; //값 가져오는 변수
				inFile >> std::skipws >> x >> y >> z; //가져오기
				glm::vec4 vertexD = { x,y,z,1 }; //벡터에 저장하고
				vertexData.push_back(vertexD); //push_back으로 넣어주기
			}
			else if (head[1] == 'n') {
				inFile >> std::noskipws >> head[2];
				if (head[2] == ' ') {
					float x = 0, y = 0, z = 0; //값 가져오는 변수
					inFile >> std::skipws >> x >> y >> z; //가져오기
					glm::vec4 vertexN = { x,y,z,1 }; //벡터에 저장하고
					vertexNomalData.push_back(vertexN); //push_back으로 넣어주기
				}
			}
			else if (head[1] == 't') {
				inFile >> std::noskipws >> head[2];
				if (head[2] == ' ') {
					float x = 0, y = 0, z = 0; //값 가져오는 변수
					inFile >> std::skipws >> x >> y >> z; //가져오기
					glm::vec4 vertexT = { x,y,z,1 }; //벡터에 저장하고
					vertexTextData.push_back(vertexT); //push_back으로 넣어주기
				}
			}
			else {
				head[1] = '\0';
			}
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				char tempC; // "/" 받아주는 변수
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
	for (auto iter = vertexFace.begin(); iter < vertexFace.end(); iter++) { //면 결정하는 인덱스에 해당하는 좌표를 넣어준다. 순서대로
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
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

std::vector<glm::vec3> vertex; //점
std::vector<glm::vec3> normal; //노말
std::vector<glm::vec2> texture; //텍스쳐

std::vector<glm::vec3> p_vertex; //점
std::vector<glm::vec3> p_normal; //노말
std::vector<glm::vec2> p_texture; //텍스쳐

std::vector<glm::vec3> t_vertex; //점
std::vector<glm::vec3> t_normal; //노말
std::vector<glm::vec2> t_texture; //텍스쳐

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f); //--- 카메라 위치
glm::mat4 A_R_matrix = glm::mat4(1.0f);
glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f); //노말벡터

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
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //부모 넣어주기
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

struct pyramid { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
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
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
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
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //부모 넣어주기
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
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //부모 넣어주기
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

void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("OPENGL"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized" << std::endl;
	}
	MakeVertexShaders(); //--- 버텍스 세이더 만들기
	MakeFragmentShaders(); //--- 프래그먼트 세이더 만들기
	initText();
	shaderProgramID = MakeShaderProgram();
	ReadDObj("cube.obj", vertex, normal, texture);
	g_Dices.emplace_back();
	ReadDObj("pyramid.obj", p_vertex, p_normal, p_texture);
	g_pyramids.emplace_back();
	ReadDObj("plane3.obj", t_vertex, t_normal, t_texture);
	g_stages.emplace_back();
	glutTimerFunc(17, Timer, 0);
	glutDisplayFunc(DrawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid DrawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
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
	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}

void MakeVertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = FileToBuf("vertex1.glsl");

	//vertex shader 생성 해주고 생성한 데이터를 vertexShader라는 변수에 저장
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//성공하면 0이 아닌값

	//세이더 객체, 1, 세이더 소스 코드, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, 문자열들의 갯수, 문자열들의 배열, 문자열들의 길이를 저장할 int배열

	//컴파일 해야지요.
	glCompileShader(vertexShader);

	//세이더 결과 어떻게 됐는지? 상태 확인하는 코드
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void MakeFragmentShaders() {
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = FileToBuf("fragment1.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint MakeShaderProgram() {
	//vertex, frament shader가 이미 컴파일은 된 상황
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
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
	glGenTextures(7, &textures[0]); //--- 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, textures[0]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage1 = 512, cubeheightImage1 = 512, cubenumberOfChannel1;
	unsigned char* cubedata1 = stbi_load("BIT_A.bmp", &cubewidthImage1, &cubeheightImage1, &cubenumberOfChannel1, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage1, cubeheightImage1, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata1);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata1);

	glBindTexture(GL_TEXTURE_2D, textures[1]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage2 = 512, cubeheightImage2 = 512, cubenumberOfChannel2;
	unsigned char* cubedata2 = stbi_load("BIT_B.bmp", &cubewidthImage2, &cubeheightImage2, &cubenumberOfChannel2, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage2, cubeheightImage2, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata2);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata2);

	glBindTexture(GL_TEXTURE_2D, textures[2]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage3 = 512, cubeheightImage3 = 512, cubenumberOfChannel3;
	unsigned char* cubedata3 = stbi_load("BIT_C.bmp", &cubewidthImage3, &cubeheightImage3, &cubenumberOfChannel3, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage3, cubeheightImage3, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata3);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata3);

	glBindTexture(GL_TEXTURE_2D, textures[3]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage4 = 512, cubeheightImage4 = 512, cubenumberOfChannel4;
	unsigned char* cubedata4 = stbi_load("BIT_D.bmp", &cubewidthImage4, &cubeheightImage4, &cubenumberOfChannel4, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage4, cubeheightImage4, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata4);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata4);

	glBindTexture(GL_TEXTURE_2D, textures[4]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage5 = 512, cubeheightImage5 = 512, cubenumberOfChannel5;
	unsigned char* cubedata5 = stbi_load("BIT_E.bmp", &cubewidthImage5, &cubeheightImage5, &cubenumberOfChannel5, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage5, cubeheightImage5, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata5);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata5);

	glBindTexture(GL_TEXTURE_2D, textures[5]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int cubewidthImage6 = 512, cubeheightImage6 = 512, cubenumberOfChannel6;
	unsigned char* cubedata6 = stbi_load("BIT_F.bmp", &cubewidthImage6, &cubeheightImage6, &cubenumberOfChannel6, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubewidthImage6, cubeheightImage6, 0, GL_RGB, GL_UNSIGNED_BYTE, cubedata6);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(cubedata6);

	glBindTexture(GL_TEXTURE_2D, textures[6]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
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
