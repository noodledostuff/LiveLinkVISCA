// Copyright (c) 2026 Timothy YU. All rights reserved.

#include "ViscaLiveLinkSettings.h"

#include "Dom/JsonObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

#include "SocketSubsystem.h"
#include "Sockets.h"

namespace ViscaConnectionStringTokens
{
	static const TCHAR* ListenIp = TEXT("ListenIp");
	static const TCHAR* ReceiveMode = TEXT("ReceiveMode");
	static const TCHAR* Receivers = TEXT("Receivers");
	static const TCHAR* CameraName = TEXT("CameraName");
	static const TCHAR* Port = TEXT("Port");
	static const TCHAR* Transport = TEXT("Transport");
	static const TCHAR* ReplyMode = TEXT("ReplyMode"); // legacy preset migration only
}

namespace ViscaJsonHelpers
{
	static int32 PortFromJsonObject(const TSharedPtr<FJsonObject>& ReceiverObject)
	{
		FString PortStr;
		if (ReceiverObject->TryGetStringField(ViscaConnectionStringTokens::Port, PortStr))
		{
			return FMath::Clamp(FCString::Atoi(*PortStr.TrimStartAndEnd()), 1, 65535);
		}
		double PortNumber = 0.0;
		if (ReceiverObject->TryGetNumberField(ViscaConnectionStringTokens::Port, PortNumber))
		{
			return FMath::Clamp(static_cast<int32>(PortNumber), 1, 65535);
		}
		return 52381;
	}

	static EViscaReceiverTransportMode TransportFromJsonObject(const TSharedPtr<FJsonObject>& ReceiverObject)
	{
		FString ModeString;
		if (!ReceiverObject->TryGetStringField(ViscaConnectionStringTokens::Transport, ModeString))
		{
			ReceiverObject->TryGetStringField(TEXT("Mode"), ModeString);
		}
		return ModeString.Equals(TEXT("TCP"), ESearchCase::IgnoreCase)
			? EViscaReceiverTransportMode::TCP
			: EViscaReceiverTransportMode::UDP;
	}

	static bool IsLegacyBidirectionalString(const FString& In)
	{
		return In.Equals(TEXT("Bidirectional"), ESearchCase::IgnoreCase);
	}
}

FViscaLiveLinkConnectionSettings FViscaLiveLinkConnectionSettings::FromString(const FString& InConnectionString)
{
	FViscaLiveLinkConnectionSettings Parsed;

	FString JsonPayload;
	if (!FParse::Value(*InConnectionString, TEXT("ViscaConnection="), JsonPayload))
	{
		return Parsed;
	}

	JsonPayload.ReplaceInline(TEXT("\\\""), TEXT("\""));
	JsonPayload.TrimQuotesInline();

	TSharedPtr<FJsonObject> RootObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonPayload);
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		return Parsed;
	}

	RootObject->TryGetStringField(ViscaConnectionStringTokens::ListenIp, Parsed.ListenInterfaceIp);

	bool bLegacyGlobalBidirectional = false;
	FString LegacyReceiveString;
	if (RootObject->TryGetStringField(ViscaConnectionStringTokens::ReceiveMode, LegacyReceiveString))
	{
		bLegacyGlobalBidirectional = ViscaJsonHelpers::IsLegacyBidirectionalString(LegacyReceiveString);
	}

	const TArray<TSharedPtr<FJsonValue>>* ReceiversField = nullptr;
	if (RootObject->TryGetArrayField(ViscaConnectionStringTokens::Receivers, ReceiversField))
	{
		for (const TSharedPtr<FJsonValue>& ReceiverValue : *ReceiversField)
		{
			const TSharedPtr<FJsonObject> ReceiverObject = ReceiverValue->AsObject();
			if (!ReceiverObject.IsValid())
			{
				continue;
			}

			FViscaReceiverConfig Receiver;
			Receiver.CameraName = FName(*ReceiverObject->GetStringField(ViscaConnectionStringTokens::CameraName));
			Receiver.PortString = FString::FromInt(ViscaJsonHelpers::PortFromJsonObject(ReceiverObject));
			Receiver.Transport = ViscaJsonHelpers::TransportFromJsonObject(ReceiverObject);

			// Legacy presets stored "bidirectional" separately from transport — that mode is TCP + ACK only.
			bool bPerReceiverBidirectional = bLegacyGlobalBidirectional;
			FString LegacyReplyString;
			if (ReceiverObject->TryGetStringField(ViscaConnectionStringTokens::ReplyMode, LegacyReplyString))
			{
				bPerReceiverBidirectional = ViscaJsonHelpers::IsLegacyBidirectionalString(LegacyReplyString);
			}
			if (bPerReceiverBidirectional)
			{
				Receiver.Transport = EViscaReceiverTransportMode::TCP;
			}

			if (Receiver.CameraName.IsNone())
			{
				continue;
			}

			Parsed.Receivers.Add(Receiver);
		}
	}

	if (Parsed.ListenInterfaceIp.IsEmpty())
	{
		Parsed.ListenInterfaceIp = TEXT("0.0.0.0");
	}

	return Parsed;
}

