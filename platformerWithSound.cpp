#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <vector>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;



class Vector3;



// function to load textures
GLuint LoadTextures(const char* image_path, int n = 0) {
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface) {
		printf("IMG_Load: %s\n", IMG_GetError());
		// handle error
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}

//Vector3 class
class Vector3{
public:
	Vector3();
	Vector3(float x, float y, float z);
	void operator=(const Vector3& rightSide);
	float x;
	float y;
	float z;
};
//Vector 3 constructor
Vector3::Vector3(){
	x = NULL;
	y = NULL;
	z = NULL;
}
Vector3::Vector3(float x, float y, float z){
	this->x = x;
	this->y = y;
	this->z = z;
}
void Vector3::operator=(const Vector3& rightSide){
	this->x = rightSide.x;
	this->y = rightSide.y;
	this->z = rightSide.z;
}
// SheetSprite class
class SheetSprite {
public:
	SheetSprite();
	SheetSprite(GLuint textureID, int index, int spriteCountX, int spriteCountY, float size);       // unsigned int     
	void operator=(const SheetSprite& rightSide);
	void Draw(ShaderProgram *program);
	void drawSingleTexture(ShaderProgram* program, float vertices[], int heightmultiplier = 1, int widhtmultiplier = 1);
	float size;
	GLuint textureID;
	float u;
	float v;
	float width;
	float height;
	float aspect;
};


// constructor for SheetSprite
SheetSprite::SheetSprite(){
	size = 1;
	textureID = NULL;
	u = 0.0f;
	v = 0.0f;
	width = 1.0f;
	height = 1.0f;
	aspect = 1.0f;
}
SheetSprite::SheetSprite(GLuint textureID, int index, int spriteCountX, int spriteCountY, float size){
	this->textureID = textureID;
	this->u = (float)(((int)index) % spriteCountX) / (float)spriteCountX; // 0 u
	this->v = (float)(((int)index) / spriteCountX) / (float)spriteCountY; // 0 v
	this->width = 1.0 / (float)spriteCountX;
	this->height = 1.0 / (float)spriteCountY;
	this->aspect = width / height;
	this->size = size;
}

// assignment operator overload SheetSprite
void SheetSprite::operator=(const SheetSprite& rightSide){
	this->aspect = rightSide.aspect;
	this->height = rightSide.height;
	this->size = rightSide.size;
	this->textureID = rightSide.textureID;
	this->u = rightSide.u;
	this->v = rightSide.v;
	this->width = rightSide.width;
}

