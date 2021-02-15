//Debugging
#include <iostream>

//Assets
#include "stb_image.h"
#include "managers/asset_manager.h"

//Maths
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Rendering
#include <GL/glew.h>
#include "rendering/vertex.h"
#include "rendering/shader.h"
#include "rendering/mesh.h"
#include "rendering/texture.h"

glm::vec2 WINDOW(1280.0f,720.0f);


/*
* Vertices
*/	
ce::Vertex vertices[] = {
	glm::vec3(0.5f,  0.5f, -0.5f), glm::vec4(0.0f,0.0f,0.0f,0.0f),  glm::vec2(1.0f, 1.0f),//0
	glm::vec3(0.5f, -0.5f, -0.5f),glm::vec4(0.0f,0.0f,1.0f,1.0f), glm::vec2(1.0f, 0.0f),//1
	glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec4(0.0f,1.0f,1.0f,1.0f), glm::vec2(0.0f, 0.0f),//2
	glm::vec3(-0.5f,  0.5f, -0.5), glm::vec4(0.0f,1.0f,0.0f,1.0f), glm::vec2(0.0f, 1.0f),//3
	
	glm::vec3(0.5f,  0.5f, 0.5f), glm::vec4(1.0f,0.0f,0.0f,0.0f),  glm::vec2(1.0f, 1.0f),//4
	glm::vec3(0.5f, -0.5f, 0.5f),glm::vec4(1.0f,1.0f,0.0f,1.0f), glm::vec2(1.0f, 0.0f),//5
	glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec4(1.0f,0.0f,1.0f,1.0f), glm::vec2(0.0f, 0.0f),//6
	glm::vec3(-0.5f,  0.5f, 0.5f), glm::vec4(1.0f,1.0f,1.0f,1.0f), glm::vec2(0.0f, 1.0f)//7
};

/*  7-4
	6-5
  3-0
  2-1
*/
unsigned vertexCount = sizeof(vertices)/sizeof(ce::Vertex);
//7<=>5
GLuint indices [] = {
	//F
	0, 1, 3,
	1, 2, 3,
	//U
	4,0,7,
	0,3,7,
	//L
	3,2,7,
	2,6,7,
	//R
	4,5,0,
	5,1,0,
	//D
	1,5,2,
	5,6,2,
	//B
	7,6,4,
	6,5,4
};
unsigned indexCount = sizeof(indices)/sizeof(GLuint);

int main(int argc, char* argv[]) {
	std::cout << "Hello World!" << std::endl;
	/*
	 * Init SDL
	 */
	if (SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "Error Intialising video\n";
		exit(1);
	}
	std::cout << "SDL has been initialized\n";
	/*
	 * Window Creation
	 */
	SDL_Window* window = SDL_CreateWindow(
		"Cinnabar",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW.x,
		WINDOW.y,
		SDL_WINDOW_OPENGL
	);
	if(window==NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		SDL_Quit();
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GLContext mainContext = SDL_GL_CreateContext(window);

	/*
	 * GLEW
	 */
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	float
		deltaTime = 0.0f,
		last = 0.0f;

	ce::Shader* shader = new ce::Shader("basic");
	
	ce::Mesh* mesh = new ce::Mesh(
		vertices,vertexCount,
		indices,indexCount
	);
	mesh->sendToShader(shader);
	ce::Texture* texture = new ce::Texture("uv-map.png");
	shader->setInt("fText",0);
	
	
	glm::vec3
		cameraPos(0.0f, 0.0f, 80.0f),
		cameraFront(0.0f,0.0f,-1.0f),
		cameraUp(0.0f,1.0f,0.0f);
		//cameraTarget(0.0f,0.0f,0.0f);
		//cameraDirection = glm::normalize(cameraPos - cameraTarget),
		//up(0.0f,1.0f,0.0f);
		//cameraRight = glm::normalize(glm::cross(up,cameraDirection));
		//cameraUp = glm::cross(cameraDirection,cameraRight);
	
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), WINDOW.x / WINDOW.y, 0.1f, 100.0f);
	shader->setMat4("uProj",proj);
		
	/*
	 * Game Loop
	 */
	SDL_Event event;
	int running = 1;
	while (running)
	{
		float
			now = (float)SDL_GetTicks()/1000;
			deltaTime = now-last;
			last = now;
		
		//Events
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					std::cout << event.key.keysym.sym << "\n";
					const float cameraSpeed = 2.5f*deltaTime; // adjust accordingly
					if (event.key.keysym.sym == SDLK_w)
						cameraPos += cameraSpeed * cameraFront;
					if (event.key.keysym.sym ==  SDLK_s)
						cameraPos -= cameraSpeed * cameraFront;
					if (event.key.keysym.sym == SDLK_a)
						cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
					if (event.key.keysym.sym == SDLK_d)
						cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				break;
			}
			if (event.type == SDL_QUIT) {
				running = 0;
				break;
			}
		}		
		
		/* Render */
		glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		/*float
			radius = 10.0f,
			camX = sin(gameTime)*radius,
			camZ = cos(gameTime)*radius;*/
		glm::mat4
			model(1.0f),
			//view = glm::lookAt(glm::vec3(camX,0.0f,camZ),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
			view = glm::lookAt(cameraPos,cameraPos + cameraFront,cameraUp);
		model = glm::rotate(model, now * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));  
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
		
		shader->setMat4("uModel",model);
		shader->setMat4("uView",view);
	
		
		texture->activate(0);
		shader->bind();
		mesh->bind();
		
		glDrawElements(GL_TRIANGLES,mesh->GetIndexCount(),GL_UNSIGNED_INT,0);
		
		mesh->unbind();
		shader->unbind();
		texture->unbind();

		// check and call events and swap the buffers
		SDL_GL_SwapWindow(window);
	}
	delete mesh;
	delete shader;
	delete texture;

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

