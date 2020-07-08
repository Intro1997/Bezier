#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Bezier.h"


#define SRC_WIDTH 640
#define SRC_HEIGHT 480

static void GLClearError(){	
	while (GLenum error = glGetError()) {
		std::cout << "FORWARD::OpenGL::ERROR [" << std::hex << error << "]" << std::endl;
		__debugbreak();
	}
}

static void GLCheckError(const char* function, const char* file, const int line) {	
	while (GLenum error = glGetError()) {
		std::cout << "INSIDE::OpenGL::ERROR [" << std::hex << error << "]" << std::endl;
		std::cout << file << "::" << line << "::" << function << std::endl;
		__debugbreak();
	}
}


#define ASSERT(x) GLClearError(); x; GLCheckError(#x, __FILE__, __LINE__);


void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(void)
{
	GLFWwindow* window;	

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Bezier Curve", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);	
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
	

	if (glewInit() != GLEW_OK) {
		std::cout << "Error" << std::endl;
	}	

	std::vector<float> positions{
		-0.50f, -0.50f,
		-0.25f,  0.50f,
		0.25f,  0.50f,
		0.50f, -0.50f,
		//0.7f, -0.62f
	};
	
	Bezier secondOrderCurve = Bezier((unsigned int)SRC_WIDTH, (unsigned int)SRC_HEIGHT, window, positions, Bezier::OrderType::SecondOrder, 50, 2, 3, true, 10.0f);
	
	
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		processInput(window);

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//secondOrderCurve.Render();
				
		/* Swap front and back buffers */	
		glfwSwapBuffers(window);		
								
		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();	
	
	return 0;
}
