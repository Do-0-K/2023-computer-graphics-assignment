#include "start.h"
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
void MakeVertexShaders();
void MakeFragmentShaders();
void Timer(int val);
GLuint MakeShaderProgram();
GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

float mouse_x = 0, mouse_y = 0;
bool g_isOn1 = false, g_isOn2 = false, g_click = false, g_isop = false;
int g_Rotate1 = 0, g_Rotate2 = 0;
float g_degree = 0.0f, g_plus = 0.0f;
int g_isOpen = 0, g_cnt = 0;

std::vector<glm::vec3> GU_vertex; //점
std::vector<glm::vec3> D_vertex; //점
std::vector<glm::vec3> S_vertex; //점

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.5f); //--- 카메라 위치

struct Stage { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	Stage() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * S_vertex.size(), &S_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
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
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glUniform3f(vColorLocation, m_color[1].r, m_color[1].g, m_color[1].b);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glUniform3f(vColorLocation, m_color[3].r, m_color[3].g, m_color[3].b);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glUniform3f(vColorLocation, m_color[4].r, m_color[4].g, m_color[4].b);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glUniform3f(vColorLocation, m_color[5].r, m_color[5].g, m_color[5].b);
		glDrawArrays(GL_TRIANGLES, 30, 6);
	}
};
vector<Stage> g_Stages;

struct Dice { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[6];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	float r = 0;
	Dice(int type) {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		switch (type) {
		case 0:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.4f, 0.4f, 0.4f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
			r = 0.23f;
			break;
		case 1:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.2f, 0.2f, 0.2f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, -0.1f));
			r = 0.12f;
			break;
		case 2:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.1f, 0.1f, 0.1f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.2f));
			r = 0.07f;
			break;
		}
		SetVBO();
	}
	void reset(int type) {
		switch (type) {
		case 0:
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
			r = 0.23f;
			break;
		case 1:
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, -0.1f));
			r = 0.12f;
			break;
		case 2:
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.2f));
			r = 0.07f;
			break;
		}
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
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
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		for (int i = 0;i < 6;i++) {
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
		}
	}
};
vector<Dice> g_Dices;

struct Sphere { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[1];
	glm::vec3 m_move = {0,0,0};
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	int how2move = 0;
	int cnt_x = 0;
	int cnt_y = 0;
	int cnt_z = 0;
	Sphere(int type) {
		m_color[0].r = 0.0f;
		m_color[0].g = 0.0f;
		m_color[0].b = 0.5f;
		how2move = type;
		S_matrix = glm::scale(S_matrix, glm::vec3(0.02f, 0.02f, 0.02f));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
		SetVBO();
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * GU_vertex.size(), &GU_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
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
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 0, GU_vertex.size());
	}
};
vector<Sphere> g_Sphere;

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
	ReadDObj("cube(1).obj", D_vertex);
	ReadDObj("cube(1).obj", S_vertex);
	ReadDObj("sphere.obj", GU_vertex);
	g_Stages.emplace_back();
	g_Dices.emplace_back(0);
	g_Dices.emplace_back(1);
	g_Dices.emplace_back(2);
	g_Sphere.emplace_back(0);
	g_Sphere.emplace_back(1);
	g_Sphere.emplace_back(2);
	g_Sphere.emplace_back(3);
	g_Sphere.emplace_back(4);
	glutTimerFunc(17, Timer, 0);
	glutTimerFunc(17, Timer, 1);
	glutTimerFunc(17, Timer, 2);
	glutDisplayFunc(DrawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(ClickFunc);
	glutMotionFunc(Motion);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid DrawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view_trans"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), WIDTH / HEIGHT, 0.1f, 50.0f);
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "proj_trans");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CW);
	g_Stages[0].Draw();
	glEnable(GL_CCW);
	for (auto& a : g_Dices) {
		a.Draw();
	}
	for (auto& t : g_Sphere) {
		t.Draw();
	}
	glDisable(GL_DEPTH_TEST);
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

