// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/Map.h"
#include "Containers/Array.h"

namespace UE
{
namespace MovieScene
{

struct FInstanceHandle;
struct FRestoreStateParams;
struct FPreAnimatedStateMetaData;
struct FPreAnimatedStorageGroupHandle;

struct IPreAnimatedCaptureSource
{
	virtual ~IPreAnimatedCaptureSource(){}

	virtual void Reset() = 0;
	virtual bool ContainsInstanceHandle(FInstanceHandle RootInstanceHandle) const = 0;
	virtual void GatherAndRemoveExpiredMetaData(const FRestoreStateParams& Params, TArray<FPreAnimatedStateMetaData>& OutExpiredMetaData) = 0;
	virtual void GatherAndRemoveMetaDataForGroup(FPreAnimatedStorageGroupHandle Group, TArray<FPreAnimatedStateMetaData>& OutExpiredMetaData) = 0;
};


} // namespace MovieScene
} // namespace UE
