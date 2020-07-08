#pragma once
#include <fstream>
#include <string>
#include <sstream>


class Shader {
private:
	unsigned int _program;

	struct ShaderSource {
		std::string vertexShaderSource, fragmentShaderSource;
	};	
public:
	Shader() {
		_program = 0;
	}

	Shader(const std::string& shaderPath) {
		try {			
			InitializeProgram(shaderPath);
		}
		catch (const char* exceptionInfo) {
			std::cout << exceptionInfo << std::endl;
		}
	}

	~Shader() {
		glDeleteProgram(_program);
	}

	void InitializeProgram(const std::string& shaderPath) {
		ShaderSource sources = LoadShaderSource(shaderPath);
		_program = CreateShader(sources.vertexShaderSource, sources.fragmentShaderSource);
	}

	static Shader getShader(const char* path) {
		return Shader(path);
	}

	ShaderSource LoadShaderSource(const std::string& shaderPath) {
		std::ifstream shaderFile(shaderPath);		
		if (!shaderFile.is_open()) {
			std::cout << "ERROR::FILE::Open shader file failed\n";			
			return{};
		}
		else {			
			std::string line;			
			std::stringstream shaderSource[2];		
			enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
			int type = static_cast<int>(ShaderType::NONE);
			while (getline(shaderFile, line)) {
				if (line.find("#shader") != std::string::npos) {
					if (line.find("vertex") != std::string::npos) {
						type = static_cast<int>(ShaderType::VERTEX);
					}
					else if (line.find("fragment") != std::string::npos) {
						type = static_cast<int>(ShaderType::FRAGMENT);
					}
				}
				else {
					shaderSource[type] << line << "\n";
				}
			}			
			return { shaderSource[0].str(), shaderSource[1].str() };
		}
	}

	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
		unsigned int program = glCreateProgram();		
		unsigned int vs, fs;
		try {
			vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
			fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
		}
		catch (const std::string& exceptionInfo) {
			std::cout << "ERROR::" << exceptionInfo << std::endl;
		}
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		int flag;
		glGetProgramiv(program, GL_LINK_STATUS, &flag);
		if (flag == GL_FALSE) {
			int length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
			char *infoLog = (char*)alloca(sizeof(char) * length);
			glGetProgramInfoLog(program, length, &length, infoLog);
			std::cout << "ERROR::PROGRAM::\n" << infoLog << std::endl;
			glDeleteProgram(program);
		}
		return program;
	}

	unsigned int CompileShader(GLenum type, const std::string& source) {
		unsigned int shader = glCreateShader(type);
		unsigned int temp = 12;
		const char* src = source.c_str();		
		glShaderSource(shader, 1, &src, nullptr);		
		glCompileShader(shader);		

		// TODO: Check comile status
		int status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			int length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			char *infoLog = (char*)alloca(sizeof(char*) * length);
			glGetShaderInfoLog(shader, length, &length, infoLog);
			throw std::string(((type == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER::\n" : "GL_FRAGMENT_SHADER::\n")) + infoLog;
			
			glDeleteShader(shader);
		}
		return shader;
	}

	void Use() {
		glUseProgram(_program);
	}

	void setUniform3f(const std::string& name, float v0, float v1, float v2) {
		int location = glGetUniformLocation(_program, name.c_str());
		if (location == -1) {
			GLenum error = glGetError();
			std::cout << "SHADER::LOCATION::ERROR::" << std::hex << error << "[" << __FILE__ << "]::" << __LINE__ << std::endl;
			__debugbreak();
		}
		glUniform3f(location, v0, v1, v2);
	}

	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
		int location = glGetUniformLocation(_program, name.c_str());	
		if (location == -1) {
			GLenum error = glGetError();
			std::cout << "SHADER::LOCATION::ERROR::"<< std::hex << error << "[" << __FILE__ << "]::" << __LINE__ << std::endl;			
			__debugbreak();
		}		
		glUniform4f(location, v0, v1, v2, v3);
	}

	
};
