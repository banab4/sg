#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SGUISubsystem.generated.h"

class USGUILayout;
class USGUserWidget;

UCLASS()
class SG_API USGUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USGUILayout* GetLayout();
	bool AddWidget(USGUserWidget* Widget, FName LayerName);

private:
	void InitLayout();

private:
	UPROPERTY(Transient)
	USGUILayout* Layout;
};
