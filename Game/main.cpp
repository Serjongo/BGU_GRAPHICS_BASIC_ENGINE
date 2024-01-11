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

	Game *scn = new Game(CAMERA_ANGLE,(float)DISPLAY_WIDTH/DISPLAY_HEIGHT,NEAR,FAR);
	Game* scn2 = new Game(45.0f, (float)DISPLAY_WIDTH / DISPLAY_HEIGHT, NEAR, FAR);
	
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	Init(display);
	
	scn->Init();
	scn2->Init();

	
	
	display.SetScene(scn);
	display.SetScene(scn2);

	while(!display.CloseWindow())
	{
		
		scn->Draw(1,0,scn->BACK,true,false);

		scn2->Draw(1, 0, scn->BACK, true, false);


		scn->Motion();

		//scn2->Draw(1, 0, scn2->BACK, true, false);
		//scn2->Motion();
		display.SwapBuffers();
		display.PollEvents();	
			
	}
	delete scn;
	return 0;
}
