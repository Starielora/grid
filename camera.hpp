#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera final
{
	float _speedMultiplier = 4 * 2.5f;
	float _yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float _pitch = 0.0f;
	float _lastX = 800.0f / 2.0;
	float _lastY = 600.0 / 2.0;
	float _fov = 45.0f;
	float _sensitivity = 0.1f;

	glm::vec3 _cameraPos = glm::vec3(5.0f, 5.0f, 40.0f);
	glm::vec3 _cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 _cameraRight;
	glm::vec3 _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

public:

	auto& speedMultiplier() { return _speedMultiplier; }
	auto& position() { return _cameraPos; }
	auto& fov() { return _fov; }

	auto projection(float width, float height) { return glm::perspective(glm::radians(_fov), width / height, 0.1f, 200.f); }
	auto view() { return glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp); }

	void moveForward(float t)
	{
		const auto cameraSpeed = _speedMultiplier * t;
		_cameraPos += cameraSpeed * _cameraFront;
	}

	void moveBack(float t)
	{
		const auto cameraSpeed = _speedMultiplier * t;
		_cameraPos -= cameraSpeed * _cameraFront;
	}

	void strafeLeft(float t)
	{
		const auto cameraSpeed = _speedMultiplier * t;
		_cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
	}

	void strafeRight(float t)
	{
		const auto cameraSpeed = _speedMultiplier * t;
		_cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
	}

	void fov(float offset)
	{
		_fov += offset;
		if (_fov < 1.0f)
			_fov = 1.0f;
		if (_fov > 45.0f)
			_fov = 45.0f;
	}

	void process_mouse(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= _sensitivity;
		yoffset *= _sensitivity;

		_yaw += xoffset;
		_pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (_pitch > 89.0f)
				_pitch = 89.0f;
			if (_pitch < -89.0f)
				_pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles

		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		front.y = sin(glm::radians(_pitch));
		front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_cameraFront = glm::normalize(front);
		// also re-calculate the Right and Up vector
		_cameraRight = glm::normalize(glm::cross(_cameraFront, _worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		_cameraUp = glm::normalize(glm::cross(_cameraRight, _cameraFront));
	}
};
