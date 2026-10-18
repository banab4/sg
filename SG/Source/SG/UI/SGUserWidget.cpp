#include "SGUserWidget.h"

#include "Engine/LocalPlayer.h"
#include "Input/SGUIInputSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(SGUserWidget)

namespace
{
	USGUIInputSubsystem* FindUIInputSubsystem(const UUserWidget& Widget)
	{
		return ULocalPlayer::GetSubsystem<USGUIInputSubsystem>(Widget.GetOwningLocalPlayer());
	}
}

void USGUserWidget::BindAction(const FPrimaryAssetId& ActionId, const FSimpleDelegate& OnExecuted)
{
	USGUIInputSubsystem* UIInputSubsystem = FindUIInputSubsystem(*this);
	if (!ensureMsgf(UIInputSubsystem, TEXT("%s has no owning local player to bind %s"), *GetName(), *ActionId.ToString()))
	{
		return;
	}

	ActionBindings.AddUnique(UIInputSubsystem->AddBinding(*this, ActionId, OnExecuted));
}

void USGUserWidget::UnbindAction(const FPrimaryAssetId& ActionId)
{
	if (USGUIInputSubsystem* UIInputSubsystem = FindUIInputSubsystem(*this))
	{
		const int32 Index = FindActionBindingIndex(*UIInputSubsystem, ActionId);
		if (Index != INDEX_NONE)
		{
			UIInputSubsystem->RemoveBinding(ActionBindings[Index]);
			ActionBindings.RemoveAt(Index);
		}
	}
}

bool USGUserWidget::HasActionBinding(const FPrimaryAssetId& ActionId) const
{
	const USGUIInputSubsystem* UIInputSubsystem = FindUIInputSubsystem(*this);
	return UIInputSubsystem && FindActionBindingIndex(*UIInputSubsystem, ActionId) != INDEX_NONE;
}

int32 USGUserWidget::FindActionBindingIndex(const USGUIInputSubsystem& UIInputSubsystem, const FPrimaryAssetId& ActionId) const
{
	return ActionBindings.IndexOfByPredicate([&UIInputSubsystem, &ActionId](const FSGUIActionBindingHandle& Handle)
	{
		const TSharedPtr<const FSGUIActionBinding> Binding = UIInputSubsystem.FindBinding(Handle);
		return Binding && Binding->ActionId == ActionId;
	});
}

void USGUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (USGUIInputSubsystem* UIInputSubsystem = FindUIInputSubsystem(*this))
	{
		UIInputSubsystem->NotifyUserWidgetConstructed(*this);
	}
}

void USGUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (USGUIInputSubsystem* UIInputSubsystem = FindUIInputSubsystem(*this))
	{
		for (const FSGUIActionBindingHandle& Handle : ActionBindings)
		{
			UIInputSubsystem->RemoveBinding(Handle);
		}
		UIInputSubsystem->NotifyUserWidgetDestructed(*this);
	}
	ActionBindings.Reset();
}
