#include "SGUIActionBinding.h"

#include "Input/SGUIInputAction.h"
#include "Input/SGUIInputSubsystem.h"
#include "UI/SGUserWidget.h"

ESGRouteUIInputResult FSGUIActionBinding::NormalInput(const FKey& Key, EInputEvent InputEvent) const
{
	if (!IsReceivable(Key))
	{
		return ESGRouteUIInputResult::Unhandled;
	}

	if (InputEvent == IE_Pressed)
	{
		const FSimpleMulticastDelegate Executed = OnExecuted;
		Executed.Broadcast();
	}
	return ESGRouteUIInputResult::Handled;
}

bool FSGUIActionBinding::IsReceivable(const FKey& Key) const
{
	if (!USGUIInputSubsystem::CanWidgetReceiveInput(Widget.Get()))
	{
		return false;
	}

	const USGUIInputAction* Action = USGUIInputAction::Find(ActionId);
	return Action && Action->Key == Key;
}
