//
// rdSpline_Settings.cpp
//
// Copyright Recourse Design ltd 2023, Inc. All Rights Reserved.
//
// Creation Date: 2nd April 2024
// Last Modified: 10th January 2025

#include "rdSplineTools.h"

#define LOCTEXT_NAMESPACE "FrdSplineToolsModule"

//----------------------------------------------------------------------------------------------------------------
// Settings
//----------------------------------------------------------------------------------------------------------------
void FrdSplineToolsModule::Settings() {

	if(ShowSettings()) {

		if(rdSplineVis && rdSplineVis->rdSplineToolsoptions.Get()) {
			FillFromOptionDefaults(TEXT("Defaults"),rdSplineVis->rdSplineToolsoptions.Get());

			rdSplineVis->visMeshIntersection=rdSplineVis->rdSplineToolsoptions->visMeshIntersection.Get();
			if(!rdSplineVis->visMeshIntersection) rdSplineVis->visMeshIntersection=rdSplineVis->rdSplineToolsoptions->visMeshIntersection.LoadSynchronous();

			rdSplineVis->visMeshSocket=rdSplineVis->rdSplineToolsoptions->visMeshSocket.Get();
			if(!rdSplineVis->visMeshSocket) rdSplineVis->visMeshSocket=rdSplineVis->rdSplineToolsoptions->visMeshSocket.LoadSynchronous();

			rdSplineVis->visMeshSplinePoint=rdSplineVis->rdSplineToolsoptions->visMeshSplinePoint.Get();
			if(!rdSplineVis->visMeshSplinePoint) rdSplineVis->visMeshSplinePoint=rdSplineVis->rdSplineToolsoptions->visMeshSplinePoint.LoadSynchronous();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------

#undef LOCTEXT_NAMESPACE
	