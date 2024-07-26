// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	//UFUNCTION()
	//void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* Mesh; 

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UFloatingPawnMovement* Movement;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* ThrusterEffect_Left;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* ThrusterEffect_Right;

	UPROPERTY(EditAnywhere, Category = FX)
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	float CurrentSpeed;


private:
	void MoveForward(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Rolling(float NewAxisValue);
};
