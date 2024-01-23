#include "InputManager.h"
// #include "../DisplayGLFW/display.h"
#include "game.h"
#include "../res/includes/glm/glm.hpp"

int main(int argc,char *argv[])
{
	//s:changed resolution to 512x512
	const int DISPLAY_WIDTH = 512;
	const int DISPLAY_HEIGHT = 512;
	const float CAMERA_ANGLE = 0.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;

	Game *scn = new Game(CAMERA_ANGLE,(float)DISPLAY_WIDTH / DISPLAY_HEIGHT,NEAR,FAR);
	
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	Init(display);
	
	scn->Init();
	//scn2->Init();

	
	
	display.SetScene(scn);
	//display.SetScene(scn2);
	//scn->RemoveShader();
	//scn->RemoveShader();
	//scn->AddShader("../res/shaders/pickingShader");
	//scn->AddShader("../res/shaders/basicShader_grayscale");

	//common to all images
	scn->AddShader("../res/shaders/pickingShader");

	//--TOP-LEFT IMAGE
	//pushing shader for image
	scn->AddShader("../res/shaders/basicShader");
	//settting texture on shape
	scn->SetShapeTex(0, 0);
	//drawing image
	scn->Draw(1, 0, scn->BACK, true, false);
	//poppig shader back out
	scn->RemoveShader();

	//--TOP-RIGHT IMAGE
	scn->AddShader("../res/shaders/basicShader_cannyedge"); //placeholder shader
	scn->SetShapeTex(0, 1);
	scn->Draw(1, 1, scn->BACK, false, false);
	scn->RemoveShader();

	//--BOTTOM-LEFT IMAGE
	scn->AddShader("../res/shaders/basicShader_grayscale"); //placeholder shader
	scn->SetShapeTex(0, 2);
	scn->Draw(1, 2, scn->BACK, false, false);
	scn->RemoveShader();

	//--BOTTOM-RIGHT IMAGE
	scn->AddShader("../res/shaders/basicShader_floydsteinberg"); //placeholder shader
	scn->SetShapeTex(0, 3);
	scn->Draw(1, 3, scn->BACK, false, false);
	scn->RemoveShader();

	while(!display.CloseWindow())
	{
	
		//s:I draw the sceen 4 times each iteration, each time I draw a different pov




		//scn->RemoveShader();
		//scn->RemoveShader();

		scn->Motion();

		//scn2->Draw(1, 0, scn2->BACK, true, false);
		//scn2->Motion();
		display.SwapBuffers();
		display.PollEvents();	
			
	}
	delete scn;
	return 0;
}
