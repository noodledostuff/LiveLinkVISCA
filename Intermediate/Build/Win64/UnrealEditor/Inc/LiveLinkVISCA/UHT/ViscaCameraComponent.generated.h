// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "ViscaCameraComponent.h"

#ifdef LIVELINKVISCA_ViscaCameraComponent_generated_h
#error "ViscaCameraComponent.generated.h already included, missing '#pragma once' in ViscaCameraComponent.h"
#endif
#define LIVELINKVISCA_ViscaCameraComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UViscaCameraComponent ****************************************************
struct Z_Construct_UClass_UViscaCameraComponent_Statics;
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaCameraComponent_NoRegister();

#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUViscaCameraComponent(); \
	friend struct ::Z_Construct_UClass_UViscaCameraComponent_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend LIVELINKVISCA_API UClass* ::Z_Construct_UClass_UViscaCameraComponent_NoRegister(); \
public: \
	DECLARE_CLASS2(UViscaCameraComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/LiveLinkVISCA"), Z_Construct_UClass_UViscaCameraComponent_NoRegister) \
	DECLARE_SERIALIZER(UViscaCameraComponent)


#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h_13_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UViscaCameraComponent(UViscaCameraComponent&&) = delete; \
	UViscaCameraComponent(const UViscaCameraComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UViscaCameraComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UViscaCameraComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UViscaCameraComponent) \
	NO_API virtual ~UViscaCameraComponent();


#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h_10_PROLOG
#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h_13_INCLASS_NO_PURE_DECLS \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UViscaCameraComponent;

// ********** End Class UViscaCameraComponent ******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaCameraComponent_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
