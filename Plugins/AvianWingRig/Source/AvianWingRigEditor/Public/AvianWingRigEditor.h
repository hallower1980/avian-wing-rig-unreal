// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAvianWingRigEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void UnregisterMenus();
	void OnPrintSetupChecklist();
	void OnCreateMagpiePresetAsset();
};
