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
void ReadDObj(const std::string objfilename, std::vector<glm::vec3>& vertex)
{
	std::vector<glm::vec4> vertexFace;
	std::vector<glm::vec4> vertexData; //버텍스 값 담아줄 벡터
	std::string line;
	std::ifstream inFile(objfilename);
	char head[3] = { 0,0,0 };
	int vertexFaceNum[3] = { 0,0,0 };
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
			head[1] = '\0';
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				char tempC; // "/" 받아주는 변수
				int tempi[6]; //아직은 노말벡터랑 텍스쳐 안쓰기 때문에 쓰레기처리
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
	for (auto iter = vertexFace.begin(); iter < vertexFace.end(); iter++) { //면 결정하는 인덱스에 해당하는 좌표를 넣어준다. 순서대로
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
int g_WhoDrawNow = 0;
int g_Rotate = 0, g_Rotate1 = 0, g_Rotate2 = 0;
int g_UseGL_DEPTH = 0;
bool g_isOn = false, g_isOn1 = false, g_isOn2 = false;
float theta = 0.0f, theta1 = 8.5f, theta2 = -1.3f, theta3 = -8.5f, theta4 = 1.3f;
int g_Side = 1;
int g_Back = 1;
int g_open = 1;
int g_cnt = 0;
int g_proj = 0;
void MakeVertexShaders();
void MakeFragmentShaders();
void Timer(int val);
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
std::vector<glm::vec3> vertex1; //점

std::vector<glm::vec3> p_vertex; //점
std::vector<glm::vec3> p_vertex1; //점

glm::mat4 A_R_matrix = glm::mat4(1.0f);
glm::mat4 R_matrix = glm::mat4(1.0f);

struct Dice { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	Dice() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		S_matrix = glm::scale(S_matrix, glm::vec3(1.5f, 1.5f, 1.5f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
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
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glUniform3f(vColorLocation, m_color[1].r, m_color[1].g, m_color[1].b);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glUniform3f(vColorLocation, m_color[2].r, m_color[2].g, m_color[2].b);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glUniform3f(vColorLocation, m_color[3].r, m_color[3].g, m_color[3].b);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glUniform3f(vColorLocation, m_color[4].r, m_color[4].g, m_color[4].b);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glUniform3f(vColorLocation, m_color[5].r, m_color[5].g, m_color[5].b);
		glDrawArrays(GL_TRIANGLES, 30, 6);
	}
};
vector<Dice> g_Dices;

struct pyramid { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	pyramid() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * p_vertex.size(), &p_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glm::mat4 parentMatrix(1.0f);
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
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUniform3f(vColorLocation, m_color[1].r, m_color[1].g, m_color[1].b);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glUniform3f(vColorLocation, m_color[2].r, m_color[2].g, m_color[2].b);
		glDrawArrays(GL_TRIANGLES, 6, 3);
		glUniform3f(vColorLocation, m_color[3].r, m_color[3].g, m_color[3].b);
		glDrawArrays(GL_TRIANGLES, 9, 3);
		glUniform3f(vColorLocation, m_color[4].r, m_color[4].g, m_color[4].b);
		glDrawArrays(GL_TRIANGLES, 12, 3);
		glUniform3f(vColorLocation, m_color[4].r, m_color[4].g, m_color[4].b);
		glDrawArrays(GL_TRIANGLES, 15, 3);
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
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glm::mat4 parentMatrix(1.0f);
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
		glLineWidth(5.0f);
		glDrawArrays(GL_LINES, 0, 2);
	}
};
vector<Line> g_lines;

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
	ReadDObj("cube(1).obj", vertex);
	g_Dices.emplace_back();
	ReadDObj("pyramid.obj", p_vertex);
	g_pyramids.emplace_back();
	g_lines.emplace_back(0);
	g_lines.emplace_back(1);
	g_lines.emplace_back(2);
	for (int i = 0;i < vertex.size();i++) {
		vertex1.push_back(vertex[i]);
	}
	for (int i = 0;i < p_vertex.size();i++) {
		p_vertex1.push_back(p_vertex[i]);
	}
	glutTimerFunc(17, Timer, 0);
	glutDisplayFunc(DrawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid DrawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	if (g_proj == 0) {
		glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 50.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	if (g_proj == 1) {
		glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f); //--- 투영 공간 설정: [-100.0, 100.0]
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	if (g_UseGL_DEPTH == 1) {
		glEnable(GL_CULL_FACE);
		glEnable(GL_CW);
		g_lines[0].Draw();
		g_lines[1].Draw();
		g_lines[2].Draw();
		if (g_WhoDrawNow == 1) {
			g_Dices[0].Draw();
		}
		if (g_WhoDrawNow == 2) {
			g_pyramids[0].Draw();
		}
		glDisable(GL_CULL_FACE);
	}
	else {
		g_lines[0].Draw();
		g_lines[1].Draw();
		g_lines[2].Draw();
		if (g_WhoDrawNow == 1) {
			g_Dices[0].Draw();
		}
		if (g_WhoDrawNow == 2) {
			g_pyramids[0].Draw();
		}
	}
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
	switch (key) {
	case 'c':
		for (int i = 0;i < p_vertex.size();i++) {
			p_vertex[i] = p_vertex1[i];
		}
		for (int i = 0;i < vertex.size();i++) {
			vertex[i] = vertex1[i];
		}
		g_WhoDrawNow = 1;
		break;
	case 'p':
		for (int i = 0;i < p_vertex.size();i++) {
			p_vertex[i] = p_vertex1[i];
		}
		for (int i = 0;i < vertex.size();i++) {
			vertex[i] = vertex1[i];
		}
		g_WhoDrawNow = 2;
		break;
	case 't':
		g_Rotate = 1;
		theta = 0;
		if (!g_isOn) {
			theta = 0;
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		else {
			theta = 0;
			g_isOn = false;
		}
		break;
	case 'f':
		g_Rotate = 2;
		theta = 9.0f;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		else {
			theta = 0;
			g_isOn = false;
		}
		break;
	case 's':
		g_Rotate = 3;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		else {
			g_isOn = false;
		}
		break;
	case 'b':
		g_Rotate = 4;
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		else {
			g_isOn = false;
		}
		break;
	case 'y':
		g_Rotate2 = 1;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
		else {
			g_isOn1 = false;
		}
		break;
	case 'Y':
		g_Rotate2 = 2;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 2);
			g_isOn1 = true;
		}
		else {
			g_isOn1 = false;
		}
		break;
	case 'o':
		g_Rotate1 = 1;
		if (!g_isOn2) {
			theta1 = 8.5f;
			theta2 = -1.3f;
			theta3 = -8.5f;
			theta4 = 1.3f;
			glutTimerFunc(17, Timer, 3);
			g_isOn2 = true;
		}
		else {
			theta1 = 8.5f;
			theta2 = -1.3f;
			theta3 = -8.5f;
			theta4 = 1.3f;
			g_isOn2 = false;
		}
		break;
	case 'r':
		g_Rotate1 = 2;
		if (!g_isOn2) {
			theta1 = 8.5f;
			theta2 = -1.3f;
			theta3 = -8.5f;
			theta4 = 1.3f;
			glutTimerFunc(17, Timer, 3);
			g_isOn2 = true;
		}
		else {
			theta1 = 8.5f;
			theta2 = -1.3f;
			theta3 = -8.5f;
			theta4 = 1.3f;
			g_isOn2 = false;
		}
		break;
	case 'h':
		g_UseGL_DEPTH = 1;
		break;
	case 'H':
		g_UseGL_DEPTH = 0;
		break;
	case 'q':
		g_proj = 1;
		break;
	case 'Q':
		g_proj = 0;
		break;
	}
}

void Timer(int val){
	switch (val){
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		if (g_isOn) {
			if (g_Rotate == 1) {
				theta += 3.0f;
				vertex[7].y = 0.5f + 0.5f * cos(180 + theta);
				vertex[7].z = 0.5f * sin(180 + theta);
				vertex[8].y = 0.5f - 0.5f * cos(180 + theta);
				vertex[8].z = -0.5f * sin(180 + theta);
				vertex[6].y = vertex[11].y = vertex[7].y;
				vertex[6].z = vertex[11].z = vertex[7].z;
				vertex[10].y = vertex[9].y = vertex[8].y;
				vertex[10].z = vertex[9].z = vertex[8].z;
			}
			if (g_Rotate == 2) {
				if (g_open == 1) {
					theta += 0.1f;
					vertex[14].y = vertex[13].y - cos(theta / 3.14f);
					vertex[14].z = vertex[13].z - sin(theta / 3.14f);
					vertex[15].y = vertex[16].y = vertex[14].y;
					vertex[15].z = vertex[16].z = vertex[14].z;
					if (theta >= 15.0f) {
						g_open = 2;
						g_isOn = false;
					}
				}
				if (g_open == 2) {
					theta -= 0.1f;
					vertex[14].y = vertex[13].y - cos(theta / 3.14f);
					vertex[14].z = vertex[13].z - sin(theta / 3.14f);
					vertex[15].y = vertex[16].y = vertex[14].y;
					vertex[15].z = vertex[16].z = vertex[14].z;
					if (theta <= 8.5f) {
						g_open = 1;
						g_isOn = false;
					}
				}
			}
			if (g_Rotate == 3) {
				if (g_Side == 1) {
					vertex[18].y += 0.01f;
					vertex[30].y += 0.01f;
					vertex[33].y += 0.01f;
					vertex[21].y += 0.01f;
					vertex[23].y = vertex[19].y = vertex[18].y;
					vertex[35].y = vertex[31].y = vertex[30].y;
					vertex[22].y = vertex[20].y = vertex[21].y;
					vertex[34].y = vertex[32].y = vertex[33].y;
					if (vertex[18].y >= 0.5f) {
						g_Side = 2;
						g_isOn = false;
					}
				}
				else {
					vertex[18].y -= 0.01f;
					vertex[30].y -= 0.01f;
					vertex[33].y -= 0.01f;
					vertex[21].y -= 0.01f;
					vertex[23].y = vertex[19].y = vertex[18].y;
					vertex[35].y = vertex[31].y = vertex[30].y;
					vertex[22].y = vertex[20].y = vertex[21].y;
					vertex[34].y = vertex[32].y = vertex[33].y;
					if (vertex[18].y <= -0.5f) {
						g_Side = 1;
						g_isOn = false;
					}
				}
			}
			if (g_Rotate == 4) {
				if (g_Back == 1) {
					vertex[29].y += 0.01f;
					vertex[24].y += 0.01f;
					vertex[25].y += 0.01f;
					vertex[26].y -= 0.01f;
					vertex[28].y -= 0.01f;
					vertex[27].y -= 0.01f;

					vertex[27].x += 0.01f;
					vertex[29].x -= 0.01f;
					vertex[24].x -= 0.01f;
					vertex[25].x += 0.01f;
					vertex[26].x += 0.01f;
					vertex[28].x -= 0.01f;
					if (vertex[27].x >= 0.0f) {
						g_Back = 2;
						g_isOn = false;
					}
				}
				if (g_Back == 2) {
					vertex[29].y -= 0.01f;
					vertex[24].y -= 0.01f;
					vertex[25].y -= 0.01f;
					vertex[26].y += 0.01f;
					vertex[28].y += 0.01f;
					vertex[27].y += 0.01f;

					vertex[27].x -= 0.01f;
					vertex[29].x += 0.01f;
					vertex[24].x += 0.01f;
					vertex[25].x -= 0.01f;
					vertex[26].x -= 0.01f;
					vertex[28].x += 0.01f;
					if (vertex[27].x <= -0.5f) {
						g_Back = 1;
						g_isOn = false;
					}
				}
			}
			glutTimerFunc(17, Timer, 1);
		}
		break;
	case 2:
		if (g_isOn1) {
			if (g_Rotate2 == 1) {
				R_matrix = glm::rotate(R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (g_Rotate2 == 2) {
				R_matrix = glm::rotate(R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			glutTimerFunc(17, Timer, 2);
		}
		break;
	case 3:
		if (g_isOn2) {
			if (g_Rotate1 == 1) {
				theta1 += 0.1f;
				p_vertex[0].y = (p_vertex[1].y + p_vertex[2].y) / 2 - cos(theta1 / 3.14f);
				p_vertex[0].z = (p_vertex[1].z + p_vertex[2].z) / 2 - sin(theta1 / 3.14f);
				theta2 += 0.1f;
				p_vertex[3].y = (p_vertex[4].y + p_vertex[5].y) / 2 + cos(theta2 / 3.14f);
				p_vertex[3].x = (p_vertex[4].x + p_vertex[5].x) / 2 + sin(theta2 / 3.14f);
				theta3 -= 0.1f;
				p_vertex[6].y = (p_vertex[7].y + p_vertex[8].y) / 2 - cos(theta3 / 3.14f);
				p_vertex[6].z = (p_vertex[7].z + p_vertex[8].z) / 2 - sin(theta3 / 3.14f);
				theta4 -= 0.1f;
				p_vertex[9].y = (p_vertex[10].y + p_vertex[11].y) / 2 + cos(theta4 / 3.14f);
				p_vertex[9].x = (p_vertex[10].x + p_vertex[11].x) / 2 + sin(theta4 / 3.14f);
				if (theta1 >= 21.0f) {
					g_isOn2 = false;
				}
			}
			if (g_Rotate1 == 2) {
				if (g_cnt == 0) {
					theta1 += 0.1f;
					p_vertex[0].y = (p_vertex[1].y + p_vertex[2].y) / 2 - cos(theta1 / 3.14f);
					p_vertex[0].z = (p_vertex[1].z + p_vertex[2].z) / 2 - sin(theta1 / 3.14f);
					if (theta1 >= 15.5f) {
						g_cnt = 1;
					}
				}
				if (g_cnt == 1) {
					theta2 += 0.1f;
					p_vertex[3].y = (p_vertex[4].y + p_vertex[5].y) / 2 + cos(theta2 / 3.14f);
					p_vertex[3].x = (p_vertex[4].x + p_vertex[5].x) / 2 + sin(theta2 / 3.14f);
					if (theta2 >= 5.4f) {
						g_cnt = 2;
					}
				}
				if (g_cnt == 2) {
					theta3 -= 0.1f;
					p_vertex[6].y = (p_vertex[7].y + p_vertex[8].y) / 2 - cos(theta3 / 3.14f);
					p_vertex[6].z = (p_vertex[7].z + p_vertex[8].z) / 2 - sin(theta3 / 3.14f);
					if (theta3 <= -15.0f) {
						g_cnt = 3;
					}
				}
				if (g_cnt == 3) {
					theta4 -= 0.1f;
					p_vertex[9].y = (p_vertex[10].y + p_vertex[11].y) / 2 + cos(theta4 / 3.14f);
					p_vertex[9].x = (p_vertex[10].x + p_vertex[11].x) / 2 + sin(theta4 / 3.14f);
					if (theta4 <= -5.3f) {
						g_isOn2 = false;
						g_cnt = 0;
					}
				}
			}
			glutTimerFunc(17, Timer, 3);
		}
		break;
	default:
		break;
	}
}