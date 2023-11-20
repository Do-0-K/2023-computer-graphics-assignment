#version 330 core

//in vec3 fColor;
uniform vec3 vColor;
out vec4 color;

void main (){
//color = vec4 (fColor.r - vColor.r,fColor.g - vColor.g,fColor.b - vColor.b, 1.0);
color = vec4 (vColor, 1.0);
}