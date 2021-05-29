// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

#include <stdio.h>
#include <iostream>

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
glm::vec3 position = glm::vec3( 0, 0, 18); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.025f;

// pair for translating striker
// first element of pair gives x direction shift
// while second element gives y direction shift
std::pair<float, float> striker_pos{0.0f, 0.0f}, striker_neg{0.0f, 0.0f}, puck{0.0f, 0.0f};
std::pair<float, float> puck_direction{0.0f, 0.0f};
float puck_speed = 0.11;
int greenGoals = 0, redGoals = 0;
int gameStatus = 0;
std::pair<float, float> getStrikerNegTrans(){
	return striker_neg;
}
std::pair<float, float> getStrikerPosTrans(){
	return striker_pos;
}
std::pair<float, float> getPuckTrans(){
	return puck;
}

int getGameStatus() {
	return gameStatus;
}

float distance(std::pair<float, float> &a, std::pair<float, float> &b){
	float x = abs(a.first - b.first);
	float y = abs(a.second - b.second);
	float d = sqrt(x * x + y * y);
	return d;
}

void resetObjects() {
	striker_neg = {0.0f, 0.0f};
	striker_pos = {0.0f, 0.0f};
	puck = {0.0f, 0.0f};
	puck_direction = {0.0f, 0.0f};

	std::cout <<"Score: Red - "<< redGoals <<
	" Green - "<< greenGoals << std::endl;

	if(redGoals >= 5)
		gameStatus = -1;
	if(greenGoals >= 5)
		gameStatus = 1;	
}

void reflectPuckIfWall() {
	glm::vec2 reflectedVector{0.0f,0.0f};
	bool isReflected = false;

	if(puck.second > 4.0f) {
		isReflected = true;
		reflectedVector = reflect(
			glm::vec2(puck_direction.first, puck_direction.second), 
			glm::vec2(0, -1));
	}

	else if(puck.second < -4.0f) {
		isReflected = true;
		reflectedVector = reflect(
			glm::vec2(puck_direction.first, puck_direction.second), 
			glm::vec2(0, 1));
	}

	else if(puck.first > 7.50f) {
		if((puck.second > -1.83f) && (puck.second < 1.83f))
		{
			greenGoals++;
			resetObjects();
			return;
		}

		isReflected = true;
		reflectedVector = reflect(
			glm::vec2(puck_direction.first, puck_direction.second), 
			glm::vec2(-1, 0));
	}

	else if(puck.first < -7.50f) {
		if((puck.second > -1.83f) && (puck.second < 1.83f))
		{
			redGoals++;
			resetObjects();
			return;
		}

		isReflected = true;
		reflectedVector = reflect(
			glm::vec2(puck_direction.first, puck_direction.second), 
			glm::vec2(1, 0));
	}

	if(isReflected)
		puck_direction = std::pair<float, float>{reflectedVector.x, reflectedVector.y};
}

// striker_pos = {0, 0};
// striker_neg = {0, 0};

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
	std::pair<float, float> striker_pos_old, striker_neg_old;
	striker_pos_old = striker_pos;
	striker_neg_old = striker_neg;

	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS && striker_pos.second < 3.9){
		striker_pos.second += 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS && striker_pos.second > -3.9){
		striker_pos.second -= 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS && striker_pos.first < 3.4){
		striker_pos.first += 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS && striker_pos.first > -3.4){
		striker_pos.first -= 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS && striker_neg.first < 3.4){
		striker_neg.first += 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS && striker_neg.first > -3.4){
		striker_neg.first -= 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS && striker_neg.second < 3.9){
		striker_neg.second += 0.1;
	}
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS && striker_neg.second > -3.9){
		striker_neg.second -= 0.1;
	}

	// std::cout << striker_neg.first << " neg " << striker_neg.second << std::endl;
	// std::cout << striker_pos.first << " pos " << striker_pos.second << std::endl;
	// std::cout << puck.first << " puck " << puck.second << std::endl;
	
	// striker's position keeping table center as origin
	std::pair<float, float> striker_neg_loc, striker_pos_loc;
	striker_neg_loc = {striker_neg.first - 4.0, striker_neg.second};
	striker_pos_loc = {striker_pos.first + 4.0, striker_pos.second};

	if(distance(striker_neg_loc, puck) < 0.7) {
		striker_neg = striker_neg_old;
		striker_neg_loc = {striker_neg.first - 4.0, striker_neg.second};
	}

	if(distance(striker_pos_loc, puck) < 0.7) {
		striker_pos = striker_pos_old;
		striker_pos_loc = {striker_pos.first + 4.0, striker_pos.second};
	}

	if(distance(striker_neg_loc, puck) < 0.9 && distance(striker_pos_loc, puck) < 0.9){
		// std::cout << "BOTH" << std::endl;
		puck_direction.first = 2 * puck.first - striker_neg_loc.first - striker_pos_loc.first;
		puck_direction.second = 2 * puck.second - striker_neg_loc.second - striker_pos_loc.second;
	}
	else if(distance(striker_neg_loc, puck) < 0.9){
		// std::cout << "neg" << std::endl;
		puck_direction.first = puck.first - striker_neg_loc.first;
		puck_direction.second = puck.second - striker_neg_loc.second;
	}
	else if(distance(striker_pos_loc, puck) < 0.9){
		// std::cout << "pos" << std::endl;
		puck_direction.first = puck.first - striker_pos_loc.first;
		puck_direction.second = puck.second - striker_pos_loc.second;
	}

	reflectPuckIfWall();
	std::pair<float, float> origin{0.0f, 0.0f};
	float mag_puck_direction = distance(puck_direction, origin);
	if(mag_puck_direction != 0.0f){
		puck_direction.first /= mag_puck_direction;
		puck_direction.second /= mag_puck_direction;
	}

	

	puck.first += puck_speed * puck_direction.first;
	puck.second += puck_speed * puck_direction.second;

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