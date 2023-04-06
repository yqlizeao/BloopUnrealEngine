// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "InputCoreTypes.h"
#include "Math/UnrealMath.h"
#include "InputState.generated.h"


/**
 * Input event data can be applicable to many possible input devices.
 * These flags are used to indicate specific or sets of device types.
 */
UENUM()
enum class EInputDevices
{
	None = 0,
	Keyboard = 1,
	Mouse = 2,
	Gamepad = 4,

	OculusTouch = 8,
	HTCViveWands = 16,
	AnySpatialDevice = OculusTouch | HTCViveWands,

	TabletFingers = 1024
};
ENUM_CLASS_FLAGS(EInputDevices);



/**
 * FInputDeviceRay represents a 3D ray created based on an input device.
 * If the device is a 2D input device like a mouse, then the ray may
 * have an associated 2D screen position.
 */
struct FInputDeviceRay
{
	/** 3D ray in 3D scene, in world coordinates */
	FRay WorldRay;

	/** If true, WorldRay has 2D device position coordinates */
	bool bHas2D;

	/** 2D device position coordinates associated with the ray */
	FVector2D ScreenPosition;


	FInputDeviceRay(const FRay& WorldRayIn)
	{
		WorldRay = WorldRayIn;
		bHas2D = false;
		ScreenPosition = FVector2D(0, 0);
	}

	FInputDeviceRay(const FRay& WorldRayIn, const FVector2D& ScreenPositionIn)
	{
		WorldRay = WorldRayIn;
		bHas2D = true;
		ScreenPosition = ScreenPositionIn;
	}
};



/*
 * FInputRayHit is returned by various hit-test interface functions.
 * Generally this is intended to be returned as the result of a hit-test with a FInputDeviceRay 
 */
USTRUCT()
struct INTERACTIVETOOLSFRAMEWORK_API FInputRayHit
{
	GENERATED_BODY()

	/** true if ray hit something, false otherwise */
	bool bHit;

	/** distance along ray at which intersection occurred */
	float HitDepth;

	/** Normal at hit point, if available */
	FVector HitNormal;

	/** True if HitNormal was set */
	bool bHasHitNormal;

	/** client-defined integer identifier for hit object/element/target/etc */
	int32 HitIdentifier;

	/** client-defined pointer for hit object/target */
	void* HitOwner;

	FInputRayHit()
	{
		bHit = false;
		HitDepth = TNumericLimits<float>::Max();
		HitNormal = FVector(0, 0, 1);
		bHasHitNormal = false;
		HitIdentifier = 0;
		HitOwner = nullptr;
	}

	explicit FInputRayHit(float HitDepthIn)
	{
		bHit = true;
		HitDepth = HitDepthIn;
		HitNormal = FVector(0, 0, 1);
		bHasHitNormal = false;
		HitIdentifier = 0;
		HitOwner = nullptr;
	}

	explicit FInputRayHit(float HitDepthIn, const FVector& HitNormalIn)
	{
		bHit = true;
		HitDepth = HitDepthIn;
		HitNormal = HitNormalIn;
		bHasHitNormal = true;
		HitIdentifier = 0;
		HitOwner = nullptr;
	}
};





/**
 * Current State of a physical device button (mouse, key, etc) at a point in time.
 * Each "click" of a button should involve at minimum two separate state
 * events, one where bPressed=true and one where bReleased=true.
 * Each of these states should occur only once.
 * In addition there may be additional frames where the button is
 * held down and bDown=true and bPressed=false.
 */
struct FDeviceButtonState
{
	/** Button identifier */
	FKey Button;

	/** Was the button pressed down this frame. This should happen once per "click" */
	bool bPressed;
	/** Is the button currently pressed down. This should be true every frame the button is pressed. */
	bool bDown;
	/** Was the button released this frame. This should happen once per "click" */
	bool bReleased;

	FDeviceButtonState()
	{
		Button = FKey();
		bPressed = bDown = bReleased = false;
	}

	FDeviceButtonState(const FKey& ButtonIn)
	{
		Button = ButtonIn;
		bPressed = bDown = bReleased = false;
	}

	/** Update the states of this button */
	void SetStates(bool bPressedIn, bool bDownIn, bool bReleasedIn)
	{
		bPressed = bPressedIn;
		bDown = bDownIn;
		bReleased = bReleasedIn;
	}
};



