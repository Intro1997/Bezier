#shader vertex
#version 330 core
layout(location = 0) in vec2 pos;
void main() {
	gl_Position = vec4(pos, 0.0, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;
uniform vec3 PointColor;
void main() {
	FragColor = vec4(PointColor, 1.0);
}