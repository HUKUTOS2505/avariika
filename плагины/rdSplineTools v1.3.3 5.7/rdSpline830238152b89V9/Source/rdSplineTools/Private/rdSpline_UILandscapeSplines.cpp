//
// rdSpline_UILandscapeSplines.cpp
//
// Copyright (c) 2023 Recourse Design ltd. All rights reserved.
//
// Creation Date: 24th November 2023
// Last Modified: 2nd April 2024

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
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

class UrdSplineToolsOptions;
class UClass;

class FrdSplineLandscapeSplineOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdSplineLandscapeSplineOptionsCustomization();
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void						CreateClassViewer();
	void						UpdateClasses();
protected:
	void						OnClassPicked(UClass* selClass);

	UrdSplineToolsOptions*		CurrentOptions;
	TSharedPtr<SClassViewer>	ClassViewer;
	TSharedPtr<FClassFilter>	filter;
};

class rdSplineLandscapeSplineOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdSplineLandscapeSplineOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdSplineLandscapeSplineOptions();
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

TSharedRef<IDetailCustomization> FrdSplineLandscapeSplineOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdSplineLandscapeSplineOptionsCustomization());
}

void FrdSplineLandscapeSplineOptionsCustomization::OnClassPicked(UClass* selClass) {
	CurrentOptions->classType=selClass;
}

//.............................................................................
// Constructor
//.............................................................................
FrdSplineLandscapeSplineOptionsCustomization::FrdSplineLandscapeSplineOptionsCustomization() : CurrentOptions(nullptr) {
}

//.............................................................................
// CreateClassViewer
//.............................................................................
void FrdSplineLandscapeSplineOptionsCustomization::CreateClassViewer() {

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
	ClassViewer=StaticCastSharedRef<SClassViewer>(ClassViewerModule.CreateClassViewer(ClassViewerOptions,FOnClassPicked::CreateSP(this,&FrdSplineLandscapeSplineOptionsCustomization::OnClassPicked)));
}

//.............................................................................
// UpdateClasses
//.............................................................................
void FrdSplineLandscapeSplineOptionsCustomization::UpdateClasses() {
	
	if(!filter) return;
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdSplineLandscapeSplineOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdSplineTools options instance in currently edited objects
	CurrentOptions=Cast<UrdSplineToolsOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdSplineToolsOptions>(Object.Get()); }))->Get());

	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=200.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);

	// .......................................................................
	// Class

	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Choose Destination Class"));

	CreateClassViewer();

	rdFullRow(bldr1,lrow1,box1,"");

	box1->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(SBox)
			.MinDesiredWidth(450.0f)
			.MinDesiredHeight(300.0f)
			.MaxDesiredHeight(300.0f)
			[
				ClassViewer.ToSharedRef()
			]
		];

	rdFullRow(bldr1,lrow2,box2,"");

	rdCheckbox(box2,CurrentOptions->removeLandscapeSplines,"rdSplinetools_LSConv_remove","Remove Landscape Splines","rdSplinetools_LSConv_ToolTip1","Removes the Landscape splines after converting to Blueprint Splines.");

	// .......................................................................
}

rdSplineLandscapeSplineOptions::rdSplineLandscapeSplineOptions() : bUserCancelled(true) {}

void rdSplineLandscapeSplineOptions::Construct(const FArguments& InArgs) {

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
														FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdSplineLandscapeSplineOptionsCustomization::MakeInstance(); }));

	// Set up root object customization to get desired layout
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));

	// Set provided objects on SDetailsView
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2) [	DetailsView->AsShared()	]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
				+ SUniformGridPanel::Slot(0,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineTools_Continue"," Continue ")).ToolTipText(LOCTEXT("rdSplineTools_LSS_ToolTip0","Close and continue")).OnClicked(this,&rdSplineLandscapeSplineOptions::OnConfirm).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]
				+ SUniformGridPanel::Slot(1,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineTools_Cancel"," Cancel ")).ToolTipText(LOCTEXT("rdSplineTools_LSS_ToolTip1","Close and cancel")).OnClicked(this,&rdSplineLandscapeSplineOptions::OnConfirm).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]

		]
	];
}

//.............................................................................
// ShowLandscapeSplineSettings
//.............................................................................
UClass* FrdSplineToolsModule::ShowLandscapeSplineSettings() {

	// Create the settings window...
	TSharedRef<SWindow> winTex=SNew(SWindow)
										.Title(FText::FromString(TEXT("Landscape Spline Settings")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
										.ClientSize(FVector2D(460,400));
	rdSplineToolsoptions=DuplicateObject(GetMutableDefault<UrdSplineToolsOptions>(),GetTransientPackage());
	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdSplineToolsoptions };
	TSharedPtr<rdSplineLandscapeSplineOptions> Options;

	winTex->SetContent(SAssignNew(Options,rdSplineLandscapeSplineOptions)
						.WidgetWindow(winTex)
						.SettingsObjects(OptionObjects)
					  );

	TSharedPtr<SWindow> ParentWindow;
	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return nullptr;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(winTex,MainFrame.GetParentWindow(),false);

	if(Options->WasUserCancelled()) return nullptr;

	return rdSplineToolsoptions->classType;
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
