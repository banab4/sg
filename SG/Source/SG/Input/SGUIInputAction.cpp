#include "SGUIInputAction.h"

#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SGUIInputAction)

const FPrimaryAssetType USGUIInputAction::PrimaryAssetType = FName(TEXT("UIInputAction"));

const USGUIInputAction* USGUIInputAction::Find(const FPrimaryAssetId& ActionId)
{
	if (!ActionId.IsValid() || !UAssetManager::IsInitialized())
	{
		return nullptr;
	}
	return Cast<USGUIInputAction>(UAssetManager::Get().GetPrimaryAssetObject(ActionId));
}

FPrimaryAssetId USGUIInputAction::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(PrimaryAssetType, GetFName());
}
