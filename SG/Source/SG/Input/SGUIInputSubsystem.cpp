#include "SGUIInputSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/LocalPlayer.h"
#include "Slate/SObjectWidget.h"
#include "UObject/UObjectGlobals.h"
#include "UI/SGUIActionNodeWidget.h"
#include "Input/SGUIActionNode.h"
#include "Input/SGUIInputAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SGUIInputSubsystem)

namespace
{
	USGUIActionNodeWidget* FindOwningActionNodeWidget(const UWidget& Widget)
	{
		TSharedPtr<SWidget> CurWidget = Widget.GetCachedWidget();
		ULocalPlayer* OwningLocalPlayer = Widget.GetOwningLocalPlayer();
		if (!CurWidget.IsValid() || !OwningLocalPlayer)
		{
			return nullptr;
		}

		while (CurWidget)
		{
			CurWidget = CurWidget->GetParentWidget();
			if (CurWidget && CurWidget->GetType().IsEqual(TEXT("SObjectWidget")))
			{
				USGUIActionNodeWidget* NodeWidget = Cast<USGUIActionNodeWidget>(StaticCastSharedPtr<SObjectWidget>(CurWidget)->GetWidgetObject());
				if (NodeWidget && NodeWidget->GetOwningLocalPlayer() == OwningLocalPlayer)
				{
					return NodeWidget;
				}
			}
		}

		return nullptr;
	}

	bool CanSlateWidgetReceiveInput(SWidget& SlateWidget)
	{
		if (!SlateWidget.IsEnabled())
		{
			return false;
		}

		const EVisibility WidgetVisibility = SlateWidget.GetVisibility();
		if (!WidgetVisibility.IsVisible() || !WidgetVisibility.AreChildrenHitTestVisible())
		{
			return false;
		}
		
		TSharedPtr<SWidget> ParentWidget = SlateWidget.GetParentWidget();
		return !ParentWidget || ParentWidget->ValidatePathToChild(&SlateWidget);
	}

	FSGUIActionNodePtr FindNodeRecursive(const FSGUIActionNodePtr& CurrentNode, const USGUIActionNodeWidget& Widget)
	{
		if (!CurrentNode)
		{
			return nullptr;
		}

		if (CurrentNode->GetWidget() == &Widget)
		{
			return CurrentNode;
		}

		for (const FSGUIActionNodeRef& Child : CurrentNode->GetChildren())
		{
			if (FSGUIActionNodePtr Found = FindNodeRecursive(Child, Widget))
			{
				return Found;
			}
		}

		return nullptr;
	}

	bool FindReceivableBindingInNode(const FSGUIActionNode& Node, const FKey& Key, const USGUIInputSubsystem& InputSubsystem, FSGUIActionBindingHandle& OutHandle)
	{
		if (!Node.IsReceivingInput())
		{
			return false;
		}

		const TArray<FSGUIActionNodeRef>& Children = Node.GetChildren();
		for (int32 i = Children.Num() - 1; i >= 0; --i)
		{
			if (FindReceivableBindingInNode(*Children[i], Key, InputSubsystem, OutHandle))
			{
				return true;
			}
		}

		const TArray<FSGUIActionBindingHandle>& Bindings = Node.GetBindings();
		for (int32 i = Bindings.Num() - 1; i >= 0; --i)
		{
			const TSharedPtr<const FSGUIActionBinding> Binding = InputSubsystem.FindBinding(Bindings[i]);
			if (Binding && Binding->IsReceivable(Key))
			{
				OutHandle = Bindings[i];
				return true;
			}
		}

		return Node.IsModal();
	}
}

bool USGUIInputSubsystem::CanWidgetReceiveInput(const UWidget* Widget)
{
	TSharedPtr<SWidget> SlateWidget = Widget ? Widget->GetCachedWidget() : nullptr;
	if (!SlateWidget)
	{
		return false;
	}

	for (; SlateWidget; SlateWidget = SlateWidget->GetParentWidget())
	{
		if (!CanSlateWidgetReceiveInput(*SlateWidget))
		{
			return false;
		}
	}
	return true;
}

void USGUIInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTSTicker::RemoveTicker(TickHandle);
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &USGUIInputSubsystem::Tick));
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &USGUIInputSubsystem::HandlePostGarbageCollect);

	LoadUIInputActions();
}

void USGUIInputSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FTSTicker::RemoveTicker(TickHandle);
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);

	SetRootNodeWidget(nullptr);
	PendingWidgets.Reset();
	Bindings.Reset();
}