FString FViscaLiveLinkConnectionSettings::ToString() const
{
	const TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetStringField(ViscaConnectionStringTokens::ListenIp, ListenInterfaceIp);

	TArray<TSharedPtr<FJsonValue>> ReceiversJson;
	for (const FViscaReceiverConfig& Receiver : Receivers)
	{
		const TSharedRef<FJsonObject> ReceiverObject = MakeShared<FJsonObject>();
		ReceiverObject->SetStringField(ViscaConnectionStringTokens::CameraName, Receiver.CameraName.ToString());
		ReceiverObject->SetStringField(ViscaConnectionStringTokens::Port, Receiver.PortString);
		ReceiverObject->SetStringField(
			ViscaConnectionStringTokens::Transport,
			Receiver.Transport == EViscaReceiverTransportMode::TCP ? TEXT("TCP") : TEXT("UDP"));
		ReceiversJson.Add(MakeShared<FJsonValueObject>(ReceiverObject));
	}
	RootObject->SetArrayField(ViscaConnectionStringTokens::Receivers, ReceiversJson);

	FString JsonPayload;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
		TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);
	FJsonSerializer::Serialize(RootObject, Writer);

	JsonPayload.ReplaceInline(TEXT("\""), TEXT("\\\""));
	return FString::Printf(TEXT("ViscaConnection=\"%s\""), *JsonPayload);
}

TArray<FString> UViscaLiveLinkSourceSettings::GetListenInterfaceOptions() const
{
	TArray<FString> Options;
	Options.Reserve(16);
	Options.Add(TEXT("0.0.0.0"));
	Options.Add(TEXT("127.0.0.1"));

	if (ISocketSubsystem* Subsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))
	{
		TArray<TSharedPtr<FInternetAddr>> Adapters;
		if (Subsystem->GetLocalAdapterAddresses(Adapters))
		{
			TSet<FString> Seen;
			Seen.Add(TEXT("0.0.0.0"));
			Seen.Add(TEXT("127.0.0.1"));
			for (const TSharedPtr<FInternetAddr>& Addr : Adapters)
			{
				if (!Addr.IsValid())
				{
					continue;
				}
				const FString Ip = Addr->ToString(false);
				if (Ip.IsEmpty() || Seen.Contains(Ip))
				{
					continue;
				}
				Seen.Add(Ip);
				Options.Add(Ip);
			}
		}
	}

	return Options;
}

#if WITH_EDITOR
namespace ViscaReceiverEditHelpers
{
	static int32 ParsePort(const FString& PortString)
	{
		return FMath::Clamp(FCString::Atoi(*PortString.TrimStartAndEnd()), 1, 65535);
	}

	static bool IsCameraNameUsed(const TArray<FViscaReceiverConfig>& Receivers, FName Candidate, int32 IgnoreIndex)
	{
		for (int32 i = 0; i < Receivers.Num(); ++i)
		{
			if (i == IgnoreIndex)
			{
				continue;
			}
			if (Receivers[i].CameraName == Candidate)
			{
				return true;
			}
		}
		return false;
	}

