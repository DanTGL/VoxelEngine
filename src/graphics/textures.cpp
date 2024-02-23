#include "textures.h"

#pragma warning(disable:4996)

#include <fstream>
#include <math.h>

GLuint loadBMP(const char* imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54];	// Each BMP file begins by a 54-bytes header
	unsigned int dataPos;		// Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;		// width * height * 3
	// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	}

	// if not 54 bytes read
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	dataPos = *(int*) &(header[0x0A]);
	imageSize = *(int*) &(header[0x22]);
	width = *(int*) &(header[0x12]);
	height = *(int*) &(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)	imageSize = width * height * 3;	// 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) dataPos = 54;	// The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture

	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(data);


	return textureID;
}

GLuint loadTextureArray(const char* imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54];	// Each BMP file begins by a 54-bytes header
	unsigned int dataPos;		// Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;		// width * height * 3
								// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	}

	// if not 54 bytes read
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	dataPos = *(int*) &(header[0x0A]);
	imageSize = *(int*) &(header[0x22]);
	width = *(int*) &(header[0x12]);
	height = *(int*) &(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)	imageSize = width * height * 3;	// 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) dataPos = 54;	// The BMP header is done that way

									// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed
	fclose(file);
	
	

	GLsizei c, tile_w = 16, tile_h = 16;

	GLuint tileset;

	GLsizei tiles_x = (width / tile_w),
			tiles_y = (height / tile_h),
			tile_count = tiles_x * tiles_y;

	printf("%d", tile_count);

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tileset);
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, tileset);
	glTextureStorage3D(tileset, 1, GL_RGB8, tile_w, tile_h, tile_count);

	{
		GLuint temp_tex = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &temp_tex);
		glTextureStorage2D(temp_tex, 1, GL_RGB8, width, height);
		glTextureSubImage2D(temp_tex, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
		
		for (GLsizei i = 0; i < tile_count; ++i) {
			GLint x = (i % tiles_x) * tile_w, y = ((i / tiles_x) * tile_h);
			glCopyImageSubData(temp_tex, GL_TEXTURE_2D, 0, x, y, 0, tileset, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tile_w, tile_h, 1);
		}

		glDeleteTextures(1, &temp_tex);
	}

	free(data);

	//glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	/*
	GLuint texture = 0;
	GLuint mipLevelCount = 1;
	GLuint t_width = 16;
	GLuint t_height = 16;
	GLuint layerCount = 256;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	// Allocate the storage
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGB, t_width, t_height, layerCount);

	// Upload pixel data
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, t_width, t_height, layerCount, GL_BGR, GL_UNSIGNED_BYTE, data);
	*/
	// Always set reasonable texture parameters
	/*glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	*/
	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	return tileset;
}

GLuint* loadTextures(std::string strings[]) {

	GLuint* textures = (GLuint*) malloc(sizeof(GLuint) * strings->size());

	for (int i = 0; i < strings->size(); i++) {
		// Data read from the header of the BMP file
		unsigned char header[54];	// Each BMP file begins by a 54-bytes header
		unsigned int dataPos;		// Position in the file where the actual data begins
		unsigned int width, height;
		unsigned int imageSize;		// width * height * 3
									// Actual RGB data
		unsigned char* data;

		FILE* file = fopen(strings[i].c_str(), "rb");
		if (!file) {
			printf("Image could not be opened\n");
			return 0;
		}

		// if not 54 bytes read
		if (fread(header, 1, 54, file) != 54) {
			printf("Not a correct BMP file\n");
			return false;
		}

		if (header[0] != 'B' || header[1] != 'M') {
			printf("Not a correct BMP file\n");
			return 0;
		}

		dataPos = *(int*) &(header[0x0A]);
		imageSize = *(int*) &(header[0x22]);
		width = *(int*) &(header[0x12]);
		height = *(int*) &(header[0x16]);

		// Some BMP files are misformatted, guess missing information
		if (imageSize == 0)	imageSize = width * height * 3;	// 3 : one byte for each Red, Green and Blue component
		if (dataPos == 0) dataPos = 54;	// The BMP header is done that way

										// Create a buffer
		data = new unsigned char[imageSize];

		// Read the actual data from the file into the buffer
		fread(data, 1, imageSize, file);

		// Everything is in memory now, the file can be closed
		fclose(file);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		free(data);

	}

	/*
	GLuint texture = 0;
	GLuint mipLevelCount = 1;
	GLuint t_width = 16;
	GLuint t_height = 16;
	GLuint layerCount = 256;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	// Allocate the storage
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGB, t_width, t_height, layerCount);

	// Upload pixel data
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, t_width, t_height, layerCount, GL_BGR, GL_UNSIGNED_BYTE, data);
	*/
	// Always set reasonable texture parameters
	/*glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	*/

	return textures;
}

float getLayer(unsigned int capacity, unsigned int layer) {
	return fmaxf(0, fminf(float(capacity - 1), floorf(float(layer) + 0.5f)));
}