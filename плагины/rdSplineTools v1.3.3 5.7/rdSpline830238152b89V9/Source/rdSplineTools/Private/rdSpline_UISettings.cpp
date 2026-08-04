//
// rdSpline_UISettings.cpp
//
// Copyright (c) 2023 Recourse Design ltd. All rights reserved.
//
// Creation Date: 24th November 2023
// Last Modified: 10th January 2025

#include "Interfaces/IMainFrameModule.h"
#include "PropertyEditorModule.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/SlateCore/Public/Brushes/SlateImageBrush.h"
#include "Runtime/Launch/Resources/Version.h"
#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"
#include "IDetailRootObjectCustomization.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "Runtime/Slate/Public/Widgets/Colors/SColorBlock.h"
#include "Runtime/AppFramework/Public/Widgets/Colors/SColorPicker.h"
#include "rdUMGHelpers.h"
#include "rdPopUpAssetPicker.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

class FrdSplineToolsModule;

class FrdSplineSettingsOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdSplineSettingsOptionsCustomization();
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	rdSnapType					getComboSnapType(const FString& text) { 
									if(text==TEXT("None")) return rdSnapType::RDSNAP_NONE; 
									if(text==TEXT("Always")) return rdSnapType::RDSNAP_ALWAYS; 
									return rdSnapType::RDSNAP_WITHKEY;
								}
	FText						getComboSnapTypeString(rdSnapType st) { 
									if(st==rdSnapType::RDSNAP_NONE) return FText::FromString(TEXT("None"));
									if(st==rdSnapType::RDSNAP_ALWAYS) return FText::FromString(TEXT("Always"));
									return FText::FromString(TEXT("Hotkey")); 
								}

	TSharedPtr<STextBlock>		snapTypeIntersection_ComboBoxLabel;
	TArray<TSharedPtr<FString>> snapTypeIntersection_ComboList;
	TSharedPtr<SWidget>			colIntersection1PickerWidget=nullptr;
	TSharedPtr<SWidget>			colIntersection2PickerWidget=nullptr;

	TSharedPtr<STextBlock>		snapTypeSocket_ComboBoxLabel;
	TArray<TSharedPtr<FString>> snapTypeSocket_ComboList;
	TSharedPtr<SWidget>			colSocket1PickerWidget=nullptr;
	TSharedPtr<SWidget>			colSocket2PickerWidget=nullptr;

	TSharedPtr<STextBlock>		snapTypeSplinePoint_ComboBoxLabel;
	TArray<TSharedPtr<FString>> snapTypeSplinePoint_ComboList;
	TSharedPtr<SWidget>			colSplinePoint1PickerWidget=nullptr;
	TSharedPtr<SWidget>			colSplinePoint2PickerWidget=nullptr;

	UrdSplineToolsOptions*		CurrentOptions=nullptr;

	void						OnIntersectionMeshSelected(const FAssetData& InAssetData) const { CurrentOptions->visMeshIntersection=Cast<UStaticMesh>(InAssetData.GetAsset()); }
	TWeakObjectPtr<UObject>		GetIntersectionMesh() const { return CurrentOptions->visMeshIntersection.Get(); }
	void						OnSocketMeshSelected(const FAssetData& InAssetData) const { CurrentOptions->visMeshSocket=Cast<UStaticMesh>(InAssetData.GetAsset()); }
	TWeakObjectPtr<UObject>		GetSocketMesh() const {	return CurrentOptions->visMeshSocket.Get(); }
	void						OnSplinePointMeshSelected(const FAssetData& InAssetData) const { CurrentOptions->visMeshSplinePoint=Cast<UStaticMesh>(InAssetData.GetAsset()); }
	TWeakObjectPtr<UObject>		GetSplinePointMesh() const { return CurrentOptions->visMeshSplinePoint.Get(); }

	FLinearColor GetIntersectionColor1() const { return CurrentOptions->visIntersectionCol1; }
	FReply ShowIntersection1ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnIntersection1ColorPickerPicked(FLinearColor col) { CurrentOptions->visIntersectionCol1=col; }
	FLinearColor GetIntersectionColor2() const { return CurrentOptions->visIntersectionCol2; }
	FReply ShowIntersection2ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnIntersection2ColorPickerPicked(FLinearColor col) { CurrentOptions->visIntersectionCol2=col; }

	FLinearColor GetSocketColor1() const { return CurrentOptions->visSocketCol1; }
	FReply ShowSocket1ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnSocket1ColorPickerPicked(FLinearColor col) { CurrentOptions->visSocketCol1=col; }
	FLinearColor GetSocketColor2() const { return CurrentOptions->visSocketCol2; }
	FReply ShowSocket2ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnSocket2ColorPickerPicked(FLinearColor col) { CurrentOptions->visSocketCol2=col; }

	FLinearColor GetSplinePointColor1() const { return CurrentOptions->visSplinePointCol1; }
	FReply ShowSplinePoint1ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnSplinePoint1ColorPickerPicked(FLinearColor col) { CurrentOptions->visSplinePointCol1=col; }
	FLinearColor GetSplinePointColor2() const { return CurrentOptions->visSplinePointCol2; }
	FReply ShowSplinePoint2ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnSplinePoint2ColorPickerPicked(FLinearColor col) { CurrentOptions->visSplinePointCol2=col; }

