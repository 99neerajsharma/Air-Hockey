// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

#include <stdio.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
static int theta1 = 0, theta2 = 0, theta3 = 0, alpha1 = 0, alpha2 = 0;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 30); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.025f;

int getTheta1(){
	return theta1;
}

int getTheta2(){
	return theta2;
}

int getTheta3(){
	return theta3;
}

int getAlpha1(){
	return (int)horizontalAngle;
}

int getAlpha2(){
	return (int)verticalAngle;
}

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );
	if(horizontalAngle > 90)
		horizontalAngle = 90;
	else if(horizontalAngle < -90)
		horizontalAngle = -90;
	if(verticalAngle > 90)
		verticalAngle = 90;
	else if(verticalAngle < -90)
		verticalAngle = -90;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	// glm::vec3 direction(
	// 	cos(verticalAngle) * sin(horizontalAngle), 
	// 	sin(verticalAngle),
	// 	cos(verticalAngle) * cos(horizontalAngle)
	// );

	glm::vec3 direction(-0.706741, -0.706083, -0.044317);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	// glm::vec3 up = glm::cross( right, direction );

	glm::vec3 up(0, 1, 0);

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
	// increase theta1
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		theta1 += 1;
		if(theta1 > 90)
			theta1 = 90;
	}
	// decrease theta1
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		theta1 -= 1;
		if(theta1 < -90)
			theta1 = -90;
	}
	// increase theta2
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		theta2 += 1;
		if(theta2 > 90)
			theta2 = 90;
	}
	// decrease theta2
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		theta2 -= 1;
		if(theta2 < -90)
			theta2 = -90;
	}
	// increase theta3
	if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS){
		theta3 += 1;
		if(theta3 > 360)
			theta3 = 360;
	}
	// decrease theta3
	if (glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS){
		theta3 -= 1;
		if(theta3 < 0)
			theta3 = 0;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								glm::vec3(0,0,0), // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}