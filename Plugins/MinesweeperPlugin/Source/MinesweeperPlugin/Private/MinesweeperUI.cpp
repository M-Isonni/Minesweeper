// Fill out your copyright notice in the Description page of Project Settings.


// Copyright 1998 - 2015 Epic Games, Inc.All Rights Reserved.   
#include "MinesweeperUI.h"
#include "Engine.h"  
#include "Misc/DefaultValueHelper.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION 
void MinesweeperUI::Construct(const FArguments & args)
{

	int32 Index = 0;
	FOnClicked Clicked = FOnClicked::CreateSP(this, &MinesweeperUI::OnClicked, Index);

	EditableWidth = SNew(SEditableTextBox).Text(FText::FromString(FString::FromInt(Width))).OnTextCommitted(this, &MinesweeperUI::ChangeWidth);
	EditableHeight = SNew(SEditableTextBox).Text(FText::FromString(FString::FromInt(Height))).OnTextCommitted(this, &MinesweeperUI::ChangeHeight);
	EditableMines = SNew(SEditableTextBox).Text(FText::FromString(FString::FromInt(Mines))).OnTextCommitted(this, &MinesweeperUI::ChangeMines).OnTextChanged(this, &MinesweeperUI::OnTextChanged);
	EditableGrid = SNew(SUniformGridPanel);
	EndScreen = SNew(STextBlock).ColorAndOpacity(FLinearColor::White).ShadowColorAndOpacity(FLinearColor::Black).ShadowOffset(FIntPoint(-1, 1)).Text(FText::FromString("YOU LOST"));

	//TSharedPtr<SButton> B = SNew(SButton).Text(FText::FromString(""));
	ChildSlot
		[
			SNew(SOverlay) + 
			SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Top)
			[
				SNew(STextBlock).ColorAndOpacity(FLinearColor::White).ShadowColorAndOpacity(FLinearColor::Black).ShadowOffset(FIntPoint(-1, 1)).Text(FText::FromString("MineSweeper"))
			] +
			SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top)
			[
				SNew(SVerticalBox) + 
				SVerticalBox::Slot()
				[
					SNew(SHorizontalBox) +
					SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString("Width"))
					] +
					SHorizontalBox::Slot()
					[
						EditableWidth.ToSharedRef()
					]
				] + 
				SVerticalBox::Slot()
				[
					SNew(SHorizontalBox) +
					SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString("Height"))
					] +
					SHorizontalBox::Slot()
					[
						EditableHeight.ToSharedRef()
					]
				] +
					SVerticalBox::Slot()
					[
						SNew(SHorizontalBox) +
						SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString("Bombs"))
					] +
					SHorizontalBox::Slot()
					[
						EditableMines.ToSharedRef()
					]
					]
			] +
			SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
					EditableGrid.ToSharedRef()
			]+
			SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				EndScreen.ToSharedRef()
			] +
			SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Bottom)
			[
				SNew(SButton).OnClicked(this, &MinesweeperUI::OnRestart)[SNew(STextBlock).Text(FText::FromString(" Restart "))]
			]
		];
		

	EndScreen->SetVisibility(EVisibility::Collapsed);

	for (int GridYIndex = 0; GridYIndex < Height; GridYIndex++)
	{
		for (int GridXIndex = 0; GridXIndex < Width; GridXIndex++)
		{
			int32 GridIndex = GridYIndex * Width + GridXIndex;

			TSharedPtr<STextBlock> Text = SNew(STextBlock).Text(FText::FromString("  "));
			GridText.Add(Text);
			GridButtons.Add(SNew(SButton).DesiredSizeScale(FVector2D(1.2,1)).OnClicked(this, &MinesweeperUI::OnClicked, GridIndex)[Text.ToSharedRef()]);
			EditableGrid->AddSlot(GridXIndex, GridYIndex)
				[
					GridButtons[GridIndex].ToSharedRef()
				];
		}
	}
} 
END_SLATE_FUNCTION_BUILD_OPTIMIZATION 

void MinesweeperUI::ChangeWidth(const FText& TextIn, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		GridButtons.Empty();
		int32 Value;
		bool Parsed = FDefaultValueHelper::ParseInt(FString(TextIn.ToString()), Value);
		if (Parsed)
		{
			Width = Value;
			Restart();
		}

	}
}
void MinesweeperUI::ChangeHeight(const FText& TextIn, ETextCommit::Type CommitType)
{ 
	if (CommitType == ETextCommit::OnEnter)
	{
		GridButtons.Empty();
		int32 Value;
		bool Parsed = FDefaultValueHelper::ParseInt(FString(TextIn.ToString()), Value);
		if (Parsed)
		{
			Height = Value;
			Restart();
		}
	}
}

void MinesweeperUI::ChangeMines(const FText& TextIn, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		int32 Value;
		bool Parsed = FDefaultValueHelper::ParseInt(FString(TextIn.ToString()), Value);
		if (Parsed)
		{
			Mines = Value;
		}
	}
}

