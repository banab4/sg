#pragma once

#include "Input/SGUIInputSubsystem.h"

class USGUIActionNodeWidget;


class SG_API FSGUIActionNode : public TSharedFromThis<FSGUIActionNode>
{
public:
	static FSGUIActionNodeRef CreateRoot(USGUIActionNodeWidget& ActionNodeWidget);

	ESGRouteUIInputResult ProcessNormalInput(const FKey& Key, EInputEvent InputEvent, const USGUIInputSubsystem& InputSubsystem) const;

	bool IsReceivingInput() const;
	bool IsModal() const;

	USGUIActionNodeWidget* GetWidget() const { return RepresentedWidget.Get(); }
	const TArray<FSGUIActionNodeRef>& GetChildren() const { return Children; }
	const TArray<FSGUIActionBindingHandle>& GetBindings() const { return Bindings; }

	FSGUIActionNodeRef AddChildNode(USGUIActionNodeWidget& InActionNodeWidget);
	void AddBinding(FSGUIActionBindingHandle Handle, FSGUIActionBinding& Binding);
	void RemoveBinding(FSGUIActionBindingHandle Handle);

private:
	explicit FSGUIActionNode(USGUIActionNodeWidget& ActionNodeWidget);
	void HandleChildNodeDestruct(UUserWidget* ChildWidget);

	TWeakObjectPtr<USGUIActionNodeWidget> RepresentedWidget;
	TArray<FSGUIActionBindingHandle> Bindings;
	TArray<FSGUIActionNodeRef> Children;
};
