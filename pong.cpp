#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

void updateBall(float& ball_dir_x,float& ball_dir_y, float& ball_pos_x, float& ball_pos_y, float lyposition, float ryposition, float top, float bottom, float elapsed){
	float ballSpeed = 4.00f;
	float racketHeight = 1.666f;
	float nadjustedPosX = ball_pos_x - 0.125;
	float padjustedPosX = ball_pos_x + 0.125;
	float nadjustedPosY = ball_pos_y - 0.125;
	float padjustedPosY = ball_pos_y + 0.125;
	//set ball_dir_x and ball_dir_y accordingly
	// hit by left racket 
	if (nadjustedPosX < -3.35f && nadjustedPosY < (lyposition+0.833f) && padjustedPosY > (lyposition-0.833f)){
		//change x direction 
		ball_dir_x = fabs(ball_dir_x);
		// change y direction
		float t = ((ball_pos_y - lyposition) / racketHeight) ; // racket height 
		ball_dir_y = t;
	}
	// hit by right racket
	if (padjustedPosX > 3.35f && nadjustedPosY < (ryposition + 0.833f) && padjustedPosY >(ryposition - 0.833f)){
		//change x direction
		ball_dir_x = -fabs(ball_dir_x);
		//change y direction
		float t = ((ball_pos_y - ryposition) / racketHeight) ;
		ball_dir_y = t;
	}
	//hit by left wall
	if (nadjustedPosX < -3.55f){
		ball_pos_x = 0.0f;
		ball_pos_y = 0.0f;
	}
	//hit by right wall
	if (padjustedPosX > 3.55f){
		ball_pos_y = 0.0f;
		ball_pos_x = 0.0f;
	}
	// hit top
	if (padjustedPosY > top){
		ball_dir_y = -fabs(ball_dir_y);
	}
	// hit bottom
	if (nadjustedPosY < bottom){
		ball_dir_y = fabs(ball_dir_y);
	}
	ball_pos_y += (ball_dir_y * ballSpeed * elapsed);
	ball_pos_x += (ball_dir_x * ballSpeed * elapsed);
}


SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	Matrix projectionMatrix;
	Matrix viewMatrix;
	Matrix modelMatrix;
	Matrix rightmodelMatrix;
	Matrix leftmodelMatrix;
	float bottom = -2.0f;
	float top = 2.0f;
	//ball variables
	float ball_dir_x = -1.0f;
	float ball_dir_y = 1.0f;
	float ball_pos_x = 0.0f;
	float ball_pos_y = 0.0f;
	
	projectionMatrix.setOrthoProjection(-3.55, 3.55, bottom, top, -1.0f, 1.0f);
	glUseProgram(program.programID);
    // timings , angles and positions
	float lastFrameTicks = 0.0f;
	float angle = 0.0f;
	float units_a_second = 3.0f;
	float ryposition = 0.0f;
	float lyposition = 0.0f;
	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
					angle = 0.0;
				}
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		program.setModelMatrix(modelMatrix);

		// BALL
		updateBall(ball_dir_x, ball_dir_y, ball_pos_x, ball_pos_y, lyposition, ryposition, top, bottom, elapsed);
		modelMatrix.identity();
		modelMatrix.setPosition(ball_pos_x, ball_pos_y, 0.0f);
		program.setModelMatrix(modelMatrix);
		float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		if (keys[SDL_SCANCODE_UP]){
			if ((ryposition+0.833f) < top){
				ryposition += elapsed * units_a_second;
			}
		}
		if (keys[SDL_SCANCODE_DOWN]){
			if ((ryposition-0.833f) > bottom){
				ryposition -= elapsed * units_a_second;
			}
		}
		if (keys[SDL_SCANCODE_W]){
			if ((lyposition+0.833f) < top){
				lyposition += elapsed * units_a_second;
			}
		}
		if (keys[SDL_SCANCODE_S]){
			if ((lyposition-0.833f)> bottom){
				lyposition -= elapsed * units_a_second;
			}
		}

		//RIGHT BAT

		rightmodelMatrix.identity();
		//modelMatrix.Rotate(angle);
		rightmodelMatrix.setPosition(0.0f, ryposition, 0.0f);
		program.setModelMatrix(rightmodelMatrix);
		float verticesRight[] = { 3.35f, -0.833f, 3.55f, -0.833f, 3.55f, 0.833f, 3.35f, -0.833f, 3.55f, 0.833f, 3.35f, 0.833f};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesRight);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
	

		//LEFT BAT
		
		leftmodelMatrix.identity();
		leftmodelMatrix.setPosition(0.0f, lyposition, 0.0f);
		program.setModelMatrix(leftmodelMatrix);
		float verticesLeft[] = { -3.55f, -.833f, -3.35f, -0.833f, -3.35f, 0.833f, -3.55f, -0.833f, -3.35f, 0.833f, -3.55f, 0.833f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesLeft);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
