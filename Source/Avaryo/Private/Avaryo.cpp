#include "GameDelegates.h"
#include "Modules/ModuleManager.h"

void RegisterAvariikaStringTable();
void UnregisterAvariikaStringTable();

class FAvaryoGameModule final : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FDefaultGameModuleImpl::StartupModule();
		RegisterAvariikaStringTable();
		CookModificationHandle = FGameDelegates::Get().GetModifyCookDelegate().AddRaw(
			this, &FAvaryoGameModule::ModifyCook);
	}

	virtual void ShutdownModule() override
	{
		FGameDelegates::Get().GetModifyCookDelegate().Remove(CookModificationHandle);
		UnregisterAvariikaStringTable();
		FDefaultGameModuleImpl::ShutdownModule();
	}

private:
	void ModifyCook(TConstArrayView<const ITargetPlatform*> TargetPlatforms,
		TArray<FName>& PackagesToCook, TArray<FName>& PackagesToNeverCook)
	{
		PackagesToCook.AddUnique(
			TEXT("/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2"));
		PackagesToCook.AddUnique(
			TEXT("/Game/Avariika/UI/CharacterCustomization/M_PreviewWorker_UI"));
	}

	FDelegateHandle CookModificationHandle;
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAvaryoGameModule, Avaryo, "Avaryo");
