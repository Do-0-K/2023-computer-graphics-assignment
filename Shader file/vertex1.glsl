#version 330 core
layout (location = 0) in vec3 vPos;

uniform float vRotate; //ȸ��
uniform vec3 vMove; //�߽�

void main(){
   gl_Position = vec4(vPos + vMove, 1.0);//ȸ�� �Ҷ��� newPos, �������� vPos
}