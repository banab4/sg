#include "SGUILayer.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "UI/SGUserWidget.h"

void USGUILayer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (WidgetTree && !Panel)
	{
		Panel = WidgetTree->ConstructWidget<UOverlay>();
		Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WidgetTree->RootWidget = Panel;
	}
}

bool USGUILayer::AddWidget(USGUserWidget* Widget)
{
	if (!Widget || !Panel)
	{
		return false;
	}

	UOverlaySlot* OverlaySlot = Panel->AddChildToOverlay(Widget);
	if (!OverlaySlot)
	{
		return false;
	}
	OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
	OverlaySlot->SetVerticalAlignment(VAlign_Fill);
	return true;
}

bool USGUILayer::RemoveWidget(USGUserWidget* Widget)
{
	return Widget && Panel && Panel->RemoveChild(Widget);
}
