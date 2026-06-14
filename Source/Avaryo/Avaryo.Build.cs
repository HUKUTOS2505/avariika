using UnrealBuildTool;

public class Avaryo : ModuleRules
{
	public Avaryo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Минимальный набор зависимостей
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",     // EKeys для тестовых биндов F/E/Q
			"NetCore",       // репликация
			"EngineCameras", // PerlinNoiseCameraShakePattern для тряски камеры
			"OnlineSubsystem",      // кооп-сессии (NULL/LAN сейчас, EOS/Steam через конфиг)
			"OnlineSubsystemUtils",  // хелперы сессий
			"UMG",                   // CreateWidget для экрана настроек (Easy Options) из меню
			"Niagara"                // VFX: взрыв/искры/газовое облако
		});
	}
}
