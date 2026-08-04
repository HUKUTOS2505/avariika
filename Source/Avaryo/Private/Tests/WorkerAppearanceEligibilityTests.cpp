#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/WorkerAppearanceComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAvaryoRandomEligibilityExhaustiveTest,
	"Avariika.WorkerAppearance.RandomEligibilityExhaustive",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAvaryoRandomEligibilityExhaustiveTest::RunTest(const FString&)
{
	FString Report;
	const bool bPassed =
		UWorkerAppearanceComponent::RunRandomEligibilityExhaustiveSelfTest(Report);
	AddInfo(Report);
	TestTrue(TEXT("Every runtime Random eligibility record passes exhaustive exact-asset policy"), bPassed);
	return bPassed;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAvaryoHeadSkinPresentationTest,
	"Avariika.WorkerAppearance.HeadSkinPresentation",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAvaryoHeadSkinPresentationTest::RunTest(const FString&)
{
	FString Report;
	const bool bPassed =
		UWorkerAppearanceComponent::RunHeadSkinPresentationSelfTest(Report);
	AddInfo(Report);
	TestTrue(
		TEXT("HeadType metadata, SkinTone normalization, Factory/Random and skin material boundaries pass"),
		bPassed);
	return bPassed;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAvaryoBodyCoverageCatalogTest,
	"Avariika.WorkerAppearance.BodyCoverageCatalog",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAvaryoBodyCoverageCatalogTest::RunTest(const FString&)
{
	FString Report;
	const bool bPassed =
		UWorkerAppearanceComponent::RunBodyCoverageSelfTest(Report);
	AddInfo(Report);
	TestTrue(
		TEXT("Package body modules, exact item coverage and unchanged catalog counts pass"),
		bPassed);
	return bPassed;
}

#endif // WITH_DEV_AUTOMATION_TESTS
