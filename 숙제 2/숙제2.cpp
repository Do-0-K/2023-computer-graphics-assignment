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
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
using namespace std;
void ReadDObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& vertexNomal)
{
	std::vector<glm::vec4> vertexFace;
	std::vector<glm::vec4> vertexNomalFace;
	std::vector<glm::vec4> vertexData; //버텍스 값 담아줄 벡터
	std::vector<glm::vec4> vertexNomalData; //버텍스 값 담아줄 벡터
	std::string line;
	std::ifstream inFile(objfilename);
	char head[3] = { 0,0,0 };
	int vertexFaceNum[3] = { 0,0,0 };
	int vertexnomalFaceNum[3] = { 0,0,0 };
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
			else {
				head[1] = '\0';
			}
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				char tempC; // "/" 받아주는 변수
				int tempi[3]; //아직은 노말벡터랑 텍스쳐 안쓰기 때문에 쓰레기처리
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
int Coc = 0, r_s = 0;
int g_garo = 0, g_sero = 0, g_type = 0, g_type2 = 0, k = 0, g_who = 0, g_last = 0;
float g_dy = 0.1f, s = 0.0f;
int rot = 0;
bool g_animation1 = false, g_animation2 = false, g_animation3 = false, g_animation4 = false, g_animation5 = false, g_animation6 = false, g_animation7 = false, g_animation8 = false, g_animation9 = false, g_animation10 = false;
bool g_Weather = false;
bool g_Rotate = false;
bool g_new = false;
void MakeVertexShaders();
void MakeFragmentShaders();
void Timer(int val);
void resize(int width, int height);
void setting();
void key_set();
void setting_simple_animation(int type,int type2);
GLuint MakeShaderProgram();
GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

std::vector<glm::vec3> g_vertex; //점
std::vector<glm::vec3> g_normal; //점

std::vector<glm::vec3> l_vertex; //점
std::vector<glm::vec3> l_normal; //점

glm::vec3 cameraPos = glm::vec3(40.0f, 90.0f, 40.0f); //--- 카메라 위치
glm::vec3 F_cameraPos = glm::vec3(0.0f, 80.0f, 0.0f); //--- 카메라 위치
glm::mat4 A_R_matrix = glm::mat4(1.0f);
glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f); //노말벡터

struct Shape {
	std::vector<glm::vec3> s_vertex; //점
	glm::vec3 m_color[1];
	GLuint m_VBOVertex;
	GLuint m_VBONormal;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	float dy = 2.0f;
	float dx = 2.0f;
	float max_y = 0.0f;
	bool shoot = false;
	int m_num = 0;
	int m_cnt = 0;
	int m_cnt_2 = 0;
	float m_x = 0.0f, m_y = 0.0f, m_z = 0.0f;
	int m_show = 50;
	Shape(int type) {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> pos_y(8, 20);
		uniform_real_distribution<float> color(0, 1);
		max_y = pos_y(dre);
		s_vertex = g_vertex;
		switch (type) {
		case 0:
			m_color[0].r = color(dre);
			m_color[0].g = color(dre);
			m_color[0].b = color(dre);
			S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 100.0f, 0.0f));
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * s_vertex.size(), &s_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_normal.size(), &g_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);
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
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 0, s_vertex.size());
	}
};
vector<Shape>g_Shape;

struct light { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
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
		m_move.y = 80.0f;
		m_move.z = 0.0f;
		S_matrix = glm::scale(S_matrix, glm::vec3(0.1f, 0.1f, 0.1f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 80.0f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_vertex.size(), &l_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_normal.size(), &l_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);
		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //부모 넣어주기
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

struct snow { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
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
		uniform_real_distribution<float> x_pos(-50.0f, 50.0f);
		uniform_real_distribution<float> y_pos(80.0f, 100.0f);
		uniform_real_distribution<float> z_pos(-50.0f, 50.0f);
		m_color[0].r = 1.0f;
		m_color[0].g = 1.0f;
		m_color[0].b = 1.0f;
		recycle_y = y_pos(dre);
		m_move.x = x_pos(dre);
		m_move.y = y_pos(dre);
		m_move.z = z_pos(dre);
		m_move.w = 1;
		S_matrix = glm::scale(S_matrix, glm::vec3(0.08f, 0.08f, 0.08f));
		T_matrix = glm::translate(T_matrix, glm::vec3(x_pos(dre), y_pos(dre), z_pos(dre)));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_vertex.size(), &l_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * l_normal.size(), &l_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);

		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //부모 넣어주기
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

struct rain { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
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
	rain() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> x_pos(-50.0f, 50.0f);
		uniform_real_distribution<float> y_pos(80.0f, 100.0f);
		uniform_real_distribution<float> z_pos(-50.0f, 50.0f);
		m_color[0].r = 0.0f;
		m_color[0].g = 0.0f;
		m_color[0].b = 0.6f;
		recycle_y = y_pos(dre);
		m_move.x = x_pos(dre);
		m_move.y = y_pos(dre);
		m_move.z = z_pos(dre);
		m_move.w = 1;
		S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 1.3f, 0.3f));
		T_matrix = glm::translate(T_matrix, glm::vec3(x_pos(dre), y_pos(dre), z_pos(dre)));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glGenBuffers(1, &m_VBONormal);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_vertex.size(), &g_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBONormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_normal.size(), &g_normal[0], GL_STATIC_DRAW);
		int normalAttrib = glGetAttribLocation(shaderProgramID, "vertexNormal");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(normalAttrib);

		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix)); //부모 넣어주기
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
		glDrawArrays(GL_TRIANGLES, 0, g_vertex.size());
	}
};
vector<rain> g_rains;

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
	shaderProgramID = MakeShaderProgram();
	cout << "garo,sero : ";
	cin >> g_garo >> g_sero;
	g_new = true;
	g_last = g_garo * g_sero - 1;
	ReadDObj("cube(1).obj", g_vertex, g_normal);
	for (int i = 0;i < g_garo * g_sero;i++) {
		g_Shape.emplace_back(0);
	}
	setting();
	ReadDObj("sphere.obj", l_vertex, l_normal);
	g_lights.emplace_back();
	for (int i = 0;i < 400;i++) {
		g_snows.emplace_back();
		g_rains.emplace_back();
	}
	glutTimerFunc(17, Timer, 0);
	glutTimerFunc(17, Timer, 14);
	glutTimerFunc(17, Timer, 11);
	key_set();
	glutDisplayFunc(DrawScene); // 출력 함수의 지정
	glutReshapeFunc(resize); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid DrawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderProgramID);
	glViewport(0, 0, WIDTH, HEIGHT);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_CCW);
	for (auto& t : g_Shape) {
		t.Draw();
	}
	if (r_s == 1) {
		for (auto& a : g_snows) {
			a.Draw();
		}
	}
	if (r_s == 2) {
		for (auto& t : g_rains) {
			t.Draw();
		}
	}
	g_lights[0].Draw();
	glDisable(GL_CULL_FACE);

	glViewport(600, 400, 150, 150);
	glm::vec3 F_cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 F_cameraUp = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 위쪽 방향
	glm::mat4 F_view = glm::mat4(1.0f);
	F_view = glm::lookAt(F_cameraPos, F_cameraDirection, F_cameraUp);
	unsigned int F_viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(F_viewLocation, 1, GL_FALSE, &F_view[0][0]);

	glm::mat4 F_projection = glm::mat4(1.0f);
	F_projection = glm::perspective(glm::radians(60.0f), WIDTH / HEIGHT, 0.1f, 1000.0f);
	F_projection *= A_R_matrix;
	unsigned int F_projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
	glUniformMatrix4fv(F_projectionLocation, 1, GL_FALSE, &F_projection[0][0]);
	unsigned int F_lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(F_lightPosLocation, g_lights[0].m_move.x, g_lights[0].m_move.y, g_lights[0].m_move.z);
	unsigned int F_lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(F_lightColorLocation, light_color.x, light_color.y, light_color.z);
	unsigned int F_viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(F_viewPosLocation, F_cameraPos.x, F_cameraPos.y, F_cameraPos.z);
	glEnable(GL_CULL_FACE);
	glEnable(GL_CCW);
	for (auto& t : g_Shape) {
		t.Draw();
	}
	g_lights[0].Draw();
	if (r_s == 1) {
		for (auto& a : g_snows) {
			a.Draw();
		}
	}
	if (r_s == 2) {
		for (auto& t : g_rains) {
			t.Draw();
		}
	}
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}

