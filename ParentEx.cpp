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
int g_HowDrawNow = 0;
int g_Rotate = 0, g_Rotate1 = 0;
bool g_isOn = false, g_isOn1 = false, g_isOn2 = false;
int g_proj = 1;
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
void InitBuffer();

std::pair< GLuint, int> g_sphere;
std::pair< GLuint, int> g_circle;

enum AXIS
{
	X, Y, Z
};
const glm::vec3 AXIS_X = glm::vec3(1, 0, 0);
const glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
const glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);

class Shape {
	GLuint m_vbo;
	int m_vertexSize;
	glm::vec3 m_color;
	glm::mat4 m_scaleMatrix;
	glm::mat4 m_transMatrix;
	glm::mat4 m_rotateMatrix;
	glm::mat4 m_revolutionMatrix;
	Shape* m_parent;

	glm::vec3 m_rotateAngle;
	glm::vec3 m_revolutionAngle;
	glm::vec3 m_translate;
public:
	Shape()
	{

	}
	Shape(GLuint vbo, int vertexSize, Shape* parent, glm::vec3 color)
		:m_vbo(vbo), m_vertexSize(vertexSize), m_parent(parent), m_color(color),
		m_scaleMatrix(glm::mat4(1.0f)),
		m_transMatrix(glm::mat4(1.0f)),
		m_rotateMatrix(glm::mat4(1.0f)),
		m_revolutionMatrix(glm::mat4(1.0f)),
		m_translate(glm::vec3(0, 0, 0)),
		m_rotateAngle(glm::vec3(0, 0, 0)),
		m_revolutionAngle(glm::vec3(0, 0, 0)) {}

	Shape(pair< GLuint, int> model, Shape* parent, glm::vec3 color)
		:m_vbo(model.first), m_vertexSize(model.second), m_parent(parent), m_color(color),
		m_scaleMatrix(glm::mat4(1.0f)),
		m_transMatrix(glm::mat4(1.0f)),
		m_rotateMatrix(glm::mat4(1.0f)),
		m_revolutionMatrix(glm::mat4(1.0f)),
		m_translate(glm::vec3(0, 0, 0)),
		m_rotateAngle(glm::vec3(0, 0, 0)),
		m_revolutionAngle(glm::vec3(0, 0, 0)) {}
	void Draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glm::mat4 parentMatrix(1.0f);
		if (!m_parent) {
			parentMatrix = m_parent->GetMatrix();
		}
		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uParent");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(parentMatrix));

		ResetMatrix();
		CalRevolution(Y);
		CalRevolution(X);
		CalRevolution(Z);
		CalRotate(Y);
		CalRotate(X);
		CalRotate(Z);
		CalTranslate();

		unsigned int revolutionLocation = glGetUniformLocation(shaderProgramID, "uRevolution");
		glUniformMatrix4fv(revolutionLocation, 1, GL_FALSE, glm::value_ptr(m_revolutionMatrix));
		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "uRotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(m_rotateMatrix));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "uTranslate");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(m_transMatrix));
		unsigned int scaleLocation = glGetUniformLocation(shaderProgramID, "uScale");
		glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(m_scaleMatrix));

		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color.r, m_color.g, m_color.b);
		glLineWidth(5.0f);
		glDrawArrays(GL_TRIANGLES, 0, m_vertexSize);
	}
public:
	void SetRotateAngle(AXIS&& axis, float angle)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_rotateAngle.x = angle;
		}
		break;
		case AXIS::Y:
		{
			m_rotateAngle.y = angle;
		}
		break;
		case AXIS::Z:
		{
			m_rotateAngle.z = angle;
		}
		break;
		default:
			break;
		}
	}
	void SetRevolutionAngle(AXIS&& axis, float angle)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_revolutionAngle.x = angle;
		}
		break;
		case AXIS::Y:
		{
			m_revolutionAngle.y = angle;
		}
		break;
		case AXIS::Z:
		{
			m_revolutionAngle.z = angle;
		}
		break;
		default:
			break;
		}
	}
	void Translate(AXIS&& axis, float diff)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_translate.x += diff;
		}
		break;
		case AXIS::Y:
		{
			m_translate.y += diff;
		}
		break;
		case AXIS::Z:
		{
			m_translate.z += diff;
		}
		break;
		default:
			break;
		}
	}
	glm::mat4 GetMatrix()
	{
		return m_revolutionMatrix * m_transMatrix * m_rotateMatrix * m_scaleMatrix;
	}
