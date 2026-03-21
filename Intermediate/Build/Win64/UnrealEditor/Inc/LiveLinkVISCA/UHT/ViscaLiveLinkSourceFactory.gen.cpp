// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ViscaLiveLinkSourceFactory.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeViscaLiveLinkSourceFactory() {}

// ********** Begin Cross Module References ********************************************************
LIVELINKINTERFACE_API UClass* Z_Construct_UClass_ULiveLinkSourceFactory();
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaLiveLinkSourceFactory();
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaLiveLinkSourceFactory_NoRegister();
UPackage* Z_Construct_UPackage__Script_LiveLinkVISCA();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UViscaLiveLinkSourceFactory **********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory;
UClass* UViscaLiveLinkSourceFactory::GetPrivateStaticClass()
{
	using TClass = UViscaLiveLinkSourceFactory;
	if (!Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("ViscaLiveLinkSourceFactory"),
			Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory.InnerSingleton,
			StaticRegisterNativesUViscaLiveLinkSourceFactory,
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
	return Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory.InnerSingleton;
}
UClass* Z_Construct_UClass_UViscaLiveLinkSourceFactory_NoRegister()
{
	return UViscaLiveLinkSourceFactory::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "ViscaLiveLinkSourceFactory.h" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSourceFactory.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UViscaLiveLinkSourceFactory constinit property declarations **************
// ********** End Class UViscaLiveLinkSourceFactory constinit property declarations ****************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UViscaLiveLinkSourceFactory>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics
UObject* (*const Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ULiveLinkSourceFactory,
	(UObject* (*)())Z_Construct_UPackage__Script_LiveLinkVISCA,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics::ClassParams = {
	&UViscaLiveLinkSourceFactory::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics::Class_MetaDataParams), Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics::Class_MetaDataParams)
};
void UViscaLiveLinkSourceFactory::StaticRegisterNativesUViscaLiveLinkSourceFactory()
{
}
UClass* Z_Construct_UClass_UViscaLiveLinkSourceFactory()
{
	if (!Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory.OuterSingleton, Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory.OuterSingleton;
}
UViscaLiveLinkSourceFactory::UViscaLiveLinkSourceFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UViscaLiveLinkSourceFactory);
UViscaLiveLinkSourceFactory::~UViscaLiveLinkSourceFactory() {}
// ********** End Class UViscaLiveLinkSourceFactory ************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h__Script_LiveLinkVISCA_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UViscaLiveLinkSourceFactory, UViscaLiveLinkSourceFactory::StaticClass, TEXT("UViscaLiveLinkSourceFactory"), &Z_Registration_Info_UClass_UViscaLiveLinkSourceFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UViscaLiveLinkSourceFactory), 1339653008U) },
	};
}; // Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h__Script_LiveLinkVISCA_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h__Script_LiveLinkVISCA_1083818600{
	TEXT("/Script/LiveLinkVISCA"),
	Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h__Script_LiveLinkVISCA_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h__Script_LiveLinkVISCA_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
