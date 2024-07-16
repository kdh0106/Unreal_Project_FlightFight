// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "FFPawn.generated.h"

UCLASS()
class FLIGHTFIGHT_API AFFPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFFPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UFloatingPawnMovement* Movement;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	//float CurrentForwardSpeed;
	//float CurrentRightSpeed;
	//float TargetForwardSpeed;
	//float TargetRightSpeed;
	float MinFlyingSpeed = 3000.0f;

	//float MaxSpeed;
	//float MinSpeed;

	//UPROPERTY(EditAnywhere)
	//float Acceleration;
	
	//UPROPERTY(EditAnywhere)
	//float Deceleration;

	UPROPERTY(VisibleAnywhere)
	float CurrentSpeed;

private:
	void MoveForward(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);
	

};
