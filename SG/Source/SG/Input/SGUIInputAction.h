#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h"
#include "SGUIInputAction.generated.h"

UCLASS()
class SG_API USGUIInputAction : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetType PrimaryAssetType;
	static const USGUIInputAction* Find(const FPrimaryAssetId& ActionId);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly)
	FKey Key;
};
