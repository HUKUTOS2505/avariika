// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Utilities/GraphMinimapSettings.h"
#include "GraphMinimap/GraphMinimapGlobals.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "GraphMinimapSettings"

namespace GraphMinimap
{
	namespace Settings
	{
		static const FName ContainerName			= TEXT("Editor");
		static const FName CategoryName				= TEXT("Plugins");
		static const FName SectionName				= TEXT("GraphMinimapSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

UGraphMinimapSettings::UGraphMinimapSettings()
	: DefaultGraphMinimapState(EGraphMinimapState::Visible)
	, DefaultGraphMinimapSize(400.f, 300.f)
	, DefaultRenderingScale(1.f)
	, bKeepGraphMinimapState(true)
	, MaxGraphSize(10000.f, 10000.f)
	, MinimapAlignment(EGraphMinimapAlignment::BottomLeft)
	, MinimapOpacity(0.7f)
	, MinimapTintColor(FLinearColor::White)
	, ModeIconSize(32.f)
	, Padding(100.f)
	, ModeIconTintColor(FLinearColor::White)
	, CameraBoundsColor(FLinearColor::Green)
	, CameraBoundsThickness(1.f)
	, DragSensitivity(3.f)
	, bDrawSizeAndScale(true)
	, SizeAndScaleTintColor(FLinearColor::White)
	, bShowMinimapArea(true)
	, MinimapAreaOpacity(1.f)
{
}

void UGraphMinimapSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphMinimap::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			GraphMinimap::Settings::ContainerName,
			GraphMinimap::Settings::CategoryName,
			GraphMinimap::Settings::SectionName,
			LOCTEXT("SettingName", "Graph Minimap"),
			LOCTEXT("SettingDescription", "Editor settings for Graph Minimap"),
			GetMutableDefault<UGraphMinimapSettings>()
		);
	}
}

void UGraphMinimapSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = GraphMinimap::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			GraphMinimap::Settings::ContainerName,
			GraphMinimap::Settings::CategoryName,
			GraphMinimap::Settings::SectionName
		);
	}
}

UGraphMinimapSettings& UGraphMinimapSettings::Get()
{
	auto* Settings = GetMutableDefault<UGraphMinimapSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UGraphMinimapSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	ModifyDefaultGraphMinimapSize();
	ModifyMaxGraphSize();
	ModifyMinimapAlignment();
	ModifyMinimapOpacity();
	ModifyMinimapTintColor();
	ModifyModeIconSize();
	ModifyPadding();
	ModifyModeIconTintColor();
	ModifyCameraBoundsColor();
	ModifyCameraBoundsThickness();
	ModifyDragSensitivity();
	ModifyDrawSizeAndScale();
	ModifySizeAndScaleTintColor();
	ModifyShowMinimapArea();
	ModifyMinimapAreaOpacity();
}

void UGraphMinimapSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, DefaultGraphMinimapSize))
	{
		ModifyDefaultGraphMinimapSize();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, MaxGraphSize))
	{
		ModifyMaxGraphSize();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, MinimapAlignment))
	{
		ModifyMinimapAlignment();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, MinimapOpacity))
	{
		ModifyMinimapOpacity();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, MinimapTintColor))
	{
		ModifyMinimapTintColor();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, ModeIconSize))
	{
		ModifyModeIconSize();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, Padding))
	{
		ModifyPadding();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, ModeIconTintColor))
	{
		ModifyModeIconTintColor();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, CameraBoundsColor))
	{
		ModifyCameraBoundsColor();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, CameraBoundsThickness))
	{
		ModifyCameraBoundsThickness();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, DragSensitivity))
	{
		ModifyDragSensitivity();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, bDrawSizeAndScale))
	{
		ModifyDrawSizeAndScale();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, SizeAndScaleTintColor))
	{
		ModifySizeAndScaleTintColor();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, bShowMinimapArea))
	{
		ModifyShowMinimapArea();
	}
	else if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphMinimapSettings, MinimapAreaOpacity))
	{
		ModifyMinimapAreaOpacity();
	}
}

void UGraphMinimapSettings::ModifyDefaultGraphMinimapSize()
{
	DefaultGraphMinimapSize.X = FMath::Max(DefaultGraphMinimapSize.X, GraphMinimap::Global::MinimumMinimapSize.X);
	DefaultGraphMinimapSize.Y = FMath::Max(DefaultGraphMinimapSize.Y, GraphMinimap::Global::MinimumMinimapSize.Y);
}

void UGraphMinimapSettings::ModifyMaxGraphSize()
{
	MaxGraphSize.X = FMath::Max(MaxGraphSize.X, 0.f);
	MaxGraphSize.Y = FMath::Max(MaxGraphSize.Y, 0.f);
}

void UGraphMinimapSettings::ModifyMinimapAlignment()
{
	const EHorizontalAlignment HAlign = GraphMinimap::GraphMinimapAlignment::ToHorizontalAlignment(MinimapAlignment);
	const EVerticalAlignment VAlign = GraphMinimap::GraphMinimapAlignment::ToVerticalAlignment(MinimapAlignment);
	OnMinimapAlignmentChanged.Broadcast(HAlign, VAlign);
}

void UGraphMinimapSettings::ModifyMinimapOpacity()
{
	MinimapOpacity = FMath::Clamp(MinimapOpacity, 0.f, 1.f);
	OnMinimapOpacityChanged.Broadcast(MinimapOpacity);
}

void UGraphMinimapSettings::ModifyMinimapTintColor()
{
	OnMinimapTintColorChanged.Broadcast(MinimapTintColor);
}

void UGraphMinimapSettings::ModifyModeIconSize()
{
	const float MaxModeIconSize = FMath::Max(GraphMinimap::Global::MinimumMinimapSize.X, GraphMinimap::Global::MinimumMinimapSize.Y);
	ModeIconSize = FMath::Clamp(ModeIconSize, 0.f, MaxModeIconSize);
	OnModeIconSizeChanged.Broadcast(ModeIconSize);
}

void UGraphMinimapSettings::ModifyPadding()
{
	OnPaddingChanged.Broadcast(Padding);
}

void UGraphMinimapSettings::ModifyModeIconTintColor()
{
	OnModeIconTintColorChanged.Broadcast(ModeIconTintColor);
}

void UGraphMinimapSettings::ModifyCameraBoundsColor()
{
	OnCameraBoundsColorChanged.Broadcast(CameraBoundsColor);
}

void UGraphMinimapSettings::ModifyCameraBoundsThickness()
{
	OnCameraBoundsThicknessChanged.Broadcast(CameraBoundsThickness);
}

void UGraphMinimapSettings::ModifyDragSensitivity()
{
	OnDragSensitivityChanged.Broadcast(DragSensitivity);
}

void UGraphMinimapSettings::ModifyDrawSizeAndScale()
{
	OnDrawSizeAndScaleChanged.Broadcast(bDrawSizeAndScale);
}

void UGraphMinimapSettings::ModifySizeAndScaleTintColor()
{
	OnSizeAndScaleTintColorChanged.Broadcast(SizeAndScaleTintColor);
}

void UGraphMinimapSettings::ModifyShowMinimapArea()
{
	OnShowMinimapAreaChanged.Broadcast(bShowMinimapArea);
}

void UGraphMinimapSettings::ModifyMinimapAreaOpacity()
{
	OnMinimapAreaOpacityChanged.Broadcast(MinimapAreaOpacity);
}

#undef LOCTEXT_NAMESPACE
