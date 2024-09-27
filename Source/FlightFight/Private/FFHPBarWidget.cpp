// Fill out your copyright notice in the Description page of Project Settings.


#include "FFHPBarWidget.h"
#include "Components/ProgressBar.h"

void UFFHPBarWidget::UpdateHPBarWidget(float CurrentHP, float MaxHP)
{
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	if (HPProgressBar)
	{
		float HPPercent = CurrentHP / MaxHP;
		HPProgressBar->SetPercent(HPPercent);
		//ABLOG(Warning, TEXT("HP Bar Percent = %f"), HPPercent);

		FLinearColor BarColor;

		if (HPPercent > 0.6f)
		{
			BarColor = FLinearColor::Green;
		}
		else if (HPPercent > 0.3f)
		{
			BarColor = FLinearColor::Yellow;
		}
		else
		{
			BarColor = FLinearColor::Red;
		}
		HPProgressBar->SetFillColorAndOpacity(BarColor);
	}
}
