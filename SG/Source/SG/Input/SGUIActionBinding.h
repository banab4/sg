#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "InputCoreTypes.h"
#include "UObject/PrimaryAssetId.h"
#include "UObject/WeakObjectPtr.h"

class FSGUIActionNode;
class USGUserWidget;
enum class ESGRouteUIInputResult : uint8;

struct FSGUIActionBindingHandle
{
	int32 Id = INDEX_NONE;

	bool IsValid() const { return Id != INDEX_NONE; }
	bool operator==(const FSGUIActionBindingHandle& Other) const { return Id == Other.Id; }
};

struct FSGUIActionBinding
{
	ESGRouteUIInputResult NormalInput(const FKey& Key, EInputEvent InputEvent) const;
	bool IsReceivable(const FKey& Key) const;

	FPrimaryAssetId ActionId;
	FSimpleMulticastDelegate OnExecuted;
	TWeakObjectPtr<USGUserWidget> Widget;
	TWeakPtr<FSGUIActionNode> OwningNode;
};