// draw function for SheetSprite
void SheetSprite::Draw(ShaderProgram *program) {
	GLfloat texCoords[] = { u, v + height, u + width, v, u, v, u + width, v, u, v + height, u + width, v + height };                                                                                          //
	// draw our arrays                                                                   //            //                                                                                                
	float vertices[] = { (-1.5f)*size*aspect, (-1.5f)*size, (1.5f)*size*aspect, (1.5f)*size, (-1.5f)*size*aspect, (1.5f)*size, (1.5)*size*aspect, (1.5f)*size, (-1.5f)*size*aspect, (-1.5)*size, (1.5f)*size*aspect, (-1.5f)*size };

	glBindTexture(GL_TEXTURE_2D, textureID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(program->positionAttribute);
}


void SheetSprite::drawSingleTexture(ShaderProgram* program, float vertices[], int heightmuliplier, int widthmultiplier){
	GLfloat texCoords[] = { 0, height*heightmuliplier, width*widthmultiplier, height*heightmuliplier, width*widthmultiplier, 0, 0, height*heightmuliplier, width*widthmultiplier, 0, 0, 0 };
	glBindTexture(GL_TEXTURE_2D, textureID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glTexParameteri(GL_TEXTURE0, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE0, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(program->positionAttribute);


}
//enumeration for entity types
enum EntityType { ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_PLATFORM };

class Entity{
public:
	Entity(Vector3 position, bool isStatic = true, EntityType type = ENTITY_PLATFORM);

	void  Update(float elapsed);
	bool CollidesWith(Entity* entity);
	void collidesWithPlatform(float x, float y);
	void jump(float elapsed);
	SheetSprite sheet;
	float x;
	float y;
	float width;
	float length;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;
	float gravity;
	float friction;
	bool isStatic;
	EntityType entityType;

	const Uint8 *keys;
	bool collidedTop;
	bool collidedBottom;
	bool collidedRight;
	bool collidedLeft;

};


Entity::Entity(Vector3 position, bool isStatic, EntityType type){
	this->isStatic = isStatic;
	collidedTop = false;
	collidedBottom = false;
	collidedRight = false;
	collidedLeft = false;
	entityType = type;
	if (type == ENTITY_ENEMY){
		velocity_x = -2.0f;
	}
	else{
		velocity_x = 0.0f;
	}
	gravity = -2.0f;
	acceleration_y = 0.0f;
	velocity_y = -0.1f;
	acceleration_x = -2.0f;
	friction = 1.0f;
	x = position.x;
	y = position.y;
	keys = SDL_GetKeyboardState(NULL);
}

void Entity::Update(float elapsed){ // was here
	if (entityType == ENTITY_PLAYER){
		// move left
		if (keys[SDL_SCANCODE_LEFT]){
			if (!collidedLeft){
				if (((x - 0.25f) > -5.00f)){ // 0.25f
					if (acceleration_x > 0){
						velocity_x = -0.5f;
					}
					x += (elapsed * velocity_x);

					acceleration_x = -fabs(acceleration_x);
					velocity_x = velocity_x + (acceleration_x* elapsed);

				}
			}
		}
		// move right
		if (keys[SDL_SCANCODE_RIGHT]){
			if (!collidedRight){
				if (((x - 0.25f) < 5.00f)){ // - 0.25f
					if (acceleration_x < 0){
						velocity_x = 0.5f;
					}
					x += (elapsed * velocity_x);

					acceleration_x = fabs(acceleration_x);
					velocity_x = velocity_x + (acceleration_x* elapsed);
				}
			}
		}

		// applying friction -- constantly if in screen
		if (((x + 0.50f) < 5.00f) && ((x - 0.50f) > -5.00f)){ // -.25f, -0.25f
			if (velocity_x != 0){
				if (velocity_x > 0){
					velocity_x -= (friction*elapsed); // friction is ++
				}
				else{
					velocity_x += (friction*elapsed);
				}
			}
			x += (elapsed * velocity_x);

		}


		// gravity effect
		if (!collidedBottom){
			gravity = -2.0f;
			velocity_y += (gravity*elapsed) + (acceleration_y*elapsed);
			y += velocity_y * elapsed;
		}
		else{
			gravity = 0.0f;
		}
	}
	else if (entityType == ENTITY_ENEMY){
		if ((x)<-5.0f){

			velocity_x = fabs(velocity_x);
		}
		else if ((x)>5.0f){
			velocity_x = -fabs(velocity_x);
		}

		else if (collidedRight){
			velocity_x = -fabs(velocity_x);
		}
		else if (collidedLeft){
			velocity_x = fabs(velocity_x);
		}
		x += velocity_x * elapsed;
		if (!collidedBottom){
			velocity_y += (gravity*elapsed);
			y += velocity_y * elapsed;
		}
		else{
			velocity_y = 0.0f;
			y += velocity_y * elapsed;
		}
	}
}



bool Entity::CollidesWith(Entity* entity){
	bool collision;


	if ((x - 0.25f > entity->x - 0.5f) && (x - 0.25f < entity->x + 0.5f)){ // (y + 0.5f > yp) && (y - 0.5f < yp + 0.5f) // 0.5  == 0.25
		if (((y - 0.5f < entity->y + 0.25f) && (y + 0.5f >= entity->y + 0.25f)) || (((y + 0.5f)>entity->y - 0.25f) && (y - 0.5f <= entity->y - 0.25f))){
			collision = true;
			return collision;
		}
		else{
			collision = false;
		}
	}

	else if ((x + 0.25f > entity->x - 0.5f) && (x + 0.25f < entity->x + 0.5f)){
		if (((y - 0.5f < entity->y + 0.25f) && (y + 0.5f >= entity->y + 0.25f)) || (((y + 0.5f)>entity->y - 0.25f) && (y - .5f <= entity->y - 0.25f))){
			collision = true;
			return collision;
		}
		else{
			collision = false;
		}
	}
	else{
		collision = false;
	}
	return collision;

}



void Entity::collidesWithPlatform(float xp, float yp){
	

	if (entityType == 1){
		if ((y + 0.25f > 10.00f) && (y - 0.25f < 10.00f)){
			collidedTop = true;
			while ((y + 0.25f > 10.00f)){
				velocity_y = 0;
				y -= 0.1f;
			}
		}
		if ((x - 0.5f > xp + 3.0f) && (x - 0.5f < xp + 4.0f)){ // (y + 0.5f > yp) && (y - 0.5f < yp + 0.5f) // 0.5  == 0.25
			if (((y - 0.25f < yp + 0.4f) && (y + 0.25f >= yp + 0.4f)) || (((y + 0.25f)>yp + 0.1f) && (y - 0.25f <= yp))){
				collidedLeft = true;
			}
		}
		if ((x + 0.5f > xp) && (x + 0.5f < xp + 0.5f)){
			if (((y - 0.25f < yp + 0.4f) && (y + 0.25f >= yp + 0.5f)) || (((y + 0.25f)>yp + 0.1f) && (y - 0.25f <= yp))){
				collidedRight = true;
			}
		}
		if ((x + 0.25f > xp) && (x - 0.25f < xp + 4.0f)){ // + 0.5f
			if ((y - 0.5f< yp + 0.5f) && (y + 0.5f > yp + 0.5f)){ //y > (yp + 0.7f) &&(
				collidedBottom = true;
				while (y - 0.5f <= (yp + 0.49f) && (y + 0.5f > yp + 0.5f)){
					velocity_y = 0.0f;
					y += 0.1f;

				}

			}
		}

	}
	if (entityType == 0){
		if ((y + 0.5f > 10.00f) && (y - 0.5f < 10.00f)){
			collidedTop = true;
			while ((y + 0.5f > 10.00f)){
				velocity_y = 0;
				y -= 0.1f;
			}
		}
		if ((x - 0.25f > xp + 3.0f) && (x - 0.25f < xp + 4.0f)){ // (y + 0.5f > yp) && (y - 0.5f < yp + 0.5f) // 0.5  == 0.25
			if (((y - 0.5f < yp + 0.4f) && (y + 0.5f >= yp + 0.4f)) || (((y + 0.5f)>yp + 0.1f) && (y - .5f <= yp))){
				collidedLeft = true;
			}
		}
		if ((x + 0.25f > xp) && (x + 0.25f < xp + 0.5f)){
			if (((y - 0.5f < yp + 0.4f) && (y + 0.5f >= yp + 0.5f)) || (((y + 0.5f)>yp + 0.1f) && (y - .5f <= yp))){
				collidedRight = true;
			}
		}
		if ((x + 0.25f > xp) && (x - 0.25f < xp + 4.0f)){ // + 0.5f
			if ((y - 0.5f< yp + 0.5f) && (y + 0.5f > yp + 0.5f)){ //y > (yp + 0.7f) &&(
				collidedBottom = true;
				while (y - 0.5f <= (yp + 0.49f) && (y + 0.5f > yp + 0.5f)){
					velocity_y = 0.0f;
					y += 0.1f;

				}

			}
		}
		if ((x + 0.25f > xp) && (x - 0.25f < xp + 4.0f)){
			if ((y + 0.5f > yp) && (y - 0.5f < yp)){
				collidedTop = true;
				while ((y + 0.5f > yp)){
					velocity_y = 0;
					y -= 0.1f;
				}
			}
		}
	}

}

void Entity::jump(float elapsed){
	if (!collidedTop){
		if (collidedBottom){
			if (velocity_y < 0){
				velocity_y = 3.0f;
			}
			gravity = -2.0f;
			acceleration_y = 2.5f;
			velocity_y += (acceleration_y*elapsed) + (gravity* elapsed);
			y += velocity_y * elapsed;
		}
	}
	acceleration_y = 0.0f;
}
void drawText(ShaderProgram *program, GLuint fontTexture, std::string text, float size, float spacing, float xPos, float yPos) {
	float texture_size = 1.0f / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	Matrix modelMatrix;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size });
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size });
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	modelMatrix.identity();
	modelMatrix.Translate(xPos, yPos, 0.0f);
	program->setModelMatrix(modelMatrix);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL); // 640, 360
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	SDL_Event event;
	int state = 0;
	bool done = false;
	glViewport(0, 0, 1200, 800); //640, 360
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	/* sound */
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Music *music;
	Mix_Chunk *jumpsound;
	jumpsound = Mix_LoadWAV("beep.wav");
	music = Mix_LoadMUS("Jim James - State Of The Art (A.E.I.O.U).mp3");
	Mix_PlayMusic(music, 10);
	Matrix projectionMatrix;
	Matrix viewMatrix;
	Matrix borderModelMatrix;
	Matrix platformModelMatrix;
	Matrix playerModelMatrix;
	Matrix enemyModelMatrix;
	bool enemyPlayerCollision = false;
	float lastFrameTicks = 0.0f;
	float left = -7.50;
	float right = 7.50;
	float top = 10.5f;
	float bottom = -5.0f;
	/* platform variables*/
	float pleft = -5.50f;// platform left x value
	float pmiddle = -2.5f; // middle platform x value
	float pright = 5.50f; // right platform x value
	float plength = 4.0f; // platform length
	float pwidth = 0.5f;
	float pmbottom = bottom + 1.50f;
	float plbottom = pmbottom + 1.50f;
	float prbottom = plbottom + 1.50f;
	float pmbottom2 = prbottom + 1.50f;
	float bottom2 = pmbottom2 + 1.50f;
	float pmbottom3 = bottom2 + 1.50f;
	float bottom3 = pmbottom3 + 1.50f;
	float pmbottom4 = bottom3 + 1.50f;
	float plbottom2 = pmbottom4 + 1.50f;
	float toplayery = plbottom2 + 1.50f;
	/* palyer variables*/
	float leftx = -0.75f;
	float bottomy = -0.50f;
	float length = 1.00f;
	float width = 1.00f;
	float eleftx = -2.5f;
	float elefty = 4.0f;
	float enemyLength = 0.5f;
	float enemyWidth = 1.0f;
	projectionMatrix.setOrthoProjection(left, right, -5.0f, 5.0f, -1.0f, 1.0f); //-4.0f,4.0f
	glUseProgram(program.programID);
	/* border start*/
	float leftbordervertices[] = { left, bottom, left + 2, bottom, left + 2, top, left, bottom, left + 2, top, left, top };
	float rightbordervertices[] = { right, bottom, right - 2, bottom, right - 2, top, right, bottom, right - 2, top, right, top };

	float left2platform[] = { pleft, bottom, pleft + plength, bottom, pleft + plength, bottom + pwidth, pleft, bottom, pleft + plength, bottom + pwidth, pleft, bottom + pwidth };
	float right2platform[] = { pright - plength, bottom, pright, bottom, pright, bottom + pwidth, pright - plength, bottom, pright, bottom + pwidth, pright - plength, bottom + pwidth };
	float middleplatform[] = { pmiddle, pmbottom, pmiddle + plength, pmbottom, pmiddle + plength, pmbottom + pwidth, pmiddle, pmbottom, pmiddle + plength, pmbottom + pwidth, pmiddle, pmbottom + pwidth };
	float leftplatform[] = { pleft, plbottom, pleft + plength, plbottom, pleft + plength, plbottom + pwidth, pleft, plbottom, pleft + plength, plbottom + pwidth, pleft, plbottom + pwidth };
	float rightplatform[] = { pright - plength, prbottom, pright, prbottom, pright, prbottom + pwidth, pright - plength, prbottom, pright, prbottom + pwidth, pright - plength, prbottom + pwidth };
	float middleplatform2[] = { pmiddle, pmbottom2, pmiddle + plength, pmbottom2, pmiddle + plength, pmbottom2 + pwidth, pmiddle, pmbottom2, pmiddle + plength, pmbottom2 + pwidth, pmiddle, pmbottom2 + pwidth };
	float left2platform2[] = { pleft, bottom2, pleft + plength, bottom2, pleft + plength, bottom2 + pwidth, pleft, bottom2, pleft + plength, bottom2 + pwidth, pleft, bottom2 + pwidth };
	float right2platform2[] = { pright - plength, bottom2, pright, bottom2, pright, bottom2 + pwidth, pright - plength, bottom2, pright, bottom2 + pwidth, pright - plength, bottom2 + pwidth };
	float middleplatform3[] = { pmiddle, pmbottom3, pmiddle + plength, pmbottom3, pmiddle + plength, pmbottom3 + pwidth, pmiddle, pmbottom3, pmiddle + plength, pmbottom3 + pwidth, pmiddle, pmbottom3 + pwidth };
	float left2platform3[] = { pleft, bottom3, pleft + plength, bottom3, pleft + plength, bottom3 + pwidth, pleft, bottom3, pleft + plength, bottom3 + pwidth, pleft, bottom3 + pwidth };
	float right2platform3[] = { pright - plength, bottom3, pright, bottom3, pright, bottom3 + pwidth, pright - plength, bottom3, pright, bottom3 + pwidth, pright - plength, bottom3 + pwidth };
	float middleplatform4[] = { pmiddle, pmbottom4, pmiddle + plength, pmbottom4, pmiddle + plength, pmbottom4 + pwidth, pmiddle, pmbottom4, pmiddle + plength, pmbottom4 + pwidth, pmiddle, pmbottom4 + pwidth };
	float leftplatform2[] = { pleft, plbottom2, pleft + plength, plbottom2, pleft + plength, plbottom2 + pwidth, pleft, plbottom2, pleft + plength, plbottom2 + pwidth, pleft, plbottom2 + pwidth };
	float toplayer[] = { pleft, toplayery, right - 2, toplayery, right - 2, toplayery + pwidth, pleft, toplayery, right - 2, toplayery + pwidth, pleft, toplayery + pwidth };
	/*border end*/

	float playervertices[] = { leftx, bottomy, leftx + width, bottomy, leftx + width, bottomy + length, leftx, bottomy, leftx + width, bottomy + length, leftx, bottomy + length };
	float enemyvertices[] = { leftx, bottomy, leftx + width, bottomy, leftx + width, bottomy + enemyLength, leftx, bottomy, leftx + width, bottomy + enemyLength, leftx, bottomy + enemyLength };

	// pmbottom2
	Entity* platform = new Entity(Vector3(0.0f, 0.0f, 0.0f));
	Entity* player = new Entity(Vector3(((leftx + (width / 2))), ((bottomy + (length / 2))), 0.0f), false, ENTITY_PLAYER);
	Entity* enemy = new Entity(Vector3(((leftx + 0.5f)), ((elefty + 0.25f)), 0.0f), false, ENTITY_ENEMY);


	GLuint playerTexture = LoadTextures("Mario.png"); // add 1 or 0

	GLuint borderTexture = LoadTextures("bricktexture.png");

	GLuint enemyTexture = LoadTextures("enemyShip.png");

	GLuint fontTexture = LoadTextures("font1.png");
	platform->sheet = SheetSprite();
	player->sheet = SheetSprite();
	enemy->sheet = SheetSprite();
	platform->sheet.textureID = borderTexture;
	player->sheet.textureID = playerTexture;
	enemy->sheet.textureID = enemyTexture;
	player->width = width;
	player->length = length;
	enemy->width = width;
	enemy->length = enemyLength;
	program.setProjectionMatrix(projectionMatrix);
	enum state { GAME_STATE, END_STATE };
	state = GAME_STATE;
	// -, - , +, + -, +, +, +, -, - , + , -
	while (!done) {
		/* ticks */
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYUP){
				if (event.key.keysym.sym == SDLK_SPACE){
					Mix_PlayChannel(-1, jumpsound, 0);

					player->jump(elapsed);
				}
			}

		}
		switch (state){
		case(GAME_STATE) :

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			viewMatrix.identity();
			/* border start*/

			program.setModelMatrix(borderModelMatrix);
			borderModelMatrix.setPosition(platform->x, platform->y, 0.0f);
			platform->sheet.drawSingleTexture(&program, leftbordervertices);
			platform->sheet.drawSingleTexture(&program, rightbordervertices);
			/* platform start*/
			program.setModelMatrix(platformModelMatrix);
			platformModelMatrix.setPosition(platform->x, platform->y, 0.0f);
			platform->sheet.drawSingleTexture(&program, left2platform, 0.1f);
			platform->sheet.drawSingleTexture(&program, right2platform, 0.1f);
			platform->sheet.drawSingleTexture(&program, middleplatform, 0.1f);
			platform->sheet.drawSingleTexture(&program, leftplatform, 0.1f);
			platform->sheet.drawSingleTexture(&program, rightplatform, 0.1f);
			platform->sheet.drawSingleTexture(&program, middleplatform2, 0.1f);
			platform->sheet.drawSingleTexture(&program, left2platform2, 0.1f);
			platform->sheet.drawSingleTexture(&program, right2platform2, 0.1f);
			platform->sheet.drawSingleTexture(&program, middleplatform3, 0.1f);
			platform->sheet.drawSingleTexture(&program, left2platform3, 0.1f);
			platform->sheet.drawSingleTexture(&program, right2platform3, 0.1f);
			platform->sheet.drawSingleTexture(&program, middleplatform4, 0.1f);
			platform->sheet.drawSingleTexture(&program, leftplatform2, 0.1f);
			platform->sheet.drawSingleTexture(&program, toplayer, 0.2f);
			/* border end*/
			/* enemy*/
			program.setModelMatrix(enemyModelMatrix);

			enemy->collidedBottom = false;
			enemy->collidedLeft = false;
			enemy->collidedRight = false;
			enemy->collidedTop = false;
			enemy->collidesWithPlatform(pleft, plbottom2);
			enemy->collidesWithPlatform(pmiddle, pmbottom4);
			enemy->collidesWithPlatform(pright - plength, bottom3);
			enemy->collidesWithPlatform(pleft, bottom3);
			enemy->collidesWithPlatform(pmiddle, pmbottom3);
			enemy->collidesWithPlatform(pright - plength, bottom2);
			enemy->collidesWithPlatform(pleft, bottom2);
			enemy->collidesWithPlatform(pmiddle, pmbottom2);
			enemy->collidesWithPlatform(pright - plength, prbottom);
			enemy->collidesWithPlatform(pleft, plbottom);
			enemy->collidesWithPlatform(pmiddle, pmbottom);
			enemy->collidesWithPlatform(pright - plength, bottom);
			enemy->collidesWithPlatform(pleft, bottom);

			enemy->Update(elapsed);
			enemyModelMatrix.setPosition(enemy->x, enemy->y, 0.0f);
			enemy->sheet.drawSingleTexture(&program, enemyvertices);

			/* player */

			program.setModelMatrix(playerModelMatrix);

			player->collidedBottom = false;
			player->collidedLeft = false;
			player->collidedRight = false;
			player->collidedTop = false;
			player->collidesWithPlatform(pleft, plbottom2);
			player->collidesWithPlatform(pmiddle, pmbottom4);
			player->collidesWithPlatform(pright - plength, bottom3);
			player->collidesWithPlatform(pleft, bottom3);
			player->collidesWithPlatform(pmiddle, pmbottom3);
			player->collidesWithPlatform(pright - plength, bottom2);
			player->collidesWithPlatform(pleft, bottom2);
			player->collidesWithPlatform(pmiddle, pmbottom2);
			player->collidesWithPlatform(pright - plength, prbottom);
			player->collidesWithPlatform(pleft, plbottom);
			player->collidesWithPlatform(pmiddle, pmbottom);
			player->collidesWithPlatform(pright - plength, bottom);
			player->collidesWithPlatform(pleft, bottom);
			if (ticks > 5.0f){
				enemyPlayerCollision = player->CollidesWith(enemy);
			}
			player->Update(elapsed);
			playerModelMatrix.setPosition(player->x, player->y, 0);
			player->sheet.drawSingleTexture(&program, playervertices);

			viewMatrix.identity();
			viewMatrix.Translate(0.0f, (-player->y), 0.0f);


			program.setViewMatrix(viewMatrix);
			if (enemyPlayerCollision || player->y - 0.5f < bottom){
				OutputDebugString("collision detected\n");
				state = END_STATE;


			}
			//END ROUTINE
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			SDL_GL_SwapWindow(displayWindow);
			break;

		case(END_STATE) :
			glClear(GL_COLOR_BUFFER_BIT);

			// Main Menu
			viewMatrix.identity();
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);
			drawText(&program, fontTexture, "You Lost!!", 1.0f, 0.01f, -4.0f, 2.0f);
			drawText(&program, fontTexture, "Better luck next time", 0.50f, 0.0f, -4.5f, 0.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			SDL_GL_SwapWindow(displayWindow);
			break;
		}
	}


	Mix_FreeMusic(music);
	SDL_Quit();
	return 0;
}


