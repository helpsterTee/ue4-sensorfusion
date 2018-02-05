#include "SensorFusionEditorIntegrationPCH.h"

#include "SCustomPathWizard.h"
#include "SContextedAutocompletionBox.h"

#include "SensorFusionCore.h"

#include "ScopedTransaction.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



void SCustomPathWizard::Construct(const FArguments& InArgs)
{
	verify(InArgs._Target && InArgs._Target->IsValidLowLevel());
	this->Target = InArgs._Target;
	this->BoneMappingChanged = InArgs._BoneMappingChanged;

	SWindow::Construct(
		SWindow::FArguments()
		.Title(LOCTEXT("AddCustomPath", "Add Custom Path"))
		.ClientSize(FVector2D(400, 80))
		.SizingRule(ESizingRule::FixedSize)
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SBorder)
			[
				SNew(SWizard)
				.ShowPageList(false)
				.OnCanceled(this, &SCustomPathWizard::Cancel)
				.OnFinished(this, &SCustomPathWizard::Finish)
				+ SWizard::Page()
				[
					SAssignNew(TextField, SContextedAutocompletionBox)
					.HintText(LOCTEXT("EnterCustomPath", "Enter custom path here..."))
					.SuggestionUpdateRule_Lambda([](const FText& NewText)-> TArray<TSharedPtr<FName>> {
						TArray<TSharedPtr<FName>> Suggestions;
						for (auto Entry : FSensorFusionCore::Get().GetSensorDataRegistry()->GetPathsContaining(NewText.ToString()))
						{
							Suggestions.Add(MakeShareable(new FName(Entry)));
						}
						return Suggestions;
					})
					.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type CommitInfo) {
						this->Finish();
					})
				]
			]
		]
	);

	FSlateApplication::Get().AddModalWindow(SharedThis(this), InArgs._Parent.ToSharedRef());
}



void SCustomPathWizard::Cancel()
{
	this->RequestDestroyWindow();
}



void SCustomPathWizard::Finish()
{
	// Add path
	bool IsPathValid;
	auto NewPath = URegistryPath::ConstructPath(*TextField->GetText().ToString(), IsPathValid);
	if (IsPathValid)
	{
		GEditor->BeginTransaction(LOCTEXT("AddCustomPath", "Add custom path"));
		this->Target->Modify();
		NewPath->Rename(*NewPath->GetName(), this->Target);
		this->Target->RegistryPaths.Add(NewPath);
		GEditor->EndTransaction();

		this->BoneMappingChanged.ExecuteIfBound(this->Target);
	}
	else
	{
		UE_LOG(SensorFusionLog, Error, TEXT("Failed to add path '%s'."), *TextField->GetText().ToString());
	}

	// Destroy stuff
	TextField = nullptr;
	this->RequestDestroyWindow();
}
