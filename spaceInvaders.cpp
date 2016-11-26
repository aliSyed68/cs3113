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
GLuint LoadTextures(const char* image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface) {
		printf("IMG_Load: %s\n", IMG_GetError());
		// handle error
	}
	GLuint textureID;
	glActiveTexture(GL_TEXTURE0);
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
	size = NULL;
	textureID = NULL;
	u = NULL;
	v = NULL;
	width = NULL;
	height = NULL;
	aspect = NULL;
}
SheetSprite::SheetSprite(GLuint textureID, int index, int spriteCountX, int spriteCountY, float size){
	this->textureID = textureID;
	this->u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	this->v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
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
	float vertices[] = { (-0.5f)*size*aspect, (-0.5f)*size, (0.5f)*size*aspect, (0.5f)*size, (-0.5f)*size*aspect, (0.5f)*size, (0.5)*size*aspect, (0.5f)*size, (-0.5f)*size*aspect, (-0.5f)*size, (0.5f)*size*aspect, (-0.5f)*size };
	glActiveTexture(GL_TEXTURE0);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(program->positionAttribute);
}

// Entity class
class Entity{
public:
	Entity(Vector3& position, Vector3& speed, float rotation, SheetSprite& sheet, float direction = 1.0f);
	Entity();
	virtual void Update(float elapsed, float right, float left, std::vector<Entity*>& v, int index = 0);
	virtual void Update(float elapsed);
	virtual void Update(float elapsed, float right, float left);
	Vector3 position;
	Vector3 speed;
	float direction;
	float rotation;
	SheetSprite sprite;
	bool active;
};

// Entity constructor

Entity::Entity(Vector3& position, Vector3& speed = Vector3(), float rotation = float(), SheetSprite& sheet = SheetSprite(), float direction){
	this->position = position;
	this->speed = speed;
	this->rotation = rotation;
	sprite = sheet;
	this->direction = direction;
	this->active = false;
}
Entity::Entity(){
	this->position = Vector3();
	this->speed = Vector3();
	this->rotation = float();
	sprite = SheetSprite();
	this->direction = 1.0f;
	this->active = false;
}
// Entity update
void Entity::Update(float elapsed, float right, float left, std::vector<Entity*>& v, int index){

}

void Entity::Update(float elapsed, float right, float left){

}
void Entity::Update(float elapsed){
	position.y += (elapsed * speed.y * direction);
}

class Enemy : public Entity{
public:
	Enemy(Vector3& position, Vector3& speed, float rotation = float(), SheetSprite& sheet = SheetSprite(), float xdirection = 1.0f) :Entity(position, speed, rotation, sheet, xdirection)
	{
		eBulletIndex = 0;
		for (int i = 0; i < 1000; i++){
			eBullets[i] = new Entity();
		}

	}

	Entity* eBullets[1000];
	int eBulletIndex;
	void shootEnemyBullet(float elapsed);
	// incorporate direction vector
	void Update(float elapsed, float right, float left, std::vector<Entity*>& v, int index){
		if (index == v.size() - 1 || index == 0){
			if ((position.x) <= left){
				fabs(speed.x);
				for (int i = 0; i < v.size(); i++){
					static_cast<Enemy*>(v[i])->direction = fabs(direction);
				}
			}
			else if ((position.x) >= right)
			{
				fabs(speed.x);
				for (int i = 0; i < v.size(); i++){
					static_cast<Enemy*>(v[i])->direction = -fabs(direction);
				}
			}
		}
		position.x += (speed.x * direction * elapsed);

	}
};