protected:
};

class rdSplineSettingsOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdSplineSettingsOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdSplineSettingsOptions();
	void			Construct(const FArguments& InArgs);

	virtual bool	SupportsKeyboardFocus() const override { return true; }
	virtual FReply	OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override { return (InKeyEvent.GetKey()==EKeys::Escape)?OnConfirm():FReply::Unhandled(); }
	FReply			OnConfirm(){
						bUserCancelled=false;
						if(splineModule) splineModule->SetOptionDefaults(TEXT("Defaults"),CurrentOptions);
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
	FReply			OnCancel(){
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
	FReply			OnReset(){
						return FReply::Handled();
					}

	bool			WasUserCancelled() { return bUserCancelled; }

	FrdSplineToolsModule*			splineModule=nullptr;
	UrdSplineToolsOptions*			CurrentOptions=nullptr;
private:
	bool							bUserCancelled=true;
	TWeakPtr<SWindow>				WidgetWindow;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdSplineSettingsOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdSplineSettingsOptionsCustomization());
}

//.............................................................................
// ShowIntersection1ColorPicker
//.............................................................................
FReply FrdSplineSettingsOptionsCustomization::ShowIntersection1ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdSplineSettingsOptionsCustomization::OnIntersection1ColorPickerPicked);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetIntersectionColor1();
#else
	cargs.InitialColorOverride=GetIntersectionColor1();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=colIntersection1PickerWidget;
	cargs.ParentWidget=colIntersection1PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// ShowIntersection2ColorPicker
//.............................................................................
FReply FrdSplineSettingsOptionsCustomization::ShowIntersection2ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdSplineSettingsOptionsCustomization::OnIntersection2ColorPickerPicked);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetIntersectionColor2();
#else
	cargs.InitialColorOverride=GetIntersectionColor2();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=colIntersection2PickerWidget;
	cargs.ParentWidget=colIntersection2PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// ShowSocket1ColorPicker
//.............................................................................
FReply FrdSplineSettingsOptionsCustomization::ShowSocket1ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdSplineSettingsOptionsCustomization::OnSocket1ColorPickerPicked);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetSocketColor1();
#else
	cargs.InitialColorOverride=GetSocketColor1();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=colSocket1PickerWidget;
	cargs.ParentWidget=colSocket1PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// ShowSocket2ColorPicker
//.............................................................................
FReply FrdSplineSettingsOptionsCustomization::ShowSocket2ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdSplineSettingsOptionsCustomization::OnSocket2ColorPickerPicked);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetSocketColor2();
#else
	cargs.InitialColorOverride=GetSocketColor2();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=colSocket2PickerWidget;
	cargs.ParentWidget=colSocket2PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// ShowSocket1ColorPicker
