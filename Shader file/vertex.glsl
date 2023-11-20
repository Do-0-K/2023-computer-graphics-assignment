#version 330 core
layout (location = 0) in vec3 vPos;

uniform mat4 uParent;
uniform mat4 uRotate; //회전
uniform mat4 uTranslate; //이동
uniform mat4 uScale; //크기
uniform mat4 uA_Rotate; //공전
uniform mat4 P_Rotate; //공전
uniform mat4 view_trans; //뷰 변환
uniform mat4 proj_trans; //투영

void main(){
   gl_Position = proj_trans * view_trans * uParent * uA_Rotate * uTranslate * uRotate * uScale * vec4(vPos, 1.0);//회전 할때는 newPos, 나머지는 vPos
}