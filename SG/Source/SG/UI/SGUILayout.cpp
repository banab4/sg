#include "SGUILayout.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Settings/SGUISettings.h"
#include "UI/SGUILayer.h"

void USGUILayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!RootCanvas)
	{
		return;
	}

	for (const FName LayerName : GetDefault<USGUISettings>()->Layers)
	{
		USGUILayer* Layer = CreateWidget<USGUILayer>(this, USGUILayer::StaticClass());
		if (!Layer)
		{
			continue;
		}

		Layer->LayerName = LayerName;
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RootCanvas->AddChild(Layer)))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetSize(FVector2D(0.f, 0.f));
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			CanvasSlot->SetAutoSize(false);
		}
		Layers.Add(Layer);
	}
}

bool USGUILayout::AddWidget(USGUserWidget* Widget, FName LayerName)
{
	USGUILayer* Layer = FindLayer(LayerName);
	return Layer && Layer->AddWidget(Widget);
}

bool USGUILayout::RemoveWidget(USGUserWidget* Widget)
{
	for (USGUILayer* Layer : Layers)
	{
		if (Layer && Layer->RemoveWidget(Widget))
		{
			return true;
		}
	}
	return false;
}

USGUILayer* USGUILayout::FindLayer(FName LayerName) const
{
	const TObjectPtr<USGUILayer>* Found = Layers.FindByPredicate([LayerName](const USGUILayer* Layer)
	{
		return Layer && Layer->GetLayerName() == LayerName;
	});
	return Found ? Found->Get() : nullptr;
}
