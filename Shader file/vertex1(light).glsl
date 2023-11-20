#version 330 core

layout(location = 0)in vec3 vPos;
layout(location = 1)in vec3 vertexNormal;

uniform mat4 uParent;
uniform mat4 uRotate; //ȸ��
uniform mat4 uTranslate; //�̵�
uniform mat4 uScale; //ũ��
uniform mat4 uA_Rotate; //����
uniform mat4 view_trans; //�� ��ȯ
uniform mat4 proj_trans; //����

out vec3 fragPos;
out vec3 normal;

void main(){
	gl_Position = proj_trans * view_trans * uParent * uA_Rotate * uTranslate * uRotate * uScale * vec4(vPos, 1.0);
	fragPos = vec3(uParent * uA_Rotate * uTranslate * uRotate * uScale * vec4(vPos, 1.0));
	normal = vec3(uParent * uA_Rotate * uTranslate * uRotate * uScale*vec4(vertexNormal, 0.0));
}