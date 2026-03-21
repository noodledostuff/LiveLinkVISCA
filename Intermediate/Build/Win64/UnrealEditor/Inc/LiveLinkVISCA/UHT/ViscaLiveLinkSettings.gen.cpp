// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ViscaLiveLinkSettings.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeViscaLiveLinkSettings() {}

// ********** Begin Cross Module References ********************************************************
LIVELINKINTERFACE_API UClass* Z_Construct_UClass_ULiveLinkSourceSettings();
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaLiveLinkSourceSettings();
LIVELINKVISCA_API UClass* Z_Construct_UClass_UViscaLiveLinkSourceSettings_NoRegister();
LIVELINKVISCA_API UEnum* Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode();
LIVELINKVISCA_API UScriptStruct* Z_Construct_UScriptStruct_FViscaReceiverConfig();
UPackage* Z_Construct_UPackage__Script_LiveLinkVISCA();
// ********** End Cross Module References **********************************************************

// ********** Begin Enum EViscaReceiverTransportMode ***********************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EViscaReceiverTransportMode;
static UEnum* EViscaReceiverTransportMode_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EViscaReceiverTransportMode.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EViscaReceiverTransportMode.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode, (UObject*)Z_Construct_UPackage__Script_LiveLinkVISCA(), TEXT("EViscaReceiverTransportMode"));
	}
	return Z_Registration_Info_UEnum_EViscaReceiverTransportMode.OuterSingleton;
}
template<> LIVELINKVISCA_NON_ATTRIBUTED_API UEnum* StaticEnum<EViscaReceiverTransportMode>()
{
	return EViscaReceiverTransportMode_StaticEnum();
}
struct Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** VISCA-over-IP transport: UDP is listen-only; TCP uses connection + ACK/reply semantics. */" },
#endif
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
		{ "TCP.DisplayName", "TCP (with ACK)" },
		{ "TCP.Name", "EViscaReceiverTransportMode::TCP" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "VISCA-over-IP transport: UDP is listen-only; TCP uses connection + ACK/reply semantics." },
#endif
		{ "UDP.DisplayName", "UDP (listen only)" },
		{ "UDP.Name", "EViscaReceiverTransportMode::UDP" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EViscaReceiverTransportMode::UDP", (int64)EViscaReceiverTransportMode::UDP },
		{ "EViscaReceiverTransportMode::TCP", (int64)EViscaReceiverTransportMode::TCP },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_LiveLinkVISCA,
	nullptr,
	"EViscaReceiverTransportMode",
	"EViscaReceiverTransportMode",
	Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics::Enum_MetaDataParams), Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode()
{
	if (!Z_Registration_Info_UEnum_EViscaReceiverTransportMode.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EViscaReceiverTransportMode.InnerSingleton, Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EViscaReceiverTransportMode.InnerSingleton;
}
// ********** End Enum EViscaReceiverTransportMode *************************************************

// ********** Begin ScriptStruct FViscaReceiverConfig **********************************************
struct Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FViscaReceiverConfig); }
	static inline consteval int16 GetStructAlignment() { return alignof(FViscaReceiverConfig); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraName_MetaData[] = {
		{ "Category", "VISCA" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PortString_MetaData[] = {
		{ "Category", "VISCA" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Enter port as text (e.g. 52381). */" },
#endif
		{ "DisplayName", "Port" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enter port as text (e.g. 52381)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Transport_MetaData[] = {
		{ "Category", "VISCA" },
		{ "DisplayName", "Transport" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FViscaReceiverConfig constinit property declarations **************
	static const UECodeGen_Private::FNamePropertyParams NewProp_CameraName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_PortString;
	static const UECodeGen_Private::FBytePropertyParams NewProp_Transport_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_Transport;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FViscaReceiverConfig constinit property declarations ****************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FViscaReceiverConfig>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FViscaReceiverConfig;
class UScriptStruct* FViscaReceiverConfig::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FViscaReceiverConfig.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FViscaReceiverConfig.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FViscaReceiverConfig, (UObject*)Z_Construct_UPackage__Script_LiveLinkVISCA(), TEXT("ViscaReceiverConfig"));
	}
	return Z_Registration_Info_UScriptStruct_FViscaReceiverConfig.OuterSingleton;
	}

// ********** Begin ScriptStruct FViscaReceiverConfig Property Definitions *************************
const UECodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_CameraName = { "CameraName", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FViscaReceiverConfig, CameraName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraName_MetaData), NewProp_CameraName_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_PortString = { "PortString", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FViscaReceiverConfig, PortString), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PortString_MetaData), NewProp_PortString_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_Transport_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_Transport = { "Transport", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FViscaReceiverConfig, Transport), Z_Construct_UEnum_LiveLinkVISCA_EViscaReceiverTransportMode, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Transport_MetaData), NewProp_Transport_MetaData) }; // 3505771629
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_CameraName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_PortString,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_Transport_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewProp_Transport,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FViscaReceiverConfig Property Definitions ***************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_LiveLinkVISCA,
	nullptr,
	&NewStructOps,
	"ViscaReceiverConfig",
	Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::PropPointers),
	sizeof(FViscaReceiverConfig),
	alignof(FViscaReceiverConfig),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FViscaReceiverConfig()
{
	if (!Z_Registration_Info_UScriptStruct_FViscaReceiverConfig.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FViscaReceiverConfig.InnerSingleton, Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FViscaReceiverConfig.InnerSingleton);
}
// ********** End ScriptStruct FViscaReceiverConfig ************************************************