void resize(int width, int height) {
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, width, height);
}

void MakeVertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = FileToBuf("vertex.glsl");

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
	fragmentSource = FileToBuf("fragment.glsl"); // 프래그세이더 읽어오기
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
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> color(0, 2);
	switch (key) {
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
	case 'n':
	case 'N':
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
	case '1':
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		if (!g_animation1) {
			for (auto& a : g_Shape) {
				a.s_vertex = g_vertex;
				a.m_cnt = 0;
				a.dy = 2.0f;
				a.dx = 2.0f;
				a.m_cnt_2 = 0;
				a.shoot = false;
				a.T_matrix = glm::mat4(1.0f);
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(a.m_x, a.m_y, a.m_z));
			}
			glutTimerFunc(17, Timer, 1);
			g_animation1 = true;
		}
		break;
	case '2':
		g_animation1 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 1;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation2) {
			glutTimerFunc(17, Timer, 2);
			g_animation2 = true;
		}
		break;
	case '3':
		g_animation1 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 2;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation2) {		
			glutTimerFunc(17, Timer, 2);
			g_animation2 = true;
		}
		break;
	case '4':
		g_animation1 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 3;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation2) {
			glutTimerFunc(17, Timer, 2);
			g_animation2 = true;
		}
		break;
	case '5':
		g_animation1 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 4;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation2) {
			glutTimerFunc(17, Timer, 2);
			g_animation2 = true;
		}
		break;
	case '7':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation10 = false;
		g_who = 0;
		g_last = g_Shape.size() - 1;
		if (!g_animation9) {
			for (auto& a : g_Shape) {
				a.s_vertex = g_vertex;
				a.m_cnt = 0;
				a.dy = 2.0f;
				a.dx = 2.0f;
				a.m_cnt_2 = 0;
				a.shoot = false;
				a.T_matrix = glm::mat4(1.0f);
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(a.m_x, a.m_y, a.m_z));
			}
			glutTimerFunc(17, Timer, 9);
			g_animation9 = true;
		}
		break;
	case '8':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_type = 1;g_type2 = 1;
		setting_simple_animation(g_type, g_type2);
		if (!g_animation10) {
			glutTimerFunc(17, Timer, 10);
			g_animation10 = true;
		}
		break;
	case 'a':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		if (!g_animation4) {
			for (auto& a : g_Shape) {
				a.s_vertex = g_vertex;
				a.m_cnt = 0;
				a.dy = 2.0f;
				a.dx = 2.0f;
				a.m_cnt_2 = 0;
				a.shoot = false;
				a.T_matrix = glm::mat4(1.0f);
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(a.m_x, a.m_y, a.m_z));
			}
			glutTimerFunc(17, Timer, 4);
			g_animation4 = true;
		}
		break;
	case 's':
		g_animation1 = false;
		g_animation2 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 1;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation3) {
			glutTimerFunc(17, Timer, 3);
			g_animation3 = true;
		}
		break;
	case 'd':
		g_animation1 = false;
		g_animation2 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 2;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation3) {
			glutTimerFunc(17, Timer, 3);
			g_animation3 = true;
		}
		break;
	case 'f':
		g_animation1 = false;
		g_animation2 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 3;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation3) {
			glutTimerFunc(17, Timer, 3);
			g_animation3 = true;
		}
		break;
	case 'g':
		g_animation1 = false;
		g_animation2 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 4;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation3) {
			glutTimerFunc(17, Timer, 3);
			g_animation3 = true;
		}
		break;
	case 'A':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		if (!g_animation5) {
			for (auto& a : g_Shape) {
				a.s_vertex = g_vertex;
				a.m_cnt = 0;
				a.dy = 2.0f;
				a.dx = 2.0f;
				a.m_cnt_2 = 0;
				a.shoot = false;
				a.T_matrix = glm::mat4(1.0f);
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(a.m_x, a.m_y, a.m_z));
			}
			glutTimerFunc(17, Timer, 5);
			g_animation5 = true;
		}
		break;
	case 'S':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 1;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation6) {
			glutTimerFunc(17, Timer, 6);
			g_animation6 = true;
		}
		break;
	case 'D':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 2;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation6) {
			glutTimerFunc(17, Timer, 6);
			g_animation6 = true;
		}
		break;
	case 'F':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 3;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation6) {
			glutTimerFunc(17, Timer, 6);
			g_animation6 = true;
		}
		break;
	case 'G':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 4;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation6) {
			glutTimerFunc(17, Timer, 6);
			g_animation6 = true;
		}
		break;
	case 'z':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		if (!g_animation7) {
			for (auto& a : g_Shape) {
				a.s_vertex = g_vertex;
				a.m_cnt = 0;
				a.dy = 2.0f;
				a.dx = 2.0f;
				a.m_cnt_2 = 0;
				a.shoot = false;
				a.T_matrix = glm::mat4(1.0f);
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(a.m_x, a.m_y, a.m_z));
			}
			glutTimerFunc(17, Timer, 7);
			g_animation7 = true;
		}
		break;
	case 'x':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 1;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation8) {
			glutTimerFunc(17, Timer, 8);
			g_animation8 = true;
		}
		break;
	case 'c':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 2;	g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation8) {
			glutTimerFunc(17, Timer, 8);
			g_animation8 = true;
		}
		break;
	case 'v':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 3;
		g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation8) {
			glutTimerFunc(17, Timer, 8);
			g_animation8 = true;
		}
		break;
	case 'b':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_type = 4;
		g_type2 = 0;
		setting_simple_animation(g_type,g_type2);
		if (!g_animation8) {
			glutTimerFunc(17, Timer, 8);
			g_animation8 = true;
		}
		break;
	case 'p':
		g_dy += 0.3f;
		if (g_dy >= 1.0f) {
			g_dy = 1.0f;
		}
		break;
	case 'o':
		g_dy -= 0.3f;
		if (g_dy <= 0.0f) {
			g_dy = 0.1f;
		}
		break;
	case 'w':
		r_s = 1;
		if (!g_Weather) {
			glutTimerFunc(17, Timer, 12);
			g_Weather = true;
			
		}
		else {
			g_Weather = false;
		}
		break;
	case 'e':
		r_s = 2;
		if (!g_Weather) {
			glutTimerFunc(17, Timer, 12);
			g_Weather = true;
		}
		else {
			g_Weather = false;
		}
		break;
	case 'r':
		g_Weather = false;
		r_s = 0;
		break;
	case 't':
		g_animation1 = false;
		g_animation2 = false;
		g_animation3 = false;
		g_animation4 = false;
		g_animation5 = false;
		g_animation6 = false;
		g_animation7 = false;
		g_animation8 = false;
		g_animation9 = false;
		g_animation10 = false;
		g_Weather = false;
		cout << "garo,sero : ";
		cin >> g_garo >> g_sero;
		g_last = g_garo * g_sero - 1;
		g_Shape.clear();
		for (int i = 0;i < g_garo * g_sero;i++) {
			g_Shape.emplace_back(0);
		}
		g_new = true;
		glutTimerFunc(17, Timer, 14);
		setting();
		key_set();
		break;
	case 'y':
		rot = 1;
		if (!g_Rotate) {
			glutTimerFunc(17, Timer, 13);
			g_Rotate = true;
		}
		else {
			g_Rotate = false;
		}
		break;
	case 'Y':
		rot = 2;
		if (!g_Rotate) {
			glutTimerFunc(17, Timer, 13);
			g_Rotate = true;
		}
		else {
			g_Rotate = false;
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
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> color(0, 1);
	switch (val)
	{
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		if (g_animation1) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.s_vertex[7].y += g_dy; //위
					a.s_vertex[6].y += g_dy;
					a.s_vertex[8].y += g_dy;
					a.s_vertex[9].y += g_dy;
					a.s_vertex[10].y += g_dy;
					a.s_vertex[11].y += g_dy;

					a.s_vertex[33].y += g_dy; //오
					a.s_vertex[32].y += g_dy;
					a.s_vertex[34].y += g_dy;

					a.s_vertex[21].y += g_dy;
					a.s_vertex[22].y += g_dy; //왼
					a.s_vertex[20].y += g_dy;

					a.s_vertex[26].y += g_dy;
					a.s_vertex[28].y += g_dy; //뒤
					a.s_vertex[27].y += g_dy;

					a.s_vertex[14].y += g_dy;
					a.s_vertex[15].y += g_dy; //앞
					a.s_vertex[16].y += g_dy;
					a.dy += g_dy;
					if (a.dy >= a.max_y) {
						a.m_cnt++;
					}
				}
				else {
					a.s_vertex[7].y -= g_dy;
					a.s_vertex[6].y -= g_dy;
					a.s_vertex[8].y -= g_dy;
					a.s_vertex[9].y -= g_dy;
					a.s_vertex[10].y -= g_dy;
					a.s_vertex[11].y -= g_dy;
									 
					a.s_vertex[33].y -= g_dy;
					a.s_vertex[32].y -= g_dy;
					a.s_vertex[34].y -= g_dy;
									 
					a.s_vertex[21].y -= g_dy;
					a.s_vertex[22].y -= g_dy;
					a.s_vertex[20].y -= g_dy;
									 
					a.s_vertex[26].y -= g_dy;
					a.s_vertex[28].y -= g_dy;
					a.s_vertex[27].y -= g_dy;
									 
					a.s_vertex[14].y -= g_dy;
					a.s_vertex[15].y -= g_dy;
					a.s_vertex[16].y -= g_dy;
					a.dy -= g_dy;
					if (a.dy < 1.0f) {
						a.m_cnt++;
					}
				}
			}
			glutTimerFunc(17, Timer, 1);
		}
		break;
	case 2:
		if (g_animation2) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.s_vertex[7].y += g_dy;
					a.s_vertex[6].y += g_dy;
					a.s_vertex[8].y += g_dy;
					a.s_vertex[9].y += g_dy;
					a.s_vertex[10].y += g_dy;
					a.s_vertex[11].y += g_dy;
					a.s_vertex[33].y += g_dy;
					a.s_vertex[32].y += g_dy;
					a.s_vertex[34].y += g_dy;
					a.s_vertex[21].y += g_dy;
					a.s_vertex[22].y += g_dy;
					a.s_vertex[20].y += g_dy;
					a.s_vertex[26].y += g_dy;
					a.s_vertex[28].y += g_dy;
					a.s_vertex[27].y += g_dy;
					a.s_vertex[14].y += g_dy;
					a.s_vertex[15].y += g_dy;
					a.s_vertex[16].y += g_dy;
					a.dy += g_dy;
					if (a.dy >= 22.0f) {
						a.m_cnt++;
					}
				}
				else {
					a.s_vertex[7].y -= g_dy;
					a.s_vertex[6].y -= g_dy;
					a.s_vertex[8].y -= g_dy;
					a.s_vertex[9].y -= g_dy;
					a.s_vertex[10].y -= g_dy;
					a.s_vertex[11].y -= g_dy;
					a.s_vertex[33].y -= g_dy;
					a.s_vertex[32].y -= g_dy;
					a.s_vertex[34].y -= g_dy;
					a.s_vertex[21].y -= g_dy;
					a.s_vertex[22].y -= g_dy;
					a.s_vertex[20].y -= g_dy;
					a.s_vertex[26].y -= g_dy;
					a.s_vertex[28].y -= g_dy;
					a.s_vertex[27].y -= g_dy;
					a.s_vertex[14].y -= g_dy;
					a.s_vertex[15].y -= g_dy;
					a.s_vertex[16].y -= g_dy;
					a.dy -= g_dy;
					if (a.dy <= 1.0f) {
						a.m_cnt++;
					}
				}
			}
			glutTimerFunc(17, Timer, 2);
		}
		break;
	case 3:
		if (g_animation3) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.s_vertex[7].y += g_dy;
					a.s_vertex[6].y += g_dy;
					a.s_vertex[8].y += g_dy;
					a.s_vertex[9].y += g_dy;
					a.s_vertex[10].y += g_dy;
					a.s_vertex[11].y += g_dy;
					a.s_vertex[33].y += g_dy;
					a.s_vertex[32].y += g_dy;
					a.s_vertex[34].y += g_dy;
					a.s_vertex[21].y += g_dy;
					a.s_vertex[22].y += g_dy;
					a.s_vertex[20].y += g_dy;
					a.s_vertex[26].y += g_dy;
					a.s_vertex[28].y += g_dy;
					a.s_vertex[27].y += g_dy;
					a.s_vertex[14].y += g_dy;
					a.s_vertex[15].y += g_dy;
					a.s_vertex[16].y += g_dy;
					a.dy += g_dy;
					if (a.dy >= 22.0f) {
						a.m_cnt++;
					}
				}
				else {
					a.s_vertex[7].y -= g_dy;
					a.s_vertex[6].y -= g_dy;
					a.s_vertex[8].y -= g_dy;
					a.s_vertex[9].y -= g_dy;
					a.s_vertex[10].y -= g_dy;
					a.s_vertex[11].y -= g_dy;
					a.s_vertex[33].y -= g_dy;
					a.s_vertex[32].y -= g_dy;
					a.s_vertex[34].y -= g_dy;
					a.s_vertex[21].y -= g_dy;
					a.s_vertex[22].y -= g_dy;
					a.s_vertex[20].y -= g_dy;
					a.s_vertex[26].y -= g_dy;
					a.s_vertex[28].y -= g_dy;
					a.s_vertex[27].y -= g_dy;
					a.s_vertex[14].y -= g_dy;
					a.s_vertex[15].y -= g_dy;
					a.s_vertex[16].y -= g_dy;
					a.dy -= g_dy;
					if (a.dy <= 1.0f) {
						a.m_cnt++;
					}
				}
				if (a.m_cnt_2 % 2 == 0) {
					a.s_vertex[7].x -= 0.01f;
					a.s_vertex[9].x -= 0.01f;
					a.s_vertex[8].x -= 0.01f;

					a.s_vertex[6].x += 0.01f;
					a.s_vertex[10].x += 0.01f;
					a.s_vertex[11].x += 0.01f;

					a.s_vertex[33].x += 0.01f;
					a.s_vertex[32].x += 0.01f;
					a.s_vertex[34].x += 0.01f;

					a.s_vertex[21].x -= 0.01f;
					a.s_vertex[22].x -= 0.01f;
					a.s_vertex[20].x -= 0.01f;

					a.s_vertex[26].x += 0.01f;
					a.s_vertex[28].x -= 0.01f;
					a.s_vertex[27].x += 0.01f;

					a.s_vertex[14].x -= 0.01f;
					a.s_vertex[15].x -= 0.01f;
					a.s_vertex[16].x += 0.01f;

					a.dx += 0.01f;
					if (a.dx >= 2.5f) {
						a.m_cnt_2++;
					}
				}
				else {
					a.s_vertex[7].x += 0.01f;
					a.s_vertex[9].x += 0.01f;
					a.s_vertex[8].x += 0.01f;

					a.s_vertex[6].x -= 0.01f;
					a.s_vertex[10].x -= 0.01f;
					a.s_vertex[11].x -= 0.01f;

					a.s_vertex[33].x -= 0.01f;
					a.s_vertex[32].x -= 0.01f;
					a.s_vertex[34].x -= 0.01f;

					a.s_vertex[21].x += 0.01f;
					a.s_vertex[22].x += 0.01f;
					a.s_vertex[20].x += 0.01f;

					a.s_vertex[26].x -= 0.01f;
					a.s_vertex[28].x += 0.01f;
					a.s_vertex[27].x -= 0.01f;

					a.s_vertex[14].x += 0.01f;
					a.s_vertex[15].x += 0.01f;
					a.s_vertex[16].x -= 0.01f;

					a.dx -= 0.01f;
					if (a.dx <= 2.0f) {
						a.m_cnt_2++;
					}
				}
			}
			glutTimerFunc(17, Timer, 3);
		}
		break;
	case 4:
		if (g_animation4) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.s_vertex[7].y += g_dy;
					a.s_vertex[6].y += g_dy;
					a.s_vertex[8].y += g_dy;
					a.s_vertex[9].y += g_dy;
					a.s_vertex[10].y += g_dy;
					a.s_vertex[11].y += g_dy;
					a.s_vertex[33].y += g_dy;
					a.s_vertex[32].y += g_dy;
					a.s_vertex[34].y += g_dy;
					a.s_vertex[21].y += g_dy;
					a.s_vertex[22].y += g_dy;
					a.s_vertex[20].y += g_dy;
					a.s_vertex[26].y += g_dy;
					a.s_vertex[28].y += g_dy;
					a.s_vertex[27].y += g_dy;
					a.s_vertex[14].y += g_dy;
					a.s_vertex[15].y += g_dy;
					a.s_vertex[16].y += g_dy;
					a.dy += g_dy;
					if (a.dy >= a.max_y) {
						a.m_cnt++;
					}
				}
				else {
					a.s_vertex[7].y -= g_dy;
					a.s_vertex[6].y -= g_dy;
					a.s_vertex[8].y -= g_dy;
					a.s_vertex[9].y -= g_dy;
					a.s_vertex[10].y -= g_dy;
					a.s_vertex[11].y -= g_dy;
					a.s_vertex[33].y -= g_dy;
					a.s_vertex[32].y -= g_dy;
					a.s_vertex[34].y -= g_dy;
					a.s_vertex[21].y -= g_dy;
					a.s_vertex[22].y -= g_dy;
					a.s_vertex[20].y -= g_dy;
					a.s_vertex[26].y -= g_dy;
					a.s_vertex[28].y -= g_dy;
					a.s_vertex[27].y -= g_dy;
					a.s_vertex[14].y -= g_dy;
					a.s_vertex[15].y -= g_dy;
					a.s_vertex[16].y -= g_dy;
					a.dy -= g_dy;
					if (a.dy <= 1.0f) {
						a.m_cnt++;
					}
				}
				if (a.m_cnt_2 % 2 == 0) {
					a.s_vertex[7].x -= 0.01f;
					a.s_vertex[9].x -= 0.01f;
					a.s_vertex[8].x -= 0.01f;

					a.s_vertex[6].x += 0.01f;
					a.s_vertex[10].x += 0.01f;
					a.s_vertex[11].x += 0.01f;

					a.s_vertex[33].x += 0.01f;
					a.s_vertex[32].x += 0.01f;
					a.s_vertex[34].x += 0.01f;

					a.s_vertex[21].x -= 0.01f;
					a.s_vertex[22].x -= 0.01f;
					a.s_vertex[20].x -= 0.01f;

					a.s_vertex[26].x += 0.01f;
					a.s_vertex[28].x -= 0.01f;
					a.s_vertex[27].x += 0.01f;

					a.s_vertex[14].x -= 0.01f;
					a.s_vertex[15].x -= 0.01f;
					a.s_vertex[16].x += 0.01f;

					a.dx += 0.01f;
					if (a.dx >= 2.5f) {
						a.m_cnt_2++;
					}
				}
				else {
					a.s_vertex[7].x += 0.01f;
					a.s_vertex[9].x += 0.01f;
					a.s_vertex[8].x += 0.01f;

					a.s_vertex[6].x -= 0.01f;
					a.s_vertex[10].x -= 0.01f;
					a.s_vertex[11].x -= 0.01f;

					a.s_vertex[33].x -= 0.01f;
					a.s_vertex[32].x -= 0.01f;
					a.s_vertex[34].x -= 0.01f;

					a.s_vertex[21].x += 0.01f;
					a.s_vertex[22].x += 0.01f;
					a.s_vertex[20].x += 0.01f;

					a.s_vertex[26].x -= 0.01f;
					a.s_vertex[28].x += 0.01f;
					a.s_vertex[27].x -= 0.01f;

					a.s_vertex[14].x += 0.01f;
					a.s_vertex[15].x += 0.01f;
					a.s_vertex[16].x -= 0.01f;

					a.dx -= 0.01f;
					if (a.dx <= 2.0f) {
						a.m_cnt_2++;
					}
				}
			}
			glutTimerFunc(17, Timer, 4);
		}
		break;
	case 5:
		if (g_animation5) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.s_vertex[7].y += g_dy;
					a.s_vertex[6].y += g_dy;
					a.s_vertex[8].y += g_dy;
					a.s_vertex[9].y += g_dy;
					a.s_vertex[10].y += g_dy;
					a.s_vertex[11].y += g_dy;
					a.s_vertex[33].y += g_dy;
					a.s_vertex[32].y += g_dy;
					a.s_vertex[34].y += g_dy;
					a.s_vertex[21].y += g_dy;
					a.s_vertex[22].y += g_dy;
					a.s_vertex[20].y += g_dy;
					a.s_vertex[26].y += g_dy;
					a.s_vertex[28].y += g_dy;
					a.s_vertex[27].y += g_dy;
					a.s_vertex[14].y += g_dy;
					a.s_vertex[15].y += g_dy;
					a.s_vertex[16].y += g_dy;
					a.dy += g_dy;
					if (a.dy >= a.max_y) {
						a.m_cnt++;
					}
				}
				else {
					a.s_vertex[7].y -= g_dy;
					a.s_vertex[6].y -= g_dy;
					a.s_vertex[8].y -= g_dy;
					a.s_vertex[9].y -= g_dy;
					a.s_vertex[10].y -= g_dy;
					a.s_vertex[11].y -= g_dy;
					a.s_vertex[33].y -= g_dy;
					a.s_vertex[32].y -= g_dy;
					a.s_vertex[34].y -= g_dy;
					a.s_vertex[21].y -= g_dy;
					a.s_vertex[22].y -= g_dy;
					a.s_vertex[20].y -= g_dy;
					a.s_vertex[26].y -= g_dy;
					a.s_vertex[28].y -= g_dy;
					a.s_vertex[27].y -= g_dy;
					a.s_vertex[14].y -= g_dy;
					a.s_vertex[15].y -= g_dy;
					a.s_vertex[16].y -= g_dy;
					a.dy -= g_dy;
					if (a.dy <= 1.0f) {
						a.m_cnt++;
					}
				}
				if (a.m_cnt_2 % 2 == 0) {
					a.s_vertex[7].x -= 0.01f;
					a.s_vertex[9].x -= 0.01f;
					a.s_vertex[7].z -= 0.01f;
					a.s_vertex[9].z += 0.01f;
					a.s_vertex[8].z += 0.01f;
					a.s_vertex[8].x -= 0.01f;
					
					a.s_vertex[6].x += 0.01f;
					a.s_vertex[10].x += 0.01f;
					a.s_vertex[6].z -= 0.01f;
					a.s_vertex[10].z += 0.01f;
					a.s_vertex[11].z -= 0.01f;
					a.s_vertex[11].x += 0.01f;
					
					a.s_vertex[33].x += 0.01f;
					a.s_vertex[32].x += 0.01f;
					a.s_vertex[34].x += 0.01f;
					a.s_vertex[33].z -= 0.01f;
					a.s_vertex[32].z -= 0.01f;
					a.s_vertex[34].z += 0.01f;
					
					a.s_vertex[21].x -= 0.01f;
					a.s_vertex[22].x -= 0.01f;
					a.s_vertex[20].x -= 0.01f;
					a.s_vertex[21].z += 0.01f;
					a.s_vertex[22].z -= 0.01f;
					a.s_vertex[20].z += 0.01f;

					a.s_vertex[26].x += 0.01f;
					a.s_vertex[28].x -= 0.01f;
					a.s_vertex[27].x += 0.01f;
					a.s_vertex[26].z += 0.01f;
					a.s_vertex[28].z += 0.01f;
					a.s_vertex[27].z += 0.01f;
					
					a.s_vertex[14].x -= 0.01f;
					a.s_vertex[15].x -= 0.01f;
					a.s_vertex[16].x += 0.01f;
					a.s_vertex[14].z -= 0.01f;
					a.s_vertex[15].z -= 0.01f;
					a.s_vertex[16].z -= 0.01f;

					a.dx += 0.01f;
					if (a.dx >= 2.5f) {
						a.m_cnt_2++;
					}
				}
				else {
					a.s_vertex[7].x += 0.01f;
					a.s_vertex[9].x += 0.01f;
					a.s_vertex[8].x += 0.01f;
					
					a.s_vertex[7].z += 0.01f;
					a.s_vertex[9].z -= 0.01f;
					a.s_vertex[8].z -= 0.01f;
					
					a.s_vertex[6].x -= 0.01f;
					a.s_vertex[10].x -= 0.01f;
					a.s_vertex[11].x -= 0.01f;
					a.s_vertex[6].z += 0.01f;
					a.s_vertex[10].z -= 0.01f;
					a.s_vertex[11].z += 0.01f;
					
					a.s_vertex[33].x -= 0.01f;
					a.s_vertex[32].x -= 0.01f;
					a.s_vertex[34].x -= 0.01f;
					a.s_vertex[33].z += 0.01f;
					a.s_vertex[32].z += 0.01f;
					a.s_vertex[34].z -= 0.01f;
					
					a.s_vertex[21].x += 0.01f;
					a.s_vertex[22].x += 0.01f;
					a.s_vertex[20].x += 0.01f;
					a.s_vertex[21].z -= 0.01f;
					a.s_vertex[22].z += 0.01f;
					a.s_vertex[20].z -= 0.01f;

					a.s_vertex[26].x -= 0.01f;
					a.s_vertex[28].x += 0.01f;
					a.s_vertex[27].x -= 0.01f;
					a.s_vertex[26].z -= 0.01f;
					a.s_vertex[28].z -= 0.01f;
					a.s_vertex[27].z -= 0.01f;
					
					a.s_vertex[14].x += 0.01f;
					a.s_vertex[15].x += 0.01f;
					a.s_vertex[16].x -= 0.01f;
					a.s_vertex[14].z += 0.01f;
					a.s_vertex[15].z += 0.01f;
					a.s_vertex[16].z += 0.01f;

					a.dx -= 0.01f;
					if (a.dx <= 2.0f) {
						a.m_cnt_2++;
					}
				}
			}
			glutTimerFunc(17, Timer, 5);
		}
		break;
	case 6:
		if (g_animation6) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.s_vertex[7].y += g_dy;
					a.s_vertex[6].y += g_dy;
					a.s_vertex[8].y += g_dy;
					a.s_vertex[9].y += g_dy;
					a.s_vertex[10].y += g_dy;
					a.s_vertex[11].y += g_dy;
					a.s_vertex[33].y += g_dy;
					a.s_vertex[32].y += g_dy;
					a.s_vertex[34].y += g_dy;
					a.s_vertex[21].y += g_dy;
					a.s_vertex[22].y += g_dy;
					a.s_vertex[20].y += g_dy;
					a.s_vertex[26].y += g_dy;
					a.s_vertex[28].y += g_dy;
					a.s_vertex[27].y += g_dy;
					a.s_vertex[14].y += g_dy;
					a.s_vertex[15].y += g_dy;
					a.s_vertex[16].y += g_dy;
					a.dy += g_dy;
					if (a.dy >= 22.0f) {
						a.m_cnt++;
					}
				}
				else {
					a.s_vertex[7].y -= g_dy;
					a.s_vertex[6].y -= g_dy;
					a.s_vertex[8].y -= g_dy;
					a.s_vertex[9].y -= g_dy;
					a.s_vertex[10].y -= g_dy;
					a.s_vertex[11].y -= g_dy;
					a.s_vertex[33].y -= g_dy;
					a.s_vertex[32].y -= g_dy;
					a.s_vertex[34].y -= g_dy;
					a.s_vertex[21].y -= g_dy;
					a.s_vertex[22].y -= g_dy;
					a.s_vertex[20].y -= g_dy;
					a.s_vertex[26].y -= g_dy;
					a.s_vertex[28].y -= g_dy;
					a.s_vertex[27].y -= g_dy;
					a.s_vertex[14].y -= g_dy;
					a.s_vertex[15].y -= g_dy;
					a.s_vertex[16].y -= g_dy;
					a.dy -= g_dy;
					if (a.dy <= 1.0f) {
						a.m_cnt++;
					}
				}
				if (a.m_cnt_2 % 2 == 0) {
					a.s_vertex[7].x -= 0.01f;
					a.s_vertex[9].x -= 0.01f;
					a.s_vertex[7].z -= 0.01f;
					a.s_vertex[9].z += 0.01f;
					a.s_vertex[8].z += 0.01f;
					a.s_vertex[8].x -= 0.01f;

					a.s_vertex[6].x += 0.01f;
					a.s_vertex[10].x += 0.01f;
					a.s_vertex[6].z -= 0.01f;
					a.s_vertex[10].z += 0.01f;
					a.s_vertex[11].z -= 0.01f;
					a.s_vertex[11].x += 0.01f;

					a.s_vertex[33].x += 0.01f;
					a.s_vertex[32].x += 0.01f;
					a.s_vertex[34].x += 0.01f;
					a.s_vertex[33].z -= 0.01f;
					a.s_vertex[32].z -= 0.01f;
					a.s_vertex[34].z += 0.01f;

					a.s_vertex[21].x -= 0.01f;
					a.s_vertex[22].x -= 0.01f;
					a.s_vertex[20].x -= 0.01f;
					a.s_vertex[21].z += 0.01f;
					a.s_vertex[22].z -= 0.01f;
					a.s_vertex[20].z += 0.01f;

					a.s_vertex[26].x += 0.01f;
					a.s_vertex[28].x -= 0.01f;
					a.s_vertex[27].x += 0.01f;
					a.s_vertex[26].z += 0.01f;
					a.s_vertex[28].z += 0.01f;
					a.s_vertex[27].z += 0.01f;

					a.s_vertex[14].x -= 0.01f;
					a.s_vertex[15].x -= 0.01f;
					a.s_vertex[16].x += 0.01f;
					a.s_vertex[14].z -= 0.01f;
					a.s_vertex[15].z -= 0.01f;
					a.s_vertex[16].z -= 0.01f;

					a.dx += 0.01f;
					if (a.dx >= 2.5f) {
						a.m_cnt_2++;
					}
				}
				else {
					a.s_vertex[7].x += 0.01f;
					a.s_vertex[9].x += 0.01f;
					a.s_vertex[8].x += 0.01f;

					a.s_vertex[7].z += 0.01f;
					a.s_vertex[9].z -= 0.01f;
					a.s_vertex[8].z -= 0.01f;

					a.s_vertex[6].x -= 0.01f;
					a.s_vertex[10].x -= 0.01f;
					a.s_vertex[11].x -= 0.01f;
					a.s_vertex[6].z += 0.01f;
					a.s_vertex[10].z -= 0.01f;
					a.s_vertex[11].z += 0.01f;

					a.s_vertex[33].x -= 0.01f;
					a.s_vertex[32].x -= 0.01f;
					a.s_vertex[34].x -= 0.01f;
					a.s_vertex[33].z += 0.01f;
					a.s_vertex[32].z += 0.01f;
					a.s_vertex[34].z -= 0.01f;

					a.s_vertex[21].x += 0.01f;
					a.s_vertex[22].x += 0.01f;
					a.s_vertex[20].x += 0.01f;
					a.s_vertex[21].z -= 0.01f;
					a.s_vertex[22].z += 0.01f;
					a.s_vertex[20].z -= 0.01f;

					a.s_vertex[26].x -= 0.01f;
					a.s_vertex[28].x += 0.01f;
					a.s_vertex[27].x -= 0.01f;
					a.s_vertex[26].z -= 0.01f;
					a.s_vertex[28].z -= 0.01f;
					a.s_vertex[27].z -= 0.01f;

					a.s_vertex[14].x += 0.01f;
					a.s_vertex[15].x += 0.01f;
					a.s_vertex[16].x -= 0.01f;
					a.s_vertex[14].z += 0.01f;
					a.s_vertex[15].z += 0.01f;
					a.s_vertex[16].z += 0.01f;

					a.dx -= 0.01f;
					if (a.dx <= 2.0f) {
						a.m_cnt_2++;
					}
				}
			}
			glutTimerFunc(17, Timer, 6);
		}
		break;
	case 7:
		if (g_animation7) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
						a.s_vertex[7].y += g_dy;
						a.s_vertex[6].y += g_dy;
						a.s_vertex[8].y += g_dy;
						a.s_vertex[9].y += g_dy;
						a.s_vertex[10].y += g_dy;
						a.s_vertex[11].y += g_dy;
						a.s_vertex[33].y += g_dy;
						a.s_vertex[32].y += g_dy;
						a.s_vertex[34].y += g_dy;
						a.s_vertex[21].y += g_dy;
						a.s_vertex[22].y += g_dy;
						a.s_vertex[20].y += g_dy;
						a.s_vertex[26].y += g_dy;
						a.s_vertex[28].y += g_dy;
						a.s_vertex[27].y += g_dy;
						a.s_vertex[14].y += g_dy;
						a.s_vertex[15].y += g_dy;
						a.s_vertex[16].y += g_dy;
						a.dy += g_dy;
						if (a.dy >= a.max_y) {
							a.m_cnt++;
						}
					}
				else {
						a.s_vertex[7].y -= g_dy;
						a.s_vertex[6].y -= g_dy;
						a.s_vertex[8].y -= g_dy;
						a.s_vertex[9].y -= g_dy;
						a.s_vertex[10].y -= g_dy;
						a.s_vertex[11].y -= g_dy;
						a.s_vertex[33].y -= g_dy;
						a.s_vertex[32].y -= g_dy;
						a.s_vertex[34].y -= g_dy;
						a.s_vertex[21].y -= g_dy;
						a.s_vertex[22].y -= g_dy;
						a.s_vertex[20].y -= g_dy;
						a.s_vertex[26].y -= g_dy;
						a.s_vertex[28].y -= g_dy;
						a.s_vertex[27].y -= g_dy;
						a.s_vertex[14].y -= g_dy;
						a.s_vertex[15].y -= g_dy;
						a.s_vertex[16].y -= g_dy;
						a.dy -= g_dy;
						if (a.dy <= 1.0f) {
							a.m_cnt++;
						}
					}
				if (a.m_cnt_2 % 2 == 0) {
						a.s_vertex[7].z -= 0.01f;
						a.s_vertex[9].z += 0.01f;
						a.s_vertex[8].z += 0.01f;

						a.s_vertex[6].z -= 0.01f;
						a.s_vertex[10].z += 0.01f;
						a.s_vertex[11].z -= 0.01f;

						a.s_vertex[33].z -= 0.01f;
						a.s_vertex[32].z -= 0.01f;
						a.s_vertex[34].z += 0.01f;

						a.s_vertex[21].z += 0.01f;
						a.s_vertex[22].z -= 0.01f;
						a.s_vertex[20].z += 0.01f;

						a.s_vertex[26].z += 0.01f;
						a.s_vertex[28].z += 0.01f;
						a.s_vertex[27].z += 0.01f;

						a.s_vertex[14].z -= 0.01f;
						a.s_vertex[15].z -= 0.01f;
						a.s_vertex[16].z -= 0.01f;

						a.dx += 0.01f;
						if (a.dx >= 2.5f) {
							a.m_cnt_2++;
						}
					}
				else {

						a.s_vertex[7].z += 0.01f;
						a.s_vertex[9].z -= 0.01f;
						a.s_vertex[8].z -= 0.01f;

						a.s_vertex[6].z += 0.01f;
						a.s_vertex[10].z -= 0.01f;
						a.s_vertex[11].z += 0.01f;

						a.s_vertex[33].z += 0.01f;
						a.s_vertex[32].z += 0.01f;
						a.s_vertex[34].z -= 0.01f;

						a.s_vertex[21].z -= 0.01f;
						a.s_vertex[22].z += 0.01f;
						a.s_vertex[20].z -= 0.01f;

						a.s_vertex[26].z -= 0.01f;
						a.s_vertex[28].z -= 0.01f;
						a.s_vertex[27].z -= 0.01f;

						a.s_vertex[14].z += 0.01f;
						a.s_vertex[15].z += 0.01f;
						a.s_vertex[16].z += 0.01f;

						a.dx -= 0.01f;
						if (a.dx <= 2.0f) {
							a.m_cnt_2++;
						}
					}
			}
			glutTimerFunc(17, Timer, 7);
		}
		break;
	case 8:
		if (g_animation8) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
						a.s_vertex[7].y += g_dy;
						a.s_vertex[6].y += g_dy;
						a.s_vertex[8].y += g_dy;
						a.s_vertex[9].y += g_dy;
						a.s_vertex[10].y += g_dy;
						a.s_vertex[11].y += g_dy;
						a.s_vertex[33].y += g_dy;
						a.s_vertex[32].y += g_dy;
						a.s_vertex[34].y += g_dy;
						a.s_vertex[21].y += g_dy;
						a.s_vertex[22].y += g_dy;
						a.s_vertex[20].y += g_dy;
						a.s_vertex[26].y += g_dy;
						a.s_vertex[28].y += g_dy;
						a.s_vertex[27].y += g_dy;
						a.s_vertex[14].y += g_dy;
						a.s_vertex[15].y += g_dy;
						a.s_vertex[16].y += g_dy;
						a.dy += g_dy;
						if (a.dy >= 22.0f) {
							a.m_cnt++;
						}
					}
				else {
						a.s_vertex[7].y -= g_dy;
						a.s_vertex[6].y -= g_dy;
						a.s_vertex[8].y -= g_dy;
						a.s_vertex[9].y -= g_dy;
						a.s_vertex[10].y -= g_dy;
						a.s_vertex[11].y -= g_dy;
						a.s_vertex[33].y -= g_dy;
						a.s_vertex[32].y -= g_dy;
						a.s_vertex[34].y -= g_dy;
						a.s_vertex[21].y -= g_dy;
						a.s_vertex[22].y -= g_dy;
						a.s_vertex[20].y -= g_dy;
						a.s_vertex[26].y -= g_dy;
						a.s_vertex[28].y -= g_dy;
						a.s_vertex[27].y -= g_dy;
						a.s_vertex[14].y -= g_dy;
						a.s_vertex[15].y -= g_dy;
						a.s_vertex[16].y -= g_dy;
						a.dy -= g_dy;
						if (a.dy <= 1.0f) {
							a.m_cnt++;
						}
					}
				if (a.m_cnt_2 % 2 == 0) {
						a.s_vertex[7].z -= 0.01f;
						a.s_vertex[9].z += 0.01f;
						a.s_vertex[8].z += 0.01f;

						a.s_vertex[6].z -= 0.01f;
						a.s_vertex[10].z += 0.01f;
						a.s_vertex[11].z -= 0.01f;

						a.s_vertex[33].z -= 0.01f;
						a.s_vertex[32].z -= 0.01f;
						a.s_vertex[34].z += 0.01f;

						a.s_vertex[21].z += 0.01f;
						a.s_vertex[22].z -= 0.01f;
						a.s_vertex[20].z += 0.01f;

						a.s_vertex[26].z += 0.01f;
						a.s_vertex[28].z += 0.01f;
						a.s_vertex[27].z += 0.01f;

						a.s_vertex[14].z -= 0.01f;
						a.s_vertex[15].z -= 0.01f;
						a.s_vertex[16].z -= 0.01f;

						a.dx += 0.01f;
						if (a.dx >= 2.5f) {
							a.m_cnt_2++;
						}
					}
				else {

						a.s_vertex[7].z += 0.01f;
						a.s_vertex[9].z -= 0.01f;
						a.s_vertex[8].z -= 0.01f;

						a.s_vertex[6].z += 0.01f;
						a.s_vertex[10].z -= 0.01f;
						a.s_vertex[11].z += 0.01f;

						a.s_vertex[33].z += 0.01f;
						a.s_vertex[32].z += 0.01f;
						a.s_vertex[34].z -= 0.01f;

						a.s_vertex[21].z -= 0.01f;
						a.s_vertex[22].z += 0.01f;
						a.s_vertex[20].z -= 0.01f;

						a.s_vertex[26].z -= 0.01f;
						a.s_vertex[28].z -= 0.01f;
						a.s_vertex[27].z -= 0.01f;

						a.s_vertex[14].z += 0.01f;
						a.s_vertex[15].z += 0.01f;
						a.s_vertex[16].z += 0.01f;

						a.dx -= 0.01f;
						if (a.dx <= 2.0f) {
							a.m_cnt_2++;
						}
					}
			}
			glutTimerFunc(17, Timer, 8);
		}
		break;
	case 9:
		if (g_animation9 && (g_who != g_last)) {
			g_Shape[g_who].s_vertex[7].x -= 0.05f;
			g_Shape[g_who].s_vertex[7].z -= 0.05f;
			g_Shape[g_who].s_vertex[7].y += 0.5f;
			g_Shape[g_who].s_vertex[9].x -= 0.05f;
			g_Shape[g_who].s_vertex[9].z += 0.05f;
			g_Shape[g_who].s_vertex[9].y += 0.5f;
			g_Shape[g_who].s_vertex[8].z += 0.05f;
			g_Shape[g_who].s_vertex[8].x -= 0.05f;
			g_Shape[g_who].s_vertex[8].y += 0.5f;

			g_Shape[g_who].s_vertex[6].x += 0.05f;
			g_Shape[g_who].s_vertex[6].z -= 0.05f;
			g_Shape[g_who].s_vertex[6].y += 0.5f;
			g_Shape[g_who].s_vertex[10].x += 0.05f;
			g_Shape[g_who].s_vertex[10].z += 0.05f;
			g_Shape[g_who].s_vertex[10].y += 0.5f;
			g_Shape[g_who].s_vertex[11].z -= 0.05f;
			g_Shape[g_who].s_vertex[11].x += 0.05f;
			g_Shape[g_who].s_vertex[11].y += 0.5f;

			g_Shape[g_who].s_vertex[33].x += 0.05f;
			g_Shape[g_who].s_vertex[32].x += 0.05f;
			g_Shape[g_who].s_vertex[34].x += 0.05f;

			g_Shape[g_who].s_vertex[33].z -= 0.05f;
			g_Shape[g_who].s_vertex[32].z -= 0.05f;
			g_Shape[g_who].s_vertex[34].z += 0.05f;

			g_Shape[g_who].s_vertex[33].y += 0.5f;
			g_Shape[g_who].s_vertex[32].y += 0.5f;
			g_Shape[g_who].s_vertex[34].y += 0.5f;

			g_Shape[g_who].s_vertex[21].x -= 0.05f;
			g_Shape[g_who].s_vertex[22].x -= 0.05f;
			g_Shape[g_who].s_vertex[20].x -= 0.05f;

			g_Shape[g_who].s_vertex[21].z += 0.05f;
			g_Shape[g_who].s_vertex[22].z -= 0.05f;
			g_Shape[g_who].s_vertex[20].z += 0.05f;

			g_Shape[g_who].s_vertex[21].y += 0.5f;
			g_Shape[g_who].s_vertex[22].y += 0.5f;
			g_Shape[g_who].s_vertex[20].y += 0.5f;

			g_Shape[g_who].s_vertex[26].x += 0.05f;
			g_Shape[g_who].s_vertex[28].x -= 0.05f;
			g_Shape[g_who].s_vertex[27].x += 0.05f;

			g_Shape[g_who].s_vertex[26].z += 0.05f;
			g_Shape[g_who].s_vertex[28].z += 0.05f;
			g_Shape[g_who].s_vertex[27].z += 0.05f;

			g_Shape[g_who].s_vertex[26].y += 0.5f;
			g_Shape[g_who].s_vertex[28].y += 0.5f;
			g_Shape[g_who].s_vertex[27].y += 0.5f;

			g_Shape[g_who].s_vertex[14].x -= 0.05f;
			g_Shape[g_who].s_vertex[15].x -= 0.05f;
			g_Shape[g_who].s_vertex[16].x += 0.05f;

			g_Shape[g_who].s_vertex[14].z -= 0.05f;
			g_Shape[g_who].s_vertex[15].z -= 0.05f;
			g_Shape[g_who].s_vertex[16].z -= 0.05f;

			g_Shape[g_who].s_vertex[14].y += 0.5f;
			g_Shape[g_who].s_vertex[15].y += 0.5f;
			g_Shape[g_who].s_vertex[16].y += 0.5f;

			///////////////////////////////////////
			g_Shape[g_last].s_vertex[7].x -= 0.05f;
			g_Shape[g_last].s_vertex[7].z -= 0.05f;
			g_Shape[g_last].s_vertex[7].y += 0.5f;
			g_Shape[g_last].s_vertex[9].x -= 0.05f;
			g_Shape[g_last].s_vertex[9].z += 0.05f;
			g_Shape[g_last].s_vertex[9].y += 0.5f;
			g_Shape[g_last].s_vertex[8].z += 0.05f;
			g_Shape[g_last].s_vertex[8].x -= 0.05f;
			g_Shape[g_last].s_vertex[8].y += 0.5f;

			g_Shape[g_last].s_vertex[6].x += 0.05f;
			g_Shape[g_last].s_vertex[6].z -= 0.05f;
			g_Shape[g_last].s_vertex[6].y += 0.5f;
			g_Shape[g_last].s_vertex[10].x += 0.05f;
			g_Shape[g_last].s_vertex[10].z += 0.05f;
			g_Shape[g_last].s_vertex[10].y += 0.5f;
			g_Shape[g_last].s_vertex[11].z -= 0.05f;
			g_Shape[g_last].s_vertex[11].x += 0.05f;
			g_Shape[g_last].s_vertex[11].y += 0.5f;

			g_Shape[g_last].s_vertex[33].x += 0.05f;
			g_Shape[g_last].s_vertex[32].x += 0.05f;
			g_Shape[g_last].s_vertex[34].x += 0.05f;

			g_Shape[g_last].s_vertex[33].z -= 0.05f;
			g_Shape[g_last].s_vertex[32].z -= 0.05f;
			g_Shape[g_last].s_vertex[34].z += 0.05f;

			g_Shape[g_last].s_vertex[33].y += 0.5f;
			g_Shape[g_last].s_vertex[32].y += 0.5f;
			g_Shape[g_last].s_vertex[34].y += 0.5f;

			g_Shape[g_last].s_vertex[21].x -= 0.05f;
			g_Shape[g_last].s_vertex[22].x -= 0.05f;
			g_Shape[g_last].s_vertex[20].x -= 0.05f;

			g_Shape[g_last].s_vertex[21].z += 0.05f;
			g_Shape[g_last].s_vertex[22].z -= 0.05f;
			g_Shape[g_last].s_vertex[20].z += 0.05f;

			g_Shape[g_last].s_vertex[21].y += 0.5f;
			g_Shape[g_last].s_vertex[22].y += 0.5f;
			g_Shape[g_last].s_vertex[20].y += 0.5f;

			g_Shape[g_last].s_vertex[26].x += 0.05f;
			g_Shape[g_last].s_vertex[28].x -= 0.05f;
			g_Shape[g_last].s_vertex[27].x += 0.05f;

			g_Shape[g_last].s_vertex[26].z += 0.05f;
			g_Shape[g_last].s_vertex[28].z += 0.05f;
			g_Shape[g_last].s_vertex[27].z += 0.05f;

			g_Shape[g_last].s_vertex[26].y += 0.5f;
			g_Shape[g_last].s_vertex[28].y += 0.5f;
			g_Shape[g_last].s_vertex[27].y += 0.5f;

			g_Shape[g_last].s_vertex[14].x -= 0.05f;
			g_Shape[g_last].s_vertex[15].x -= 0.05f;
			g_Shape[g_last].s_vertex[16].x += 0.05f;

			g_Shape[g_last].s_vertex[14].z -= 0.05f;
			g_Shape[g_last].s_vertex[15].z -= 0.05f;
			g_Shape[g_last].s_vertex[16].z -= 0.05f;

			g_Shape[g_last].s_vertex[14].y += 0.5f;
			g_Shape[g_last].s_vertex[15].y += 0.5f;
			g_Shape[g_last].s_vertex[16].y += 0.5f;

			g_Shape[g_who].dx += 0.05f;
			g_Shape[g_last].dx += 0.05f;
			if (g_Shape[g_who].dx >= 2.5f) {
				g_Shape[g_who].shoot = true;
				g_Shape[g_last].shoot = true;
				g_who++;
				g_last--;
			}
			for (auto& a : g_Shape) {
				if (a.shoot) {
					a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
					a.dy += 5.0f;
					if (a.dy >= 90.0f) {
						a.shoot = false;
					}
				}
			}
			glutTimerFunc(17, Timer, 9);
		}
		break;
	case 10:
		if (g_animation10) {
			for (auto& a : g_Shape) {
				if (a.m_cnt % 2 == 0) {
					a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, 0.0f, 0.5f));
					a.dx += 0.5f;
					if (a.dx >= 10.0f) {
						a.m_cnt++;
					}
				}
				else {
					a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
					a.dx -= 0.5f;
					if (a.dx < -10.0f) {
						a.m_cnt++;
					}
				}
			}
			glutTimerFunc(17, Timer, 10);
		}
		break;
	case 11:
		for (auto& a : g_Shape) {
			a.m_color->r = color(dre);
			a.m_color->g = color(dre);
			a.m_color->b = color(dre);
		}
		glutTimerFunc(110, Timer, 11);
		break;
	case 12:
		if (g_Weather) {
			for (auto& a : g_snows) {
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
				a.m_move.y -= 1.0f;
				if (a.m_move.y <= 0.0f) {
					a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, a.recycle_y, 0.0f));
					a.m_move.y = a.recycle_y;
				}
			}
			for (auto& t : g_rains) {
				t.T_matrix = glm::translate(t.T_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
				t.m_move.y -= 1.0f;
				if (t.m_move.y <= 0.0f) {
					t.T_matrix = glm::translate(t.T_matrix, glm::vec3(0.0f, t.recycle_y, 0.0f));
					t.m_move.y = t.recycle_y;
				}
			}
			glutTimerFunc(17, Timer, 12);
		}
		break;
	case 13:
		if (g_Rotate) {
			if (rot == 1) {
				for (auto& a : g_snows) {
					a.A_R_matrix = glm::rotate(a.A_R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				for (auto& a : g_rains) {
					a.A_R_matrix = glm::rotate(a.A_R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				for (auto& a : g_Shape) {
					a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			if (rot == 2) {
				for (auto& a : g_snows) {
					a.A_R_matrix = glm::rotate(a.A_R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				for (auto& a : g_rains) {
					a.A_R_matrix = glm::rotate(a.A_R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				for (auto& a : g_Shape) {
					a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			glutTimerFunc(17, Timer, 13);
		}
		break;
	case 14:
		if (g_new) {
			for (auto& a : g_Shape) {
				a.T_matrix = glm::translate(a.T_matrix, glm::vec3(0.0f, -2.0f, 0.0f));
				a.m_show--;
				if (a.m_show == 0) {
					g_new = false;
					a.m_show = 50;
				}
			}
			glutTimerFunc(17, Timer, 14);
		}
		break;
	default:
		break;
	}
}

void setting()
{
	for (int i = 0;i < g_sero;i++) {
		for (int j = 0;j < g_garo;j++) {
			g_Shape[i * g_garo + j].T_matrix = glm::translate(g_Shape[i * g_garo + j].T_matrix, glm::vec3(-2.0f * g_garo / 2 + 2.0f * i, 0.0f, -2.0f * g_sero / 2 + 2.0f * j));
			g_Shape[i * g_garo + j].m_x = -2.0f * g_garo / 2 + 2.0f * i;
			g_Shape[i * g_garo + j].m_y = 0.0f;
			g_Shape[i * g_garo + j].m_z = -2.0f * g_sero / 2 + 2.0f * j;
		}
	}
}

void setting_simple_animation(int type, int type2) {
	k = 0;
	s = 0.0f;
	for (auto& a : g_Shape) {
		a.s_vertex = g_vertex;
		a.m_cnt = 0;
		a.dy = 2.0f;
		a.dx = 2.0f;
		a.m_cnt_2 = 0;
		a.shoot = false;
		a.T_matrix = glm::mat4(1.0f);
		a.T_matrix = glm::translate(a.T_matrix,glm::vec3(a.m_x,a.m_y,a.m_z));
	}
	switch (type) {
	case 0:
		break;
	case 1:
		for (int i = 0;i < g_sero;i++) {
			for (int j = 0;j < g_garo;j++) {
				if (g_sero >= g_garo) {
					g_Shape[i * g_garo + j].m_num = i;
				}
				else {
					g_Shape[i * g_garo + j].m_num = j;
				}
			}
		}
		break;
	case 2:
		for (int i = 0;i < g_sero;i++) {
			for (int j = 0;j < g_garo;j++) {
				if (i % 2 == 0) {
					if (j % 2 == 0) {
						g_Shape[i * g_garo + j].m_num = i;
					}
					else {
						g_Shape[i * g_garo + j].m_num = j;
					}
				}
				else {
					if (j % 2 == 0) {
						g_Shape[i * g_garo + j].m_num = j;
					}
					else {
						g_Shape[i * g_garo + j].m_num = i;
					}
				}
			}
		}
		break;
	case 3:
		for (int i = 0;i < g_sero;i++) {
			for (int j = 0;j < g_garo;j++) {
				if (i % 2 == 0) {
					if (j % 2 == 0) {
						g_Shape[i * g_garo + j].m_num = 0;
					}
					else {
						g_Shape[i * g_garo + j].m_num = 20;
					}
				}
				else {
					if (j % 2 == 0) {
						g_Shape[i * g_garo + j].m_num = 20;
					}
					else {
						g_Shape[i * g_garo + j].m_num = 0;
					}
				}
			}
		}
		break;
	case 4:
		for (int g = 0;g < g_sero;g++) {
			for (int i = k;i < g_sero-k;i++) {
				for (int j = k;j < g_garo-k;j++) {
					g_Shape[i * g_garo + j].m_num = k;
				}
			}
			k++;
		}
		break;
	}
	switch (type2) {
	case 0:
		for (int i = 0;i < g_Shape.size();i++) {
			g_Shape[i].s_vertex[7].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[6].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[8].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[9].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[10].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[11].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[33].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[32].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[34].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[21].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[22].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[20].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[26].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[28].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[27].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[14].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[15].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].s_vertex[16].y = 20.0f - g_Shape[i].m_num * 1.0f;
			g_Shape[i].dy = 20.0f - g_Shape[i].m_num * 1.0f;
		}
		break;
	case 1:
		for (auto& e : g_Shape) {
			s = 40.0f - e.m_num * 2.0f;
			e.T_matrix = glm::translate(e.T_matrix, glm::vec3(0.0f, 0.0f, s));
			e.dx = s;
		}
		break;
	}
}

void key_set() {
	cout << "////////////애니메이션 단축키////////////" << endl;
	cout << "1~5 : 애니메이션" << endl;
	cout << "a,s,d,f,g : 애니메이션 + z 신축" << endl;
	cout << "z,x,c,v,b : 애니메이션 + x 신축" << endl;
	cout << "A,S,D,F,G : 애니메이션 + x 신축 + z 신축" << endl;
	cout << "7 : 발사 애니메이션" << endl;
	cout << "8 : 가로 웨이브" << endl;
	cout << "y/Y : 카메라 양/음 회전" << endl;
	cout << "////////////날씨 애니메이션 단축키////////////" << endl;
	cout << "w : 눈 내리기/ 멈추기" << endl;
	cout << "e : 비 내리기/ 멈추기" << endl;
	cout << "r : 날씨 초기화" << endl;
	cout << "////////////조명////////////" << endl;
	cout << "+ : 조명 밝기 증가" << endl;
	cout << "- : 조명 밝기 감소" << endl;
	cout << "m : 조명 켜기" << endl;
	cout << "M : 조명 끄기" << endl;
	cout << "n : 조명 3원색 랜덤" << endl;
	cout << "////////////기타////////////" << endl;
	cout << "p : 속도 증가" << endl;
	cout << "o : 속도 감소" << endl;
	cout << "t : 초기화 " << endl;
	cout << "q/Q : 프로그램 종료" << endl;
}