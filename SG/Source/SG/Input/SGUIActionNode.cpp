#include "SGUIActionNode.h"

#include "UI/SGUIActionNodeWidget.h"

FSGUIActionNodeRef FSGUIActionNode::CreateRoot(USGUIActionNodeWidget& ActionNodeWidget)
{
	return MakeShareable(new FSGUIActionNode(ActionNodeWidget));
}

FSGUIActionNode::FSGUIActionNode(USGUIActionNodeWidget& ActionNodeWidget)
	: RepresentedWidget(&ActionNodeWidget)
{
}

ESGRouteUIInputResult FSGUIActionNode::ProcessNormalInput(const FKey& Key, EInputEvent InputEvent, const USGUIInputSubsystem& InputSubsystem) const
{
	if (!IsReceivingInput())
	{
		return ESGRouteUIInputResult::Unhandled;
	}

	for (int32 i = Children.Num() - 1; i >= 0; --i)
	{
		const FSGUIActionNodeRef Child = Children[i];
		const ESGRouteUIInputResult ChildResult = Child->ProcessNormalInput(Key, InputEvent, InputSubsystem);
		if (ChildResult != ESGRouteUIInputResult::Unhandled)
		{
			return ChildResult;
		}
	}

	for (int32 i = Bindings.Num() - 1; i >= 0; --i)
	{
		if (const TSharedPtr<const FSGUIActionBinding> Binding = InputSubsystem.FindBinding(Bindings[i]))
		{
			const ESGRouteUIInputResult Result = Binding->NormalInput(Key, InputEvent);
			if (Result != ESGRouteUIInputResult::Unhandled)
			{
				return Result;
			}
		}
	}

	return IsModal() ? ESGRouteUIInputResult::BlockedByModal : ESGRouteUIInputResult::Unhandled;
}

bool FSGUIActionNode::IsReceivingInput() const
{
	return USGUIInputSubsystem::CanWidgetReceiveInput(RepresentedWidget.Get());
}

bool FSGUIActionNode::IsModal() const
{
	return RepresentedWidget.IsValid() && RepresentedWidget->IsModal();
}

FSGUIActionNodeRef FSGUIActionNode::AddChildNode(USGUIActionNodeWidget& InActionNodeWidget)
{
	InActionNodeWidget.OnNativeDestruct.AddSP(this, &FSGUIActionNode::HandleChildNodeDestruct);

	FSGUIActionNodeRef NewNode = MakeShareable(new FSGUIActionNode(InActionNodeWidget));
	Children.Add(NewNode);
	return NewNode;
}

void FSGUIActionNode::AddBinding(FSGUIActionBindingHandle Handle, FSGUIActionBinding& Binding)
{
	if (!Binding.OwningNode.IsValid())
	{
		Bindings.Add(Handle);
		Binding.OwningNode = AsShared();
	}
}

void FSGUIActionNode::RemoveBinding(FSGUIActionBindingHandle Handle)
{
	Bindings.Remove(Handle);
}

void FSGUIActionNode::HandleChildNodeDestruct(UUserWidget* ChildWidget)
{
	if (ChildWidget)
	{
		ChildWidget->OnNativeDestruct.RemoveAll(this);

		const int32 ChildToRemoveIdx = Children.IndexOfByPredicate([ChildWidget](const FSGUIActionNodeRef& Node) { return Node->GetWidget() == ChildWidget; });
		if (ChildToRemoveIdx != INDEX_NONE)
		{
			Children.RemoveAt(ChildToRemoveIdx);
		}
	}
}
