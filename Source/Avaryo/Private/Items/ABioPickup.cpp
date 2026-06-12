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
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh.Succeeded())
		{
			MeshComponent->SetStaticMesh(SphereMesh.Object);
		}
		MeshComponent->SetRelativeScale3D(FVector(0.18f));
	}
}