	static bool IsPortUsed(const TArray<FViscaReceiverConfig>& Receivers, int32 Port, int32 IgnoreIndex)
	{
		for (int32 i = 0; i < Receivers.Num(); ++i)
		{
			if (i == IgnoreIndex)
			{
				continue;
			}
			if (ParsePort(Receivers[i].PortString) == Port)
			{
				return true;
			}
		}
		return false;
	}

	static FName NextDefaultCameraName(const TArray<FViscaReceiverConfig>& Receivers, int32 ForIndex)
	{
		for (int32 N = 1; N <= 999; ++N)
		{
			const FName Candidate(*FString::Printf(TEXT("Cam%d"), N));
			if (!IsCameraNameUsed(Receivers, Candidate, ForIndex))
			{
				return Candidate;
			}
		}
		return FName(*FString::Printf(TEXT("Cam%d"), ForIndex + 1));
	}

	static int32 NextDefaultPort(const TArray<FViscaReceiverConfig>& Receivers, int32 ForIndex)
	{
		const int32 BasePort = 52381;
		for (int32 Delta = 0; Delta < 2000; ++Delta)
		{
			const int32 Candidate = BasePort + Delta;
			if (!IsPortUsed(Receivers, Candidate, ForIndex))
			{
				return Candidate;
			}
		}
		return BasePort + ForIndex;
	}

	static void SanitizeReceivers(TArray<FViscaReceiverConfig>& Receivers, const FPropertyChangedEvent* PropertyChangedEvent)
	{
		if (Receivers.IsEmpty())
		{
			return;
		}

		static const FName ReceiversName = GET_MEMBER_NAME_CHECKED(UViscaLiveLinkSourceSettings, Receivers);
		if (PropertyChangedEvent && PropertyChangedEvent->Property
			&& PropertyChangedEvent->Property->GetFName() == ReceiversName
			&& PropertyChangedEvent->ChangeType == EPropertyChangeType::ArrayAdd)
		{
			const int32 AddedIndex = PropertyChangedEvent->GetArrayIndex(ReceiversName.ToString());
			if (AddedIndex != INDEX_NONE && Receivers.IsValidIndex(AddedIndex))
			{
				FViscaReceiverConfig& Added = Receivers[AddedIndex];
				Added.CameraName = NextDefaultCameraName(Receivers, AddedIndex);
				Added.PortString = FString::FromInt(NextDefaultPort(Receivers, AddedIndex));
				Added.Transport = EViscaReceiverTransportMode::UDP;
			}
		}

		for (int32 i = 0; i < Receivers.Num(); ++i)
		{
			FViscaReceiverConfig& R = Receivers[i];
			if (R.CameraName.IsNone() || R.CameraName.ToString().TrimStartAndEnd().IsEmpty())
			{
				R.CameraName = NextDefaultCameraName(Receivers, i);
			}
		}

		for (int32 i = 0; i < Receivers.Num(); ++i)
		{
			FViscaReceiverConfig& R = Receivers[i];
			while (IsCameraNameUsed(Receivers, R.CameraName, i))
			{
				R.CameraName = NextDefaultCameraName(Receivers, i);
			}
		}

		for (int32 i = 0; i < Receivers.Num(); ++i)
		{
			FViscaReceiverConfig& R = Receivers[i];
			int32 P = ParsePort(R.PortString);
			if (R.PortString.TrimStartAndEnd().IsEmpty() || P < 1 || P > 65535)
			{
				P = NextDefaultPort(Receivers, i);
			}
			while (IsPortUsed(Receivers, P, i))
			{
				P = FMath::Min(P + 1, 65535);
			}
			R.PortString = FString::FromInt(P);
		}
	}
}

void UViscaLiveLinkSourceSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName ReceiversName = GET_MEMBER_NAME_CHECKED(UViscaLiveLinkSourceSettings, Receivers);
	const FName PropName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	const FName MemberName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (PropName == ReceiversName || MemberName == ReceiversName)
	{
		ViscaReceiverEditHelpers::SanitizeReceivers(Receivers, &PropertyChangedEvent);
	}
}
#endif
