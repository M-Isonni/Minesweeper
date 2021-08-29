// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperPluginCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperPluginModule"

void FMinesweeperPluginCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MinesweeperPlugin", "Execute MinesweeperPlugin action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
