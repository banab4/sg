#pragma once

#include "CoreMinimal.h"
#include "UI/SGUIActionNodeWidget.h"
#include "SGUILayout.generated.h"

class UCanvasPanel;
class USGUILayer;

UCLASS()
class SG_API USGUILayout : public USGUIActionNodeWidget
{
	GENERATED_BODY()

public:
	const TArray<TObjectPtr<USGUILayer>>& GetLayers() const { return Layers; }
	USGUILayer* FindLayer(FName LayerName) const;

	bool AddWidget(USGUserWidget* Widget, FName LayerName);
	bool RemoveWidget(USGUserWidget* Widget);

protected:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* RootCanvas;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<USGUILayer>> Layers;
};
