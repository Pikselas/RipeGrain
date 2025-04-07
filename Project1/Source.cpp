#include "../RipeGrain/SceneManager.h"
#include "../RipeGrain/MainScene.h"

extern "C" __declspec(dllexport) void* GetScene();
extern "C" __declspec(dllexport) void UnloadScene(void* scene);

class BaseScene : public RipeGrain::Scene
{
public:
	virtual ~BaseScene() = default;
public:
	template<typename T, typename... ParamsT>
	void LoadScene(ParamsT&& ... params)
	{
		GetSceneLoader()->LoadScene<T,ParamsT...>(params... , UnloadScene);
	}
};

class Snight;

class BaseS : public RipeGrain::Scene
{
private:
	RipeGrain::SceneObject object;
	RipeGrain::ObjectsLayer objects_layer;
public:
	void Initialize() override
	{
		auto s = CreateSprite(Image{ "D:/SeqDownLogo.bmp" });
		object.AddSprite(s);
		object.SetPosition(100, 100);
		objects_layer.AddObject(&object);
		AddLayer(&objects_layer);
		SetViewPortSize(718, 432);
	}
	void Update() override
	{
		Scene::Update();
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
		if (ev.event_type_index == typeid(RipeGrain::EventKeyBoardInput))
		{
			auto ev_ = RipeGrain::GetEventData<RipeGrain::EventKeyBoardInput>(ev);
			switch (ev_.key_code)
			{
			case VK_SPACE:
				LoadScene<Snight>();
				break;
			}
		}
	}
};

class Snght : public BaseScene
{
private:
	RipeGrain::SceneObject object;
	RipeGrain::ObjectsLayer objects_layer;
public:
	void Initialize() override
	{
		auto s = CreateSprite(Image{ "D:/SeqDownLogo.bmp" });
		object.AddSprite(s);
		object.SetPosition(200, 100);
		objects_layer.AddObject(&object);
		AddLayer(&objects_layer);
		SetViewPortSize(718, 432);
	}
	void Update() override
	{
		Scene::Update();
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
		if (ev.event_type_index == typeid(RipeGrain::EventKeyBoardInput))
		{
			auto ev_ = RipeGrain::GetEventData<RipeGrain::EventKeyBoardInput>(ev);
			switch (ev_.key_code)
			{
			case VK_SPACE:
				LoadScene<BaseS>();
				break;
			}
		}
	}
};

std::unique_ptr<MainScene> base_scene = std::make_unique<MainScene>();
void* GetScene()
{
	return base_scene.get();
}

void UnloadScene(void* scene)
{
	if ( dynamic_cast<MainScene*>(static_cast<RipeGrain::Scene*>(scene)) != nullptr)
	{
		OutputDebugStringA("\n\nBaseS Unload\n\n");
		base_scene.reset();
	}
}