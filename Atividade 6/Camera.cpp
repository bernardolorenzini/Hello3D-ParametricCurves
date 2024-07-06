#include "Camera.h"
using namespace std;



void Camera::initialize(Shader* shader, int width, int height, float sensiilidade_camera, float initialPitch, float initialYaw, glm::vec3 initialCameraFront, glm::vec3 initialCameraPos, glm::vec3 initialCameraUp)
{
	isFirstMouse = true;
	isRotatingX = false;
	isRotatingY = false;
	isRotatingZ = false;
	this->shader = shader;
	this->sensiilidade_camera = sensiilidade_camera;
	pitch = initialPitch;
	yaw = initialYaw;
	cameraFront = initialCameraFront;
	cameraPos = initialCameraPos;
	cameraUp = initialCameraUp;
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader->setMat4("view", glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader->setMat4("projection", glm::value_ptr(projection));
}

void Camera::rotate(GLFWwindow* window, double xpos, double ypos)
{
	if (isFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		isFirstMouse = false;
	}
	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	offsetX *= sensiilidade_camera;
	offsetY *= sensiilidade_camera;

	pitch += offsetY;
	yaw += offsetX;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void Camera::update()
{
	// Update camera position and orientation
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	shader->setMat4("view", glm::value_ptr(view));
	// Update shader with camera position
	shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}

void Camera::move(GLFWwindow* window, int key, int action)
{
	float velocidade_camera = 0.05;

	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraFront * velocidade_camera;
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraFront * velocidade_camera;
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocidade_camera;
	}
	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocidade_camera;
	}
}
