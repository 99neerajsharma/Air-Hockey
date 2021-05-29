#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

std::pair<float, float> getStrikerNegTrans();
std::pair<float, float> getStrikerPosTrans();
std::pair<float, float> getPuckTrans();
int getGameStatus();



#endif