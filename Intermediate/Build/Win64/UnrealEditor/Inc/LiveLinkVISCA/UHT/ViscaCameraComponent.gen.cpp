// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ViscaCameraComponent.h"
#include "LiveLinkTypes.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeViscaCameraComponent() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UActorComponent();
LIVELINKINTERFACE_API UScriptStruct* Z_Construct_UScriptStruct_FLiveLinkSubjectName();
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaCameraComponent();
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaCameraComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_LiveLinkVISCA();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UViscaCameraComponent ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UViscaCameraComponent;
UClass* UViscaCameraComponent::GetPrivateStaticClass()
{
	using TClass = UViscaCameraComponent;
	if (!Z_Registration_Info_UClass_UViscaCameraComponent.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("ViscaCameraComponent"),
			Z_Registration_Info_UClass_UViscaCameraComponent.InnerSingleton,
			StaticRegisterNativesUViscaCameraComponent,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UViscaCameraComponent.InnerSingleton;
}
UClass* Z_Construct_UClass_UViscaCameraComponent_NoRegister()
{
	return UViscaCameraComponent::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UViscaCameraComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "LiveLink" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Applies Live Link Camera-role data from a VISCA source to the owner's camera (Cine Camera recommended).\n * Evaluation and editor ticking options mirror Live Link Component Controller for consistent Sequencer and editor behavior.\n */" },
#endif
		{ "DisplayName", "Live Link VISCA Camera" },
		{ "IncludePath", "ViscaCameraComponent.h" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Applies Live Link Camera-role data from a VISCA source to the owner's camera (Cine Camera recommended).\nEvaluation and editor ticking options mirror Live Link Component Controller for consistent Sequencer and editor behavior." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LiveLinkSubject_MetaData[] = {
		{ "Category", "Live Link" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Live Link subject to read (must use the Camera role). */" },
#endif
		{ "DisplayName", "Subject Name" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Live Link subject to read (must use the Camera role)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bEvaluateLiveLink_MetaData[] = {
		{ "Category", "Live Link" },
		{ "DisplayName", "Evaluate Live Link" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If false, does not evaluate Live Link (effectively pauses driving)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDisableEvaluateLiveLinkWhenSpawnable_MetaData[] = {
		{ "Category", "Live Link" },
		{ "DisplayName", "Disable Evaluate Live Link when Spawnable" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If true, does not evaluate Live Link when the owner is a Level Sequence spawnable." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bUpdateInEditor_MetaData[] = {
		{ "Category", "Live Link" },
		{ "DisplayName", "Update in Editor" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When enabled, ticks in the editor (level viewport), not only during Play In Editor." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bUpdateInPreviewEditor_MetaData[] = {
		{ "Category", "Live Link" },
		{ "DisplayName", "Update in Preview Editor" },
		{ "EditCondition", "bUpdateInEditor" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If true, ticks when the world is an editor preview (for example Blueprint Class editor)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bTreatIncomingValuesAsNormalized_MetaData[] = {
		{ "Category", "Camera Mapping" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** When true, focal length, aperture, and focus distance are treated as normalized 0\xe2\x80\x93""1 and mapped to the Cine Camera lens range. */" },
#endif
		{ "DisplayName", "Map Lens Values from 0\xe2\x80\x93""1" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When true, focal length, aperture, and focus distance are treated as normalized 0\xe2\x80\x93""1 and mapped to the Cine Camera lens range." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UViscaCameraComponent constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_LiveLinkSubject;
	static void NewProp_bEvaluateLiveLink_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bEvaluateLiveLink;
	static void NewProp_bDisableEvaluateLiveLinkWhenSpawnable_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDisableEvaluateLiveLinkWhenSpawnable;
	static void NewProp_bUpdateInEditor_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bUpdateInEditor;
	static void NewProp_bUpdateInPreviewEditor_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bUpdateInPreviewEditor;
	static void NewProp_bTreatIncomingValuesAsNormalized_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bTreatIncomingValuesAsNormalized;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UViscaCameraComponent constinit property declarations **********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UViscaCameraComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UViscaCameraComponent_Statics

// ********** Begin Class UViscaCameraComponent Property Definitions *******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_LiveLinkSubject = { "LiveLinkSubject", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UViscaCameraComponent, LiveLinkSubject), Z_Construct_UScriptStruct_FLiveLinkSubjectName, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LiveLinkSubject_MetaData), NewProp_LiveLinkSubject_MetaData) }; // 1111008801
void Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bEvaluateLiveLink_SetBit(void* Obj)
{
	((UViscaCameraComponent*)Obj)->bEvaluateLiveLink = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bEvaluateLiveLink = { "bEvaluateLiveLink", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UViscaCameraComponent), &Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bEvaluateLiveLink_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bEvaluateLiveLink_MetaData), NewProp_bEvaluateLiveLink_MetaData) };
void Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bDisableEvaluateLiveLinkWhenSpawnable_SetBit(void* Obj)
{
	((UViscaCameraComponent*)Obj)->bDisableEvaluateLiveLinkWhenSpawnable = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bDisableEvaluateLiveLinkWhenSpawnable = { "bDisableEvaluateLiveLinkWhenSpawnable", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UViscaCameraComponent), &Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bDisableEvaluateLiveLinkWhenSpawnable_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDisableEvaluateLiveLinkWhenSpawnable_MetaData), NewProp_bDisableEvaluateLiveLinkWhenSpawnable_MetaData) };
void Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInEditor_SetBit(void* Obj)
{
	((UViscaCameraComponent*)Obj)->bUpdateInEditor = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInEditor = { "bUpdateInEditor", nullptr, (EPropertyFlags)0x0010040000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UViscaCameraComponent), &Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInEditor_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bUpdateInEditor_MetaData), NewProp_bUpdateInEditor_MetaData) };
void Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInPreviewEditor_SetBit(void* Obj)
{
	((UViscaCameraComponent*)Obj)->bUpdateInPreviewEditor = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInPreviewEditor = { "bUpdateInPreviewEditor", nullptr, (EPropertyFlags)0x0010040000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UViscaCameraComponent), &Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInPreviewEditor_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bUpdateInPreviewEditor_MetaData), NewProp_bUpdateInPreviewEditor_MetaData) };
void Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized_SetBit(void* Obj)
{
	((UViscaCameraComponent*)Obj)->bTreatIncomingValuesAsNormalized = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized = { "bTreatIncomingValuesAsNormalized", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UViscaCameraComponent), &Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bTreatIncomingValuesAsNormalized_MetaData), NewProp_bTreatIncomingValuesAsNormalized_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UViscaCameraComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_LiveLinkSubject,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bEvaluateLiveLink,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bDisableEvaluateLiveLinkWhenSpawnable,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInEditor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bUpdateInPreviewEditor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaCameraComponent_Statics::PropPointers) < 2048);
// ********** End Class UViscaCameraComponent Property Definitions *********************************
UObject* (*const Z_Construct_UClass_UViscaCameraComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UActorComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_LiveLinkVISCA,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaCameraComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UViscaCameraComponent_Statics::ClassParams = {
	&UViscaCameraComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UViscaCameraComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UViscaCameraComponent_Statics::PropPointers),
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaCameraComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UViscaCameraComponent_Statics::Class_MetaDataParams)
};
void UViscaCameraComponent::StaticRegisterNativesUViscaCameraComponent()
{
}
UClass* Z_Construct_UClass_UViscaCameraComponent()
{
	if (!Z_Registration_Info_UClass_UViscaCameraComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UViscaCameraComponent.OuterSingleton, Z_Construct_UClass_UViscaCameraComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UViscaCameraComponent.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UViscaCameraComponent);
UViscaCameraComponent::~UViscaCameraComponent() {}
// ********** End Class UViscaCameraComponent ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UViscaCameraComponent, UViscaCameraComponent::StaticClass, TEXT("UViscaCameraComponent"), &Z_Registration_Info_UClass_UViscaCameraComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UViscaCameraComponent), 490210161U) },
	};
}; // Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_3363579435{
	TEXT("/Script/LiveLinkVISCA"),
	Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
