//
// rdSpline_UILandscapeSplineTransforms.cpp
//
// Copyright (c) 2023 Recourse Design ltd. All rights reserved.
//
// Creation Date: 27th March 2024
// Last Modified: 29th March 2024

#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/SlateCore/Public/Brushes/SlateImageBrush.h"
#include "Runtime/Launch/Resources/Version.h"
#include "DetailLayoutBuilder.h"
#include "Interfaces/IMainFrameModule.h"
#include "IDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"
#include "IDetailRootObjectCustomization.h"
#include "rdUMGHelpers.h"
#include "rdSpline_Style.h"
#include "rdSplineToolsOptions.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//class UrdSplineToolsOptions;
//class UClass;

class FrdSplineLandscapeSplineTransformsOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdSplineLandscapeSplineTransformsOptionsCustomization();
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void						CreateClassViewer();
	void						UpdateClasses();
protected:
	void						OnClassPicked(UClass* selClass);

	UrdSplineToolsOptions*		CurrentOptions;
	TSharedPtr<SClassViewer>	ClassViewer;
	TSharedPtr<FClassFilter>	filter;
};

class rdSplineLandscapeSplineTransformsOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdSplineLandscapeSplineTransformsOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdSplineLandscapeSplineTransformsOptions();
	void			Construct(const FArguments& InArgs);

	virtual bool	SupportsKeyboardFocus() const override { return true; }
	virtual FReply	OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override { return (InKeyEvent.GetKey()==EKeys::Escape)?OnConfirm():FReply::Unhandled(); }
	FReply			OnConfirm(){
						bUserCancelled=false;
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
	bool			WasUserCancelled() { return bUserCancelled; }
private:
	bool							bUserCancelled;
	TWeakPtr<SWindow>				WidgetWindow;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdSplineLandscapeSplineTransformsOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdSplineLandscapeSplineTransformsOptionsCustomization());
}

void FrdSplineLandscapeSplineTransformsOptionsCustomization::OnClassPicked(UClass* selClass) {
	CurrentOptions->classType=selClass;
}

//.............................................................................
// Constructor
//.............................................................................
FrdSplineLandscapeSplineTransformsOptionsCustomization::FrdSplineLandscapeSplineTransformsOptionsCustomization() : CurrentOptions(nullptr) {
}

//.............................................................................
// CreateClassViewer
//.............................................................................
void FrdSplineLandscapeSplineTransformsOptionsCustomization::CreateClassViewer() {

	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions ClassViewerOptions;
	ClassViewerOptions.Mode							= EClassViewerMode::ClassPicker;
	ClassViewerOptions.DisplayMode					= EClassViewerDisplayMode::TreeView;
	ClassViewerOptions.bShowObjectRootClass			= true;
	ClassViewerOptions.bIsPlaceableOnly				= true;
	ClassViewerOptions.bIsBlueprintBaseOnly			= true;
	ClassViewerOptions.bShowUnloadedBlueprints		= true;
	ClassViewerOptions.bEnableClassDynamicLoading	= true;
	ClassViewerOptions.NameTypeToDisplay			= EClassViewerNameTypeToDisplay::Dynamic;

	filter=MakeShareable(new FClassFilter(AActor::StaticClass()));

#if ENGINE_MAJOR_VERSION>4
	ClassViewerOptions.ClassFilters.Add(filter.ToSharedRef());
#else
	ClassViewerOptions.ClassFilter=filter.ToSharedRef();
#endif
	ClassViewerOptions.InitiallySelectedClass=AActor::StaticClass();
	UpdateClasses();
	ClassViewer=StaticCastSharedRef<SClassViewer>(ClassViewerModule.CreateClassViewer(ClassViewerOptions,FOnClassPicked::CreateSP(this,&FrdSplineLandscapeSplineTransformsOptionsCustomization::OnClassPicked)));
}