FReply MinesweeperUI::OnClicked(int32 Index)
{

	int32 GridX = Index % Width;
	int32 GridY = Index / Width;
	TArray<int32> ClickIndexes = GetTilesAround(Index);

	//When starting the game the first click and the tiles around the first click will always be free of mines.
	if (bFirstClick)
	{
		bFirstClick = false;
		EditableMines->SetEnabled(false);
		MinesIndexes.Empty();
		for (int MineIndex = 0; MineIndex < Mines; MineIndex++)
		{
			//Generating mines position after receiving first click
			int32 RandomIndex = FMath::RandRange(0, Width*Height - 1);
			while (ClickIndexes.Contains(RandomIndex) || RandomIndex == Index || MinesIndexes.Contains(RandomIndex))
			{
				RandomIndex = FMath::RandRange(0, Width*Height - 1);
			}
			MinesIndexes.Add(RandomIndex);
		}
	}

	if (MinesIndexes.Contains(Index))
	{
		EndScreen->SetText(FText::FromString("YOU LOST!"));
		EditableGrid->SetVisibility(EVisibility::Collapsed);
		EndScreen->SetVisibility(EVisibility::Visible);
	}
	else
	{		
		CheckForMines(Index);
	}
	return FReply::Handled();
}

void MinesweeperUI::CheckForMines(int32 Index)
{
	//Recursively check for mines around every tile that has no mines around itself.
	Checked.Add(Index);
	TArray<int32> Indexes = GetTilesAround(Index);
	int32 MinesCount = 0;
	for (int32 AroundIndex : Indexes)
	{
		if (MinesIndexes.Contains(AroundIndex))
		{
			MinesCount++;
		}
	}
	if (MinesCount == 0)
	{
		GridButtons[Index]->SetEnabled(false);
		for (int32 AroundIndex : Indexes)
		{
			if (!Checked.Contains(AroundIndex))
			{
				CheckForMines(AroundIndex);
			}
		}
	}
	else
	{		
		GridButtons[Index]->SetEnabled(false);
		GridButtons[Index]->SetToolTipText(FText::FromString(FString::FromInt(MinesCount)));
		GridText[Index]->SetText(FText::FromString(FString::FromInt(MinesCount)));
	}
}

TArray<int32> MinesweeperUI::GetTilesAround(int32 Index)
{
	int32 X = Index % Width;
	int32 Y = Index / Width;
	TArray<int32> OutArray;
	for (int32 XOffset = -1; XOffset < 2; XOffset++)
	{
		for (int32 YOffset = -1; YOffset < 2; YOffset++)
		{
			int32 Index = (Y + YOffset)*Width + X + XOffset;
			if (XOffset == 0 && YOffset == 0 || Index/Width != Y + YOffset)
			{
				continue;
			}
			if (Index >= 0 && Index < Width*Height)
			{
				OutArray.Add(Index);
			}
		}
	}
	return OutArray;
}


FReply MinesweeperUI::OnRestart()
{
	Restart();
	return FReply::Handled();
}

void MinesweeperUI::Restart()
{
	EndScreen->SetVisibility(EVisibility::Collapsed);
	EditableGrid->SetVisibility(EVisibility::Visible);
	bFirstClick = true;
	MinesIndexes.Empty();
	Checked.Empty();
	EditableMines->SetEnabled(true);
	EditableGrid->ClearChildren();
	GridButtons.Empty();
	GridText.Empty();
	for (int GridYIndex = 0; GridYIndex < Height; GridYIndex++)
	{
		for (int GridXIndex = 0; GridXIndex < Width; GridXIndex++)
		{
			int32 GridIndex = GridYIndex * Width + GridXIndex;
			TSharedPtr<STextBlock> Text = SNew(STextBlock).Text(FText::FromString("  "));
			GridText.Add(Text);
			GridButtons.Add(SNew(SButton).DesiredSizeScale(FVector2D(1.2, 1)).OnClicked(this, &MinesweeperUI::OnClicked, GridIndex)[Text.ToSharedRef()]);
			EditableGrid->AddSlot(GridXIndex, GridYIndex)
				[
					GridButtons[GridIndex].ToSharedRef()
				];
		}
	}
}

void MinesweeperUI::OnTextChanged(const FText & TextIn)
{
	//resetting mines number when it exceeds maximum tiles number
	int32 Value;
	bool Parsed = FDefaultValueHelper::ParseInt(FString(TextIn.ToString()), Value);
	if (Parsed)
	{
		if (Value > Width*Height)
		{
			//Subtracting first click from maximum mines number. We want the first click in the grid to always be clean.
			int32 FirstClick = 9;
			EditableMines->SetText(FText::FromString(FString::FromInt(Width*Height - FirstClick)));
		}
	}
}

