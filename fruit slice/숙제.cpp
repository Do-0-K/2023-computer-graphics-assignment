#include "start1.h"
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
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
float mouse_x = 0, mouse_y = 0;
bool g_Drag = false, g_cutting = false;
int g_Dumynum = 0, g_Shapenum = 1, g_cutlist = 0, cnt = 0, g_How2save = 0, g_Who = 0, g_HowtoDraw = 0, g_whatShape = 0;
float equation_x1 = 0, equation_x2 = 0, equation_y1 = 0, equation_y2 = 0, g_MovePnt_x = 0, g_MovePnt_y = 0;
float r = 0.0f, g = 0.0f, b = 0.0f, g_Speed = 0.008f;
int size_dum = 0, size_Sha = 0, g_Cosnum = 1, g_Drawcos = 0, g_chosecos = 0, g_miniCosnum = 0;
int point[2] = {0,0};
void make_vertexShaders();
void make_fragmentShaders();
void Timer(int val);
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid drawScene(GLvoid);
GLvoid Motion(int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

const float MIN_DIFF_FLT = 0.00001f;

enum TIMER_CASE {
	RENDERING,
	POP_OUT,
	MAKE_SHAPE,
	POCKET_MOVE,
	SHAPE_MOVE,
	SHAPE_ROTATE
};

std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

struct Shape {
	std::vector<glm::vec3> s_vertex; //점
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0.0f,0.0f,0.0f };
	GLuint m_VBOVertex;
	int m_cnt1 = 0;
	int m_exist = 1;
	float m_course = 0;
	float plus_y = 0.0f;
	float plus_x = 0.0f;
	float course1_x = 0.0f;
	float course1_y = 0.0f;
	float course2_x = 0.0f;
	float course2_y = 0.0f;
	float course3_x = 0.0f;
	float course3_y = 0.0f;
	float rotate = 0.0f;
	Shape(int type,int cos) {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		uniform_real_distribution<float> pos_y(0.0, 0.8);
		uniform_real_distribution<float> pos_x1(-0.7, 1.5);
		uniform_real_distribution<float> pos_x2(-1.5, 0.7);
		uniform_int_distribution<int> pos_x(0, 1);
		m_color[0].r = color(dre);
		m_color[0].g = color(dre);
		m_color[0].b = color(dre);
		switch (type) {
		case 0:
			break;
		case 1:
			s_vertex.emplace_back(0.2f, -0.2f, 0.0f);
			s_vertex.emplace_back(-0.2f, -0.2f, 0.0f);
			s_vertex.emplace_back(0.0f, 0.2f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 3;
			break;
		case 2:
			s_vertex.emplace_back(0.2f, -0.2f, 0.0f);
			s_vertex.emplace_back(-0.2f, -0.2f, 0.0f);
			s_vertex.emplace_back(0.0f, 0.2f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 3;
			break;
		case 3:
			s_vertex.emplace_back(0.2f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.0f, -0.2f, 0.0f);
			s_vertex.emplace_back(-0.2f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.0f, 0.2f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 4;
			break;
		case 4:
			s_vertex.emplace_back(0.2f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.0f, -0.2f, 0.0f);
			s_vertex.emplace_back(-0.2f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.0f, 0.2f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 4;
			break;
		case 5:
			s_vertex.emplace_back(0.3f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.28f, -0.27f, 0.0f);
			s_vertex.emplace_back(-0.04f, -0.28f, 0.0f);
			s_vertex.emplace_back(-0.3f, 0.22f, 0.0f);
			s_vertex.emplace_back(0.02f, 0.3f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 5;
			break;
		case 6:
			s_vertex.emplace_back(0.3f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.28f, -0.27f, 0.0f);
			s_vertex.emplace_back(-0.04f, -0.28f, 0.0f);
			s_vertex.emplace_back(-0.3f, 0.22f, 0.0f);
			s_vertex.emplace_back(0.02f, 0.3f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 5;
			break;
		case 7:
			s_vertex.emplace_back(0.3f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.28f, -0.27f, 0.0f);
			s_vertex.emplace_back(-0.04f, -0.28f, 0.0f);
			s_vertex.emplace_back(-0.3f, 0.22f, 0.0f);
			s_vertex.emplace_back(-0.08f, 0.37f, 0.0f);
			s_vertex.emplace_back(0.22f, 0.3f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 6;
			break;
		case 8:
			s_vertex.emplace_back(0.3f, 0.0f, 0.0f);
			s_vertex.emplace_back(0.28f, -0.27f, 0.0f);
			s_vertex.emplace_back(-0.04f, -0.28f, 0.0f);
			s_vertex.emplace_back(-0.3f, 0.22f, 0.0f);
			s_vertex.emplace_back(-0.08f, 0.37f, 0.0f);
			s_vertex.emplace_back(0.22f, 0.3f, 0.0f);
			m_course = 0.0f;
			m_cnt1 = 6;
			break;
		}
		switch (cos) {
		case 0:
			if (pos_x(dre) == 0) {
				m_move.x = -1.5f;
			}
			else {
				m_move.x = 1.5f;
			}
			m_move.y = pos_y(dre);
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = 0.0f;
			course2_y = 1.2f;
			course3_x = pos_x1(dre);
			course3_y = -2.0f;
			break;
		case 1: //왼쪽 오른쪽 멀리
			m_move.x = 1.5f;
			m_move.y = 0.5f;
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = 0.0f;
			course2_y = 1.2f;
			course3_x = -1.2f;
			course3_y = -2.0f;
			break;
		case 2://왼쪽 오른쪽 가까이
			m_move.x = 1.5f;
			m_move.y = 0.5f;
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = 0.0f;
			course2_y = 1.2f;
			course3_x = -0.2f;
			course3_y = -2.0f;
			break;
		case 3://오른쪽 왼쪽 멀리
			m_move.x = -1.5f;
			m_move.y = 0.5f;
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = 0.0f;
			course2_y = 1.2f;
			course3_x = 1.2f;
			course3_y = -2.0f;
			break;
		case 4://오른쪽 왼쪽 가까이
			m_move.x = -1.5f;
			m_move.y = 0.5f;
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = 0.0f;
			course2_y = 1.2f;
			course3_x = 0.2f;
			course3_y = -2.0f;
			break;
		}
		SetVAO();
	}
	void SetVAO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		if (s_vertex.empty()) return;
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * s_vertex.size(), &s_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		int vRotateLocation = glGetUniformLocation(shaderProgramID, "vRotate");
		glUniform1f(vRotateLocation, rotate);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, 0);
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		switch (g_HowtoDraw) {
		case 0:
			glLineWidth(4.0f);
			glDrawArrays(GL_LINE_LOOP, 0, s_vertex.size());
			break;
		case 1:
			glDrawArrays(GL_TRIANGLE_FAN, 0, s_vertex.size());
			break;
		}
	}
};
vector<Shape> g_Shapes;

struct DUMY {
	std::vector<glm::vec3> d_vertex; //점
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0.0f,0.0f,0.0f };
	GLuint m_VBOVertex;
	float plus_y = 0.0f;
	float plus_x = 0.0f;
	int m_exist = 1;
	int m_cnt1 = 0;
	float m_course = 0.0f;
	float course1_x = 0.0f;
	float course1_y = 0.0f;
	float course2_x = 0.0f;
	float course2_y = 0.0f;
	float course3_x = 0.0f;
	float course3_y = 0.0f;
	float p_x = 0.0f, p_y = 0.0f;
	DUMY(int type) {
		m_color[0].r = 0;
		m_color[0].g = 0;
		m_color[0].b = 0;
		switch (type) {
		case 0:
			break;
		case 1:
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = course1_x + 0.1f;
			course2_y = course1_y + 0.1f;
			course3_x = course1_x + 0.2f;
			course3_y = -2.0f;
			m_course = 1;
			break;
		case 2:
			course1_x = m_move.x;
			course1_y = m_move.y;
			course2_x = course1_x - 0.1f;
			course2_y = course1_y - 0.1f;
			course3_x = course1_x - 0.2f;
			course3_y = -2.0f;
			m_course = 2;
			break;
		}
		SetVAO();
	}
	void SetVAO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		if (d_vertex.empty()) return;
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * d_vertex.size(), &d_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, 0);
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		switch (g_HowtoDraw) {
		case 0:
			glLineWidth(4.0f);
			glDrawArrays(GL_LINE_LOOP, 0, d_vertex.size());
			break;
		case 1:
			glDrawArrays(GL_TRIANGLE_FAN, 0, d_vertex.size());
			break;
		}
	}
};
vector<DUMY> g_Dumys;

struct Line {
	std::vector<glm::vec3> s_vertex; //점
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	float rotate = 0.0f;
	Line() {
		m_color[0].r = 1.0f;
		m_color[0].g = 1.0f;
		m_color[0].b = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * s_vertex.size(), &s_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		int vRotateLocation = glGetUniformLocation(shaderProgramID, "vRotate");
		glUniform1f(vRotateLocation, rotate);
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glLineWidth(3.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);
	}
};
vector<Line> g_lines;

struct Cos {
	std::vector<glm::vec3> c_vertex; //점
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	float rotate = 0.0f;
	float m_course = 0.0f;
	float course1_x = 0.0f;
	float course1_y = 0.0f;
	float course2_x = 0.0f;
	float course2_y = 0.0f;
	float course3_x = 0.0f;
	float course3_y = 0.0f;
	bool g_DrawCos = true;
	Cos() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		m_color[0].r = color(dre);
		m_color[0].g = color(dre);
		m_color[0].b = color(dre);
		SetVAO();
	}
	void Setvertex() {
		while (m_course <= 1.0f) {
			m_move.x = (1 - m_course) * (1 - m_course) * course1_x + 2 * m_course * (1 - m_course) * course2_x + m_course * m_course * course3_x;
			m_move.y = (1 - m_course) * (1 - m_course) * course1_y + 2 * m_course * (1 - m_course) * course2_y + m_course * m_course * course3_y;
			m_course += 0.01f;
			c_vertex.emplace_back(m_move.x, m_move.y, 0);
		}
		m_move.x = 0;
		m_move.y = 0;
		SetVAO();
	}
	void SetVAO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * c_vertex.size(), &c_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, m_move.z);
		glLineWidth(7.0f);
		glDrawArrays(GL_LINES, 0, c_vertex.size());
	}
};
vector<Cos> g_Cos;

