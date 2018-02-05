#include "SensorFusionEditorIntegrationPCH.h"
#include "AvateeringDetails.h"
#include "SensorFusionEditorIntegration.h"

#include "AvateeringComponent.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



TSharedRef<IDetailCustomization> FAvateeringDetails::MakeInstance()
{
	return MakeShareable(new FAvateeringDetails);
}



void FAvateeringDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray< TWeakObjectPtr<UObject> > ObjectsCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsCustomized);
	for (auto Object : ObjectsCustomized)
	{
		UAvateeringComponent* SensorDataMapper = Cast<UAvateeringComponent>(Object.Get());
		AActor* Owner = SensorDataMapper->GetOwner();
		if (nullptr != SensorDataMapper && Owner != nullptr)
		{
			auto SensorDataMapping = SensorDataMapper->SelectedProfile;
			// find skeletons
			SkinnedMeshSet.Empty();
			for (auto Component : Owner->GetComponents())
			{				
				auto SkinnedMeshComp = Cast<USkinnedMeshComponent>(Component);
				if (nullptr != SkinnedMeshComp)
				{
					SkinnedMeshSet.Add(SkinnedMeshComp);
				}
			}				
			
			// Add button and combo box
			IDetailCategoryBuilder& AvateeringCategory = DetailBuilder.EditCategory("Avateering", LOCTEXT("SensorFusionHeading", "Avateering"), ECategoryPriority::Important);
			AvateeringCategory.AddCustomRow(LOCTEXT("EditMappingName","Edit Mapping"))
				.NameContent()
				[
					SNew(SButton)
					.Text(LOCTEXT("EditMappingName", "Edit Mapping"))
					.OnClicked_Lambda([SensorDataMapping, this]()->FReply {
						FModuleManager::LoadModuleChecked< ISensorFusionEditorIntegration >("SensorFusionEditorIntegration").CreateSensorFusionToolkit(EToolkitMode::Standalone, nullptr, SensorDataMapping, TargetComboBox->GetSelectedItem()->SkeletalMesh);
						return FReply::Handled();
					})
				]
				.ValueContent()
				[
					SAssignNew(TargetComboBox, SComboBox<USkinnedMeshComponent*>)
					.	OptionsSource(&SkinnedMeshSet)
						.OnGenerateWidget(this, &FAvateeringDetails::GenerateTargetComboBoxWidget)
						[
							SNew(STextBlock)
								.Text(this, &FAvateeringDetails::GetTargetName)
						]
				];

			if (SkinnedMeshSet.Num())
			{
				TargetComboBox->SetSelectedItem(SkinnedMeshSet[0]);
			}

			return;
		}
	}
}




TSharedRef<SWidget> FAvateeringDetails::GenerateTargetComboBoxWidget(USkinnedMeshComponent* Item) const
{
	return SNew(STextBlock).Text(Item ? FText::FromString(Item->GetReadableName()) : FText::GetEmpty());
}


FText FAvateeringDetails::GetTargetName() const
{
	auto Target = TargetComboBox->GetSelectedItem();
	return  Target ? FText::FromString(Target->GetReadableName()) : FText::GetEmpty();
}