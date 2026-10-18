#pragma once

#include "Engine/GameViewportClient.h"
#include "SGGameViewportClient.generated.h"

UCLASS()
class SG_API USGGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;

private:
	bool HandleRerouteInput(const FKey& Key, EInputEvent EventType) const;
};