//.............................................................................
FReply FrdSplineSettingsOptionsCustomization::ShowSplinePoint1ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdSplineSettingsOptionsCustomization::OnSplinePoint1ColorPickerPicked);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetSplinePointColor1();
#else
	cargs.InitialColorOverride=GetSplinePointColor1();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=colSplinePoint1PickerWidget;
	cargs.ParentWidget=colSplinePoint1PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// ShowSocket2ColorPicker
//.............................................................................
FReply FrdSplineSettingsOptionsCustomization::ShowSplinePoint2ColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdSplineSettingsOptionsCustomization::OnSplinePoint2ColorPickerPicked);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetSplinePointColor2();
#else
	cargs.InitialColorOverride=GetSplinePointColor2();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=colSplinePoint2PickerWidget;
	cargs.ParentWidget=colSplinePoint2PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// Constructor
//.............................................................................
FrdSplineSettingsOptionsCustomization::FrdSplineSettingsOptionsCustomization() {

	snapTypeIntersection_ComboList.Add(MakeShareable(new FString(TEXT("None"))));
	snapTypeIntersection_ComboList.Add(MakeShareable(new FString(TEXT("Always"))));
	snapTypeIntersection_ComboList.Add(MakeShareable(new FString(TEXT("Hotkey"))));

	snapTypeSocket_ComboList.Add(MakeShareable(new FString(TEXT("None"))));
	snapTypeSocket_ComboList.Add(MakeShareable(new FString(TEXT("Always"))));
	snapTypeSocket_ComboList.Add(MakeShareable(new FString(TEXT("Hotkey"))));

	snapTypeSplinePoint_ComboList.Add(MakeShareable(new FString(TEXT("None"))));
	snapTypeSplinePoint_ComboList.Add(MakeShareable(new FString(TEXT("Always"))));
	snapTypeSplinePoint_ComboList.Add(MakeShareable(new FString(TEXT("Hotkey"))));
}

TSharedRef<SWidget> FrdSplineSettingsOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdSplineSettingsOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdTextools options instance in currently edited objects
	CurrentOptions=Cast<UrdSplineToolsOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdSplineToolsOptions>(Object.Get()); }))->Get());

	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=200.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);
