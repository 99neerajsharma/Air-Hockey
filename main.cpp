// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <utils/controls.hpp>
#include <utils/objloader.hpp>
#include <utils/shader.hpp>

int main(){
    // Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Robotic Arm", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    std::vector<glm::vec3> table_vertices;
	std::vector<glm::vec2> table_uvs;
	std::vector<glm::vec3> table_normals; 
	std::vector<glm::vec3> table_parts_vertices;
	std::vector<glm::vec2> table_parts_uvs;
	std::vector<glm::vec3> table_parts_normals;
    std::vector<glm::vec3> puck_vertices;
	std::vector<glm::vec2> puck_uvs;
	std::vector<glm::vec3> puck_normals;
    std::vector<glm::vec3> striker_neg_vertices;
	std::vector<glm::vec2> striker_neg_uvs;
	std::vector<glm::vec3> striker_neg_normals;
    std::vector<glm::vec3> striker_pos_vertices;
	std::vector<glm::vec2> striker_pos_uvs;
	std::vector<glm::vec3> striker_pos_normals;
    
    bool res = loadOBJ("objects/table.obj", table_vertices, table_uvs, table_normals);
    res = loadOBJ("objects/table-parts.obj", table_parts_vertices, table_parts_uvs, table_parts_normals);
    res = loadOBJ("objects/puck.obj", puck_vertices, puck_uvs, puck_normals);
    res = loadOBJ("objects/striker-4.obj", striker_neg_vertices, striker_neg_uvs, striker_neg_normals);
    res = loadOBJ("objects/striker4.obj", striker_pos_vertices, striker_pos_uvs, striker_pos_normals);
    
    int table_vertices_size =  table_vertices.size();
    int table_parts_vertices_size = table_parts_vertices.size();
    int puck_vertices_size = puck_vertices.size();
    int striker_vertices_size = striker_neg_vertices.size();

    GLfloat puck_color_buffer_data[puck_vertices_size*3];
	for (int v = 0; v < puck_vertices_size ; v++){
		puck_color_buffer_data[3*v+0] = 1.0;
		puck_color_buffer_data[3*v+1] = 1.0;
		puck_color_buffer_data[3*v+2] = 1.0;
	}

    GLfloat striker_neg_color_buffer_data[striker_vertices_size*3];
	for (int v = 0; v < striker_vertices_size ; v++){
		striker_neg_color_buffer_data[3*v+0] = 0.134;
		striker_neg_color_buffer_data[3*v+1] = 0.79;
		striker_neg_color_buffer_data[3*v+2] = 0;
	}

    GLfloat striker_pos_color_buffer_data[striker_vertices_size*3];
	for (int v = 0; v < striker_vertices_size ; v++){
		striker_pos_color_buffer_data[3*v+0] = 0.9;
		striker_pos_color_buffer_data[3*v+1] = 0.09;
		striker_pos_color_buffer_data[3*v+2] = 0.09;
	}

    GLuint table_vertex_buffer;
	glGenBuffers(1, &table_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, table_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, table_vertices.size() * sizeof(glm::vec3), &table_vertices[0], GL_STATIC_DRAW);

    GLuint table_parts_vertex_buffer;
	glGenBuffers(1, &table_parts_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, table_parts_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, table_parts_vertices.size() * sizeof(glm::vec3), &table_parts_vertices[0], GL_STATIC_DRAW);

    GLuint puck_vertex_buffer;
	glGenBuffers(1, &puck_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, puck_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, puck_vertices.size() * sizeof(glm::vec3), &puck_vertices[0], GL_STATIC_DRAW);

    GLuint striker_neg_vertex_buffer;
	glGenBuffers(1, &striker_neg_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, striker_neg_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, striker_neg_vertices.size() * sizeof(glm::vec3), &striker_neg_vertices[0], GL_STATIC_DRAW);

    GLuint striker_pos_vertex_buffer;
	glGenBuffers(1, &striker_pos_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, striker_pos_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, striker_pos_vertices.size() * sizeof(glm::vec3), &striker_pos_vertices[0], GL_STATIC_DRAW);

 
 
    GLuint puck_color_buffer;
	glGenBuffers(1, &puck_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, puck_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(puck_color_buffer_data), puck_color_buffer_data, GL_STATIC_DRAW);

    GLuint striker_neg_color_buffer;
	glGenBuffers(1, &striker_neg_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, striker_neg_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(striker_neg_color_buffer_data), striker_neg_color_buffer_data, GL_STATIC_DRAW);

    GLuint striker_pos_color_buffer;
	glGenBuffers(1, &striker_pos_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, striker_pos_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(striker_pos_color_buffer_data), striker_pos_color_buffer_data, GL_STATIC_DRAW);
    
    do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);

		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, table_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// glEnableVertexAttribArray(1);
		// glBindBuffer(GL_ARRAY_BUFFER, lowercolorbuffer);
		// glVertexAttribPointer(
		// 	1,                  // attribute
		// 	3,                  // size
		// 	GL_FLOAT,           // type
		// 	GL_FALSE,           // normalized?
		// 	0,                  // stride
		// 	(void*)0            // array buffer offset
		// );
		// glPushMatrix();
		glDrawArrays(GL_TRIANGLES, 0, table_vertices.size() );
		// glPopMatrix();

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, table_parts_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// glEnableVertexAttribArray(1);
		// glBindBuffer(GL_ARRAY_BUFFER, lowercolorbuffer);
		// glVertexAttribPointer(
		// 	1,                  // attribute
		// 	3,                  // size
		// 	GL_FLOAT,           // type
		// 	GL_FALSE,           // normalized?
		// 	0,                  // stride
		// 	(void*)0            // array buffer offset
		// );
		// glPushMatrix();
		glDrawArrays(GL_TRIANGLES, 0, table_parts_vertices.size() );
		// glPopMatrix();

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, puck_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, puck_color_buffer);
		glVertexAttribPointer(
			1,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// glPushMatrix();
		glDrawArrays(GL_TRIANGLES, 0, puck_vertices.size() );
		// glPopMatrix();

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, striker_neg_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, striker_neg_color_buffer);
		glVertexAttribPointer(
			1,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// glPushMatrix();
		glDrawArrays(GL_TRIANGLES, 0, striker_neg_vertices.size() );
		// glPopMatrix();

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, striker_pos_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, striker_pos_color_buffer);
		glVertexAttribPointer(
			1,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// glPushMatrix();
		glDrawArrays(GL_TRIANGLES, 0, striker_pos_vertices.size() );
		// glPopMatrix();

		


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		// sleep(1);

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    glDeleteBuffers(1, &table_vertex_buffer);
    glDeleteBuffers(1, &table_parts_vertex_buffer);
    glDeleteBuffers(1, &puck_vertex_buffer);
    glDeleteBuffers(1, &striker_neg_vertex_buffer);
    glDeleteBuffers(1, &striker_pos_vertex_buffer);
    glDeleteBuffers(1, &puck_color_buffer);
    glDeleteBuffers(1, &striker_neg_color_buffer);
    glDeleteBuffers(1, &striker_pos_color_buffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);       

    glfwTerminate();
    return 0;
}