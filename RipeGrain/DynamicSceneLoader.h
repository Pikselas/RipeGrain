#pragma once
class DynamicSceneLoader
{
private:
	HMODULE lib = nullptr;
	std::function<void(RipeGrain::Scene*)> unload_scene_fptr = nullptr;
	RipeGrain::Scene* scene_data = nullptr;
private:
	static DynamicSceneLoader instance;
public:
	static void Unload()
	{
		if (instance.lib)
		{
			if (instance.scene_data)
				DeleteScene(instance.scene_data);
			FreeLibrary(instance.lib);
		}
	}
public:
	static void Load(std::filesystem::path path)
	{
		if (instance.lib)
		{
			FreeLibrary(instance.lib);
			instance.unload_scene_fptr = nullptr;
		}

		if (instance.lib = LoadLibrary(path.string().c_str()))
		{
			auto create_scene_fptr = (RipeGrain::Scene * (*)())GetProcAddress(instance.lib, "GetScene");
			instance.unload_scene_fptr = (void(*)(RipeGrain::Scene*))GetProcAddress(instance.lib, "UnloadScene");

			if (create_scene_fptr && instance.unload_scene_fptr)
			{
				instance.scene_data = create_scene_fptr();
			}
			else
			{
				MessageBoxA(nullptr, "Failed to load", "Error", MB_OK);
				return;
			}
		}
		else
		{
			MessageBoxA(nullptr, "Failed to load", "Error", MB_OK);
			return;
		}
	}
	static RipeGrain::Scene* GetScene()
	{
		if (instance.lib && instance.scene_data)
			return instance.scene_data;
		else
			return nullptr;
	}
	static void DeleteScene(RipeGrain::Scene* scene)
	{
		if (!instance.lib || !scene)
			return;
		if (scene == instance.scene_data)
		{
			instance.unload_scene_fptr(instance.scene_data);
		}
		else
		{
			MessageBox(nullptr, "Failed to delete", "Error", MB_OK);
			return;
		}
		instance.scene_data = nullptr;
	}
};

DynamicSceneLoader DynamicSceneLoader::instance;