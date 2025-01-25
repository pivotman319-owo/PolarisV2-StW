#include "globals.h"

namespace polaris::globals
{
	SDK::UWorld** gpWorld;
	SDK::ULevel* gpLevel;
	SDK::TArray<SDK::AActor*>* gpActors;

	SDK::UGameInstance* gpGameInstance;

	SDK::TArray<SDK::ULocalPlayer*> gpLocalPlayers;
	SDK::ULocalPlayer* gpLocalPlayer;

	SDK::APlayerController* gpPlayerController;
	SDK::UObject* (*StaticConstructObject_Internal)(SDK::UClass* InClass, SDK::UObject* InOuter, SDK::FName InName, int64_t InFlags, SDK::FUObjectItem::ObjectFlags InternalSetFlags, SDK::UObject* InTemplate, bool bCopyTransientsFromClassDefaults, void* InInstanceGraph, bool bAssumeTemplateIsArchetype);
}