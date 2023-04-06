// Copyright Epic Games, Inc. All Rights Reserved.

#include "Insights/Tests/InsightsTestUtils.h"

#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "TraceServices/AnalysisService.h"

#include "Insights/IUnrealInsightsModule.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

FInsightsTestUtils::FInsightsTestUtils(FAutomationTestBase* InTest) :
	Test(InTest)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool FInsightsTestUtils::AnalyzeTrace(const TCHAR* Path) const
{
	IUnrealInsightsModule& TraceInsightsModule = FModuleManager::LoadModuleChecked<IUnrealInsightsModule>("TraceInsights");

	if (!FPaths::FileExists(Path))
	{
		Test->AddError(FString::Printf(TEXT("File does not exist: %s."), Path));
		return false;
	}

	TraceInsightsModule.StartAnalysisForTraceFile(Path);
	auto Session = TraceInsightsModule.GetAnalysisSession();
	if (Session == nullptr)
	{
		Test->AddError(TEXT("Session analysis failed to start."));
		return false;
	}

	int32 MaxWaits = 2000;
	while (!Session->IsAnalysisComplete())
	{
		FPlatformProcess::Sleep(0.033f);

		if (MaxWaits < 0)
		{
			Test->AddError(TEXT("Session analysis takes too long to complete. Aborting test."));
			return false;
		}
		--MaxWaits;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
