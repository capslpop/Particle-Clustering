#pragma once

#include "../main.h"

class RenderPass
{
public:
	RenderPass();
	void addVertexShader(const char* vertexShaderSource);
	void addGeometryShader(const char* geometryShaderSource);
	void addFragmentShader(const char* fragmentShaderSource);
	void addComputeShader(const char* computeShaderSource);
	void linkShaders();
	void use();
	~RenderPass();

	unsigned int shaderProgram;
private:
	std::vector<unsigned int> p_shaderStages;

};