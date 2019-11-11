#pragma once

#include <GL\glew.h>

GLuint loadBMP(const char* imagepath);

GLuint loadTextureArray(const char* imagepath);

float getLayer(unsigned int capacity, unsigned int layer);