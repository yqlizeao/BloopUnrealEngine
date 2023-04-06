// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Views/SInteractiveCurveEditorView.h"

/**
 * A Normalized curve view supporting one or more curves with their own screen transform that normalizes the vertical curve range to [-1,1]
 */
class CURVEEDITOR_API SCurveEditorViewAbsolute : public SInteractiveCurveEditorView
{
public:

	void Construct(const FArguments& InArgs, TWeakPtr<FCurveEditor> InCurveEditor);

};
