//
// rdSpline_UIAbout.cpp
//
// Copyright (c) 2023 Recourse Design ltd. All rights reserved.
//
// Creation Date: 6th May 2023
// Last Modified: 4th October 2025

#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/SlateCore/Public/Brushes/SlateImageBrush.h"
#include "Runtime/Launch/Resources/Version.h"
#include "DetailLayoutBuilder.h"
#include "Interfaces/IMainFrameModule.h"
#include "IDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"
#include "rdSpline_Style.h"
#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

#if ENGINE_MAJOR_VERSION<5 && ENGINE_MINOR_VERSION<25
TSharedPtr<SWidget> FSimpleRootObjectCustomization::CustomizeObjectHeader(const UObject* InRootObject) {
#else
TSharedPtr<SWidget> FSimpleRootObjectCustomization::CustomizeObjectHeader(const FDetailsObjectSet & InRootObjectSet) {
#endif
	return SNullWidget::NullWidget;
}

class FrdSplineAboutOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdSplineAboutOptionsCustomization();
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
protected:
};

class rdSplineAboutOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdSplineAboutOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdSplineAboutOptions();
	void			Construct(const FArguments& InArgs);

	virtual bool	SupportsKeyboardFocus() const override { return true; }
	virtual FReply	OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override { return (InKeyEvent.GetKey()==EKeys::Escape)?OnConfirm():FReply::Unhandled(); }
	FReply			OnConfirm(){
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
private:
	TWeakPtr<SWindow>				WidgetWindow;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdSplineAboutOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdSplineAboutOptionsCustomization());
}

//.............................................................................
// Constructor
//.............................................................................
FrdSplineAboutOptionsCustomization::FrdSplineAboutOptionsCustomization() {
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdSplineAboutOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	IDetailCategoryBuilder& CategoryBuilder0=DetailBuilder.EditCategory(TEXT(" "));

	const FSlateBrush* sbrush=FPluginStyle::Get()->GetBrush("rdSplineTools.Logo");
	FString ver=FString::Printf(TEXT("rdSplineTools\n\nversion: %d.%d.%d - %s\nRecourse Design ltd.\n\nUE version:%d.%d.%d\n\n\nvisit www.recourse.nz"),RDSPLINETOOLS_MAJOR_VERSION,RDSPLINETOOLS_MINOR_VERSION,RDSPLINETOOLS_UPDATE_VERSION, TEXT(RDSPLINETOOLS_RELEASEDATE), ENGINE_MAJOR_VERSION,ENGINE_MINOR_VERSION,ENGINE_PATCH_VERSION);

	FDetailWidgetRow& lrow0=CategoryBuilder0.AddCustomRow(FText::FromString(TEXT(" "))).IsEnabled(true);
	lrow0.NameContent()
		[
			SNew(SImage)
			.Image(sbrush)
		];

	TSharedPtr<SHorizontalBox> ContentBox0;
	lrow0.ValueContent()
		[
			SAssignNew(ContentBox0,SHorizontalBox)
		];

	ContentBox0->AddSlot()
		.Padding(FMargin(2.0f,2.0f,2.0f,2.0f))
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(*ver))
			.Font(DetailBuilder.GetDetailFontBold())
		];

	// .......................................................................
}

rdSplineAboutOptions::rdSplineAboutOptions() {}

void rdSplineAboutOptions::Construct(const FArguments& InArgs) {

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
														FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdSplineAboutOptionsCustomization::MakeInstance(); }));

	// Set up root object customization to get desired layout
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));

	// Set provided objects on SDetailsView
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2) [	DetailsView->AsShared()	]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
				+ SUniformGridPanel::Slot(0,0) [
					SNew(SButton).HAlign(HAlign_Center).Text(LOCTEXT("rdSplineTools_Continue"," Continue ")).ToolTipText(LOCTEXT("rdSplineTools_About_ToolTip0","Close and continue")).OnClicked(this,&rdSplineAboutOptions::OnConfirm).ContentPadding(FMargin(1.0,1.0)).TextStyle(GetAppStyle(),"NormalText")
				]

		]
	];
}

//.............................................................................
// ShowAbout
//.............................................................................
void FrdSplineToolsModule::ShowAbout() {

	// Create the settings window...
	TSharedRef<SWindow> winTex=SNew(SWindow)
										.Title(FText::FromString(TEXT("About rdSplineTools")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
										.ClientSize(FVector2D(460,230));

	UrdSplineToolsOptions* rdSplineoptions=DuplicateObject(GetMutableDefault<UrdSplineToolsOptions>(),GetTransientPackage());
	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdSplineoptions };
	TSharedPtr<rdSplineAboutOptions> Options;

	winTex->SetContent(SAssignNew(Options,rdSplineAboutOptions)
						.WidgetWindow(winTex)
						.SettingsObjects(OptionObjects)
					  );

	TSharedPtr<SWindow> ParentWindow;
	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(winTex,MainFrame.GetParentWindow(),false);
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
