#pragma once 


//! @brief Detail panel to directly launch the mapping editor from an selected actor.
class FAvateeringDetails : public IDetailCustomization
{
private:
	TArray<USkinnedMeshComponent*> SkinnedMeshSet;
	TSharedPtr<SComboBox<USkinnedMeshComponent*>> TargetComboBox;

public:
	//!
	static TSharedRef<IDetailCustomization> MakeInstance();


	//!
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;


	//!
	TSharedRef<SWidget> GenerateTargetComboBoxWidget(USkinnedMeshComponent* Item) const;


	//!
	FText GetTargetName() const;
};