struct Pocket {
	glm::vec3 m_vertex[4]; //점
	glm::vec3 m_color[1];
	glm::vec3 m_move;
	GLuint m_VBOVertex;
	int m_cnt = 0;
	float rotate = 0.0f;
	Pocket() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		m_color[0].r = color(dre);
		m_color[0].g = color(dre);
		m_color[0].b = color(dre);
		m_vertex[1].x = -0.25f;
		m_vertex[1].y = 0.07f;
		m_vertex[0].x = -0.25f;
		m_vertex[0].y = -0.07f;
		m_vertex[2].x = +0.25f;
		m_vertex[2].y = -0.07f;
		m_vertex[3].x = +0.25f;
		m_vertex[3].y = +0.07f;
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = 0.0f;
		m_move.y = -0.7f;
		m_move.x = 0;
		SetVAO();
	}
	void SetVAO() {
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
		int vRotateLocation = glGetUniformLocation(shaderProgramID, "vRotate");
		glUniform1f(vRotateLocation, rotate);
		int vMoveLocation = glGetUniformLocation(shaderProgramID, "vMove");
		glUniform3f(vMoveLocation, m_move.x, m_move.y, 0);
		int vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
	}
};
vector<Pocket> g_Pockets;

std::vector<glm::vec3> vertex;

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
	g_Shapes.emplace_back(1,0);
	g_Cos.emplace_back();
	g_Cos[0].course1_x = g_Shapes[0].course1_x;
	g_Cos[0].course1_y = g_Shapes[0].course1_y;
	g_Cos[0].course2_x = g_Shapes[0].course2_x;
	g_Cos[0].course2_y = g_Shapes[0].course2_y;
	g_Cos[0].course3_x = g_Shapes[0].course3_x;
	g_Cos[0].course3_y = g_Shapes[0].course3_y;
	g_Cos[0].Setvertex();
	g_lines.emplace_back();
	g_Pockets.emplace_back();
	glutTimerFunc(17, Timer, RENDERING);
	glutTimerFunc(17, Timer, POP_OUT);
	glutTimerFunc(17, Timer, POCKET_MOVE);
	glutTimerFunc(2500, Timer, MAKE_SHAPE);
	glutTimerFunc(17, Timer, SHAPE_MOVE);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(ClickFunc);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	g_Pockets[0].Draw();

	if (g_Drawcos == 1) {
		for (auto& r : g_Cos) {
			if (r.g_DrawCos) {
				r.Draw();
			}
		}
	}
	for (auto& t : g_Shapes) {
		t.Draw();
	}
	for (auto& q : g_Dumys) {
		q.Draw();
	}
	if (g_Drag) {
		g_lines[0].Draw();
	}
	glutPostRedisplay();
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
	vertexSource = filetobuf("vertex1.glsl");

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
	fragmentSource = filetobuf("fragment1.glsl"); // 프래그세이더 읽어오기
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