//	FLinearColor lcRed=FLinearColor(1,0,0,1);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);
//	FLinearColor lcBlue=FLinearColor(0,0,1,1);

	// .......................................................................
	// AutoDraw Settings

	IDetailCategoryBuilder& bldr0=DetailBuilder.EditCategory(TEXT("AutoDraw Settings"));

	rdFullRow(bldr0,lrow0,box0,"");
	rdCheckbox(box0,CurrentOptions->doFullDraw,"AutoDraw","AutoDraw (UE5.4 up)","AutoDrawTT","AutoDraw: Draw the Spline contents after an amount of inactivity.");

	rdFullRow(bldr0,lrow0a,box0a,"");
	rdLabel(box0a,100,"Time","Time (secs)");
	rdNumBox(box0a,"",CurrentOptions->fullDrawTime,0.0f,100.0f,100,lcGreen);

	// .......................................................................
	// Intersection Settings

	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Intersection Snapping Settings"));

	rdFullRow(bldr1,lrow1,box1,"");
	rdLabel(box1,120,"Behaviour","Behaviour");
	box1->AddSlot()
		.Padding(margin)
		.AutoWidth() [
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&snapTypeIntersection_ComboList)
			.OnGenerateWidget(this,&FrdSplineSettingsOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->snapIntersectionsType=getComboSnapType(*Selection); snapTypeIntersection_ComboBoxLabel.Get()->SetText(FText::FromString(*Selection)); })
			.ToolTipText(LOCTEXT("rdSplineTools_Settings_TT1","Intersection Snap Type: "))
				.Content() [
					SAssignNew(snapTypeIntersection_ComboBoxLabel,STextBlock)
					.Text(getComboSnapTypeString(CurrentOptions->snapIntersectionsType))
#if ENGINE_MAJOR_VERSION>4
					.MinDesiredWidth(102)
#else
					.MinDesiredWidth(124)
#endif
				]
		];
	rdCheckbox(box1,CurrentOptions->snapIntersectionsTangent,"SnapTangents","Snap Tangents","SnapTangentsTT","Snap the spline points Tangents to that of the destination");

	rdFullRow(bldr1,lrow1a,box1a,"");
	rdLabel(box1a,120,"Snapping Radius","Snapping Radius");
	rdNumBox(box1a,"",CurrentOptions->snapIntersectionRadius,0.0f,90000.0f,100,lcGreen);

	rdFullRow(bldr1,lrow1b,box1b,"");
	rdLabel(box1b,120,"Vis Radius","Vis Radius");
	rdNumBox(box1b,"",CurrentOptions->visIntersectionRadius,0.0f,90000.0f,100,lcGreen);

	rdFullRow(bldr1,lrow1c,box1c,"");
	rdLabel(box1c,120,"Color1","Far Color");
	rdColorPicker(box1c,colIntersection1PickerWidget,CurrentOptions->visIntersectionCol1,&FrdSplineSettingsOptionsCustomization::ShowIntersection1ColorPicker,"is1cp","Far Vis Color");
	rdLabel(box1c,80,"Color2","   Near Color");
	rdColorPicker(box1c,colIntersection2PickerWidget,CurrentOptions->visIntersectionCol2,&FrdSplineSettingsOptionsCustomization::ShowIntersection2ColorPicker,"is2cp","Near Vis Color");

	rdFullRow(bldr1,lrow1d,box1d,"");
	rdLabel(box1d,120,"Mesh","StaticMesh");
	rdAssetPicker(box1d,&FrdSplineSettingsOptionsCustomization::OnIntersectionMeshSelected,&FrdSplineSettingsOptionsCustomization::GetIntersectionMesh,290,UStaticMesh::StaticClass());

	rdFullRow(bldr1,lrow1e,box1e,"");
	rdLabel(box1e,120,"MeshScale","Mesh Scale (%)");
	rdSpinBox(box1e,CurrentOptions->visMeshIntersectionScale,0.1,1000.0,120,"rdSplineTools_Settings_ScaleTT1","Vis mesh Scale: The scale at which to display the mesh");

	// .......................................................................
	// Socket Settings

	IDetailCategoryBuilder& bldr2=DetailBuilder.EditCategory(TEXT("Socket Snapping Settings"));

	rdFullRow(bldr2,lrow2,box2,"");
	rdLabel(box2,120,"Behaviour","Behaviour");
	box2->AddSlot()
		.Padding(margin)
		.AutoWidth() [
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&snapTypeSocket_ComboList)
			.OnGenerateWidget(this,&FrdSplineSettingsOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->snapSocketsType=getComboSnapType(*Selection); snapTypeSocket_ComboBoxLabel.Get()->SetText(FText::FromString(*Selection)); })
			.ToolTipText(LOCTEXT("rdSplineTools_Settings_TT2","Socket Snap Type: "))
				.Content() [
					SAssignNew(snapTypeSocket_ComboBoxLabel,STextBlock)
					.Text(getComboSnapTypeString(CurrentOptions->snapSocketsType))
#if ENGINE_MAJOR_VERSION>4
					.MinDesiredWidth(102)
#else
					.MinDesiredWidth(124)
#endif
				]
		];
	rdCheckbox(box2,CurrentOptions->snapSocketsTangent,"SnapTangents","Snap Tangents","SnapTangentsTT","Snap the spline points Tangents to that of the destination");

	rdFullRow(bldr2,lrow2a,box2a,"");
	rdLabel(box2a,120,"Snapping Radius","Snapping Radius");
	rdNumBox(box2a,"",CurrentOptions->snapSocketRadius,0.0f,90000.0f,100,lcGreen);

	rdFullRow(bldr2,lrow2b,box2b,"");
	rdLabel(box2b,120,"Vis Radius","Vis Radius");
	rdNumBox(box2b,"",CurrentOptions->visSocketRadius,0.0f,90000.0f,100,lcGreen);

	rdFullRow(bldr2,lrow2c,box2c,"");
	rdLabel(box2c,120,"Color1","Far Color");
	rdColorPicker(box2c,colSocket1PickerWidget,CurrentOptions->visSocketCol1,&FrdSplineSettingsOptionsCustomization::ShowSocket1ColorPicker,"is1cp","Far Vis Color");
	rdLabel(box2c,80,"Color2","   Near Color");
	rdColorPicker(box2c,colSocket2PickerWidget,CurrentOptions->visSocketCol2,&FrdSplineSettingsOptionsCustomization::ShowSocket2ColorPicker,"is2cp","Near Vis Color");

	rdFullRow(bldr2,lrow2d,box2d,"");
	rdLabel(box2d,120,"Mesh","StaticMesh");
	rdAssetPicker(box2d,&FrdSplineSettingsOptionsCustomization::OnSocketMeshSelected,&FrdSplineSettingsOptionsCustomization::GetSocketMesh,290,UStaticMesh::StaticClass());

	rdFullRow(bldr2,lrow2e,box2e,"");
	rdLabel(box2e,120,"MeshScale","Mesh Scale (%)");
	rdSpinBox(box2e,CurrentOptions->visMeshSocketScale,0.1,1000.0,120,"rdSplineTools_Settings_ScaleTT2","Vis mesh Scale: The scale at which to display the mesh");

	// .......................................................................
	// ControlPoint Settings

	IDetailCategoryBuilder& bldr3=DetailBuilder.EditCategory(TEXT("SplinePoint Snapping Settings"));

	rdFullRow(bldr3,lrow3,box3,"");
	rdLabel(box3,120,"Behaviour","Behaviour");
	box3->AddSlot()
		.Padding(margin)
		.AutoWidth() [
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&snapTypeSplinePoint_ComboList)
			.OnGenerateWidget(this,&FrdSplineSettingsOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->snapSplinePointsType=getComboSnapType(*Selection); snapTypeSplinePoint_ComboBoxLabel.Get()->SetText(FText::FromString(*Selection)); })
			.ToolTipText(LOCTEXT("rdSplineTools_Settings_TT3","Spline Point Snap Type: "))
				.Content() [
					SAssignNew(snapTypeSplinePoint_ComboBoxLabel,STextBlock)
					.Text(getComboSnapTypeString(CurrentOptions->snapSplinePointsType))
#if ENGINE_MAJOR_VERSION>4
					.MinDesiredWidth(102)
#else
					.MinDesiredWidth(124)
#endif
				]
		];
	rdCheckbox(box3,CurrentOptions->snapSplinePointsTangent,"SnapTangents","Snap Tangents","SnapTangentsTT","Snap the spline points Tangents to that of the destination");

	rdFullRow(bldr3,lrow3a,box3a,"");
	rdLabel(box3a,120,"Snapping Radius","Snapping Radius");
	rdNumBox(box3a,"",CurrentOptions->snapSplinePointRadius,0.0f,90000.0f,100,lcGreen);

	rdFullRow(bldr3,lrow3b,box3b,"");
	rdLabel(box3b,120,"Vis Radius","Vis Radius");
	rdNumBox(box3b,"",CurrentOptions->visSplinePointRadius,0.0f,90000.0f,100,lcGreen);

	rdFullRow(bldr3,lrow3c,box3c,"");
	rdLabel(box3c,120,"Color1","Far Color");
	rdColorPicker(box3c,colSplinePoint1PickerWidget,CurrentOptions->visSplinePointCol1,&FrdSplineSettingsOptionsCustomization::ShowSplinePoint1ColorPicker,"is1cp","Far Vis Color");
	rdLabel(box3c,80,"Color2","   Near Color");
	rdColorPicker(box3c,colSplinePoint2PickerWidget,CurrentOptions->visSplinePointCol2,&FrdSplineSettingsOptionsCustomization::ShowSplinePoint2ColorPicker,"is2cp","Near Vis Color");

	rdFullRow(bldr3,lrow3d,box3d,"");
	rdLabel(box3d,120,"Mesh","StaticMesh");
	rdAssetPicker(box3d,&FrdSplineSettingsOptionsCustomization::OnSplinePointMeshSelected,&FrdSplineSettingsOptionsCustomization::GetSplinePointMesh,290,UStaticMesh::StaticClass());

	rdFullRow(bldr3,lrow3e,box3e,"");
	rdLabel(box3e,120,"MeshScale","Mesh Scale (%)");
	rdSpinBox(box3e,CurrentOptions->visMeshSplinePointScale,0.1,1000.0,120,"rdSplineTools_Settings_ScaleTT3","Vis mesh Scale: The scale at which to display the mesh");

	// .......................................................................
}

