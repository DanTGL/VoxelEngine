#pragma once

#include <GL/glew.h>
#include <string>

class Shader {
public:
	unsigned int ID;
	
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();
	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
};
