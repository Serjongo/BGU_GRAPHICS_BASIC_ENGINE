#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game::Game() : Scene()
{
}

Game::Game(float angle ,float relationWH, float near1, float far1) : Scene(angle,relationWH,near1,far1)
{ 	
}

void Game::Init()
{		
	//s:here by default, but I'm commenting this out and moving to the main func for readability
	//AddShader("../res/shaders/pickingShader");	
	//AddShader("../res/shaders/basicShader_grayscale");
	
	
	//s: changed texture to the one included in the project, and added 4 instances of it
	AddTexture("../res/textures/lena256.jpg",false); // top-left
	AddTexture("../res/textures/lena256.jpg", false); //top-right
	AddTexture("../res/textures/lena256.jpg", false); //bottom-left
	AddTexture("../res/textures/lena256.jpg", false); //bottom-right


	AddShape(Plane,-1,TRIANGLES);
	
	pickedShape = 0;
	
	//s: Setting up the 4 cameras, originally meant to point at different things, but eventually since I keep the same shape and just swap out textures I just left it as it is
	MoveCamera(0,zTranslate,10);
	MoveCamera(1, zTranslate, 10);
	MoveCamera(2, zTranslate, 10);
	MoveCamera(3, zTranslate, 10);
	pickedShape = -1;
	//pickedShape = -2;
	
	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal",Model);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
	if(shaderIndx == 0)
		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
	else 
		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	s->Unbind();
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Motion()
{
	if(isActive)
	{
	}
}

Game::~Game(void)
{
}