rdSplineSettingsOptions::rdSplineSettingsOptions() {}

void rdSplineSettingsOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=false;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Register instance property customization
	DetailsView->RegisterInstancedCustomPropertyLayout(UrdSplineToolsOptions::StaticClass(), 
														FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdSplineSettingsOptionsCustomization::MakeInstance(); }));

	// Set up root object customization to get desired layout
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));

	// Set provided objects on SDetailsView
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2) [	DetailsView->AsShared()	]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
				+ SUniformGridPanel::Slot(0,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineTools_Save"," Save ")).ToolTipText(LOCTEXT("rdSplineTools_About_ToolTip0","Close and continue")).OnClicked(this,&rdSplineSettingsOptions::OnConfirm).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]
				+ SUniformGridPanel::Slot(1,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineTools_Reset"," Defaults ")).ToolTipText(LOCTEXT("rdSplineTools_About_ToolTip1","Reset to the defaults")).OnClicked(this,&rdSplineSettingsOptions::OnReset).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]
				+ SUniformGridPanel::Slot(2,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineTools_Cancel"," Cancel ")).ToolTipText(LOCTEXT("rdSplineTools_About_ToolTip2","Close and continue")).OnClicked(this,&rdSplineSettingsOptions::OnCancel).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]

		]
	];
}

