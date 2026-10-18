#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SGUISettings.generated.h"

class USGUILayout;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "SG UI"))
class SG_API USGUISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "UI")
	TSoftClassPtr<USGUILayout> LayoutClass;

	UPROPERTY(config, EditAnywhere, Category = "UI")
	TArray<FName> Layers;
};
