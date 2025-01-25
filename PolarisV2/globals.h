#ifndef GLOBALS_H
#define GLOBALS_H

#include "SDK.hpp"

namespace polaris::globals
{
	extern SDK::UWorld** gpWorld;
	extern SDK::ULevel* gpLevel;
	extern SDK::TArray<SDK::AActor*>* gpActors;

	extern SDK::UGameInstance* gpGameInstance;

	extern SDK::TArray<SDK::ULocalPlayer*> gpLocalPlayers;
	extern SDK::ULocalPlayer* gpLocalPlayer;

	extern SDK::APlayerController* gpPlayerController;
	extern SDK::UObject* (*StaticConstructObject_Internal)(SDK::UClass* InClass, SDK::UObject* InOuter, SDK::FName InName, int64_t InFlags, SDK::FUObjectItem::ObjectFlags InternalSetFlags, SDK::UObject* InTemplate, bool bCopyTransientsFromClassDefaults, void* InInstanceGraph, bool bAssumeTemplateIsArchetype);
};

#endif // !GLOBALS_H