void Cut_Check(float x, float y, float x1, float y1, float s_x, float s_y, float s_x1, float s_y1) {
	float increase1 = 0, fconstant1 = 0, fsamevalue1 = 0, Fsamevalue1 = 0;
	float increase2 = 0, fconstant2 = 0, fsamevalue2 = 0, Fsamevalue2 = 0;
	float max_x = 0, max_y = 0;
	float min_x = 0, min_y = 0;
	float max1_x = 0, max1_y = 0;
	float min1_x = 0, min1_y = 0;
	
	if (x >= x1) {
		max1_x = x;
		min1_x = x1;
	}
	else {
		min1_x = x;
		max1_x = x1;
	}
	if (y >= y1) {
		max1_y = y;
		min1_y = y1;
	}
	else {
		min1_y = y;
		max1_y = y1;
	}

	if (s_x >= s_x1) {
		max_x = s_x;
		min_x = s_x1;
	}
	else {
		min_x = s_x;
		max_x = s_x1;
	}
	if (s_y>= s_y1) {
		max_y = s_y;
		min_y = s_y1;
	}
	else {
		min_y = s_y;
		max_y = s_y1;
	}
	
	if (x == x1) {
		fsamevalue1 = x;
	}
	else {
		increase1 = (float)(y1 - y) / (x1 - x);
		fconstant1 = y - increase1 * x;
	}

	if (s_x == s_x1) {
		fsamevalue2 = s_x;
	}
	else {
		increase2 = (float)(s_y1 - s_y) / (s_x1 - s_x);
		fconstant2 = s_y - increase2 * s_x;
	}

	float result_x = 0, result_y = 0;
	if (x == x1 && s_x == s_x1) { return; }
	if (x == x1 ) {
		result_x = fsamevalue1;
		result_y = increase2 * fsamevalue1 + fconstant2;
	}
	else if (s_x == s_x1) {
		result_x = fsamevalue2;
		result_y = increase1 * fsamevalue2 + fconstant1;
	}
	else {
		result_x = -(fconstant1 - fconstant2) / (increase1 - increase2);
		result_y = increase1 * result_x + fconstant1;
	}

	if (min1_x <= result_x && result_x <= max1_x) {
		if (min1_y <= result_y && result_y <= max1_y) {
			if (min_x < result_x && result_x < max_x) {
				if (min_y < result_y && result_y < max_y) {
					vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = vertex.size() - 1;
					cnt++;
				}
				else if (abs(result_y - min_y) < MIN_DIFF_FLT || abs(result_y - max_y) < MIN_DIFF_FLT) {
					vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = vertex.size() - 1;
					cnt++;
				}
			}
			else if (abs(result_x - min_x) < MIN_DIFF_FLT || abs(result_x - max_x) < MIN_DIFF_FLT) {
				if (min_y < result_y && result_y < max_y) {
					vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = vertex.size() - 1;
					cnt++;
				}
				else if (abs(result_y - min_y) < MIN_DIFF_FLT || abs(result_y - max_y) < MIN_DIFF_FLT) {
					vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = vertex.size() - 1;
					cnt++;
				}
			}
		}
	}
}

