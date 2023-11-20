#version 330 core

layout(location = 0)in vec3 vPos;
layout(location = 1)in vec3 vertexNormal;

uniform mat4 uParent;
uniform mat4 uRotate; //회전
uniform mat4 uTranslate; //이동
uniform mat4 uScale; //크기
uniform mat4 uA_Rotate; //공전
uniform mat4 view_trans; //뷰 변환
uniform mat4 proj_trans; //투영

out vec3 fragPos;
out vec3 normal;

void main(){
	gl_Position = proj_trans * view_trans * uParent * uA_Rotate * uTranslate * uRotate * uScale * vec4(vPos, 1.0);
	fragPos = vec3(uParent * uA_Rotate * uTranslate * uRotate * uScale * vec4(vPos, 1.0));
	normal = vec3(uParent * uA_Rotate * uTranslate * uRotate * uScale*vec4(vertexNormal, 0.0));
}