// ********** Begin Class UViscaLiveLinkSourceSettings Function GetListenInterfaceOptions **********
struct Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics
{
	struct ViscaLiveLinkSourceSettings_eventGetListenInterfaceOptions_Parms
	{
		TArray<FString> ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetListenInterfaceOptions constinit property declarations *************
	static const UECodeGen_Private::FStrPropertyParams NewProp_ReturnValue_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetListenInterfaceOptions constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetListenInterfaceOptions Property Definitions ************************
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ViscaLiveLinkSourceSettings_eventGetListenInterfaceOptions_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::NewProp_ReturnValue_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::PropPointers) < 2048);
// ********** End Function GetListenInterfaceOptions Property Definitions **************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UViscaLiveLinkSourceSettings, nullptr, "GetListenInterfaceOptions", 	Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::ViscaLiveLinkSourceSettings_eventGetListenInterfaceOptions_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x40020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::Function_MetaDataParams), Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::ViscaLiveLinkSourceSettings_eventGetListenInterfaceOptions_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UViscaLiveLinkSourceSettings::execGetListenInterfaceOptions)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(TArray<FString>*)Z_Param__Result=P_THIS->GetListenInterfaceOptions();
	P_NATIVE_END;
}
// ********** End Class UViscaLiveLinkSourceSettings Function GetListenInterfaceOptions ************

