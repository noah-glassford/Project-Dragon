#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <Scene.h>
#include <Transform.h>
static class InstantiatingSystem
{
public:


	//to load a prefab from a file
	static void LoadPrefabFromFile(int index, glm::vec3 origin, std::string filename);

private:
};