//.............................................................................
// ShowSettings
//.............................................................................
bool FrdSplineToolsModule::ShowSettings() {

	rdSplineToolsoptions=DuplicateObject(GetMutableDefault<UrdSplineToolsOptions>(),GetTransientPackage());
	rdSplineToolsoptions->splineModule=this;
	FillFromOptionDefaults(TEXT("Defaults"),rdSplineToolsoptions);

	rdSplineToolsoptions->visMeshIntersection.LoadSynchronous();
	rdSplineToolsoptions->visMeshSocket.LoadSynchronous();
	rdSplineToolsoptions->visMeshSplinePoint.LoadSynchronous();

	// Create the settings window...
	TSharedRef<SWindow> winTex=SNew(SWindow)
										.Title(FText::FromString(TEXT("rdSplineTools Settings")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
										.ClientSize(FVector2D(500,700));

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdSplineToolsoptions };
	TSharedPtr<rdSplineSettingsOptions> Options;

	winTex->SetContent(SAssignNew(Options,rdSplineSettingsOptions)
						.WidgetWindow(winTex)
						.SettingsObjects(OptionObjects)
					  );
	Options->CurrentOptions=rdSplineToolsoptions;
	Options->splineModule=this;

	TSharedPtr<SWindow> ParentWindow;
	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return false;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(winTex,MainFrame.GetParentWindow(),false);

	if(Options->WasUserCancelled()) return false;
	return true;
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
