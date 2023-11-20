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
bool g_isOn = false, g_isOn1 = false,g_isOn2 = false, g_isOn3 = true;
float theta = 9.0f, g_Speed = 0.02f, g_Sangle = 1.0f, g_mountx = 0.0f, g_mountz = 0.0f, g_mounty = 0.0f;
int g_open = 1, g_Rotate1 = 0, g_Smount = 1, g_jump = 0;
int g_num = 0, g_num1 = 0, g_num2 = 0, g_num3 = 0;
int g_Fmove = 0, g_Lmove = 0, g_Rmove = 0, g_Bmove = 0, g_Jmove = 0;
int g_movenum = 0, g_movehand_foot = 0, g_Jup_down = 0, g_JisOn = 0, g_crash = 0, g_climb = 0;
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

std::vector<glm::vec3> S_vertex; //점
std::vector<glm::vec3> R_vertex; //점
std::vector<glm::vec3> C_vertex; //점

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.5f); //--- 카메라 위치

struct Robot { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_vertex[3];
	glm::vec3 m_color;
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 AR_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	Robot* m_parent = nullptr;
	int m_type = 0;
	glm::mat4 GetMatrix()
	{
		if (m_parent == nullptr) {
			return AR_matrix * T_matrix * R_matrix * S_matrix;
		}
		return m_parent->GetMatrix() * AR_matrix * T_matrix * R_matrix * S_matrix; //재귀적 발상
	}
	Robot(int type, Robot* parent) {
		switch (type) {
		case 0: //몸통
			m_color.r = 0.0f;
			m_color.g = 0.0f;
			m_color.b = 1.0f;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.2f, 0.3f, 0.2f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, -0.5f, 0.0f));
			m_type = 1;
			break;
		case 1: //머리
			m_color.r = 1.0f;
			m_color.g = 0.0f;
			m_color.b = 1.0f;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.6f, 0.4f, 0.6f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.7f, 0.0f));
			m_type = 1;
			break;
		case 2: //왼팔
			m_color.r = 1.0f;
			m_color.g = 0.2f;
			m_color.b = 0.5f;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(-0.6f, -0.1f, 0.0f));
			m_type = 1;
			break;
		case 3: //오른팔
			m_color.r = 1.0f;
			m_color.g = 0.2f;
			m_color.b = 0.5f;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.6f, -0.1f, 0.0f));
			m_type = 1;
			break;
		case 4: //왼다리
			m_color.r = 1.0f;
			m_color.g = 1.0f;
			m_color.b = 0.0f;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(-0.2f, -0.8f, 0.0f));
			m_type = 1;
			break;
		case 5: //오른다리
			m_color.r = 1.0f;
			m_color.g = 1.0f;
			m_color.b = 0.0f;
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.2f, -0.8f, 0.0f));
			m_type = 1;
			break;
		case 6: //코
			m_color.r = 0.0f;
			m_color.g = 0.0f;
			m_color.b = 0.0f;
			m_vertex[2].x = 0.0f;
			m_vertex[2].y = 0.1f;
			m_vertex[0].x = -0.08f;
			m_vertex[0].y = -0.06f;
			m_vertex[1].x = 0.08f;
			m_vertex[1].y = -0.06f;
			m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
			S_matrix = glm::scale(S_matrix, glm::vec3(2.0f,2.0f, 2.0f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.55f));
			m_type = 2;
			break;
		}
		m_parent = parent;
		SetVBO();
	}
	void Reset(int type) {
		T_matrix = glm::mat4(1.0f);
		S_matrix = glm::mat4(1.0f);
		AR_matrix = glm::mat4(1.0f);
		R_matrix = glm::mat4(1.0f);
		switch (type) {
		case 0:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.2f, 0.3f, 0.2f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, -0.5f, 0.0f));
			break;
		case 1:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.6f, 0.4f, 0.6f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.7f, 0.0f));
			break;
		case 2:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(-0.6f, -0.1f, 0.0f));
			break;
		case 3:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.6f, -0.1f, 0.0f));
			break;
		case 4:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(-0.2f, -0.8f, 0.0f));
			break;
		case 5:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.3f, 0.6f, 0.3f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.2f, -0.8f, 0.0f));
			break;
		}
	}
	void SetVBO() {
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
	}
	void Draw() {
		if (m_type == 1) {
			glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * R_vertex.size(), &R_vertex[0], GL_STATIC_DRAW);
			int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
			//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
			glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(positionAttrib);
		}
		if (m_type == 2) {
			glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_vertex, GL_STATIC_DRAW);
			int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
			//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
			glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(positionAttrib);
		}
		glm::mat4 parentMatrix(1.0f);
		if (m_parent != nullptr) {
			parentMatrix = m_parent->GetMatrix(); //부모 가져오기
		}
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
		glUniform3f(vColorLocation, m_color.r, m_color.g, m_color.b);
		if (m_type == 1) {
			glDrawArrays(GL_TRIANGLES, 0, R_vertex.size());
		}
		else if (m_type == 2) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
};
vector<Robot> g_Robots;

