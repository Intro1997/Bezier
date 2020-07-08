#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
void main() {
	gl_Position = vec4(position, 0.0, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec3 LineColor;
 
void main() {
	FragColor = vec4(LineColor, 1.0);
}