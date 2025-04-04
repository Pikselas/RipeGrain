## TOY Framework for creating 2D games
### Createing a Scene
> Refer [MainScene.h](RipeGrain/MainScene.h)
```C++
#include "Audio.h"
#include "BoxCollider.h"
#include "SceneManager.h"

class DemoScene final: public RipeGrain::Scene
{
private:
	RipeGrain::SceneObject hero;
	RipeGrain::ObjectsLayer objects_layer;
private:
	std::unique_ptr<RipeGrain::Audio> sound;
public:
	void Initialize() override
	{
		// executed once when the scene is loaded

		// creating the hero
		hero.AddSprite(CreateSprite(Image{ "hero/location" }));
		hero.SetPosition(100 , 100);

		// loading the sound effect (WAVEFORM FILE FORMAT ONLY)
		sound = std::make_unique<RipeGrain::Audio>("path/to/sound.wav");

		// adding the game objects to a layer 
		objects_layer.AddObject(&poster);

		// adding layers (layers are rendered based on the orders they are added)
		AddLayer(&objects_layer);

		//setting the window size
		SetViewPortSize(718, 432);
	}
	void Update() override
	{
		// executed in each frame
		Scene::Update();
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
		// executed when events are available
		// checking for keyboard events
		if (ev.event_type_index == typeid(RipeGrain::EventKeyBoardInput))
		{
			// switching on SPACEBAR
			switch (RipeGrain::GetEventData<RipeGrain::EventKeyBoardInput>(ev).key_code)
			{
			case VK_SPACE:
				hero.SetX(hero.GetX() + 10);
				hero.SetY(hero.GetY() + 10);

				// playing the audio
				RegisterEvent(RipeGrain::CreatePlayBackEvent(*sound));
				break;
			}
		}
	}
};

```
### Engine configuration 
> Edit [main.cpp](RipeGrain/main.cpp)
```C++
engine.ConfigureWith<RipeGrain::UISystem>();
engine.ConfigureWith<RipeGrain::AudioSystem>();
engine.ConfigureWith<RipeGrain::PhysicsSystem>();
engine.ConfigureWith<RipeGrain::InputSystem>(window.mouse);
engine.ConfigureWith<RipeGrain::SceneManager>(render_engine);
engine.ConfigureWith<RipeGrain::InputSystem>(window.keyboard);
engine.ConfigureWith<RipeGrain::RenderSystem>(render_engine , window);

// loading the scene
engine.ConfigureWith<RipeGrain::SceneLoader>().LoadScene<DemoScene>();

while (window.IsOpen())
{
	engine.Run();
	Window::DispatchWindowEventsNonBlocking();
}
