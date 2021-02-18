#include "GameScene.h"
#include <IBehaviour.h>
#include <CameraControlBehaviour.h>
#include <PhysicsBody.h>
#include <GreyscaleEffect.h>
#include <ColorCorrection.h>
#include <InstantiatingSystem.h>
#include <AssetLoader.h>
#include <Player.h>
#include <WorldBuilderV2.h>
#include "Enemy.h"
#include <Bloom.h>
#include <LightSource.h>


void MainGameScene::InitGameScene()
{
	GameScene::RegisterComponentType<Camera>();
	GameScene::RegisterComponentType<RendererComponent>();
	GameScene::RegisterComponentType<BehaviourBinding>();
	GameScene::RegisterComponentType<PhysicsBody>();

	scene = GameScene::Create("GameScene");
	Application::Instance().ActiveScene = scene;
	RenderingManager::activeScene = scene;

	Texture2D::sptr grass = Texture2D::LoadFromFile("image/grass.jpg");
	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
	Texture2D::sptr EarthEnemy = Texture2D::LoadFromFile("image/earthenemytexture.png");
	Texture2D::sptr Barrel = Texture2D::LoadFromFile("image/BARREL.png");
	TextureCubeMap::sptr environmentMap = TextureCubeMap::LoadFromImages("image/skybox/ToonSky.jpg");

	Texture2D::sptr hand = Texture2D::LoadFromFile("image/handtexture.png");

	// Creating an empty texture
	Texture2DDescription desc = Texture2DDescription();
	desc.Width = 1;
	desc.Height = 1;
	desc.Format = InternalFormat::RGB8;
	Texture2D::sptr texture2 = Texture2D::Create(desc);
	// Clear it with a white colour
	texture2->Clear();

	ShaderMaterial::sptr Floor_Mat = ShaderMaterial::Create();
	Floor_Mat->Shader = RenderingManager::NoOutline;
	Floor_Mat->Set("s_Diffuse", grass);
	Floor_Mat->Set("s_Specular", noSpec);
	Floor_Mat->Set("u_Shininess", 2.0f);
	Floor_Mat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr handMat = ShaderMaterial::Create();
	handMat->Shader = RenderingManager::BaseShader;
	handMat->Set("s_Diffuse", hand);
	handMat->Set("s_Specular", noSpec);
	handMat->Set("u_Shininess", 1.0f);
	handMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr EE_MAT = ShaderMaterial::Create();
	EE_MAT->Shader = RenderingManager::NoOutline;
	EE_MAT->Set("s_Diffuse", EarthEnemy);
	EE_MAT->Set("s_Specular", noSpec);
	EE_MAT->Set("u_Shininess", 3.0f);
	EE_MAT->Set("u_TextureMix", 0.0f);


	ShaderMaterial::sptr BarrelMat = ShaderMaterial::Create();
	BarrelMat->Shader = RenderingManager::NoOutline;
	BarrelMat->Set("s_Diffuse", Barrel);
	BarrelMat->Set("s_Specular", noSpec);
	BarrelMat->Set("u_Shininess", 3.0f);
	BarrelMat->Set("u_TextureMix", 0.0f);

	// Create an object to be our camera
	GameObject cameraObject = scene->CreateEntity("Camera");
	{
		cameraObject.get<Transform>().SetLocalPosition(0, 0, 0).LookAt(glm::vec3(0, 1, 5));
		//cameraObject.get<Transform>().setForward(glm::vec3(0, 0, -1));
		// We'll make our camera a component of the camera object
		Camera& camera = cameraObject.emplace<Camera>();// Camera::Create();
		Player& player = cameraObject.emplace<Player>();
		camera.SetPosition(glm::vec3(0, 3, 3));
		camera.SetUp(glm::vec3(0, 0, 1));
		camera.LookAt(glm::vec3(0));
		camera.SetFovDegrees(90.0f); // Set an initial FOV
		camera.SetOrthoHeight(3.0f);

		cameraObject.emplace<PhysicsBody>();
		cameraObject.get<PhysicsBody>().AddBody(1.f, btVector3(0, 0, 5), btVector3(1, 1, 2));

		BehaviourBinding::Bind<CameraControlBehaviour>(cameraObject);
	}
	GameObject obj2 = scene->CreateEntity("Hand");
	{
		obj2.get<Transform>().SetLocalPosition(1, -1, 0).SetLocalRotation(-90, 0, 0);
		obj2.get<Transform>().SetParent(cameraObject);
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/hand.obj");
		obj2.emplace<RendererComponent>().SetMesh(vao).SetMaterial(handMat);
	}



	GameObject obj3 = scene->CreateEntity("Test Enemy");
	{
		obj3.get<Transform>().SetLocalRotation(90, 0, 0);

		RendererComponent& RC =obj3.emplace<RendererComponent>();
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/Earth_En.obj");
		RC.SetMesh(vao);
		RC.SetMaterial(EE_MAT);
	

		
		PhysicsBody& p = obj3.emplace<PhysicsBody>();
		Enemy& e = obj3.emplace<Enemy>();
		p.AddBody(1.f, btVector3(0.f, 3.f, 8.f), btVector3(2.f, 2.f, 2.f));
		p.GetBody()->setUserIndex(5);
		p.GetBody()->setUserPointer((void*)&e);
		
	}

	GameObject obj4 = scene->CreateEntity("Barrel");
	{
		obj4.get<Transform>().SetLocalRotation(90, 0, 0);

		RendererComponent& RC = obj4.emplace<RendererComponent>();
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/Barrel.obj");
		RC.SetMesh(vao);
		RC.SetMaterial(BarrelMat);



		PhysicsBody& p = obj4.emplace<PhysicsBody>();
		//Enemy& e = obj4.emplace<Enemy>();
		p.AddBody(0.f, btVector3(2.f, 3.f,1.f), btVector3(2.f, 2.f, 2.f));
		//p.GetBody()->setUserIndex(5);
		//p.GetBody()->setUserPointer((void*)&e);

	}

	//skybox
	{
		ShaderMaterial::sptr skyboxMat = ShaderMaterial::Create();
		skyboxMat->Shader = RenderingManager::SkyBox;
		skyboxMat->Set("s_Environment", environmentMap);
		skyboxMat->Set("u_EnvironmentRotation", glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0))));
		skyboxMat->RenderLayer = 100;
		MeshBuilder<VertexPosNormTexCol> mesh;
		MeshFactory::AddIcoSphere(mesh, glm::vec3(0.0f), 1.0f);
		MeshFactory::InvertFaces(mesh);
		VertexArrayObject::sptr meshVao = mesh.Bake();

		GameObject skyboxObj = scene->CreateEntity("skybox");
		skyboxObj.get<Transform>().SetLocalPosition(0.0f, 0.0f, 0.0f);
		skyboxObj.get_or_emplace<RendererComponent>().SetMesh(meshVao).SetMaterial(skyboxMat);
	}




	//WorldBuilderV2 builder;
	//builder.BuildNewWorld();
	InstantiatingSystem::LoadPrefabFromFile(glm::vec3(0, 0, 0), "node/Blank_Floor_Tile.node");

	GameObject temp = InstantiatingSystem::InstantiateEmpty("Barrel");
	
	temp.get<Transform>().SetLocalPosition(1,1,1);
	temp.emplace<LightSource>();

	GameObject temp2 = InstantiatingSystem::InstantiateEmpty("Barrel");
	
	temp2.get<Transform>().SetLocalPosition(10, 4, 1);
	temp2.emplace<LightSource>();

	
}