// ********** Begin Class UViscaLiveLinkSourceSettings *********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings;
UClass* UViscaLiveLinkSourceSettings::GetPrivateStaticClass()
{
	using TClass = UViscaLiveLinkSourceSettings;
	if (!Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("ViscaLiveLinkSourceSettings"),
			Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings.InnerSingleton,
			StaticRegisterNativesUViscaLiveLinkSourceSettings,
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
	return Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings.InnerSingleton;
}
UClass* Z_Construct_UClass_UViscaLiveLinkSourceSettings_NoRegister()
{
	return UViscaLiveLinkSourceSettings::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "ViscaLiveLinkSettings.h" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ListenInterfaceIp_MetaData[] = {
		{ "Category", "VISCA" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Dropdown: All interfaces (0.0.0.0), loopback, and local adapter IPv4s. */" },
#endif
		{ "GetOptions", "GetListenInterfaceOptions" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Dropdown: All interfaces (0.0.0.0), loopback, and local adapter IPv4s." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Receivers_MetaData[] = {
		{ "Category", "VISCA" },
		{ "ModuleRelativePath", "Public/ViscaLiveLinkSettings.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UViscaLiveLinkSourceSettings constinit property declarations *************
	static const UECodeGen_Private::FStrPropertyParams NewProp_ListenInterfaceIp;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Receivers_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Receivers;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UViscaLiveLinkSourceSettings constinit property declarations ***************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("GetListenInterfaceOptions"), .Pointer = &UViscaLiveLinkSourceSettings::execGetListenInterfaceOptions },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UViscaLiveLinkSourceSettings_GetListenInterfaceOptions, "GetListenInterfaceOptions" }, // 2437735504
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UViscaLiveLinkSourceSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics

// ********** Begin Class UViscaLiveLinkSourceSettings Property Definitions ************************
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::NewProp_ListenInterfaceIp = { "ListenInterfaceIp", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UViscaLiveLinkSourceSettings, ListenInterfaceIp), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ListenInterfaceIp_MetaData), NewProp_ListenInterfaceIp_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::NewProp_Receivers_Inner = { "Receivers", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FViscaReceiverConfig, METADATA_PARAMS(0, nullptr) }; // 622858001
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::NewProp_Receivers = { "Receivers", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UViscaLiveLinkSourceSettings, Receivers), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Receivers_MetaData), NewProp_Receivers_MetaData) }; // 622858001
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::NewProp_ListenInterfaceIp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::NewProp_Receivers_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::NewProp_Receivers,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::PropPointers) < 2048);
// ********** End Class UViscaLiveLinkSourceSettings Property Definitions **************************
UObject* (*const Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ULiveLinkSourceSettings,
	(UObject* (*)())Z_Construct_UPackage__Script_LiveLinkVISCA,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::ClassParams = {
	&UViscaLiveLinkSourceSettings::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::Class_MetaDataParams)
};
void UViscaLiveLinkSourceSettings::StaticRegisterNativesUViscaLiveLinkSourceSettings()
{
	UClass* Class = UViscaLiveLinkSourceSettings::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::Funcs));
}
UClass* Z_Construct_UClass_UViscaLiveLinkSourceSettings()
{
	if (!Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings.OuterSingleton, Z_Construct_UClass_UViscaLiveLinkSourceSettings_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings.OuterSingleton;
}
UViscaLiveLinkSourceSettings::UViscaLiveLinkSourceSettings() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UViscaLiveLinkSourceSettings);
UViscaLiveLinkSourceSettings::~UViscaLiveLinkSourceSettings() {}
// ********** End Class UViscaLiveLinkSourceSettings ***********************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ EViscaReceiverTransportMode_StaticEnum, TEXT("EViscaReceiverTransportMode"), &Z_Registration_Info_UEnum_EViscaReceiverTransportMode, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3505771629U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FViscaReceiverConfig::StaticStruct, Z_Construct_UScriptStruct_FViscaReceiverConfig_Statics::NewStructOps, TEXT("ViscaReceiverConfig"),&Z_Registration_Info_UScriptStruct_FViscaReceiverConfig, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FViscaReceiverConfig), 622858001U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UViscaLiveLinkSourceSettings, UViscaLiveLinkSourceSettings::StaticClass, TEXT("UViscaLiveLinkSourceSettings"), &Z_Registration_Info_UClass_UViscaLiveLinkSourceSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UViscaLiveLinkSourceSettings), 211851665U) },
	};
}; // Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_2624743079{
	TEXT("/Script/LiveLinkVISCA"),
	Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics::ScriptStructInfo),
	Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_VISCALivelinkDev_Plugins_LiveLinkVISCA_Source_LiveLinkVISCA_Public_ViscaLiveLinkSettings_h__Script_LiveLinkVISCA_Statics::EnumInfo),
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
