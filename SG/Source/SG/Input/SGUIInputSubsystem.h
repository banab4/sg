#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Containers/Ticker.h"
#include "Engine/EngineBaseTypes.h"
#include "InputCoreTypes.h"
#include "Input/SGUIActionBinding.h"
#include "UObject/WeakObjectPtr.h"
#include "SGUIInputSubsystem.generated.h"

class USGUIActionNodeWidget;
class USGUserWidget;
class UUserWidget;
class UWidget;

class FSGUIActionNode;
using FSGUIActionNodePtr = TSharedPtr<FSGUIActionNode>;
using FSGUIActionNodeRef = TSharedRef<FSGUIActionNode>;

enum class ESGRouteUIInputResult : uint8
{
	Handled,
	BlockedByModal,
	Unhandled,
};

UCLASS()
class SG_API USGUIInputSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static bool CanWidgetReceiveInput(const UWidget* Widget);

	const USGUserWidget* FindReceivableWidget(const FKey& Key) const;
	FSGUIActionNodePtr GetRootNode() const { return RootNode; }

	ESGRouteUIInputResult ProcessInput(FKey Key, EInputEvent InputEvent) const;

	void SetRootNodeWidget(USGUIActionNodeWidget* InRootWidget);

	FSGUIActionBindingHandle AddBinding(USGUserWidget& Widget, const FPrimaryAssetId& ActionId, const FSimpleDelegate& OnExecuted);
	void RemoveBinding(FSGUIActionBindingHandle Handle);
	TSharedPtr<const FSGUIActionBinding> FindBinding(FSGUIActionBindingHandle Handle) const;

	void NotifyUserWidgetConstructed(USGUserWidget& Widget);
	void NotifyUserWidgetDestructed(USGUserWidget& Widget);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	FSGUIActionNodePtr FindNode(const USGUIActionNodeWidget* Widget) const;
	FSGUIActionNodePtr FindOwningNode(const UWidget& Widget) const;

	void RegisterWidgetBindings(const FSGUIActionNodeRef& Node, const TArray<FSGUIActionBindingHandle>& BindingHandles);

	bool Tick(float DeltaTime);
	void ProcessPendingWidgets();
	void AssembleTreeRecursive(const FSGUIActionNodeRef& CurNode, TMap<USGUIActionNodeWidget*, TArray<USGUIActionNodeWidget*>>& WidgetsByDirectParent);

	void HandleRootWidgetDestructed(UUserWidget* RootWidget);
	void HandlePostGarbageCollect();
	void LoadUIInputActions();

private:
	TArray<TWeakObjectPtr<USGUserWidget>> PendingWidgets;

	TMap<int32, TSharedRef<FSGUIActionBinding>> Bindings;
	int32 NextBindingId = 0;

	FSGUIActionNodePtr RootNode;
	FTSTicker::FDelegateHandle TickHandle;
};
