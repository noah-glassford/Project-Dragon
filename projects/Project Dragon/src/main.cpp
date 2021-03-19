#include "GameScene.h"
#include "PhysicsSystem.h"
#include "Timer.h"
#include <AssetLoader.h>
#include <MainMenu.h>
#include <DeathScreen.h>
#include <ft2build.h>
#include FT_FREETYPE_H
int main() 
{ 
	BackendHandler::InitAll();
	PhysicsSystem::Init();
	AssetLoader::Init();

	BackendHandler::m_Scenes.push_back(new MainMenuScene());
	BackendHandler::m_Scenes.push_back(new MainGameScene());
	BackendHandler::m_Scenes.push_back(new DeathSceen());
	BackendHandler::m_Scenes[0]->InitGameScene();
	BackendHandler::m_ActiveScene = 0;

	

	///// Game loop /////
	while (!glfwWindowShouldClose(BackendHandler::window)) {
		glfwPollEvents();
		Timer::Tick();
		BackendHandler::UpdateInput();
		BackendHandler::UpdateAudio();
		PhysicsSystem::Update();
		
		RenderingManager::Render();
	
		
		
	
		
	}
	BackendHandler::ShutdownImGui();
	
	return 0; 
} 