private:
	void ResetMatrix()
	{
		m_scaleMatrix = glm::mat4(1.0f);
		m_transMatrix = glm::mat4(1.0f);
		m_rotateMatrix = glm::mat4(1.0f);
		m_revolutionMatrix = glm::mat4(1.0f);
	}
	void CalRotate(AXIS&& axis)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_rotateMatrix = glm::rotate(m_rotateMatrix, glm::radians(m_rotateAngle[0]), AXIS_X);
		}
		break;
		case AXIS::Y:
		{
			m_rotateMatrix = glm::rotate(m_rotateMatrix, glm::radians(m_rotateAngle[1]), AXIS_Y);
		}
		break;
		case AXIS::Z:
		{
			m_rotateMatrix = glm::rotate(m_rotateMatrix, glm::radians(m_rotateAngle[2]), AXIS_Z);
		}
		break;
		default:
			break;
		}
	}
	void CalRevolution(AXIS&& axis)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_revolutionMatrix = glm::rotate(m_revolutionMatrix, glm::radians(m_revolutionAngle[0]), AXIS_X);
		}
		break;
		case AXIS::Y:
		{
			m_revolutionMatrix = glm::rotate(m_revolutionMatrix, glm::radians(m_revolutionAngle[1]), AXIS_Y);
		}
		break;
		case AXIS::Z:
		{
			m_revolutionMatrix = glm::rotate(m_revolutionMatrix, glm::radians(m_revolutionAngle[2]), AXIS_Z);
		}
		break;
		default:
			break;
		}
	}

	void CalRotate(AXIS&& axis, float angle)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_rotateMatrix = glm::rotate(m_rotateMatrix, glm::radians(angle), AXIS_X);
		}
		break;
		case AXIS::Y:
		{
			m_rotateMatrix = glm::rotate(m_rotateMatrix, glm::radians(angle), AXIS_Y);
		}
		break;
		case AXIS::Z:
		{
			m_rotateMatrix = glm::rotate(m_rotateMatrix, glm::radians(angle), AXIS_Z);
		}
		break;
		default:
			break;
		}
	}
	void CalRevolution(AXIS&& axis, float angle)
	{
		switch (axis)
		{
		case AXIS::X:
		{
			m_revolutionMatrix = glm::rotate(m_revolutionMatrix, glm::radians(angle), AXIS_X);
		}
		break;
		case AXIS::Y:
		{
			m_revolutionMatrix = glm::rotate(m_revolutionMatrix, glm::radians(angle), AXIS_Y);
		}
		break;
		case AXIS::Z:
		{
			m_revolutionMatrix = glm::rotate(m_revolutionMatrix, glm::radians(angle), AXIS_Z);
		}
		break;
		default:
			break;
		}
	}
	void CalTranslate()
	{
		m_transMatrix = glm::translate(m_transMatrix, m_translate);
	}
};


vector<Shape>g_Shapes;

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
	InitBuffer();
	g_Shapes.emplace_back(g_sphere, nullptr, glm::vec3(1, 0.5, 0.2));//solor
	g_Shapes.emplace_back(g_sphere, &g_Shapes[0], glm::vec3(0.5, 1.0, 0.2));
	g_Shapes.emplace_back(g_circle, &g_Shapes[0], glm::vec3(0.5, 1.0, 0.2));



	glutTimerFunc(17, Timer, 0);
	glutTimerFunc(17, Timer, 3);
	glutTimerFunc(17, Timer, 4);
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
		glm::vec3 cameraPos = glm::vec3(40.0f, 40.0f, 40.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 500.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	if (g_proj == 1) {
		glm::vec3 cameraPos = glm::vec3(2.0f, 3.0f, 3.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f); //--- 투영 공간 설정: [-100.0, 100.0]
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_CW);
	for (auto& t : g_Shapes) {
		t.Draw();
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

	}
}

void Timer(int val) {
	switch (val) {
	case 0:
	{
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
	}
	break;
	default:
		break;
	}
}

void InitBuffer()
{
	vector<glm::vec3> sphereVertex;
	ReadDObj("Sphere.obj", sphereVertex);
	glGenBuffers(1, &g_sphere.first);
	g_sphere.second = sphereVertex.size();
	glBindBuffer(GL_VERTEX_ARRAY, g_sphere.first);
	int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_sphere.second, &sphereVertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(positionAttrib);

	vector<glm::vec3> circle;
	for (int i = 0; i < 640; ++i) {
		circle.emplace_back(sin(glm::radians((float)i)), 0, cos(glm::radians((float)i)));
	}
	glGenBuffers(1, &g_circle.first);
	g_circle.second = circle.size();
	glBindBuffer(GL_VERTEX_ARRAY, g_circle.first);
	int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_circle.second, &circle[0], GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(positionAttrib);

}