void Enemy::shootEnemyBullet(float elapsed){
	eBullets[eBulletIndex]->position.x = this->position.x;
	eBullets[eBulletIndex]->position.y = this->position.y;
	eBullets[eBulletIndex]->speed.y = 2.0f;
	eBullets[eBulletIndex]->direction = -1.0f;
	eBullets[eBulletIndex]->active = true;
	eBullets[eBulletIndex]->Update(elapsed);
	eBulletIndex++;
	if (eBulletIndex > (1000 - 1)){
		eBulletIndex = 0;
	}
}
class BattleShip : public Entity{

public:
	BattleShip(Vector3& position, Vector3& speed, Vector3& size = Vector3(), float rotation = float(), SheetSprite& sheet = SheetSprite()) : Entity(speed, position, rotation, sheet), units_a_second(6.0f){
		keys = SDL_GetKeyboardState(NULL);
		bulletIndex = 0;
		for (int i = 0; i < 1000; i++){
			bullets[i] = new Entity(Vector3(0.0f, 10.0f, 0.0f));
		}
	}
	BattleShip() :Entity(Vector3(), Vector3(), float(), SheetSprite()), units_a_second(6.0f){
		keys = SDL_GetKeyboardState(NULL);
		bulletIndex = 0;
		for (int i = 0; i < 1000; i++){
			bullets[i] = new Entity(Vector3(0.0f, 10.0f, 0.0f));
		}
	}
	float units_a_second;
	const Uint8 *keys;
	Entity* bullets[1000];
	void shootBullet();
	int bulletIndex;
	void Update(float elapsed, float right, float left){

		if (keys[SDL_SCANCODE_LEFT]){
			if ((position.x - 0.5f) > left){
				position.x -= elapsed * units_a_second;
			}
		}
		if (keys[SDL_SCANCODE_RIGHT]){
			if ((position.x + 0.5f) < right){
				position.x += elapsed * units_a_second;
			}
		}
	}
};


void BattleShip::shootBullet(){
	bullets[bulletIndex]->position.x = this->position.x;
	bullets[bulletIndex]->position.y = -2.0f;
	bullets[bulletIndex]->speed.y = 2.0f;
	bullets[bulletIndex]->active = true;
	bulletIndex++;
	if (bulletIndex > (1000 - 1)){
		bulletIndex = 0;
	}
	//OutputDebugString("shoot bullet\n");
}