ESGRouteUIInputResult USGUIInputSubsystem::ProcessInput(FKey Key, EInputEvent InputEvent) const
{
	if (const FSGUIActionNodePtr Node = RootNode)
	{
		return Node->ProcessNormalInput(Key, InputEvent, *this);
	}
	return ESGRouteUIInputResult::Unhandled;
}

const USGUserWidget* USGUIInputSubsystem::FindReceivableWidget(const FKey& Key) const
{
	FSGUIActionBindingHandle Handle;
	if (RootNode)
	{
		FindReceivableBindingInNode(*RootNode, Key, *this, Handle);
	}

	const TSharedPtr<const FSGUIActionBinding> Binding = FindBinding(Handle);
	return Binding ? Binding->Widget.Get() : nullptr;
}

void USGUIInputSubsystem::SetRootNodeWidget(USGUIActionNodeWidget* InRootWidget)
{
	if (RootNode)
	{
		if (USGUIActionNodeWidget* PrevRootWidget = RootNode->GetWidget())
		{
			PrevRootWidget->OnNativeDestruct.RemoveAll(this);
		}
		RootNode.Reset();
	}

	if (InRootWidget)
	{
		RootNode = FSGUIActionNode::CreateRoot(*InRootWidget);
		RegisterWidgetBindings(RootNode.ToSharedRef(), InRootWidget->GetActionBindings());
		InRootWidget->OnNativeDestruct.AddUObject(this, &USGUIInputSubsystem::HandleRootWidgetDestructed);
	}
}

FSGUIActionBindingHandle USGUIInputSubsystem::AddBinding(USGUserWidget& Widget, const FPrimaryAssetId& ActionId, const FSimpleDelegate& OnExecuted)
{
	for (const FSGUIActionBindingHandle& Handle : Widget.GetActionBindings())
	{
		const TSharedRef<FSGUIActionBinding>* Existing = Bindings.Find(Handle.Id);
		if (Existing && (*Existing)->ActionId == ActionId)
		{
			(*Existing)->OnExecuted.Clear();
			(*Existing)->OnExecuted.Add(OnExecuted);
			return Handle;
		}
	}

	const FSGUIActionBindingHandle Handle{ NextBindingId++ };
	const TSharedRef<FSGUIActionBinding> Binding = MakeShared<FSGUIActionBinding>();
	Binding->ActionId = ActionId;
	Binding->OnExecuted.Add(OnExecuted);
	Binding->Widget = &Widget;
	Bindings.Add(Handle.Id, Binding);

	if (FSGUIActionNodePtr OwnerNode = FindOwningNode(Widget))
	{
		OwnerNode->AddBinding(Handle, *Binding);
	}
	return Handle;
}

void USGUIInputSubsystem::RemoveBinding(FSGUIActionBindingHandle Handle)
{
	if (const TSharedRef<FSGUIActionBinding>* Binding = Bindings.Find(Handle.Id))
	{
		if (FSGUIActionNodePtr OwningNode = (*Binding)->OwningNode.Pin())
		{
			OwningNode->RemoveBinding(Handle);
		}
		Bindings.Remove(Handle.Id);
	}
}

TSharedPtr<const FSGUIActionBinding> USGUIInputSubsystem::FindBinding(FSGUIActionBindingHandle Handle) const
{
	if (const TSharedRef<FSGUIActionBinding>* Binding = Bindings.Find(Handle.Id))
	{
		return *Binding;
	}
	return nullptr;
}

void USGUIInputSubsystem::RegisterWidgetBindings(const FSGUIActionNodeRef& Node, const TArray<FSGUIActionBindingHandle>& BindingHandles)
{
	for (const FSGUIActionBindingHandle& Handle : BindingHandles)
	{
		if (const TSharedRef<FSGUIActionBinding>* Binding = Bindings.Find(Handle.Id))
		{
			Node->AddBinding(Handle, **Binding);
		}
	}
}

void USGUIInputSubsystem::NotifyUserWidgetConstructed(USGUserWidget& Widget)
{
	PendingWidgets.AddUnique(&Widget);
}

void USGUIInputSubsystem::NotifyUserWidgetDestructed(USGUserWidget& Widget)
{
	PendingWidgets.Remove(&Widget);
}

bool USGUIInputSubsystem::Tick(float DeltaTime)
{
	if (PendingWidgets.Num() > 0)
	{
		ProcessPendingWidgets();
	}

	return true;
}

