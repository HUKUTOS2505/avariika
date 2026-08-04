//
// rdSplineToolsOptions
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 2nd April 2024
// Last Modified: 10th January 2025

#include "rdSplineToolsOptions.h"

//----------------------------------------------------------------------------------------------------------------
// ToString
//----------------------------------------------------------------------------------------------------------------
FString UrdSplineToolsOptions::ToString() {

	FString str="2"; // version
	str+=FString::Printf(TEXT(",%d,%s,%d,%f,%f,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%s,%d,%f,%f,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%s,%d,%f,%f,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f"),

						(int32)snapIntersectionsType,
						*snapIntersectionsHotkey,
						snapIntersectionsTangent,
						snapIntersectionRadius,
						visIntersectionRadius,
						*visMeshIntersection.ToString(),
						visMeshIntersectionScale,
						visIntersectionCol1.R,
						visIntersectionCol1.G,
						visIntersectionCol1.B,
						visIntersectionCol1.A,
						visIntersectionCol2.R,
						visIntersectionCol2.G,
						visIntersectionCol2.B,
						visIntersectionCol2.A,
						visIntersectionSnappedOpacity,

						(int32)snapSocketsType,
						*snapSocketsHotkey,
						snapSocketsTangent,
						snapSocketRadius,
						visSocketRadius,
						*visMeshSocket.ToString(),
						visMeshSocketScale,
						visSocketCol1.R,
						visSocketCol1.G,
						visSocketCol1.B,
						visSocketCol1.A,
						visSocketCol2.R,
						visSocketCol2.G,
						visSocketCol2.B,
						visSocketCol2.A,
						visSocketSnappedOpacity,

						(int32)snapSplinePointsType,
						*snapSplinePointsHotkey,
						snapSplinePointsTangent,
						snapSplinePointRadius,
						visSplinePointRadius,
						*visMeshSplinePoint.ToString(),
						visMeshSplinePointScale,
						visSplinePointCol1.R,
						visSplinePointCol1.G,
						visSplinePointCol1.B,
						visSplinePointCol1.A,
						visSplinePointCol2.R,
						visSplinePointCol2.G,
						visSplinePointCol2.B,
						visSplinePointCol2.A,
						visSplinePointSnappedOpacity,

						doFullDraw,
						fullDrawTime
					);

	return str;
}

//----------------------------------------------------------------------------------------------------------------
// FromString
//----------------------------------------------------------------------------------------------------------------
bool UrdSplineToolsOptions::FromString(const FString& str) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT(","),false);
	if(num<2) {
		return false;
	}

	int32 version=FCString::Atoi(*vals[0]);
	if(version>2 || (version==1 && num!=2) || (version==2 && num!=51)) {
		return false;
	}

	int i=1;
	if(version==1) {
		snapIntersectionRadius=FCString::Atof(*vals[i++]);
	} else {
		snapIntersectionsType=(rdSnapType)FCString::Atoi(*vals[i++]);
		snapIntersectionsHotkey=vals[i++];
		snapIntersectionsTangent=vals[i++].ToBool();
		snapIntersectionRadius=FCString::Atof(*vals[i++]);
		visIntersectionRadius=FCString::Atof(*vals[i++]);
		visMeshIntersection=FSoftObjectPath(vals[i++]);
		visMeshIntersectionScale=FCString::Atof(*vals[i++]);
		visIntersectionCol1.R=FCString::Atof(*vals[i++]);
		visIntersectionCol1.G=FCString::Atof(*vals[i++]);
		visIntersectionCol1.B=FCString::Atof(*vals[i++]);
		visIntersectionCol1.A=FCString::Atof(*vals[i++]);
		visIntersectionCol2.R=FCString::Atof(*vals[i++]);
		visIntersectionCol2.G=FCString::Atof(*vals[i++]);
		visIntersectionCol2.B=FCString::Atof(*vals[i++]);
		visIntersectionCol2.A=FCString::Atof(*vals[i++]);
		visIntersectionSnappedOpacity=FCString::Atof(*vals[i++]);

		snapSocketsType=(rdSnapType)FCString::Atoi(*vals[i++]);
		snapSocketsHotkey=vals[i++];
		snapSocketsTangent=vals[i++].ToBool();
		snapSocketRadius=FCString::Atof(*vals[i++]);
		visSocketRadius=FCString::Atof(*vals[i++]);
		visMeshSocket=FSoftObjectPath(vals[i++]);
		visMeshSocketScale=FCString::Atof(*vals[i++]);
		visSocketCol1.R=FCString::Atof(*vals[i++]);
		visSocketCol1.G=FCString::Atof(*vals[i++]);
		visSocketCol1.B=FCString::Atof(*vals[i++]);
		visSocketCol1.A=FCString::Atof(*vals[i++]);
		visSocketCol2.R=FCString::Atof(*vals[i++]);
		visSocketCol2.G=FCString::Atof(*vals[i++]);
		visSocketCol2.B=FCString::Atof(*vals[i++]);
		visSocketCol2.A=FCString::Atof(*vals[i++]);
		visSocketSnappedOpacity=FCString::Atof(*vals[i++]);

		snapSplinePointsType=(rdSnapType)FCString::Atoi(*vals[i++]);
		snapSplinePointsHotkey=vals[i++];
		snapSplinePointsTangent=vals[i++].ToBool();
		snapSplinePointRadius=FCString::Atof(*vals[i++]);
		visSplinePointRadius=FCString::Atof(*vals[i++]);
		visMeshSplinePoint=FSoftObjectPath(vals[i++]);
		visMeshSplinePointScale=FCString::Atof(*vals[i++]);
		visSplinePointCol1.R=FCString::Atof(*vals[i++]);
		visSplinePointCol1.G=FCString::Atof(*vals[i++]);
		visSplinePointCol1.B=FCString::Atof(*vals[i++]);
		visSplinePointCol1.A=FCString::Atof(*vals[i++]);
		visSplinePointCol2.R=FCString::Atof(*vals[i++]);
		visSplinePointCol2.G=FCString::Atof(*vals[i++]);
		visSplinePointCol2.B=FCString::Atof(*vals[i++]);
		visSplinePointCol2.A=FCString::Atof(*vals[i++]);
		visSplinePointSnappedOpacity=FCString::Atof(*vals[i++]);
		
		doFullDraw=vals[i++].ToBool();
		fullDrawTime=FCString::Atof(*vals[i++]);
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------
	