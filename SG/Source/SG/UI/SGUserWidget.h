#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/SGUIActionBinding.h"
#include "UObject/PrimaryAssetId.h"
#include "SGUserWidget.generated.h"

class USGUIInputSubsystem;

UCLASS(Abstract)
class SG_API USGUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindAction(const FPrimaryAssetId& ActionId, const FSimpleDelegate& OnExecuted);
	void UnbindAction(const FPrimaryAssetId& ActionId);
	bool HasActionBinding(const FPrimaryAssetId& ActionId) const;

	const TArray<FSGUIActionBindingHandle>& GetActionBindings() const { return ActionBindings; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	int32 FindActionBindingIndex(const USGUIInputSubsystem& UIInputSubsystem, const FPrimaryAssetId& ActionId) const;

private:
	TArray<FSGUIActionBindingHandle> ActionBindings;
};
