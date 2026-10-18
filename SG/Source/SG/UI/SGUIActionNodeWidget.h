#pragma once

#include "CoreMinimal.h"
#include "UI/SGUserWidget.h"
#include "SGUIActionNodeWidget.generated.h"

UCLASS(Abstract)
class SG_API USGUIActionNodeWidget : public USGUserWidget
{
	GENERATED_BODY()
	
public:
	bool IsModal() const { return bIsModal; }
	void SetModal(bool bInIsModal);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	UPROPERTY(EditAnywhere, Category = "SG|Input")
	bool bIsModal = false;

	TSharedPtr<SWidget> ModalInputBlocker;
};