void Point_ReArray() {
	std::vector<glm::vec3> array;
	std::vector<glm::vec3> b_array;
	int q = point[0];
	if (vertex.size() < g_Who + 2) {
		return;
	}
	if (vertex.size() > g_Who + 1) {
		for (int i = 0;i < vertex.size();i++) {
			array.emplace_back(vertex[i]);
			if (i == point[0]) {
				i = point[1] - 1;
			}
		}
		while (q <= point[1]+1) {
			b_array.emplace_back(vertex[q]);
			q++;
			if (q == point[1] + 1) {
				break;
			}
		}
		g_Dumys.emplace_back(1);
		g_Dumys[g_Dumynum].m_move.x = g_MovePnt_x;
		g_Dumys[g_Dumynum].m_move.y = g_MovePnt_y;
		g_Dumys[g_Dumynum].course1_x = 0;
		g_Dumys[g_Dumynum].course1_y = 0;
		g_Dumys[g_Dumynum].d_vertex = array;
		g_Dumys[g_Dumynum].m_exist = 0;
		g_Dumys[g_Dumynum].m_course = 0.0f;
		g_Dumys[g_Dumynum].m_cnt1 = array.size();
		g_Dumys[g_Dumynum].m_color->r = r;
		g_Dumys[g_Dumynum].m_color->g = g;
		g_Dumys[g_Dumynum].m_color->b = b;
		g_Dumynum++;
		g_Dumys.emplace_back(2);
		g_Dumys[g_Dumynum].d_vertex = b_array;
		g_Dumys[g_Dumynum].m_move.x = g_MovePnt_x;
		g_Dumys[g_Dumynum].m_move.y = g_MovePnt_y;
		g_Dumys[g_Dumynum].course1_x = 0;
		g_Dumys[g_Dumynum].course1_y = 0;
		g_Dumys[g_Dumynum].m_exist = 0;
		g_Dumys[g_Dumynum].m_course = 0.0f;
		g_Dumys[g_Dumynum].m_cnt1 = b_array.size();
		g_Dumys[g_Dumynum].m_color->r = r;
		g_Dumys[g_Dumynum].m_color->g = g;
		g_Dumys[g_Dumynum].m_color->b = b;
		g_Dumynum++;
		if (g_cutlist == 1) {
			g_Shapes.erase(g_Shapes.begin() + g_whatShape);
			g_Shapenum--;
			g_Cos.erase(g_Cos.begin() + g_whatShape);
			g_Cosnum--;
		}
		if (g_cutlist == 2) {
			g_Dumys.erase(g_Dumys.begin() + g_whatShape);
			g_Dumynum--;
		}
		array.clear();
		b_array.clear();
		g_cutting = true;
	}
}

