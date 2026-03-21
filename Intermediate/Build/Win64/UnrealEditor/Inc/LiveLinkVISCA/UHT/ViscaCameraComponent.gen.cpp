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
		{ "ClassGroupNames", "VISCA" },
		{ "IncludePath", "ViscaCameraComponent.h" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LiveLinkSubject_MetaData[] = {
		{ "Category", "LiveLink" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bTreatIncomingValuesAsNormalized_MetaData[] = {
		{ "Category", "Behavior" },
		{ "ModuleRelativePath", "Public/ViscaCameraComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UViscaCameraComponent constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_LiveLinkSubject;
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
void Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized_SetBit(void* Obj)
{
	((UViscaCameraComponent*)Obj)->bTreatIncomingValuesAsNormalized = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized = { "bTreatIncomingValuesAsNormalized", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UViscaCameraComponent), &Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_bTreatIncomingValuesAsNormalized_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bTreatIncomingValuesAsNormalized_MetaData), NewProp_bTreatIncomingValuesAsNormalized_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UViscaCameraComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaCameraComponent_Statics::NewProp_LiveLinkSubject,
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
		{ Z_Construct_UClass_UViscaCameraComponent, UViscaCameraComponent::StaticClass, TEXT("UViscaCameraComponent"), &Z_Registration_Info_UClass_UViscaCameraComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UViscaCameraComponent), 2262088126U) },
	};
}; // Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_4111086892{
	TEXT("/Script/LiveLinkVISCA"),
	Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h__Script_LiveLinkVISCA_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
