//
// rdSpline_Clipboard.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 7th August 2023 (moved from helpers.cpp)
// Last Modified: 7th August 2023

#include "rdSplineTools.h"
#include "Misc/OutputDeviceNull.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// CopySplineDataToClipboard
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CopySplineDataToClipboard(FrdSplineComponentVisualizer* vis) {
	
	CopySplineDataToClipboard2();
}

//----------------------------------------------------------------------------------------------------------------
// CreateSplineDataFromClipboard
//----------------------------------------------------------------------------------------------------------------
ESplinePointType::Type cnvInterpCurveType(EInterpCurveMode mode) {
	switch(mode) {
		case CIM_Linear:			return ESplinePointType::Linear;
		case CIM_CurveAuto:			return ESplinePointType::Curve;
		case CIM_Constant:			return ESplinePointType::Constant;
		case CIM_CurveUser:			return ESplinePointType::CurveCustomTangent;
		case CIM_CurveAutoClamped:	return ESplinePointType::CurveClamped;
	}
	return ESplinePointType::Constant;
}
void FrdSplineToolsModule::CreateSplineDataFromClipboard(FrdSplineComponentVisualizer* vis) {
	
	CreateSplineDataFromClipboard2();
}

//----------------------------------------------------------------------------------------------------------------
// CopySplineDataToClipboard2
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CopySplineDataToClipboard2() {

	USplineComponent* spline=splineList[0];
	if(!spline) return;

	FSplineCurves& splineCurves=spline->SplineCurves;
	FInterpCurveVector& position=splineCurves.Position;
	FInterpCurveQuat& rotation=splineCurves.Rotation;
	FInterpCurveVector& scale=splineCurves.Scale;
	int32 numPoints=position.Points.Num();
	TArray<FInterpCurvePointVector>& points=position.Points;
	TArray<FInterpCurvePointQuat>& rotations=rotation.Points;
	TArray<FInterpCurvePointVector>& scales=scale.Points;

	FString str=FString::Printf(TEXT("rdSplineData|1|%d|"),numPoints);
	for(int32 i=0;i<numPoints;i++) {

		str+=FString::Printf(TEXT("%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%d:%s:%s:%s:%s:%s:%s|"),
								*FString::SanitizeFloat(points[i].InVal,0),
								*FString::SanitizeFloat(points[i].OutVal.X,0),
								*FString::SanitizeFloat(points[i].OutVal.Y,0),
								*FString::SanitizeFloat(points[i].OutVal.Z,0),
								*FString::SanitizeFloat(points[i].ArriveTangent.X,0),
								*FString::SanitizeFloat(points[i].ArriveTangent.Y,0),
								*FString::SanitizeFloat(points[i].ArriveTangent.Z,0),
								*FString::SanitizeFloat(points[i].LeaveTangent.X,0),
								*FString::SanitizeFloat(points[i].LeaveTangent.Y,0),
								*FString::SanitizeFloat(points[i].LeaveTangent.Z,0),

								(int32)points[i].InterpMode,

								*FString::SanitizeFloat(rotations[i].OutVal.X,0),
								*FString::SanitizeFloat(rotations[i].OutVal.Z,0),
								*FString::SanitizeFloat(rotations[i].OutVal.Y,0),

								*FString::SanitizeFloat(scales[i].OutVal.X,0),
								*FString::SanitizeFloat(scales[i].OutVal.Y,0),
								*FString::SanitizeFloat(scales[i].OutVal.Z,0)
							);

	}

	FPlatformApplicationMisc::ClipboardCopy(*str);
}

//----------------------------------------------------------------------------------------------------------------
// CreateSplineDataFromClipboard
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::CreateSplineDataFromClipboard2() {

	USplineComponent* spline=splineList[0];
	if(!spline) return;

	FString str;
	FPlatformApplicationMisc::ClipboardPaste(str);

	if(!str.StartsWith(TEXT("rdSplineData|"))) {
		return;
	}

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"));

	if(num<2) {
		return;
	}

	int32 version=FCString::Atoi(*vals[1]);
	if(version!=1) {
		return;
	}

	AActor* actor=actorList[0];
	FOutputDeviceNull ar;
	actor->CallFunctionByNameWithArguments(TEXT("rdClearSpline"),ar,NULL,true);

	spline->ClearSplinePoints();

	for(int32 i=3;i<num;i++) {

		TArray<FString> vs;
		int32 n=vals[i].ParseIntoArray(vs,TEXT(":"));
		if(n!=17) {
			return;
		}
		int32 ind=0;
		FSplinePoint pt;
		pt.InputKey=FCString::Atof(*vs[ind++]);
		float x=FCString::Atof(*vs[ind++]);
		float y=FCString::Atof(*vs[ind++]);
		float z=FCString::Atof(*vs[ind++]);
		pt.Position=FVector(x,y,z);
		x=FCString::Atof(*vs[ind++]);
		y=FCString::Atof(*vs[ind++]);
		z=FCString::Atof(*vs[ind++]);
		pt.ArriveTangent=FVector(x,y,z);
		x=FCString::Atof(*vs[ind++]);
		y=FCString::Atof(*vs[ind++]);
		z=FCString::Atof(*vs[ind++]);
		pt.LeaveTangent=FVector(x,y,z);

		pt.Type=cnvInterpCurveType((EInterpCurveMode)FCString::Atoi(*vs[ind++]));
		
		x=FCString::Atof(*vs[ind++]);
		y=FCString::Atof(*vs[ind++]);
		z=FCString::Atof(*vs[ind++]);
		pt.Rotation=FRotator(x,y,z);//P,Y,R
		x=FCString::Atof(*vs[ind++]);
		y=FCString::Atof(*vs[ind++]);
		z=FCString::Atof(*vs[ind++]);
		pt.Scale=FVector(x,y,z);
										
		spline->AddPoint(pt,true);

	}
	spline->bSplineHasBeenEdited=true;
	spline->UpdateSpline();

	actor->CallFunctionByNameWithArguments(TEXT("rdBuildSpline"),ar,NULL,true);
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	