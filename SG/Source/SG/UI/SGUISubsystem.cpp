#include "SGUISubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Settings/SGUISettings.h"
#include "UI/SGUserWidget.h"
#include "Input/SGUIInputSubsystem.h"
#include "UI/SGUILayout.h"


void USGUISubsystem::InitLayout()
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	if (!GameInstance)
	{
		return;
	}

	UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
	if (!GameViewportClient)
	{
		return;
	}

	const TSubclassOf<USGUILayout> LayoutClass = GetDefault<USGUISettings>()->LayoutClass.LoadSynchronous();
	if (!LayoutClass)
	{
		return;
	}

	Layout = CreateWidget<USGUILayout>(GameInstance, LayoutClass);
	if (Layout)
	{
		GameViewportClient->AddViewportWidgetContent(Layout->TakeWidget(), 0);

		if (USGUIInputSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<USGUIInputSubsystem>(GameInstance->GetFirstGamePlayer()))
		{
			InputSubsystem->SetRootNodeWidget(Layout);
		}
	}
}

bool USGUISubsystem::AddWidget(USGUserWidget* Widget, FName LayerName)
{
	USGUILayout* LayoutWidget = GetLayout();
	return LayoutWidget && LayoutWidget->AddWidget(Widget, LayerName);
}

USGUILayout* USGUISubsystem::GetLayout()
{
	if (!Layout)
	{
		InitLayout();
	}

	return Layout;
}
