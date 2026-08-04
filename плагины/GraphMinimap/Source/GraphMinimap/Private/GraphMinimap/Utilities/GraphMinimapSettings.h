// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphMinimap/Types/GraphMinimapState.h"
#include "GraphMinimap/Types/ClearCacheFileButton.h"
#include "GraphMinimap/Types/GraphMinimapAlignment.h"
#include "GraphMinimapSettings.generated.h"

/**
 * A editor preferences class for this plugin.
 */
UCLASS(Config = Editor, GlobalUserConfig)
class GRAPHMINIMAP_API UGraphMinimapSettings : public UObject
{
	GENERATED_BODY()

public:
	// The default minimap state used when the minimap state is not cached.
	UPROPERTY(EditAnywhere, Config, Category = "Default State")
	EGraphMinimapState DefaultGraphMinimapState;

	// The size of the minimap to display on the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Default State")
	FVector2D DefaultGraphMinimapSize;

	// If the scale for drawing the graph is 0.5, it will be drawn at half the resolution.
	// Decrease the value if you want to draw a graph larger than MaxGraphSize.
	UPROPERTY(EditAnywhere, Config, Category = "Default State", meta = (ClampMin = 0.1f, ClampMax = 1.f, UIMin = 0.1f, UIMax = 1.f))
	float DefaultRenderingScale;
	
	// Whether to restore the state of the minimap for each graph at the next startup.
	UPROPERTY(EditAnywhere, Config, Category = "Default State")
	bool bKeepGraphMinimapState;
	
	// The button to delete the cached minimap data.
	UPROPERTY(EditAnywhere, Category = "Default State")
	FClearCacheFileButton ClearCacheFileButton;

	// A maximum size of graph that can be drawn in the minimap.
	// If set it too high, you may run out of video memory and crash.
	UPROPERTY(EditAnywhere, Config, Category = "Limitation")
	FVector2D MaxGraphSize;
	
	// The position on which the minimap is displayed in the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap")
	EGraphMinimapAlignment MinimapAlignment;
	
	// The opacity of the minimap to display on the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float MinimapOpacity;

	// The tint color of the minimap to display on the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap")
	FLinearColor MinimapTintColor;

	// The size of the mode icon displayed in the upper left of the minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ModeIconSize;

	// The size of the margin applied when drawing the graph.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (ClampMin = 0.f, ClampMax = 1000.f, UIMin = 0.f, UIMax = 1000.f))
	float Padding;
	
	// The tint color of the mode icon displayed in the upper left of the minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap")
	FLinearColor ModeIconTintColor;
	
	// The color of camera bounds displayed on the minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap")
	FLinearColor CameraBoundsColor;

	// The thickness of camera bounds displayed on the minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (ClampMin = 0.f, ClampMax = 10.f, UIMin = 0.f, UIMax = 10.f))
	float CameraBoundsThickness;

	// The mouse sensitivity when dragging.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (ClampMin = 1.f, ClampMax = 10.f, UIMin = 1.f, UIMax = 10.f))
	float DragSensitivity;

	// Whether to draw the drawing size and scale of the graph on the graph minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap")
	bool bDrawSizeAndScale;

	// The tint color of the text of size and scale of the graph on the graph minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (EditCondition = "bDrawSizeAndScale"))
	FLinearColor SizeAndScaleTintColor;
	
	// Whether to show the name of the currently displayed minimap area on the graph minimap.
    UPROPERTY(EditAnywhere, Config, Category = "Minimap")
    bool bShowMinimapArea;

	// The opacity of the name of the currently displayed minimap area on the graph minimap.
	UPROPERTY(EditAnywhere, Config, Category = "Minimap", meta = (ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f, EditCondition = "bShowMinimapArea"))
	float MinimapAreaOpacity;
	
public:
	// Called when MinimapAlignment changed.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMinimapAlignmentChanged, const EHorizontalAlignment /* HAlign */, const EVerticalAlignment /* VAlign */);
	FOnMinimapAlignmentChanged OnMinimapAlignmentChanged;
	
	// Called when MinimapOpacity changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinimapOpacityChanged, const float /* NewMinimapOpacity */);
	FOnMinimapOpacityChanged OnMinimapOpacityChanged;

	// Called when MinimapTintColor changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinimapTintColorChanged, const FLinearColor& /* NewMinimapTintColor */);
	FOnMinimapTintColorChanged OnMinimapTintColorChanged;

	// Called when ModeIconSize changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnModeIconSizeChanged, const float /* NewModeIconSize */);
	FOnModeIconSizeChanged OnModeIconSizeChanged;

	// Called when Padding changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPaddingChanged, const float /* NewPadding */);
	FOnPaddingChanged OnPaddingChanged;

	// Called when ModeIconTintColor changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnModeIconTintColorChanged, const FLinearColor& /* NewModeIconTintColor */);
	FOnModeIconTintColorChanged OnModeIconTintColorChanged;

	// Called when CameraBoundsColor changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraBoundsColorChanged, const FLinearColor& /* NewCameraBoundsColor */);
	FOnCameraBoundsColorChanged OnCameraBoundsColorChanged;

	// Called when CameraBoundsThickness changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraBoundsThicknessChanged, const float /* NewCameraBoundsThickness */);
	FOnCameraBoundsThicknessChanged OnCameraBoundsThicknessChanged;

	// Called when DragSensitivity changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDragSensitivityChanged, const float /* NewDragSensitivity */);
	FOnDragSensitivityChanged OnDragSensitivityChanged;

	// Called when bDrawSizeAndScale changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDrawSizeAndScaleChanged, const bool /* bNewDrawSizeAndScale */);
	FOnDrawSizeAndScaleChanged OnDrawSizeAndScaleChanged;

	// Called when SizeAndScaleTintColor changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSizeAndScaleTintColorChanged, const FLinearColor& /* NewSizeAndScaleTintColor */);
	FOnSizeAndScaleTintColorChanged OnSizeAndScaleTintColorChanged;

	// Called when bShowMinimapArea changed.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowMinimapAreaChanged, const bool /* bNewShowMinimapArea */);
	FOnShowMinimapAreaChanged OnShowMinimapAreaChanged;

	// Called when MinimapAreaOpacity changed.
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinimapAreaOpacityChanged, const float /* NewMinimapAreaOpacity */);
    FOnMinimapAreaOpacityChanged OnMinimapAreaOpacityChanged;
	
public:
	// Constructor.
	UGraphMinimapSettings();
	
	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();

	// Returns reference of this settings.
	static UGraphMinimapSettings& Get();
	
protected:
	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.

	// Functions called when the value changes.
	void ModifyDefaultGraphMinimapSize();
	void ModifyMaxGraphSize();
	void ModifyMinimapAlignment();
	void ModifyMinimapOpacity();
	void ModifyMinimapTintColor();
	void ModifyModeIconSize();
	void ModifyPadding();
	void ModifyModeIconTintColor();
	void ModifyCameraBoundsColor();
	void ModifyCameraBoundsThickness();
	void ModifyDragSensitivity();
	void ModifyDrawSizeAndScale();
	void ModifySizeAndScaleTintColor();
	void ModifyShowMinimapArea();
	void ModifyMinimapAreaOpacity();
};
