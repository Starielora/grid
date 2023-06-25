#pragma once

#include <glad/glad.h>

#include <string>
#include <stdexcept>

namespace gl
{
	void glCheckError_(const char* file, int line)
	{
#ifndef NDEBUG
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			throw std::runtime_error(error + " | " + std::string(file) + " (" + std::to_string(line) + ")");
		}
#endif
	}
#define checkError() glCheckError_(__FILE__, __LINE__) 

	GLuint genVertexArray()
	{
		auto VAO = GLuint{ 0 };
		glGenVertexArrays(1, &VAO);
		gl::checkError();
		return VAO;
	}

	GLuint genBuffer()
	{
		auto buffer = GLuint{ 0 };
		glGenBuffers(1, &buffer);
		gl::checkError();
		return buffer;
	}

	GLuint genTexture(unsigned int width, unsigned int height)
	{
		auto texture = GLuint{ 0 };
		glGenTextures(1, &texture);
		gl::checkError();

		glBindTexture(GL_TEXTURE_2D, texture);
		gl::checkError();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		gl::checkError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		gl::checkError();

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	GLuint genFramebuffer(GLuint texture)
	{
		auto fbo = GLuint{ 0 };
		glGenFramebuffers(1, &fbo);
		gl::checkError();
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		gl::checkError();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("Framebuffer not complete.");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return fbo;
	}
}