// check for bullet collision
bool bulletCollision(float bulletpositionX, float bulletPositionY, float entityPosX, float entityPosY){
	if ((bulletpositionX < entityPosX + 0.5f) && (bulletpositionX > entityPosX - 0.5f)){  // check if bullet is in range on x axis
		if ((bulletPositionY < entityPosY + 0.5f) && (bulletPositionY > entityPosY - 0.5f)){ // now check if bullet has collided with object
			return true;
		}
		else{
			return false;
		}
	}
	else{ return false; }
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
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	SDL_Event event;
	int state = 0;
	bool done = false;
	glViewport(0, 0, 640, 360);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	Matrix projectionMatrix;
	Matrix viewMatrix;
	Matrix BattleShipModelMatrix;
	Matrix enemyMatrix;
	Matrix bulletMatrix;
	Matrix eBulletMatrix;
	float enemyTime = 0.0f;
	float yMovement = 0.0f;
	float lastFrameTicks = 0.0f;
	float left = -7.55;
	float right = 7.55;
	float enemyShootingInterval = 0.0f;
	projectionMatrix.setOrthoProjection(left, right, -5.0f, 5.0f, -4.0f, 4.0f);
	glUseProgram(program.programID);
	/**/
	//vector of Entities
	std::vector<Entity*> enemyLineOne;
	std::vector<Entity*> enemyLineTwo;
	std::vector<Entity*> enemyLineThree;
	/**/
	//defender
	BattleShip* defender = new BattleShip();
	GLuint fontTexture = LoadTextures("font1.png");
	GLuint spriteSheetTexture = LoadTextures("characters_3.png");

	int defenderindex = 10;
	int spriteCountX = 8;
	int spriteCountY = 4;
	float defendersize = 2.0f;
	defender->sprite = SheetSprite(spriteSheetTexture, defenderindex, spriteCountX, spriteCountY, defendersize); //number parameters --- size used 0.2 from slides 0.453125, 0.0, 0.40625, 0.46875,
	defender->position.y = -4.0f;
	/**/
	// loop to add enemies
	int enemyindex = 0;
	float enemysize = 1.0f;
	Vector3 enemySpeed = Vector3(1.0f, 0.0f, 0.0f);
	/**/
	// adding enemies to vectors
	for (float y = -1; y <= 3; y += 2){
		for (float i = -3; i <= 3; i += 2){
			if (y == -1){
				Vector3 enemyPosition = Vector3(float(i), float(y), 0.0f);
				Enemy* enemy = new Enemy(enemyPosition, enemySpeed);
				enemy->sprite = SheetSprite(spriteSheetTexture, enemyindex, spriteCountX, spriteCountY, enemysize);
				enemyLineOne.push_back(enemy);
			}
			if (y == 1){
				Vector3 enemyPosition1 = Vector3(float(i), float(y), 0.0f);
				Enemy* enemy = new Enemy(enemyPosition1, enemySpeed);
				enemy->sprite = SheetSprite(spriteSheetTexture, enemyindex, spriteCountX, spriteCountY, enemysize);
				enemyLineTwo.push_back(enemy);
			}
			if (y == 3){
				Vector3 enemyPosition2 = Vector3(float(i), float(y), 0.0f);
				Enemy* enemy = new Enemy(enemyPosition2, enemySpeed);
				enemy->sprite = SheetSprite(spriteSheetTexture, enemyindex, spriteCountX, spriteCountY, enemysize);
				enemyLineThree.push_back(enemy);
			}
		}
	}
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYUP){
				if (event.key.keysym.sym == SDLK_SPACE){
					defender->shootBullet();
				}
				if (event.key.keysym.sym == SDLK_0){
					state = 1;
				}
			}
		}
		if (state == 0){
			glClear(GL_COLOR_BUFFER_BIT);

			// Main Menu


			drawText(&program, fontTexture, "Space Invaders", 1.0f, 0.01f, -6.5f, 2.0f);
			drawText(&program, fontTexture, "Press the '0' key", 0.75f, 0.0f, -5.5f, 0.0f);
			drawText(&program, fontTexture, " to start", 0.75f, 0.0f, -3.0f, -1.0f);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);

			//OutputDebugString("title screen\n");
		}
		else if (state == 1){
			glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;
			glClearColor(0.0f, 1.0f, 1.0f, 1.0f); //0.4f, 0.2f, 0.4f, 1.0f
			glClear(GL_COLOR_BUFFER_BIT);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);
			float fixedElapsed = elapsed;
			/**/
			// drawing battleship
			program.setModelMatrix(BattleShipModelMatrix);
			BattleShipModelMatrix.identity();
			defender->Update(fixedElapsed, right, left);
			BattleShipModelMatrix.setPosition(defender->position.x, defender->position.y, defender->position.z);
			defender->sprite.Draw(&program);
			/**/
			//drawing battleship bullets
			program.setModelMatrix(bulletMatrix);
			for (int i = 0; i < 1000; i++){
				for (int u = 0; u < enemyLineOne.size(); u++){
					if (bulletCollision(defender->bullets[i]->position.x, defender->bullets[i]->position.y, enemyLineOne[u]->position.x, enemyLineOne[u]->position.y)){
						//OutputDebugString("enemy 1 got hit \n");
						defender->bullets[i]->position.y = 10.0f;
						defender->bullets[i]->active = false;
						delete enemyLineOne[u];
						enemyLineOne.erase(enemyLineOne.begin() + u);
					}
				}
				for (int u = 0; u < enemyLineTwo.size(); u++){
					if (bulletCollision(defender->bullets[i]->position.x, defender->bullets[i]->position.y, enemyLineTwo[u]->position.x, enemyLineTwo[u]->position.y)){
						//OutputDebugString("enemy 2 got hit \n");
						defender->bullets[i]->position.y = 10.0f;
						defender->bullets[i]->active = false;
						delete enemyLineTwo[u];
						enemyLineTwo.erase(enemyLineTwo.begin() + u);
					}
				}
				for (int u = 0; u < enemyLineThree.size(); u++){
					if (bulletCollision(defender->bullets[i]->position.x, defender->bullets[i]->position.y, enemyLineThree[u]->position.x, enemyLineThree[u]->position.y)){
						//OutputDebugString("enemy 3 got hit \n");
						defender->bullets[i]->position.y = 10.0f;
						defender->bullets[i]->active = false;
						delete enemyLineThree[u];
						enemyLineThree.erase(enemyLineThree.begin() + u);
					}
				}
				bulletMatrix.identity();
				defender->bullets[i]->Update(fixedElapsed);
				bulletMatrix.setPosition(defender->bullets[i]->position.x, defender->bullets[i]->position.y, defender->bullets[i]->position.z);
				program.setModelMatrix(bulletMatrix);
				float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
				/**/
				defender->bullets[i]->sprite = SheetSprite(spriteSheetTexture, 25, spriteCountX, spriteCountY, 0.5);
				defender->bullets[i]->sprite.Draw(&program);
			}

			/**/
			// draw enemies
			enemyTime += elapsed;;
			for (int i = 0; i < enemyLineOne.size(); i++) {
				if (enemyLineOne[i] != nullptr){
					program.setModelMatrix(enemyMatrix);
					enemyMatrix.identity();
					enemyLineOne[i]->Update(fixedElapsed, right, left, enemyLineOne, i);  // right, left
					if (enemyTime > 10){
						enemyLineOne[i]->position.y -= 1.0f;
						enemyLineOne[i]->speed.x += 2.0f;
					}
					enemyMatrix.setPosition(enemyLineOne[i]->position.x, enemyLineOne[i]->position.y, enemyLineOne[i]->position.z);

					/**/
					if (bulletCollision(enemyLineOne[i]->position.x, enemyLineOne[i]->position.y, defender->position.x, defender->position.y)){
						//OutputDebugString("enemy - defender collision \n");
						state = 3;
						break;
					}
					enemyLineOne[i]->sprite.Draw(&program);
				}

			}

			for (int i = 0; i < enemyLineTwo.size(); i++){
				program.setModelMatrix(enemyMatrix);
				enemyMatrix.identity();
				enemyLineTwo[i]->Update(fixedElapsed, right, left, enemyLineTwo, i);  // right, left
				if (enemyTime > 10){
					enemyLineTwo[i]->position.y -= 1.0f;
					enemyLineTwo[i]->speed.x += 2.0f;


				}
				enemyMatrix.setPosition(enemyLineTwo[i]->position.x, enemyLineTwo[i]->position.y, enemyLineTwo[i]->position.z);
				enemyMatrix.Translate(0.0f, yMovement, 0.0f);
				enemyLineTwo[i]->sprite.Draw(&program);
			}

			for (int i = 0; i < enemyLineThree.size(); i++){
				program.setModelMatrix(enemyMatrix);
				enemyMatrix.identity();
				enemyLineThree[i]->Update(fixedElapsed, right, left, enemyLineThree, i);  // right, left
				if (enemyTime > 10){
					enemyLineThree[i]->position.y -= 1.0f;
					enemyLineThree[i]->speed.x += 2.0f;
					if (i == enemyLineThree.size() - 1){
						enemyTime = 0.0f;
					}
				}
				enemyMatrix.setPosition(enemyLineThree[i]->position.x, enemyLineThree[i]->position.y, enemyLineThree[i]->position.z);
				enemyLineThree[i]->sprite.Draw(&program);
			}

			// fire an enemy bullet from first line
			enemyShootingInterval += elapsed;
			program.setModelMatrix(eBulletMatrix);
			if (enemyShootingInterval > 3.0f){
				if (enemyLineOne.size() != 0){
					int random = rand() % enemyLineOne.size();
					Enemy* enemyShooter = dynamic_cast<Enemy*> (enemyLineOne[random]);
					int currentEBullet = (enemyShooter->eBulletIndex);
					enemyShooter->shootEnemyBullet(fixedElapsed);
					eBulletMatrix.identity();
					program.setModelMatrix(eBulletMatrix);
					eBulletMatrix.setPosition(enemyShooter->eBullets[currentEBullet]->position.x, enemyShooter->eBullets[currentEBullet]->position.y, enemyShooter->eBullets[currentEBullet]->position.z);
					float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					enemyShootingInterval = 0;
				}
			}

			//loop over enemyLineOne and draw every enemy's bullets from first line
			for (int u = 0; u < enemyLineOne.size(); u++){
				Enemy* enemyShooter = dynamic_cast<Enemy*> (enemyLineOne[u]);

				for (int i = 0; i < 1000; i++){
					if (enemyShooter->eBullets[i]->active){
						eBulletMatrix.Translate(0.0f, yMovement, 0.0f);
						program.setModelMatrix(eBulletMatrix);
						enemyShooter->eBullets[i]->Update(fixedElapsed);

						eBulletMatrix.setPosition(enemyShooter->eBullets[i]->position.x, enemyShooter->eBullets[i]->position.y, enemyShooter->eBullets[i]->position.z);

						float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
						enemyShooter->eBullets[i]->sprite = SheetSprite(spriteSheetTexture, 25, spriteCountX, spriteCountY, 0.5);
						// enemy bullet collision with Battleship
						if (bulletCollision(enemyShooter->eBullets[i]->position.x, enemyShooter->eBullets[i]->position.y, defender->position.x, defender->position.y)){
							//OutputDebugString("defender got hit \n");
							state = 3;
							u = enemyLineOne.size(); // breaking out of nested loop
							break;

						}
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);
						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						enemyShooter->eBullets[i]->sprite.Draw(&program);
					}
				}
			}
			//  fire enemy bullet from second line
			if (enemyLineOne.size() == 0){
				program.setModelMatrix(eBulletMatrix);
				if (enemyShootingInterval > 2.0f){
					if (enemyLineTwo.size() != 0){
						int random = rand() % enemyLineTwo.size();
						Enemy* enemyShooter = dynamic_cast<Enemy*> (enemyLineTwo[random]);
						int currentEBullet = (enemyShooter->eBulletIndex);
						enemyShooter->shootEnemyBullet(fixedElapsed);
						eBulletMatrix.identity();
						program.setModelMatrix(eBulletMatrix);
						eBulletMatrix.setPosition(enemyShooter->eBullets[currentEBullet]->position.x, enemyShooter->eBullets[currentEBullet]->position.y, enemyShooter->eBullets[currentEBullet]->position.z);
						float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);
						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						enemyShootingInterval = 0;
					}
				}

				//loop over enemyLineOne and draw every enemy's bullets from second line
				for (int u = 0; u < enemyLineTwo.size(); u++){
					Enemy* enemyShooter = dynamic_cast<Enemy*> (enemyLineTwo[u]);
					for (int i = 0; i < 1000; i++){
						if (enemyShooter->eBullets[i]->active){
							eBulletMatrix.Translate(0.0f, yMovement, 0.0f);
							program.setModelMatrix(eBulletMatrix);
							enemyShooter->eBullets[i]->Update(fixedElapsed);
							eBulletMatrix.setPosition(enemyShooter->eBullets[i]->position.x, enemyShooter->eBullets[i]->position.y, enemyShooter->eBullets[i]->position.z);
							float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
							enemyShooter->eBullets[i]->sprite = SheetSprite(spriteSheetTexture, 25, spriteCountX, spriteCountY, 0.5);
							// enemy bullet collision with Battleship
							if (bulletCollision(enemyShooter->eBullets[i]->position.x, enemyShooter->eBullets[i]->position.y, defender->position.x, defender->position.y)){
								//OutputDebugString("defender got hit \n");

								state = 3;
								u = enemyLineTwo.size(); // breaking out of nested loop
								break;
							}
							glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
							glEnableVertexAttribArray(program.positionAttribute);
							glDrawArrays(GL_TRIANGLES, 0, 6);
							glDisableVertexAttribArray(program.positionAttribute);
							enemyShooter->eBullets[i]->sprite.Draw(&program);
						}
					}
				}
			}
			if (enemyLineTwo.size() == 0){
				// fire enemy bullet from third line
				program.setModelMatrix(eBulletMatrix);
				if (enemyShootingInterval > 1.0f){
					if (enemyLineThree.size() != 0){
						int random = rand() % enemyLineThree.size();
						Enemy* enemyShooter = dynamic_cast<Enemy*> (enemyLineThree[random]);
						int currentEBullet = (enemyShooter->eBulletIndex);
						enemyShooter->shootEnemyBullet(fixedElapsed);
						eBulletMatrix.identity();
						program.setModelMatrix(eBulletMatrix);
						eBulletMatrix.setPosition(enemyShooter->eBullets[currentEBullet]->position.x, enemyShooter->eBullets[currentEBullet]->position.y, enemyShooter->eBullets[currentEBullet]->position.z);
						float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);
						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						enemyShootingInterval = 0;
					}
				}

				//loop over enemyLineOne and draw every enemy's bullets from third line
				for (int u = 0; u < enemyLineThree.size(); u++){
					Enemy* enemyShooter = dynamic_cast<Enemy*> (enemyLineThree[u]);

					for (int i = 0; i < 1000; i++){
						if (enemyShooter->eBullets[i]->active){
							eBulletMatrix.Translate(0.0f, yMovement, 0.0f);
							program.setModelMatrix(eBulletMatrix);
							enemyShooter->eBullets[i]->Update(fixedElapsed);
							eBulletMatrix.setPosition(enemyShooter->eBullets[i]->position.x, enemyShooter->eBullets[i]->position.y, enemyShooter->eBullets[i]->position.z);
							float vertices[] = { -0.125f, -0.125f, 0.125f, -0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f };
							enemyShooter->eBullets[i]->sprite = SheetSprite(spriteSheetTexture, 25, spriteCountX, spriteCountY, 0.5);
							// enemy bullet collision with Battleship
							if (bulletCollision(enemyShooter->eBullets[i]->position.x, enemyShooter->eBullets[i]->position.y, defender->position.x, defender->position.y)){
								//OutputDebugString("defender got hit \n");
								state = 3;
								u = enemyLineThree.size(); // breaking out of nested loop
								break;
							}

							glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
							glEnableVertexAttribArray(program.positionAttribute);
							glDrawArrays(GL_TRIANGLES, 0, 6);
							glDisableVertexAttribArray(program.positionAttribute);
							enemyShooter->eBullets[i]->sprite.Draw(&program);
						}
					}
				}
			}
			// check if any enemies alie
			if (enemyLineOne.size() == 0 && enemyLineTwo.size() == 0 && enemyLineThree.size() == 0){
				state = 2;
			}
		}

		else if (state == 2){

			// Victory game state
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			drawText(&program, fontTexture, "You Win!", 1.0f, 0.0f, -4.0f, 0.5f);
		}
		else if (state == 3){
			// Game over state
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			drawText(&program, fontTexture, "Game Over", 1.0f, 0.0f, -4.0f, 1.5f);
			drawText(&program, fontTexture, "You Lose", 1.0f, 0.0f, -3.5f, 0.0f);

		}

		//END ROUTINE
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SDL_GL_SwapWindow(displayWindow);
	}

	// deleting entities

	for (int i = 0; i < 1000; i++){
		if (defender != NULL){
			if (defender->bullets[i] != NULL){
				delete defender->bullets[i];
			}
		}
	}
	/**/
	//deleting enemy bullets
	for (int i = 0; i < enemyLineOne.size(); i++){
		if (enemyLineOne[i] != NULL){
			Enemy* pointer = dynamic_cast<Enemy*>(enemyLineOne[i]);
			for (int u = 0; u < 1000; u++){
				if (pointer->eBullets[u] != NULL){
					delete pointer->eBullets[u];
				}
			}
		}

		if (enemyLineOne[i] != NULL){
			delete enemyLineOne[i];
		}

	}


	for (int i = 0; i < enemyLineTwo.size(); i++){
		if (enemyLineTwo[i] != NULL){
			Enemy* pointer = dynamic_cast<Enemy*>(enemyLineTwo[i]);
			for (int u = 0; u < 1000; u++){
				if (pointer->eBullets[u] != NULL){
					delete pointer->eBullets[u];
				}
			}
		}

		if (enemyLineTwo[i] != NULL){
			delete enemyLineTwo[i];
		}
	}
	for (int i = 0; i < enemyLineThree.size(); i++){
		if (enemyLineThree[i] != NULL){
			Enemy* pointer = dynamic_cast<Enemy*>(enemyLineThree[i]);
			for (int u = 0; u < 1000; u++){
				if (pointer->eBullets[u] != NULL){
					delete pointer->eBullets[u];
				}
			}
		}

		if (enemyLineThree[i] != NULL){
			delete enemyLineThree[i];
		}
	}
	if (defender){
		delete defender;
	}
	SDL_Quit();
	return 0;
}


