#include <iostream>
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
using namespace std;
float r = 1.0, g = 1.0, b = 1.0;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int val);
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
glm::mat4 R_matrix = glm::mat4(1.0f);
glm::mat4 S_matrix = glm::mat4(1.0f);
glm::mat4 T_matrix = glm::mat4(1.0f);
glm::mat4 A_R_matrix = glm::mat4(1.0f);
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
struct Dice {
	glm::vec4 m_vertex[36];
	glm::vec4 m_color[8];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
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
		for (int i = 0;i < 8;i++) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		m_vertex[0].x = 0.0f;m_vertex[0].y = 0.0f;m_vertex[0].z = 0.0f;
		m_vertex[1].x = 0.0f;m_vertex[1].y = 0.0f;m_vertex[1].z = 0.3f;
		m_vertex[2].x = 0.0f;m_vertex[2].y = 0.3f;m_vertex[2].z = 0.0f;
		m_vertex[3].x = 0.0f;m_vertex[3].y = 0.3f;m_vertex[3].z = 0.3f;
		m_vertex[4].x = 0.3f;m_vertex[4].y = 0.0f;m_vertex[4].z = 0.0f;
		m_vertex[5].x = 0.3f;m_vertex[5].y = 0.0f;m_vertex[5].z = 0.3f;
		m_vertex[6].x = 0.3f;m_vertex[6].y = 0.3f;m_vertex[6].z = 0.0f;
		m_vertex[7].x = 0.3f;m_vertex[7].y = 0.3f;m_vertex[7].z = 0.3f;
		m_vertex[0].w = m_vertex[1].w = m_vertex[2].w = m_vertex[3].w = m_vertex[4].w = m_vertex[5].w = m_vertex[6].w = m_vertex[7].w = 1;

		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.3f, 0.3f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
		//for (int i = 0;i < 8;i++) {
		//	R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//	m_vertex[i] = R_matrix * m_vertex[i];
		//	R_matrix = glm::mat4(1.0f);
		//	R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//	m_vertex[i] = R_matrix * m_vertex[i];
		//	R_matrix = glm::mat4(1.0f);
		//}
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void Draw() {
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
		glBindVertexArray(m_VAO);
		glPointSize(4.0f);
		glDrawArrays(GL_POINTS, 0, 36);
	}
};
vector<Dice> g_Dices;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
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
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	g_Dices.emplace_back(400, 300);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutTimerFunc(17, Timer, 0);
	glutKeyboardFunc(Keyboard);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	g_Dices[0].Draw();
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex.glsl");

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
void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
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
GLuint make_shaderProgram() {
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
	case '1':
		R_matrix = glm::rotate(R_matrix, glm::radians(3.0f), glm::vec3(0, 1, 0));
		break;
	case '2':
		R_matrix = glm::rotate(R_matrix, glm::radians(-3.0f), glm::vec3(0, 1, 0));
		break;
	case '3':
		R_matrix = glm::rotate(R_matrix, glm::radians(3.0f), glm::vec3(1, 0, 0));
		break;
	case '4':
		R_matrix = glm::rotate(R_matrix, glm::radians(-3.0f), glm::vec3(1, 0, 0));
		break;
	case '5':
		break;
	case '6':
		break;
	case '7':
		break;
	case '8':
		break;
	case '9':
		S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		break;
	case '0':
		S_matrix = glm::scale(S_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
		break;
	case 'q':
		break;
	case 'w':
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, -0.05f));
		break;
	case 'a':
		T_matrix = glm::translate(T_matrix, glm::vec3(-0.05f, 0.0f, 0));
		break;
	case 's':
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.05f));
		break;
	case 'd':
		T_matrix = glm::translate(T_matrix, glm::vec3(0.05f, 0.0f, 0));
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