GLvoid ClickFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //왼쪽 버튼 누른 경우
		auto center = ConvertWinToGL(x, y);
		mouse_x = center.first;
		mouse_y = center.second;
		g_lines[0].s_vertex.emplace_back(mouse_x, mouse_y, 0.0f);
		g_lines[0].s_vertex.emplace_back(mouse_x, mouse_y, 0.0f);
		g_Drag = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		g_Drag = false;
		equation_x1 = g_lines[0].s_vertex[0].x;
		equation_y1 = g_lines[0].s_vertex[0].y;
		equation_x2 = g_lines[0].s_vertex[1].x;
		equation_y2 = g_lines[0].s_vertex[1].y;
		size_dum = g_Dumys.size();
		size_Sha = g_Shapes.size();
		if (g_Shapes.size() > 0) {
			for (int i = 0;i < size_Sha; i++) {
				if (g_Shapes[i].m_cnt1 == 3) {
					vertex.emplace_back(g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y);
					g_Who = g_Shapes[i].m_cnt1;
					g_MovePnt_x = g_Shapes[i].m_move.x;
					g_MovePnt_y = g_Shapes[i].m_move.y;
					g_whatShape = i;
					r = g_Shapes[i].m_color->r;
					g = g_Shapes[i].m_color->g;
					b = g_Shapes[i].m_color->b;
					g_cutlist = 1;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Shapes[i].m_cnt1 == 4) {
					vertex.emplace_back(g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y);
					g_Who = g_Shapes[i].m_cnt1;
					g_MovePnt_x = g_Shapes[i].m_move.x;
					g_MovePnt_y = g_Shapes[i].m_move.y;
					g_whatShape = i;
					r = g_Shapes[i].m_color->r;
					g = g_Shapes[i].m_color->g;
					b = g_Shapes[i].m_color->b;
					g_cutlist = 1;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Shapes[i].m_cnt1 == 5) {
					vertex.emplace_back(g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[4].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[4].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[4].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[4].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[4].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[4].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y);
					g_Who = g_Shapes[i].m_cnt1;
					g_MovePnt_x = g_Shapes[i].m_move.x;
					g_MovePnt_y = g_Shapes[i].m_move.y;
					g_whatShape = i;
					r = g_Shapes[i].m_color->r;
					g = g_Shapes[i].m_color->g;
					b = g_Shapes[i].m_color->b;
					g_cutlist = 1;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Shapes[i].m_cnt1 == 6) {
					vertex.emplace_back(g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[1].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[1].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[2].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[2].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[3].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[3].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[4].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[4].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[4].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[4].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[4].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[4].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[5].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[5].y + g_Shapes[i].m_move.y);
					vertex.emplace_back(g_Shapes[i].s_vertex[5].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[5].y + g_Shapes[i].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Shapes[i].s_vertex[5].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[5].y + g_Shapes[i].m_move.y, g_Shapes[i].s_vertex[0].x + g_Shapes[i].m_move.x, g_Shapes[i].s_vertex[0].y + g_Shapes[i].m_move.y);
					g_Who = g_Shapes[i].m_cnt1;
					g_MovePnt_x = g_Shapes[i].m_move.x;
					g_MovePnt_y = g_Shapes[i].m_move.y;
					g_whatShape = i;
					r = g_Shapes[i].m_color->r;
					g = g_Shapes[i].m_color->g;
					b = g_Shapes[i].m_color->b;
					g_cutlist = 1;
					Point_ReArray();
					vertex.clear();
				}
				if (g_cutting) {
					i--;
					size_Sha--;
					g_cutting = false;
				}
				cnt = 0;
				g_cutlist = 0;
				g_Who = 0;
				g_whatShape = 0;
				r = g = b = 0;
			}
		}
		for (int j = 0;j < size_dum; j++) {
			if (g_Dumys[j].m_exist != 2) {
				if (g_Dumys[j].m_cnt1 == 3) {
					vertex.emplace_back(g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					g_Who = g_Dumys[j].m_cnt1;
					g_MovePnt_x = g_Dumys[j].m_move.x;
					g_MovePnt_y = g_Dumys[j].m_move.y;
					g_whatShape = j;
					r = g_Dumys[j].m_color->r;
					g = g_Dumys[j].m_color->g;
					b = g_Dumys[j].m_color->b;
					g_cutlist = 2;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Dumys[j].m_cnt1 == 4) {
					vertex.emplace_back(g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y);
					g_Who = g_Dumys[j].m_cnt1;
					g_MovePnt_x = g_Dumys[j].m_move.x;
					g_MovePnt_y = g_Dumys[j].m_move.y;
					g_whatShape = j;
					r = g_Dumys[j].m_color->r;
					g = g_Dumys[j].m_color->g;
					b = g_Dumys[j].m_color->b;
					g_cutlist = 2;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Dumys[j].m_cnt1 == 5) {
					vertex.emplace_back(g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y);
					g_Who = g_Dumys[j].m_cnt1;
					g_MovePnt_x = g_Dumys[j].m_move.x;
					g_MovePnt_y = g_Dumys[j].m_move.y;
					g_whatShape = j;
					r = g_Dumys[j].m_color->r;
					g = g_Dumys[j].m_color->g;
					b = g_Dumys[j].m_color->b;
					g_cutlist = 2;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Dumys[j].m_cnt1 == 6) {
					vertex.emplace_back(g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y);
					g_Who = g_Dumys[j].m_cnt1;
					g_MovePnt_x = g_Dumys[j].m_move.x;
					g_MovePnt_y = g_Dumys[j].m_move.y;
					g_whatShape = j;
					r = g_Dumys[j].m_color->r;
					g = g_Dumys[j].m_color->g;
					b = g_Dumys[j].m_color->b;
					g_cutlist = 2;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Dumys[j].m_cnt1 == 7) {
					vertex.emplace_back(g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[6].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[6].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[6].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[6].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[6].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[6].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y);
					g_Who = g_Dumys[j].m_cnt1;
					g_MovePnt_x = g_Dumys[j].m_move.x;
					g_MovePnt_y = g_Dumys[j].m_move.y;
					g_whatShape = j;
					r = g_Dumys[j].m_color->r;
					g = g_Dumys[j].m_color->g;
					b = g_Dumys[j].m_color->b;
					g_cutlist = 2;
					Point_ReArray();
					vertex.clear();
				}
				else if (g_Dumys[j].m_cnt1 == 8) {
					vertex.emplace_back(g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[1].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[1].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[2].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[2].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[3].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[3].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[4].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[4].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[5].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[5].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[6].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[6].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[6].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[6].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[6].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[6].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[7].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[7].y + g_Dumys[j].m_move.y);
					vertex.emplace_back(g_Dumys[j].d_vertex[7].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[7].y + g_Dumys[j].m_move.y, 0);
					Cut_Check(equation_x1, equation_y1, equation_x2, equation_y2, g_Dumys[j].d_vertex[7].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[7].y + g_Dumys[j].m_move.y, g_Dumys[j].d_vertex[0].x + g_Dumys[j].m_move.x, g_Dumys[j].d_vertex[0].y + g_Dumys[j].m_move.y);
					g_Who = g_Dumys[j].m_cnt1;
					g_MovePnt_x = g_Dumys[j].m_move.x;
					g_MovePnt_y = g_Dumys[j].m_move.y;
					g_whatShape = j;
					r = g_Dumys[j].m_color->r;
					g = g_Dumys[j].m_color->g;
					b = g_Dumys[j].m_color->b;
					g_cutlist = 2;
					Point_ReArray();
					vertex.clear();
				}
				if (g_cutting) {
					j--;
					size_dum--;
					g_cutting = false;
				}
				cnt = 0;
				g_cutlist = 0;
				g_Who = 0;
				g_whatShape = 0;
				r = g = b = 0;
			}
		}
		g_lines[0].s_vertex.clear();
		std::cout << g_Dumys.size() << endl;
	}
}

