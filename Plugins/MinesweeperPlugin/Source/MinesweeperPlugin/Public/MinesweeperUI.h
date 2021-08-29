// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
/** * MainMenuUI.h – Provides an implementation of the Slate UI representing the main menu. */
#pragma once   
#include "SlateBasics.h" 
// Lays out and controls the Main Menu UI for our tutorial. 
class MinesweeperUI : public SCompoundWidget 
{   
	public: 
		SLATE_BEGIN_ARGS(MinesweeperUI) 
		{} 
		SLATE_END_ARGS()   
		void Construct(const FArguments& args);  
		void ChangeWidth(const FText& TextIn, ETextCommit::Type CommitType);
		void ChangeHeight(const FText& TextIn, ETextCommit::Type CommitType);
		void ChangeMines(const FText& TextIn, ETextCommit::Type CommitType);
		void OnTextChanged(const FText & TextIn);

		TSharedPtr<SEditableTextBox> EditableWidth;
		TSharedPtr<SEditableTextBox> EditableHeight;
		TSharedPtr<SEditableTextBox> EditableMines;
		TSharedPtr<SUniformGridPanel> EditableGrid;
		TSharedPtr<STextBlock> EndScreen;
		FReply OnRestart();
		FReply OnClicked(int32 Index);

private:
	int32 Width = 10;
	int32 Height = 10;
	int32 Mines = 10;
	TArray<int32> MinesIndexes;
	TArray<int32> Checked;
	TArray<TSharedPtr<SButton>> GridButtons;
	TArray<TSharedPtr<STextBlock>> GridText;
	void CheckForMines(int32 Index);
	bool bFirstClick = true;
	void Restart();
	TArray<int32> GetTilesAround(int32 Index);
};