#version 330 core
layout (location = 0) in vec3 vPos;

uniform float vRotate; //회전
uniform vec3 vMove; //중심

void main(){
   gl_Position = vec4(vPos + vMove, 1.0);//회전 할때는 newPos, 나머지는 vPos
}