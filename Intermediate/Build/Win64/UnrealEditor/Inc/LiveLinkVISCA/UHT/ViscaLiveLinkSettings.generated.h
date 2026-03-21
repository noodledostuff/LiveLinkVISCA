// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "ViscaLiveLinkSettings.h"

#ifdef LIVELINKVISCA_ViscaLiveLinkSettings_generated_h
#error "ViscaLiveLinkSettings.generated.h already included, missing '#pragma once' in ViscaLiveLinkSettings.h"
#endif
#define LIVELINKVISCA_ViscaLiveLinkSettings_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FViscaReceiverConfig **********************************************
struct Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics;
#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_24_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics; \
	LIVELINKVISCA_API static class UScriptStruct* StaticStruct();


struct FViscaReceiverConfig;
// ********** End ScriptStruct FViscaReceiverConfig ************************************************

// ********** Begin Class UViscaLiveLinkSourceSettings *********************************************
#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetListenInterfaceOptions);


struct Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics;
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaLiveLinkSourceSettings_NoRegister();

#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUViscaLiveLinkSourceSettings(); \
	friend struct ::Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend LIVELINKVISCA_API UClass* ::Z_Construct_UClass_UViscaLiveLinkSourceSettings_NoRegister(); \
public: \
	DECLARE_CLASS2(UViscaLiveLinkSourceSettings, ULiveLinkSourceSettings, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/LiveLinkVISCA"), Z_Construct_UClass_UViscaLiveLinkSourceSettings_NoRegister) \
	DECLARE_SERIALIZER(UViscaLiveLinkSourceSettings)


#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UViscaLiveLinkSourceSettings(); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UViscaLiveLinkSourceSettings(UViscaLiveLinkSourceSettings&&) = delete; \
	UViscaLiveLinkSourceSettings(const UViscaLiveLinkSourceSettings&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UViscaLiveLinkSourceSettings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UViscaLiveLinkSourceSettings); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UViscaLiveLinkSourceSettings) \
	NO_API virtual ~UViscaLiveLinkSourceSettings();


#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_50_PROLOG
#define FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_INCLASS_NO_PURE_DECLS \
	FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h_53_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UViscaLiveLinkSourceSettings;

// ********** End Class UViscaLiveLinkSourceSettings ***********************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h

// ********** Begin Enum EViscaReceiverTransportMode ***********************************************
#define FOREACH_ENUM_EVISCARECEIVERTRANSPORTMODE(op) \
	op(EViscaReceiverTransportMode::UDP) \
	op(EViscaReceiverTransportMode::TCP) 

enum class EViscaReceiverTransportMode : uint8;
template<> struct TIsUEnumClass<EViscaReceiverTransportMode> { enum { Value = true }; };
template<> LIVELINKVISCA_NON_ATTRIBUTED_API UEnum* StaticEnum<EViscaReceiverTransportMode>();
// ********** End Enum EViscaReceiverTransportMode *************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