void USGUIInputSubsystem::ProcessPendingWidgets()
{
	TMap<USGUIActionNodeWidget*, TArray<USGUIActionNodeWidget*>> WidgetsByDirectParent;
	for (const TWeakObjectPtr<USGUserWidget>& Widget : PendingWidgets)
	{
		if (USGUIActionNodeWidget* NodeWidget = Cast<USGUIActionNodeWidget>(Widget.Get()))
		{
			if (NodeWidget->GetCachedWidget())
			{
				if (USGUIActionNodeWidget* ParentNodeWidget = FindOwningActionNodeWidget(*NodeWidget))
				{
					WidgetsByDirectParent.FindOrAdd(ParentNodeWidget).Add(NodeWidget);
				}
			}
		}
	}

	if (RootNode)
	{
		AssembleTreeRecursive(RootNode.ToSharedRef(), WidgetsByDirectParent);
	}

	int32 NumWidgetsLeft = INDEX_NONE;
	while (WidgetsByDirectParent.Num() > 0 && NumWidgetsLeft != WidgetsByDirectParent.Num())
	{
		NumWidgetsLeft = WidgetsByDirectParent.Num();
		for (const TPair<USGUIActionNodeWidget*, TArray<USGUIActionNodeWidget*>>& ParentChildrenPair : WidgetsByDirectParent)
		{
			if (FSGUIActionNodePtr ExistingNode = FindNode(ParentChildrenPair.Key))
			{
				AssembleTreeRecursive(ExistingNode.ToSharedRef(), WidgetsByDirectParent);
				break;
			}
		}
	}
	
	for (const TWeakObjectPtr<USGUserWidget>& Widget : PendingWidgets)
	{
		if (Widget.IsValid() && !Widget->IsA<USGUIActionNodeWidget>() && Widget->GetCachedWidget())
		{
			if (FSGUIActionNodePtr OwnerNode = FindOwningNode(*Widget))
			{
				RegisterWidgetBindings(OwnerNode.ToSharedRef(), Widget->GetActionBindings());
			}
		}
	}

	PendingWidgets.Reset();
}

void USGUIInputSubsystem::AssembleTreeRecursive(const FSGUIActionNodeRef& CurNode, TMap<USGUIActionNodeWidget*, TArray<USGUIActionNodeWidget*>>& WidgetsByDirectParent)
{
	TArray<USGUIActionNodeWidget*> Children;
	if (WidgetsByDirectParent.RemoveAndCopyValue(CurNode->GetWidget(), Children))
	{
		for (USGUIActionNodeWidget* Widget : Children)
		{
			const FSGUIActionNodeRef ChildNode = CurNode->AddChildNode(*Widget);
			RegisterWidgetBindings(ChildNode, Widget->GetActionBindings());
			AssembleTreeRecursive(ChildNode, WidgetsByDirectParent);
		}
	}
}

FSGUIActionNodePtr USGUIInputSubsystem::FindNode(const USGUIActionNodeWidget* Widget) const
{
	return Widget ? FindNodeRecursive(RootNode, *Widget) : nullptr;
}

FSGUIActionNodePtr USGUIInputSubsystem::FindOwningNode(const UWidget& Widget) const
{
	if (const USGUIActionNodeWidget* NodeWidget = Cast<USGUIActionNodeWidget>(&Widget))
	{
		return FindNode(NodeWidget);
	}
	return FindNode(FindOwningActionNodeWidget(Widget));
}

void USGUIInputSubsystem::HandleRootWidgetDestructed(UUserWidget* RootWidget)
{
	SetRootNodeWidget(nullptr);
}

void USGUIInputSubsystem::HandlePostGarbageCollect()
{
	TArray<FSGUIActionBindingHandle> StaleHandles;
	for (const TPair<int32, TSharedRef<FSGUIActionBinding>>& Pair : Bindings)
	{
		if (!Pair.Value->Widget.IsValid())
		{
			StaleHandles.Add(FSGUIActionBindingHandle{ Pair.Key });
		}
	}

	for (const FSGUIActionBindingHandle& Handle : StaleHandles)
	{
		RemoveBinding(Handle);
	}
}

void USGUIInputSubsystem::LoadUIInputActions()
{
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> UIInputActionIds;
	AssetManager.GetPrimaryAssetIdList(USGUIInputAction::PrimaryAssetType, UIInputActionIds);
	if (const TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssets(UIInputActionIds))
	{
		Handle->WaitUntilComplete();
	}
}
