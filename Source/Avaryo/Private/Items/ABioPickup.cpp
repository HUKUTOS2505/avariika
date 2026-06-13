#include "Items/ABioPickup.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABioPickup::ABioPickup()
{
	DisplayName = FText::FromString(TEXT("Подозрительный комок"));
	ItemSize = EItemSize::Light;
	ItemEffect = EItemEffect::ThrowBio;
	Charges = 1;
	RotationSpeed = 0.f; // лежит комком, не крутится как лут

	if (MeshComponent)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> BioMesh(TEXT("/Game/Avariika/Meshes/SM_BioBlob/SM_BioBlob.SM_BioBlob"));
		if (BioMesh.Succeeded())
		{
			MeshComponent->SetStaticMesh(BioMesh.Object); // реальный био-комок (~15 см)
		}
		MeshComponent->SetRelativeScale3D(FVector(0.8f));
	}
}