GLvoid ClickFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //왼쪽 버튼 누른 경우
		float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
		float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
		mouse_x = mx;
		mouse_y = my;
		g_Rotate2 = 0;
		g_click = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		g_click = false;
		mouse_x = 0.0f;
		mouse_y = 0.0f;
	}
}

GLvoid Motion(int x, int y) {
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -(y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	if (g_click) {
		if (mouse_x > mx) {
			g_Stages[0].AR_matrix = glm::rotate(g_Stages[0].AR_matrix, glm::radians(-0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
			for (auto& a : g_Sphere) {
				a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(-0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			for (auto& t : g_Dices) {
				t.AR_matrix = glm::rotate(t.AR_matrix, glm::radians(-0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			g_degree -= 0.01f;
		}
		if (mouse_x < mx) {
			g_Stages[0].AR_matrix = glm::rotate(g_Stages[0].AR_matrix, glm::radians(0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
			for (auto& a : g_Sphere) {
				a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			for (auto& t : g_Dices) {
				t.AR_matrix = glm::rotate(t.AR_matrix, glm::radians(0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			g_degree += 0.01f;
		}
		mouse_x = mx;
		mouse_y = my;
		glutPostRedisplay();
	}
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'y':
		g_Rotate1 = 1;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 3);
			g_isOn1 = true;
		}
		else {
			g_isOn1 = false;
		}
		break;
	case 'Y':
		g_Rotate1 = 2;
		if (!g_isOn1) {
			glutTimerFunc(17, Timer, 3);
			g_isOn1 = true;
		}
		else {
			g_isOn1 = false;
		}
		break;
	case 'z':
		g_Rotate2 = 1;
		if (!g_isOn2) {
			glutTimerFunc(17, Timer, 4);
			g_isOn2 = true;
		}
		else {
			g_isOn2 = false;
		}
		break;
	case 'Z':
		g_Rotate2 = 2;
		if (!g_isOn2) {
			glutTimerFunc(17, Timer, 4);
			g_isOn2 = true;
		}
		else {
			g_isOn2 = false;
		}
		break;
	case 'b':
	case 'B':
		for (auto& t : g_Sphere) {
			t.cnt_x = 0;
			t.cnt_y = 0;
			t.cnt_z = 0;
			t.m_move.x = 0;
			t.m_move.t = 0;
			t.m_move.z = 0;
			t.T_matrix = glm::mat4(1.0f);
			t.AR_matrix = g_Stages[0].AR_matrix;
		}
		break;
	case 'c':
		g_Rotate2 = 0;
		g_degree = 0;
		g_Stages[0].AR_matrix = glm::mat4(1.0f);
		for (auto& a : g_Sphere) {
			a.AR_matrix = glm::mat4(1.0f);
		}
		for (auto& t : g_Dices) {
			t.AR_matrix = glm::mat4(1.0f);
			t.T_matrix = glm::mat4(1.0f);
			t.m_move = {0,0,0};
		}
		g_Dices[0].reset(0);
		g_Dices[1].reset(1);
		g_Dices[2].reset(2);
		break;
	case 't':
		if (!g_isop) {
			glutTimerFunc(17, Timer, 5);
			g_isop = true;
		}
		else {
			g_isop = false;
		}
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
		for (int i = 0;i < g_Dices.size();i++) {
			if (g_isOpen != 1) {
				if (g_Dices[i].m_move.y - g_Dices[i].r >= -1.0f && g_degree == 0.0f) {
					g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
					g_Dices[i].m_move.y -= 0.02f;
				}
				if (g_Rotate2 == 0) {
					if (0.2f < g_degree && g_degree < 90.0f) {
						if (g_Dices[i].m_move.x - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(-0.01f, 0.0f, 0.0f));
							g_Dices[i].m_move.x -= 0.01f;
						}
					}
					if (-90.f < g_degree && g_degree < -0.2f) {
						if (g_Dices[i].m_move.x + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.01f, 0.0f, 0.0f));
							g_Dices[i].m_move.x += 0.01f;
						}
					}
				}
				if (g_Rotate2 == 1) {
					if (0 < g_degree && g_degree < 90.0f) {
						if (g_Dices[i].m_move.x - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(-0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x -= 0.02f;
						}
					}
					if (90.0f < g_degree && g_degree < 180.0f) {
						if (g_Dices[i].m_move.y + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
							g_Dices[i].m_move.y += 0.02f;
						}
					}
					if (180.0f < g_degree && g_degree < 270.0f) {
						if (g_Dices[i].m_move.x + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x += 0.02f;
						}
					}
					if (270.0f < g_degree && g_degree < 360.0f) {
						if (g_Dices[i].m_move.y - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
							g_Dices[i].m_move.y -= 0.02f;
						}
					}
					if (-360.0f < g_degree && g_degree < -270.0f) {
						if (g_Dices[i].m_move.x - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(-0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x -= 0.02f;
						}
					}
					if (-270.0f < g_degree && g_degree < -180.0f) {
						if (g_Dices[i].m_move.y + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
							g_Dices[i].m_move.y += 0.02f;
						}
					}
					if (-180.0f < g_degree && g_degree < -90.0f) {
						if (g_Dices[i].m_move.x + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x += 0.02f;
						}
					}
					if (-90.f < g_degree && g_degree < 0.0f) {
						if (g_Dices[i].m_move.y - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
							g_Dices[i].m_move.y -= 0.02f;
						}
					}
				}
				if (g_Rotate2 == 2) {
					if (0 < g_degree && g_degree < 90.0f) {
						if (g_Dices[i].m_move.y - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
							g_Dices[i].m_move.y -= 0.02f;
						}
					}
					if (90.0f < g_degree && g_degree < 180.0f) {
						if (g_Dices[i].m_move.x - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(-0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x -= 0.02f;
						}
					}
					if (180.0f < g_degree && g_degree < 270.0f) {
						if (g_Dices[i].m_move.y + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
							g_Dices[i].m_move.y += 0.02f;
						}
					}
					if (270.0f < g_degree && g_degree < 360.0f) {
						if (g_Dices[i].m_move.x + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x += 0.02f;
						}
					}
					if (-360.0f < g_degree && g_degree < -270.0f) {
						if (g_Dices[i].m_move.y - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
							g_Dices[i].m_move.y -= 0.02f;
						}
					}
					if (-270.0f < g_degree && g_degree < -180.0f) {
						if (g_Dices[i].m_move.x - g_Dices[i].r >= -1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(-0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x -= 0.02f;
						}
					}
					if (-180.0f < g_degree && g_degree < -90.0f) {
						if (g_Dices[i].m_move.y + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
							g_Dices[i].m_move.y += 0.02f;
						}
					}
					if (-90.f < g_degree && g_degree < 0.0f) {
						if (g_Dices[i].m_move.x + g_Dices[i].r <= 1.0f) {
							g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.02f, 0.0f, 0.0f));
							g_Dices[i].m_move.x += 0.02f;
						}
					}
				}
			}
			if (g_isOpen == 1) {
				g_Dices[i].T_matrix = glm::translate(g_Dices[i].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
				g_Dices[i].m_move.y -= 0.02f;
			}
		}
		glutTimerFunc(17, Timer, 1);
		break;
	case 2:
		for (int i = 0;i < g_Sphere.size();i++) {
			if (g_Sphere[i].how2move == 0) {
				if (g_Sphere[i].cnt_x % 2 == 0) {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(-0.005f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x -= 0.005f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
							g_Sphere[i].m_move.y -= 0.01f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.01f, 0.0f));
							g_Sphere[i].m_move.y += 0.01f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
						g_Sphere[i].m_move.y -= 0.01f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, -0.01f));
						g_Sphere[i].m_move.z -= 0.01f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, 0.01f));
						g_Sphere[i].m_move.z += 0.01f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x < -0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
				else {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.005f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x += 0.005f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
							g_Sphere[i].m_move.y -= 0.01f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.01f, 0.0f));
							g_Sphere[i].m_move.y += 0.01f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
						g_Sphere[i].m_move.y -= 0.01f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, -0.01f));
						g_Sphere[i].m_move.z -= 0.01f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, 0.01f));
						g_Sphere[i].m_move.z += 0.01f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x > 0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
			}
			if (g_Sphere[i].how2move == 1) {
				if (g_Sphere[i].cnt_x % 2 == 0) {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(-0.01f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x -= 0.01f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.005f, 0.0f));
							g_Sphere[i].m_move.y += 0.005f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
							g_Sphere[i].m_move.y -= 0.005f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
						g_Sphere[i].m_move.y -= 0.01f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, 0.01f));
						g_Sphere[i].m_move.z += 0.01f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, -0.01f));
						g_Sphere[i].m_move.z -= 0.01f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x < -0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
				else {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.01f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x += 0.01f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.005f, 0.0f));
							g_Sphere[i].m_move.y += 0.005f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
							g_Sphere[i].m_move.y -= 0.005f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
						g_Sphere[i].m_move.y -= 0.01f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, 0.01f));
						g_Sphere[i].m_move.z += 0.01f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, -0.01f));
						g_Sphere[i].m_move.z -= 0.01f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x > 0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
			}
			if (g_Sphere[i].how2move == 2) {
				if (g_Sphere[i].cnt_x % 2 == 0) {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(-0.005f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x -= 0.005f;
					if (g_Sphere[i].cnt_y % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.005f, 0.0f));
						g_Sphere[i].m_move.y += 0.005f;
						if (g_Sphere[i].m_move.y > 0.88f) {
							g_Sphere[i].cnt_y++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
						g_Sphere[i].m_move.y -= 0.005f;
						if (g_isOpen != 1) {
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, -0.005f));
						g_Sphere[i].m_move.z -= 0.005f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, 0.005f));
						g_Sphere[i].m_move.z += 0.005f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x < -0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
				else {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.005f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x += 0.005f;
					if (g_Sphere[i].cnt_y % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.005f, 0.0f));
						g_Sphere[i].m_move.y += 0.005f;
						if (g_Sphere[i].m_move.y > 0.88f) {
							g_Sphere[i].cnt_y++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
						g_Sphere[i].m_move.y -= 0.005f;
						if (g_isOpen != 1) {
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, -0.005f));
						g_Sphere[i].m_move.z -= 0.005f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, 0.005f));
						g_Sphere[i].m_move.z += 0.005f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x > 0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
			}
			if (g_Sphere[i].how2move == 3) {
				if (g_Sphere[i].cnt_x % 2 == 0) {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.005f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x += 0.005f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
							g_Sphere[i].m_move.y -= 0.005f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.005f, 0.0f));
							g_Sphere[i].m_move.y += 0.005f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
						g_Sphere[i].m_move.y -= 0.005f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, 0.005f));
						g_Sphere[i].m_move.z += 0.005f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, -0.005f));
						g_Sphere[i].m_move.z -= 0.005f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x > 0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
				else {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(-0.01f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x -= 0.01f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
							g_Sphere[i].m_move.y -= 0.005f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.005f, 0.0f));
							g_Sphere[i].m_move.y += 0.005f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.005f, 0.0f));
						g_Sphere[i].m_move.y -= 0.005f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, 0.005f));
						g_Sphere[i].m_move.z += 0.005f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, -0.005f));
						g_Sphere[i].m_move.z -= 0.005f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x < -0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
			}
			if (g_Sphere[i].how2move == 4) {
				if (g_Sphere[i].cnt_x % 2 == 0) {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.01f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x += 0.01f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
							g_Sphere[i].m_move.y -= 0.01f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.01f, 0.0f));
							g_Sphere[i].m_move.y += 0.01f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
						g_Sphere[i].m_move.y -= 0.01f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, 0.005f));
						g_Sphere[i].m_move.z += 0.005f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, -0.005f));
						g_Sphere[i].m_move.z -= 0.005f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x > 0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
				else {
					g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(-0.005f, 0.0f, 0.0f));
					g_Sphere[i].m_move.x -= 0.005f;
					if (g_isOpen != 1) {
						if (g_Sphere[i].cnt_y % 2 == 0) {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
							g_Sphere[i].m_move.y -= 0.01f;
							if (g_Sphere[i].m_move.y < -0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
						else {
							g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.01f, 0.0f));
							g_Sphere[i].m_move.y += 0.01f;
							if (g_Sphere[i].m_move.y > 0.88f) {
								g_Sphere[i].cnt_y++;
							}
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
						g_Sphere[i].m_move.y -= 0.01f;
					}
					if (g_Sphere[i].cnt_z % 2 == 0) {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.00f, 0.005f));
						g_Sphere[i].m_move.z += 0.005f;
						if (g_Sphere[i].m_move.z > 0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					else {
						g_Sphere[i].T_matrix = glm::translate(g_Sphere[i].T_matrix, glm::vec3(0.0f, 0.0f, -0.005f));
						g_Sphere[i].m_move.z -= 0.005f;
						if (g_Sphere[i].m_move.z < -0.88f) {
							g_Sphere[i].cnt_z++;
						}
					}
					if (g_Sphere[i].m_move.x < -0.88f) {
						g_Sphere[i].cnt_x++;
					}
				}
			}
		}
		glutTimerFunc(17, Timer, 2);
		break;
	case 3:
		if (g_isOn1) {
			if (g_Rotate1 == 1) {
				g_Stages[0].AR_matrix = glm::rotate(g_Stages[0].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				for (auto& a : g_Sphere) {
					a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				for (auto& t : g_Dices) {
					t.AR_matrix = glm::rotate(t.AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			if (g_Rotate1 == 2) {
				g_Stages[0].AR_matrix = glm::rotate(g_Stages[0].AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				for (auto& a : g_Sphere) {
					a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				for (auto& t : g_Dices) {
					t.AR_matrix = glm::rotate(t.AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			glutTimerFunc(17, Timer, 3);
		}
		break;
	case 4:
		if (g_isOn2) {
			if (g_Rotate2 == 1) {
				g_Stages[0].AR_matrix = glm::rotate(g_Stages[0].AR_matrix, glm::radians(0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
				for (auto& a : g_Sphere) {
					a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				for (auto& t : g_Dices) {
					t.AR_matrix = glm::rotate(t.AR_matrix, glm::radians(0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				g_degree += 0.5f;
				if (g_degree >= 360.0f) {
					g_degree = 0;
				}
			}
			if (g_Rotate2 == 2) {
				g_Stages[0].AR_matrix = glm::rotate(g_Stages[0].AR_matrix, glm::radians(-0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
				for (auto& a : g_Sphere) {
					a.AR_matrix = glm::rotate(a.AR_matrix, glm::radians(-0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				for (auto& t : g_Dices) {
					t.AR_matrix = glm::rotate(t.AR_matrix, glm::radians(-0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				g_degree -= 0.5f;
				if (g_degree <= -360.0f) {
					g_degree = 0;
				}
			}
			glutTimerFunc(17, Timer, 4);
		}
		break;
	case 5:
		if (g_isop) {
			if (g_isOpen == 0) {
				S_vertex[0].z -= 0.01f;
				S_vertex[1].z -= 0.01f;
				S_vertex[2].z -= 0.01f;
				S_vertex[3].z -= 0.01f;
				S_vertex[4].z -= 0.01f;
				S_vertex[5].z -= 0.01f;
				g_cnt++;
				if (g_cnt >= 100) {
					g_cnt = 0;
					g_isop = false;
					g_isOpen = 1;
				}
			}
			if (g_isOpen == 1) {
				S_vertex[0].z += 0.01f;
				S_vertex[1].z += 0.01f;
				S_vertex[2].z += 0.01f;
				S_vertex[3].z += 0.01f;
				S_vertex[4].z += 0.01f;
				S_vertex[5].z += 0.01f;
				g_cnt++;
				if (g_cnt >= 100) {
					g_cnt = 0;
					g_isop = false;
					g_isOpen = 0;
				}
			}
			glutTimerFunc(17, Timer, 5);
		}
		break;
	default:
		break;
	}
}