GLvoid Motion(int x, int y) {
	auto center = ConvertWinToGL(x, y);
	float mx = center.first;
	float my = center.second;
	if (g_Drag) {
		g_lines[0].s_vertex[1].x = mx;
		g_lines[0].s_vertex[1].y = my;
	}
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'l':
		g_HowtoDraw = 0;
		break;
	case 'L':
		g_HowtoDraw = 1;
		break;
	case 't':
		g_Drawcos = 0;
		break;
	case 'T':
		g_Drawcos = 1;
		break;
	case '+':
		g_Speed += 0.004f;
		if (g_Speed >= 0.05f) {
			g_Speed = 0.05f;
		}
		break;
	case '-':
		g_Speed -= 0.004f;
		if (g_Speed <= 0.002f) {
			g_Speed = 0.002f;
		}
		break;
	case 'r': //랜덤
		g_chosecos = 0;
		break;
	case 'z': //왼쪽 오른쪽 멀리
		g_chosecos = 1;
		break;
	case 'x': //왼쪽 오른쪽 가까이
		g_chosecos = 2;
		break;
	case 'c': //오른쪽 왼쪽 멀리
		g_chosecos = 3;
		break;
	case 'v'://오른쪽 왼쪽 가까이
		g_chosecos = 4;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	}
}

