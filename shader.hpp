#pragma once

#include "openglutils.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <filesystem>
#include <vector>

namespace
{
	std::string readFile(std::filesystem::path path)
	{
		auto file = std::ifstream(path.c_str());
		assert(file.is_open());
		return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}

	auto compileShader(int type, std::string_view src)
	{
		assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER);
		assert(src.length());

		const auto id = glCreateShader(type);
		gl::checkError();

		const char* srcPtr = src.data();
		glShaderSource(id, 1, &srcPtr, nullptr);
		gl::checkError();

		glCompileShader(id);

		GLint success;
		GLchar infoLog[1024];
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(id, 1024, NULL, infoLog);
			throw std::runtime_error(infoLog);
		}

		return id;
	}

	auto loadShaderSources(std::filesystem::path vertex, std::filesystem::path fragment)
	{
		auto shadersSources = std::unordered_map<unsigned, std::string>{};
		shadersSources[GL_VERTEX_SHADER] = readFile(vertex);
		shadersSources[GL_FRAGMENT_SHADER] = readFile(fragment);

		assert(shadersSources[GL_VERTEX_SHADER].length());
		assert(shadersSources[GL_FRAGMENT_SHADER].length());

		return shadersSources;
	}

	auto createProgram(std::unordered_map<unsigned, std::string> shadersSources)
	{
		auto shaders = std::vector<unsigned>();
		shaders.reserve(shadersSources.size());

		for (const auto& [type, source] : shadersSources)
		{
			const auto id = compileShader(type, source);
			shaders.push_back(id);
		}

		const auto id = glCreateProgram();
		gl::checkError();

		for (auto shader : shaders)
		{
			glAttachShader(id, shader);
			gl::checkError();
		}

		glLinkProgram(id);
		gl::checkError();

		GLint success;
		GLchar infoLog[1024];
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		gl::checkError();
		if (!success)
		{
			glGetProgramInfoLog(id, 1024, NULL, infoLog);
			gl::checkError();
			throw std::runtime_error(infoLog);
		}

		for (auto shader : shaders)
		{
			glDeleteShader(shader);
			gl::checkError();
		}

		return id;
	}
}

class shader final
{
	const unsigned _id;

public:
	shader(std::filesystem::path vertex, std::filesystem::path fragment)
		: _id(createProgram(loadShaderSources(vertex, fragment)))
	{

	}

	~shader()
	{
		glDeleteProgram(_id);
		gl::checkError();
	}

	void use() const
	{
		glUseProgram(_id);
		gl::checkError();
	}

	void setInt(const std::string& name, int value) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform1i(location, value);
		gl::checkError();
	}

	void setFloat(const std::string& name, float value) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform1f(location, value);
		gl::checkError();
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform2fv(location , 1, &value[0]);
		gl::checkError();
	}

	void setVec2(const std::string& name, float x, float y) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform2f(location, x, y);
		gl::checkError();
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform3fv(location, 1, &value[0]);
		gl::checkError();
	}

	void setVec3(const std::string& name, float x, float y, float z) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform3f(location, x, y, z);
		gl::checkError();
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform4fv(location, 1, &value[0]);
		gl::checkError();
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniform4f(location, x, y, z, w);
		gl::checkError();
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
		gl::checkError();
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
		gl::checkError();
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		const auto location = glGetUniformLocation(_id, name.c_str());
		gl::checkError();
		glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
		gl::checkError();
	}
};
