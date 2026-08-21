// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Utilities/GraphMinimapStyle.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#if UE_5_00_OR_LATER
#include "Styling/SlateStyleMacros.h"
#include "Styling/CoreStyle.h"
#endif
#include "Textures/SlateIcon.h"
#include "Misc/Paths.h"

namespace GraphMinimap
{
#if !UE_5_00_OR_LATER
	namespace CoreStyleConstants
	{
		static const FVector2D Icon64x64(64.0f, 64.0f);
	}
#endif
	
	FGraphMinimapStyle::FGraphMinimapStyle()
		: FSlateStyleSet(TEXT("GraphMinimapStyle"))
	{
	}

#if !UE_5_00_OR_LATER
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#endif

	void FGraphMinimapStyle::RegisterInternal()
	{
		SetCoreContentRoot(FPaths::EngineContentDir());
		{
			FString StyleContentRoot;
			{
				const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(Global::PluginName.ToString());
				check(Plugin.IsValid());
				StyleContentRoot = FPaths::ConvertRelativePathToFull(
					Plugin->GetBaseDir() / TEXT("Resources") / TEXT("Icons")
				);
			}
			SetContentRoot(StyleContentRoot);
		}

		Set(
			GetPropertyNameFromIconType(EGraphMinimapStyleIconType::Resizable),
			new IMAGE_BRUSH("Resizable64", CoreStyleConstants::Icon64x64)
		);
		Set(
			GetPropertyNameFromIconType(EGraphMinimapStyleIconType::Visible),
			new IMAGE_BRUSH("Visible64", CoreStyleConstants::Icon64x64)
		);
		Set(
			GetPropertyNameFromIconType(EGraphMinimapStyleIconType::Controllable),
			new IMAGE_BRUSH("Controllable64", CoreStyleConstants::Icon64x64)
		);
	}

#if !UE_5_00_OR_LATER
#undef IMAGE_BRUSH
#endif
	
	void FGraphMinimapStyle::Register()
	{
		Instance = MakeUnique<FGraphMinimapStyle>();
		Instance->RegisterInternal();
		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

	void FGraphMinimapStyle::Unregister()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*Instance);
		Instance.Reset();
	}

	const ISlateStyle& FGraphMinimapStyle::Get()
	{
		check(Instance.IsValid()); // Don't call before Register is called or after Unregister is called.
		return *Instance.Get();
	}

	const FSlateBrush* FGraphMinimapStyle::GetBrushFromIconType(const EGraphMinimapStyleIconType IconType)
	{
		return Get().GetBrush(GetPropertyNameFromIconType(IconType));
	}

	FSlateIcon FGraphMinimapStyle::GetSlateIconFromIconType(const EGraphMinimapStyleIconType IconType)
	{
		return FSlateIcon(Get().GetStyleSetName(), GetPropertyNameFromIconType(IconType));
	}

	FName FGraphMinimapStyle::GetPropertyNameFromIconType(const EGraphMinimapStyleIconType IconType)
	{
		const UEnum* EnumPtr = StaticEnum<EGraphMinimapStyleIconType>();
		check(IsValid(EnumPtr));
		
		const FString EnumName = EnumPtr->GetNameStringByValue(static_cast<int64>(IconType));
		return *FString::Printf(TEXT("%s.%s"), *Global::PluginName.ToString(), *EnumName);
	}

	TUniquePtr<FGraphMinimapStyle> FGraphMinimapStyle::Instance = nullptr;
}