void Timer(int val) {
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> course(1, 8);
	switch (val) {
	case RENDERING:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, RENDERING);
		break;
	case POP_OUT:
		for (int i = 0;i < g_Dumys.size();i++) {
			if (g_Dumys[i].m_exist == 0) {
				g_Dumys[i].m_move.x = (1 - g_Dumys[i].m_course) * (1 - g_Dumys[i].m_course) * g_Dumys[i].course1_x + 2 * g_Dumys[i].m_course * (1 - g_Dumys[i].m_course) * g_Dumys[i].course2_x + g_Dumys[i].m_course * g_Dumys[i].m_course * g_Dumys[i].course3_x;
				g_Dumys[i].m_move.y = (1 - g_Dumys[i].m_course) * (1 - g_Dumys[i].m_course) * g_Dumys[i].course1_y + 2 * g_Dumys[i].m_course * (1 - g_Dumys[i].m_course) * g_Dumys[i].course2_y + g_Dumys[i].m_course * g_Dumys[i].m_course * g_Dumys[i].course3_y;
				g_Dumys[i].m_course += g_Speed;
				if (g_Pockets[0].m_move.x - 0.23f <  g_Dumys[i].d_vertex[0].x + g_Dumys[i].m_move.x && g_Dumys[i].d_vertex[0].x + g_Dumys[i].m_move.x < g_Pockets[0].m_move.x + 0.23f) {
					if (g_Pockets[0].m_move.y <= g_Dumys[i].d_vertex[0].y + g_Dumys[i].m_move.y  && g_Dumys[i].d_vertex[0].y + g_Dumys[i].m_move.y <= g_Pockets[0].m_move.y + 0.07f) {
						g_Dumys[i].m_exist = 2;
					}
				}
				if (g_Dumys[i].m_course == 1.0f) {
					g_Dumys.erase(g_Dumys.begin() + i);
					g_Dumynum--;
				}
			}
		}
		glutTimerFunc(40, Timer, POP_OUT);
		break;
	case MAKE_SHAPE:
		g_Shapes.emplace_back(course(dre),g_chosecos);
		g_Cos.emplace_back();
		g_Cos[g_Cosnum].course1_x = g_Shapes[g_Shapenum].course1_x;
		g_Cos[g_Cosnum].course1_y = g_Shapes[g_Shapenum].course1_y;
		g_Cos[g_Cosnum].course2_x = g_Shapes[g_Shapenum].course2_x;
		g_Cos[g_Cosnum].course2_y = g_Shapes[g_Shapenum].course2_y;
		g_Cos[g_Cosnum].course3_x = g_Shapes[g_Shapenum].course3_x;
		g_Cos[g_Cosnum].course3_y = g_Shapes[g_Shapenum].course3_y;
		g_Cos[g_Cosnum].Setvertex();
		g_Cos[g_Cosnum].g_DrawCos = true;
		g_Shapenum++;
		g_Cosnum++;
		glutTimerFunc(3500, Timer, MAKE_SHAPE);
		break;
	case POCKET_MOVE:
		for (int i = 0;i < g_Dumys.size();i++){
			if (g_Dumys[i].m_exist == 2) {
				if (g_Pockets[0].m_cnt % 2 == 0) {
					g_Dumys[i].m_move.x += 0.01f;
				}
				else {
					g_Dumys[i].m_move.x -= 0.01f;
				}
			}
		}
		if (g_Pockets[0].m_cnt % 2 == 0) {
			g_Pockets[0].m_move.x += 0.01f;
			if (g_Pockets[0].m_move.x >= 0.75f) {
				g_Pockets[0].m_cnt++;
			}
		}
		else {
			g_Pockets[0].m_move.x -= 0.01f;
			if (g_Pockets[0].m_move.x <= -0.75f) {
				g_Pockets[0].m_cnt++;
			}
		}
		glutTimerFunc(30, Timer, POCKET_MOVE);
		break;
	case SHAPE_MOVE:
		for (int i = 0;i < g_Shapes.size();i++) {
			if (g_Shapes[i].m_exist == 1) {
				g_Shapes[i].m_move.x = (1 - g_Shapes[i].m_course) * (1 - g_Shapes[i].m_course) * g_Shapes[i].course1_x + 2 * g_Shapes[i].m_course * (1 - g_Shapes[i].m_course) * g_Shapes[i].course2_x + g_Shapes[i].m_course * g_Shapes[i].m_course * g_Shapes[i].course3_x;
				g_Shapes[i].m_move.y = (1 - g_Shapes[i].m_course) * (1 - g_Shapes[i].m_course) * g_Shapes[i].course1_y + 2 * g_Shapes[i].m_course * (1 - g_Shapes[i].m_course) * g_Shapes[i].course2_y + g_Shapes[i].m_course * g_Shapes[i].m_course * g_Shapes[i].course3_y;
				g_Shapes[i].m_course += g_Speed;
				if (g_Shapes[i].m_course >= 0.1f) {
					g_Cos[i].g_DrawCos = false;
				}
				if (g_Shapes[i].m_course >= 1.0f) {
					g_Shapes[i].m_course = 0.0f;
					g_Cos[i].g_DrawCos = true;
				}
			}
		}
		glutTimerFunc(30, Timer, SHAPE_MOVE);
		break;
	default:
		break;
	}
}