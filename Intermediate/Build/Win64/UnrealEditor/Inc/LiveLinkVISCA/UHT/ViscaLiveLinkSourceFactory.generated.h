// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "ViscaLiveLinkSourceFactory.h"

#ifdef LIVELINKVISCA_ViscaLiveLinkSourceFactory_generated_h
#error "ViscaLiveLinkSourceFactory.generated.h already included, missing '#pragma once' in ViscaLiveLinkSourceFactory.h"
#endif
#define LIVELINKVISCA_ViscaLiveLinkSourceFactory_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UViscaLiveLinkSourceFactory **********************************************
struct Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics;
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaLiveLinkSourceFactory_NoRegister();

#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUViscaLiveLinkSourceFactory(); \
	friend struct ::Z_Construct_UClass_UViscaLiveLinkSourceFactory_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend LIVELINKVISCA_API UClass* ::Z_Construct_UClass_UViscaLiveLinkSourceFactory_NoRegister(); \
public: \
	DECLARE_CLASS2(UViscaLiveLinkSourceFactory, ULiveLinkSourceFactory, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/LiveLinkVISCA"), Z_Construct_UClass_UViscaLiveLinkSourceFactory_NoRegister) \
	DECLARE_SERIALIZER(UViscaLiveLinkSourceFactory)


#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h_12_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UViscaLiveLinkSourceFactory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UViscaLiveLinkSourceFactory(UViscaLiveLinkSourceFactory&&) = delete; \
	UViscaLiveLinkSourceFactory(const UViscaLiveLinkSourceFactory&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UViscaLiveLinkSourceFactory); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UViscaLiveLinkSourceFactory); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UViscaLiveLinkSourceFactory) \
	NO_API virtual ~UViscaLiveLinkSourceFactory();


#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h_9_PROLOG
#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h_12_INCLASS_NO_PURE_DECLS \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UViscaLiveLinkSourceFactory;

// ********** End Class UViscaLiveLinkSourceFactory ************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSourceFactory_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