//.............................................................................
// UpdateClasses
//.............................................................................
void FrdSplineLandscapeSplineTransformsOptionsCustomization::UpdateClasses() {
	
	if(!filter) return;
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdSplineLandscapeSplineTransformsOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdSplineTools options instance in currently edited objects
	CurrentOptions=Cast<UrdSplineToolsOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdSplineToolsOptions>(Object.Get()); }))->Get());

	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=200.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);
	FLinearColor lcRed=FLinearColor(1,0,0,1);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);
	FLinearColor lcBlue=FLinearColor(0,0,1,1);

	// .......................................................................
	// Location

	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("All Splines Transform Shift"));

	rdFullRow(bldr1,lrow1,box1,"");

	rdLabel(box1,80,"Location","Location");
	rdNumBox(box1,"X",CurrentOptions->transformLocation.X,-9999999.0f,9999999.0f,100,lcRed);
	rdNumBox(box1,"Y",CurrentOptions->transformLocation.Y,-9999999.0f,9999999.0f,100,lcGreen);
	rdNumBox(box1,"Z",CurrentOptions->transformLocation.Z,-9999999.0f,9999999.0f,100,lcBlue);

	rdFullRow(bldr1,lrow2,box2,"");

	rdLabel(box2,80,"Rotation","Rotation");
	rdNumBox(box2,"P",CurrentOptions->transformRotation.Pitch,-180.0f,180.0f,100,lcRed);
	rdNumBox(box2,"R",CurrentOptions->transformRotation.Roll,-180.0f,180.0f,100,lcGreen);
	rdNumBox(box2,"Y",CurrentOptions->transformRotation.Yaw,-180.0f,180.0f,100,lcBlue);

	rdFullRow(bldr1,lrow3,box3,"");

	rdLabel(box3,80,"Scale","Scale");
	rdNumBox(box3,"X",CurrentOptions->transformScale.X,-180.0f,180.0f,100,lcRed);
	rdNumBox(box3,"Y",CurrentOptions->transformScale.Y,-180.0f,180.0f,100,lcGreen);
	rdNumBox(box3,"Z",CurrentOptions->transformScale.Z,-180.0f,180.0f,100,lcBlue);

	// .......................................................................
}

rdSplineLandscapeSplineTransformsOptions::rdSplineLandscapeSplineTransformsOptions() : bUserCancelled(true) {}

void rdSplineLandscapeSplineTransformsOptions::Construct(const FArguments& InArgs) {

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
														FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdSplineLandscapeSplineTransformsOptionsCustomization::MakeInstance(); }));

	// Set up root object customization to get desired layout
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));

	// Set provided objects on SDetailsView
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2) [	DetailsView->AsShared()	]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
				+ SUniformGridPanel::Slot(0,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineToolsTransform_Continue"," Transform ")).ToolTipText(LOCTEXT("rdSplineTools_LST_ToolTip0","Close and continue")).OnClicked(this,&rdSplineLandscapeSplineTransformsOptions::OnConfirm).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]
				+ SUniformGridPanel::Slot(1,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineToolsTransform_Cancel"," Cancel ")).ToolTipText(LOCTEXT("rdSplineTools_LST_ToolTip1","Close and cancel")).OnClicked(this,&rdSplineLandscapeSplineTransformsOptions::OnConfirm).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]
		]
	];
}

//.............................................................................
// ShowLandscapeSplineTransformsSettings
//.............................................................................
bool FrdSplineToolsModule::ShowLandscapeSplineTransformsSettings() {

	// Create the settings window...
	TSharedRef<SWindow> winTex=SNew(SWindow)
										.Title(FText::FromString(TEXT("Landscape Spline Transform")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
										.ClientSize(FVector2D(550,150));

	rdSplineToolsoptions=DuplicateObject(GetMutableDefault<UrdSplineToolsOptions>(),GetTransientPackage());
	rdSplineToolsoptions->splineModule=this;
	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdSplineToolsoptions };
	TSharedPtr<rdSplineLandscapeSplineTransformsOptions> Options;
	
	winTex->SetContent(SAssignNew(Options,rdSplineLandscapeSplineTransformsOptions)
						.WidgetWindow(winTex)
						.SettingsObjects(OptionObjects)
					  );

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
