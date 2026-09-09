// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#include "AvianWingRigEditor.h"
#include "AvianWingPresetDataAsset.h"
#include "AvianWingMathLibrary.h"
#include "AvianWingTypes.h"
#include "ToolMenus.h"
#include "Misc/MessageDialog.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#define LOCTEXT_NAMESPACE "FAvianWingRigEditorModule"

void FAvianWingRigEditorModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAvianWingRigEditorModule::RegisterMenus));
}

void FAvianWingRigEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	UnregisterMenus();
}

void FAvianWingRigEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	FToolMenuSection& Section = Menu->FindOrAddSection("AvianWingRig");
	Section.Label = LOCTEXT("AvianWingSection", "AvianWingRig");

	Section.AddMenuEntry(
		"AWR_PrintChecklist",
		LOCTEXT("PrintChecklist", "AvianWingRig: Print Setup Checklist"),
		LOCTEXT("PrintChecklistTooltip", "Log a Control Rig / AnimBP wiring checklist to the Output Log."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FAvianWingRigEditorModule::OnPrintSetupChecklist)));

	Section.AddMenuEntry(
		"AWR_CreateMagpiePreset",
		LOCTEXT("CreateMagpie", "AvianWingRig: Create Magpie Preset DataAsset"),
		LOCTEXT("CreateMagpieTooltip", "Create /Game/AvianWingRig/DA_AWR_Magpie with Blender-parity defaults."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FAvianWingRigEditorModule::OnCreateMagpiePresetAsset)));
}

void FAvianWingRigEditorModule::UnregisterMenus()
{
}

void FAvianWingRigEditorModule::OnPrintSetupChecklist()
{
	const FAvianWingFoldPose Closed = UAvianWingMathLibrary::GetDefaultClosedPose();
	const FAvianWingFoldPose Open = UAvianWingMathLibrary::GetDefaultOpenPose();

	UE_LOG(LogTemp, Display, TEXT("========== AvianWingRig Setup Checklist =========="));
	UE_LOG(LogTemp, Display, TEXT("1. Enable plugin AvianWingRig (Edit > Plugins)."));
	UE_LOG(LogTemp, Display, TEXT("2. Skeleton bones: AWR_L/R_Shoulder, _Elbow, _Wrist, _P##, _S##, _T##, _Alula."));
	UE_LOG(LogTemp, Display, TEXT("3. Create Control Rig — do NOT expect shipped .uasset CR binaries."));
	UE_LOG(LogTemp, Display, TEXT("4. Expose floats: awr_fold, awr_alula, awr_fan_influence, awr_elastic, awr_ligament_strength."));
	UE_LOG(LogTemp, Display, TEXT("5. L0: LerpFoldPose(Fold, Closed, Open) -> Set Bone Transform (local)."));
	UE_LOG(LogTemp, Display, TEXT("   Closed deg: S=%.2f E=%.2f W=%.2f"), Closed.ShoulderDegrees, Closed.ElbowDegrees, Closed.WristDegrees);
	UE_LOG(LogTemp, Display, TEXT("   Open   deg: S=%.2f E=%.2f W=%.2f"), Open.ShoulderDegrees, Open.ElbowDegrees, Open.WristDegrees);
	UE_LOG(LogTemp, Display, TEXT("6. L1: RemigeFanAngle per primary/secondary/tertial; PRIMARY_COUNT=10, max~0.95 rad."));
	UE_LOG(LogTemp, Display, TEXT("7. L2: EvaluateTrajectory, ApplyLigamentNeighbor, AlulaAngle, ApplyElasticFold."));
	UE_LOG(LogTemp, Display, TEXT("8. AnimBP: Control Rig node pins OR UAvianWingComponent + IAvianWingPoseConsumer."));
	UE_LOG(LogTemp, Display, TEXT("9. See docs/WALKTHROUGH.md and docs/CONTROL_RIG.md in the plugin repo."));
	UE_LOG(LogTemp, Display, TEXT("10. Blender sibling: https://github.com/hallower1980/avian-wing-rig"));
	UE_LOG(LogTemp, Display, TEXT("==================================================="));

	FMessageDialog::Open(EAppMsgType::Ok,
		LOCTEXT("ChecklistDone", "AvianWingRig setup checklist printed to the Output Log (LogTemp)."));
}

void FAvianWingRigEditorModule::OnCreateMagpiePresetAsset()
{
	const FString PackagePath = TEXT("/Game/AvianWingRig/DA_AWR_Magpie");
	UPackage* Package = CreatePackage(*PackagePath);
	Package->FullyLoad();

	UAvianWingPresetDataAsset* Asset = NewObject<UAvianWingPresetDataAsset>(
		Package, UAvianWingPresetDataAsset::StaticClass(), FName(TEXT("DA_AWR_Magpie")), RF_Public | RF_Standalone);
	Asset->ApplyMagpieDefaults();

	FAssetRegistryModule::AssetCreated(Asset);
	Package->MarkPackageDirty();

	const FString PackageFileName = FPackageName::LongPackageNameToFilename(
		PackagePath, FPackageName::GetAssetPackageExtension());

	// Ensure parent directory exists on disk
	const FString Dir = FPaths::GetPath(PackageFileName);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	const bool bSaved = UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs);

	if (bSaved)
	{
		UE_LOG(LogTemp, Display, TEXT("AvianWingRig: created Magpie preset at %s"), *PackagePath);
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::Format(LOCTEXT("PresetCreated", "Created Magpie preset:\n{0}"), FText::FromString(PackagePath)));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AvianWingRig: failed to save Magpie preset (package may still exist in memory)."));
		FMessageDialog::Open(EAppMsgType::Ok,
			LOCTEXT("PresetFail", "Could not save package to disk. Asset may still be in /Game/AvianWingRig/ — save manually."));
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAvianWingRigEditorModule, AvianWingRigEditor)