/**
 * Current state of active keyboard key at a point in time
 * @todo would be useful to track set of active keys
 */
struct FKeyboardInputDeviceState
{
	/** state of active key that was modified (ie press or release) */
	FDeviceButtonState ActiveKey;
};



/**
 * Current State of a physical Mouse device at a point in time.
 */
struct FMouseInputDeviceState
{
	/** State of the left mouse button */
	FDeviceButtonState Left;
	/** State of the middle mouse button */
	FDeviceButtonState Middle;
	/** State of the right mouse button */
	FDeviceButtonState Right;

	/** Change in 'ticks' of the mouse wheel since last state event */
	float WheelDelta;

	/** Current 2D position of the mouse, in application-defined coordinate system */
	FVector2D Position2D;

	/** Change in 2D mouse position from last state event */
	FVector2D Delta2D;

	/** Ray into current 3D scene at current 2D mouse position */
	FRay WorldRay;


	FMouseInputDeviceState()
	{
		Left = FDeviceButtonState(EKeys::LeftMouseButton);
		Middle = FDeviceButtonState(EKeys::MiddleMouseButton);
		Right = FDeviceButtonState(EKeys::RightMouseButton);
		WheelDelta = false;
		Position2D = FVector2D::ZeroVector;
		Delta2D = FVector2D::ZeroVector;
		WorldRay = FRay();
	}
};



/**
 * Current state of physical input devices at a point in time.
 * Assumption is that the state refers to a single physical input device,
 * ie InputDevice field is a single value of EInputDevices and not a combination.
 */
struct FInputDeviceState
{
	/** Which InputDevice member is valid in this state */
	EInputDevices InputDevice;

	//
	// keyboard modifiers
	//

	/** Is they keyboard SHIFT modifier key currently pressed down */
	bool bShiftKeyDown;
	/** Is they keyboard ALT modifier key currently pressed down */
	bool bAltKeyDown;
	/** Is they keyboard CTRL modifier key currently pressed down */
	bool bCtrlKeyDown;
	/** Is they keyboard CMD modifier key currently pressed down (only on Apple devices) */
	bool bCmdKeyDown;


	/** Current state of Keyboard device, if InputDevice == EInputDevices::Keyboard */
	FKeyboardInputDeviceState Keyboard;

	/** Current state of Mouse device, if InputDevice == EInputDevices::Mouse */
	FMouseInputDeviceState Mouse;


	FInputDeviceState() 
	{
		InputDevice = EInputDevices::None;
		bShiftKeyDown = bAltKeyDown = bCtrlKeyDown = bCmdKeyDown = false;
		Keyboard = FKeyboardInputDeviceState();
		Mouse = FMouseInputDeviceState();
	}

	/** Update keyboard modifier key states */
	void SetModifierKeyStates(bool bShiftDown, bool bAltDown, bool bCtrlDown, bool bCmdDown) 
	{
		bShiftKeyDown = bShiftDown;
		bAltKeyDown = bAltDown;
		bCtrlKeyDown = bCtrlDown;
		bCmdKeyDown = bCmdDown;
	}

	/**
	 * @param DeviceType Combination of device-type flags
	 * @return true if this input state is for an input device that matches the query flags 
	 */
	bool IsFromDevice(EInputDevices DeviceType) const
	{
		return ((InputDevice & DeviceType) != EInputDevices::None);
	}




	//
	// utility functions to pass as lambdas
	// 

	/** @return true if shift key is down in input state */
	static bool IsShiftKeyDown(const FInputDeviceState& InputState)
	{
		return InputState.bShiftKeyDown;
	}

	/** @return true if ctrl key is down in input state */
	static bool IsCtrlKeyDown(const FInputDeviceState& InputState)
	{
		return InputState.bCtrlKeyDown;
	}

	/** @return true if alt key is down in input state */
	static bool IsAltKeyDown(const FInputDeviceState& InputState)
	{
		return InputState.bAltKeyDown;
	}

	/** @return true if Apple Command key is down in input state */
	static bool IsCmdKeyDown(const FInputDeviceState& InputState)
	{
		return InputState.bCmdKeyDown;
	}
};

