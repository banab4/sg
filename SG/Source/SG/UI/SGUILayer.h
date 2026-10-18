#pragma once

#include "CoreMinimal.h"
#include "UI/SGUIActionNodeWidget.h"
#include "SGUILayer.generated.h"

class UOverlay;

UCLASS(NotBlueprintable)
class SG_API USGUILayer : public USGUIActionNodeWidget
{
	GENERATED_BODY()

	friend class USGUILayout;

public:
	FName GetLayerName() const { return LayerName; }
	
	bool AddWidget(USGUserWidget* Widget);
	bool RemoveWidget(USGUserWidget* Widget);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UOverlay> Panel;
	
	FName LayerName;
};
