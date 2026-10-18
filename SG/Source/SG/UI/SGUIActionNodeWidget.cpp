#include "SGUIActionNodeWidget.h"
#include "Styling/StyleDefaults.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"

namespace
{
	FReply ConsumePointerEvent(const FGeometry&, const FPointerEvent&)
	{
		return FReply::Handled();
	}
}

void USGUIActionNodeWidget::SetModal(bool bInIsModal)
{
	bIsModal = bInIsModal;

	if (ModalInputBlocker.IsValid())
	{
		ModalInputBlocker->SetVisibility(bIsModal ? EVisibility::Visible : EVisibility::Collapsed);
	}
}

TSharedRef<SWidget> USGUIActionNodeWidget::RebuildWidget()
{
	if (IsDesignTime())
	{
		return Super::RebuildWidget();
	}

	TSharedRef<SWidget> RebuiltWidget = SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(ModalInputBlocker, SBorder)
			.BorderImage(FStyleDefaults::GetNoBrush())
			.Padding(0.f)
			.OnMouseButtonDown_Static(&ConsumePointerEvent)
			.OnMouseButtonUp_Static(&ConsumePointerEvent)
			.OnMouseDoubleClick_Static(&ConsumePointerEvent)
		]

		+ SOverlay::Slot()
		[
			Super::RebuildWidget()
		];

	SetModal(bIsModal);

	return RebuiltWidget;
}

void USGUIActionNodeWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	ModalInputBlocker.Reset();
}
