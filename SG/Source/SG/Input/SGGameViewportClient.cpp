#include "SGGameViewportClient.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "InputKeyEventArgs.h"
#include "Input/SGUIInputSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SGGameViewportClient)

bool USGGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (HandleRerouteInput(EventArgs.Key, EventArgs.Event))
	{
		return true;
	}

	return Super::InputKey(EventArgs);
}

bool USGGameViewportClient::HandleRerouteInput(const FKey& Key, EInputEvent EventType) const
{
	bool bHandled = false;
	if (GameInstance)
	{
		if (const ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer())
		{
			if (const USGUIInputSubsystem* UIInputSubsystem = LocalPlayer->GetSubsystem<USGUIInputSubsystem>())
			{
				bHandled = UIInputSubsystem->ProcessInput(Key, EventType) != ESGRouteUIInputResult::Unhandled;
			}
		}
	}
	
	return bHandled;
}