struct Stage { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 A_R_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	float Px = 0, Pz = 0, Py = 0.0f;
	int s = 0;
	Stage(int type) {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		uniform_real_distribution<float> pos_x(-0.8, 0.8);
		uniform_real_distribution<float> pos_z(-0.8, 0.8);
		Px = pos_x(dre);
		Pz = pos_z(dre);
		for (int i = 0;i < 6;i++) {
			m_color[i].r = color(dre);
			m_color[i].g = color(dre);
			m_color[i].b = color(dre);
		}
		switch (type) {
		case 0:
			S_matrix = glm::scale(S_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
			T_matrix = glm::translate(T_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
			s = 0;
			break;
		case 1:
			S_matrix = glm::scale(S_matrix, glm::vec3(0.2f, 0.2f, 0.2f));
			T_matrix = glm::translate(T_matrix, glm::vec3(Px, -0.8f, Pz));
			s = 1;
			break;
		}
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
		if (s == 1) {
			glUniform3f(vColorLocation, m_color[2].r, m_color[2].g, m_color[2].b);
			glDrawArrays(GL_TRIANGLES, 12, 6);
		}
		glUniform3f(vColorLocation, m_color[3].r, m_color[3].g, m_color[3].b);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glUniform3f(vColorLocation, m_color[4].r, m_color[4].g, m_color[4].b);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glUniform3f(vColorLocation, m_color[5].r, m_color[5].g, m_color[5].b);
		glDrawArrays(GL_TRIANGLES, 30, 6);
	}
};
vector<Stage> g_Stages;

struct Front { //VB0 = 변수 하나, VAO = 배열 -> 요런 느끰
	glm::vec3 m_color[6];
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	glm::mat4 T_matrix = glm::mat4(1.0f);
	glm::mat4 S_matrix = glm::mat4(1.0f);
	glm::mat4 A_R_matrix = glm::mat4(1.0f);
	glm::mat4 R_matrix = glm::mat4(1.0f);
	glm::mat4 parentMatrix = glm::mat4(1.0f);
	Front() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> color(0, 1);
		m_color[0].r = color(dre);
		m_color[0].g = color(dre);
		m_color[0].b = color(dre);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * C_vertex.size(), &C_vertex[0], GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);
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
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glUniform3f(vColorLocation, m_color[0].r, m_color[0].g, m_color[0].b);
		glDrawArrays(GL_TRIANGLES, 6, 6);
	}
};
vector<Front> g_Fronts;

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
	g_Robots.reserve(10);
	ReadDObj("cube(1).obj", S_vertex);
	g_Stages.emplace_back(0);
	g_Stages.emplace_back(1);
	g_Stages.emplace_back(1);
	g_Stages.emplace_back(1);
	C_vertex.emplace_back(S_vertex[12]);
	C_vertex.emplace_back((S_vertex[12].x+S_vertex[13].x)/2, S_vertex[12].y,S_vertex[12].z);
	C_vertex.emplace_back(S_vertex[16]);
	C_vertex.emplace_back(S_vertex[16]);
	C_vertex.emplace_back((S_vertex[12].x + S_vertex[13].x) / 2, S_vertex[12].y, S_vertex[12].z);
	C_vertex.emplace_back((S_vertex[16].x + S_vertex[14].x) / 2, S_vertex[16].y, S_vertex[16].z);
	C_vertex.emplace_back((S_vertex[12].x + S_vertex[13].x) / 2, S_vertex[12].y, S_vertex[12].z);
	C_vertex.emplace_back(S_vertex[13]);
	C_vertex.emplace_back((S_vertex[16].x + S_vertex[14].x) / 2, S_vertex[16].y, S_vertex[16].z);
	C_vertex.emplace_back((S_vertex[16].x + S_vertex[14].x) / 2, S_vertex[16].y, S_vertex[16].z);
	C_vertex.emplace_back(S_vertex[13]);
	C_vertex.emplace_back(S_vertex[15]);
	g_Fronts.emplace_back();
	ReadDObj("cube(1).obj", R_vertex);
	g_Robots.emplace_back(0,nullptr);
	g_Robots.emplace_back(1,&g_Robots[0]);
	g_Robots.emplace_back(2, &g_Robots[0]);
	g_Robots.emplace_back(3, &g_Robots[0]);
	g_Robots.emplace_back(4, &g_Robots[0]);
	g_Robots.emplace_back(5, &g_Robots[0]);
	g_Robots.emplace_back(6, &g_Robots[1]);
	glutTimerFunc(17, Timer, 0);
	glutTimerFunc(17, Timer, 2);
	glutTimerFunc(17, Timer, 5);
	glutDisplayFunc(DrawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
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
	g_Fronts[0].Draw();
	glEnable(GL_CCW);
	for (auto& s : g_Robots) {
		s.Draw();
	}
	g_Stages[1].Draw();
	g_Stages[2].Draw();
	g_Stages[3].Draw();
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

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'o':
	case 'O':
		if (!g_isOn) {
			glutTimerFunc(17, Timer, 1);
			g_isOn = true;
		}
		else {
			theta = 0;
			g_isOn = false;
		}
		break;
	case 'a':
		if (g_num == 0) {
			g_Robots[0].R_matrix = glm::mat4(1.0f);
			g_Robots[0].R_matrix = glm::rotate(g_Robots[0].R_matrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
			g_num = 1;
			g_num1 = g_num2 = g_num3 = 0;
		}
		g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(-g_Speed, 0, 0));
		g_Lmove++;
		g_Rmove--;
		g_mountx -= g_Speed;
		for (int i = 1;i < g_Stages.size();i++) {
			if (g_Stages[i].Py + 0.2f > g_mounty) {
				if (g_Stages[i].Pz - 0.2f < g_mountz && g_mountz < g_Stages[i].Pz + 0.2f) {
					if (g_Stages[i].Px - 0.2f < g_mountx && g_mountx < g_Stages[i].Px + 0.2f) {
						g_crash = 1;
					}
				}
			}
		}
		if (g_crash == 1 && g_JisOn == 0) {
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(g_Speed, 0, 0));
			g_Lmove--;
			g_Rmove++;
			g_mountx += g_Speed;
			g_crash = 0;
		}
		if (g_Lmove >= 40 / g_Smount) {
			g_Lmove = -40 / g_Smount;
			g_Rmove = 40 / g_Smount;
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(1.5f, 0, 0));
		}
		break;
	case 's':
		if (g_num1 == 0) {
			g_Robots[0].R_matrix = glm::mat4(1.0f);
			g_num1 = 1;
			g_num = g_num2 = g_num3 = 0;
		}
		g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0, 0, g_Speed));
		g_Fmove++;
		g_Bmove--;
		g_mountz += g_Speed;
		for (int i = 1;i < g_Stages.size();i++) {
			if (g_Stages[i].Py + 0.2f > g_mounty) {
				if (g_Stages[i].Pz - 0.2f < g_mountz && g_mountz < g_Stages[i].Pz + 0.2f) {
					if (g_Stages[i].Px - 0.2f < g_mountx && g_mountx < g_Stages[i].Px + 0.2f) {
						g_crash = 1;
					}
				}
			}
		}
		if (g_crash == 1 && g_JisOn == 0) {
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0, 0, -g_Speed));
			g_Fmove--;
			g_Bmove++;
			g_mountz -= g_Speed;
			g_crash = 0;
		}
		if (g_Fmove >= 70 / g_Smount) {
			g_Fmove = -70 / g_Smount;
			g_Bmove = 10 / g_Smount;
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0, 0, -1.5f));
		}
		break;
	case 'd':
		if (g_num2 == 0) {
			g_Robots[0].R_matrix = glm::mat4(1.0f);
			g_Robots[0].R_matrix = glm::rotate(g_Robots[0].R_matrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
			g_num2 = 1;
			g_num = g_num1 = g_num3 = 0;
		}
		g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(g_Speed, 0, 0));
		g_Rmove++;
		g_Lmove--;
		g_mountx += g_Speed;
		for (int i = 1;i < g_Stages.size();i++) {
			if (g_Stages[i].Py + 0.2f > g_mounty) {
				if (g_Stages[i].Pz - 0.2f < g_mountz && g_mountz < g_Stages[i].Pz + 0.2f) {
					if (g_Stages[i].Px - 0.2f < g_mountx && g_mountx < g_Stages[i].Px + 0.2f) {
						g_crash = 1;
					}
				}
			}
		}
		if (g_crash == 1 && g_JisOn == 0) {
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(-g_Speed, 0, 0));
			g_Rmove--;
			g_Lmove++;
			g_mountx -= g_Speed;
			g_crash = 0;
		}
		if (g_Rmove >= 40 / g_Smount) {
			g_Rmove = -40 / g_Smount;
			g_Lmove = 40/ g_Smount;
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(-1.5f, 0, 0));
		}
		break;
	case 'w':
		if (g_num3 == 0) {
			g_Robots[0].R_matrix = glm::mat4(1.0f);
			g_Robots[0].R_matrix = glm::rotate(g_Robots[0].R_matrix, glm::radians(-180.0f), glm::vec3(0, 1, 0));
			g_num3 = 1;
			g_num = g_num1 = g_num2 = 0;
		}
		g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0, 0, -g_Speed));
		g_Bmove++;
		g_Fmove--;
		g_mountz -= g_Speed;
		for (int i = 1;i < g_Stages.size();i++) {
			if (g_Stages[i].Py + 0.2f > g_mounty) {
				if (g_Stages[i].Pz - 0.2f < g_mountz && g_mountz < g_Stages[i].Pz + 0.2f) {
					if (g_Stages[i].Px - 0.2f < g_mountx && g_mountx < g_Stages[i].Px + 0.2f) {
						g_crash = 1;
					}
				}
			}
		}
		if (g_crash == 1 && g_JisOn == 0) {
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0, 0, g_Speed));
			g_Bmove--;
			g_Fmove++;
			g_mountz += g_Speed;
			g_crash = 0;
		}
		if (g_Bmove >= 10/g_Smount) {
			g_Bmove = -70/ g_Smount;
			g_Fmove = 70/ g_Smount;
			g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0, 0, 1.5f));
		}
		break;
	case '+':
		g_Speed += 0.02f;
		if (g_Speed > 0.1f) {
			g_Speed = 0.1f;
		}
		g_Smount++;
		if (g_Smount > 5) {
			g_Smount = 5;
		}
		g_Sangle += 0.2f;
		if (g_Sangle > 2.0f) {
			g_Sangle = 2.0f;
		}
		break;
	case '-':
		g_Speed -= 0.02f;
		if (g_Speed < 0.02f) {
			g_Speed = 0.02f;
		}
		g_Smount--;
		if (g_Smount < 1) {
			g_Smount = 1;
		}
		g_Sangle -= 0.2f;
		if (g_Sangle < 1.0f) {
			g_Sangle = 1.0f;
		}
		break;
	case 'Z':
		cameraPos.z -= 0.1f;
		break;
	case 'z':
		cameraPos.z += 0.1f;
		break;
	case 'x':
		cameraPos.x += 0.1f;
		break;
	case 'X':
		cameraPos.x -= 0.1f;
		break;
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
	case 'i':
		g_Rotate1 = 0;
		g_isOn1 = false;
		g_isOn2 = false;
		g_isOn3 = true;
		g_num = 0, g_num1 = 0, g_num2 = 0, g_num3 = 0;
		g_Fmove = 0, g_Lmove = 0, g_Rmove = 0, g_Bmove = 0;
		g_movenum = 0;
		g_movehand_foot = 0;
		g_Speed = 0.02f;
		g_Smount = 1;
		g_Sangle = 1.0f;
		g_Robots[0].Reset(0);
		g_Robots[1].Reset(1);
		g_Robots[2].Reset(2);
		g_Robots[3].Reset(3);
		g_Robots[4].Reset(4);
		g_Robots[5].Reset(5);
		g_Stages[0].A_R_matrix = glm::mat4(1.0f);
		cameraPos = glm::vec3(0.0f, 0.0f, 3.5f);
		break;
	case 'j':
		if (g_JisOn == 0) {
			g_JisOn = 1;
			g_Jup_down = 1;
			if (!g_isOn2) {
				glutTimerFunc(17, Timer, 4);
				g_isOn2 = true;
				g_isOn3 = false;
			}
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
		if (g_isOn) {
			theta += 0.1f;
			C_vertex[1].x = (C_vertex[0].x - cos(theta / 3.14f))/2;
			C_vertex[1].z = (C_vertex[0].z - sin(theta / 3.14f))/2;
			C_vertex[5].x = C_vertex[4].x = C_vertex[1].x;
			C_vertex[4].z = C_vertex[5].z = C_vertex[1].z;

			C_vertex[6].x = (C_vertex[7].x + cos(theta / 3.14f))/2;
			C_vertex[6].z = (C_vertex[7].z - sin(theta / 3.14f))/2;
			C_vertex[9].x = C_vertex[8].x = C_vertex[6].x;
			C_vertex[9].z = C_vertex[8].z = C_vertex[6].z;
			if (theta >= 19.0f) {
				g_open = 2;
				g_isOn = false;
			}
		}
		glutTimerFunc(17, Timer, 1);
		break;
	case 2:
		if (g_movehand_foot == 0) {
			g_Robots[2].AR_matrix = glm::rotate(g_Robots[2].AR_matrix, glm::radians(-g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[5].AR_matrix = glm::rotate(g_Robots[5].AR_matrix, glm::radians(-g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[3].AR_matrix = glm::rotate(g_Robots[3].AR_matrix, glm::radians(g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[4].AR_matrix = glm::rotate(g_Robots[4].AR_matrix, glm::radians(g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_movenum++;
			if (g_movenum >= 20) {
				g_movenum = 0;
				g_movehand_foot = 1;
			}
		}
		if (g_movehand_foot == 1) {
			g_Robots[2].AR_matrix = glm::rotate(g_Robots[2].AR_matrix, glm::radians(g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[5].AR_matrix = glm::rotate(g_Robots[5].AR_matrix, glm::radians(g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[3].AR_matrix = glm::rotate(g_Robots[3].AR_matrix, glm::radians(-g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[4].AR_matrix = glm::rotate(g_Robots[4].AR_matrix, glm::radians(-g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_movenum++;
			if (g_movenum >= 40) {
				g_movenum = 0;
				g_movehand_foot = 2;
			}
		}
		if (g_movehand_foot == 2) {
			g_Robots[2].AR_matrix = glm::rotate(g_Robots[2].AR_matrix, glm::radians(-g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[5].AR_matrix = glm::rotate(g_Robots[5].AR_matrix, glm::radians(-g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[3].AR_matrix = glm::rotate(g_Robots[3].AR_matrix, glm::radians(g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_Robots[4].AR_matrix = glm::rotate(g_Robots[4].AR_matrix, glm::radians(g_Sangle), glm::vec3(1.0f, 0.0f, 0.0f));
			g_movenum++;
			if (g_movenum >= 40) {
				g_movenum = 0;
				g_movehand_foot = 1;
			}
		}
		glutTimerFunc(17, Timer, 2);
		break;
	case 3:
		if (g_isOn1) {
			if (g_Rotate1 == 1) {
				g_Stages[0].A_R_matrix = glm::rotate(g_Stages[0].A_R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				g_Robots[0].AR_matrix = glm::rotate(g_Robots[0].AR_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				g_Fronts[0].A_R_matrix = glm::rotate(g_Fronts[0].A_R_matrix, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (g_Rotate1 == 2) {
				g_Stages[0].A_R_matrix = glm::rotate(g_Stages[0].A_R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				g_Robots[0].AR_matrix = glm::rotate(g_Robots[0].AR_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				g_Fronts[0].A_R_matrix = glm::rotate(g_Fronts[0].A_R_matrix, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			glutTimerFunc(17, Timer, 3);
		}
		break;
	case 4:
		if (g_isOn2) {
			if (g_Jup_down == 1) {
				g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
				g_Jmove++;
				g_mounty += 0.02f;
				if (g_Jmove >= 20) {
					g_Jmove = 0;
					g_Jup_down = 2;
				}
			}
			if (g_Jup_down == 2) {
				g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0.0f, -0.02f, 0.0f));
				g_mounty -= 0.02f;
				for (int i = 1;i < g_Stages.size();i++) {
					if (g_Stages[i].Pz - 0.2f < g_mountz && g_mountz < g_Stages[i].Pz + 0.2f) {
						if (g_Stages[i].Px - 0.2f < g_mountx && g_mountx < g_Stages[i].Px + 0.2f) {
							if (g_Stages[i].Py + 0.2f < g_mounty && g_mounty <= g_Stages[i].Py + 0.3f) {
								g_climb = 1;
							}
						}
					}
				}
				if (g_climb == 1) {
					g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
					g_mounty += 0.02f;
				}
				g_Jmove++;
				if (g_Jmove >= 20) {
					g_Jmove = 0;
					g_Jup_down = 0;
					g_JisOn = 0;
					g_climb = 0;
					g_isOn2 = false;
					g_isOn3 = true;
				}
			}
			glutTimerFunc(17, Timer, 4);
		}
		break;
	case 5:
		if (g_isOn3) {
			for (int i = 1;i < g_Stages.size();i++) {
				if (g_Stages[i].Pz - 0.2f < g_mountz && g_mountz < g_Stages[i].Pz + 0.2f) {
					if (g_Stages[i].Px - 0.2f < g_mountx && g_mountx < g_Stages[i].Px + 0.2f) {
						if (g_Stages[i].Py + 0.2f < g_mounty && g_mounty <= g_Stages[i].Py + 0.3f) {
							g_climb = 1;
						}
					}
				}
			}
			if (g_climb == 0) {
				if (g_mounty >= 0.0f) {
					g_Robots[0].T_matrix = glm::translate(g_Robots[0].T_matrix, glm::vec3(0.0f, -0.01f, 0.0f));
					g_mounty -= 0.01f;
				}
			}
		}
		glutTimerFunc(17, Timer, 5);
		break;
	default:
		break;
	}
}