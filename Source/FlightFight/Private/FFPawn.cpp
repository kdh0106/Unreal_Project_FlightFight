// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPawn.h"

// Sets default values
AFFPawn::AFFPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));

	RootComponent = Mesh; 
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 800.0f;
	SpringArm->SetRelativeLocationAndRotation(FVector(-1000.0f, 0.0f, 500.0f), FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FF_Flight(TEXT("/Game/Book/SkeletalMesh/SK_West_Fighter_Typhoon.SK_West_Fighter_Typhoon"));
	if (FF_Flight.Succeeded())
	{
		Mesh->SetSkeletalMesh(FF_Flight.Object);
	}

	CurrentForwardSpeed = 0.0f;
	CurrentRightSpeed = 0.0f;
	TargetForwardSpeed = 0.0f;
	TargetRightSpeed = 0.0f;
	MaxSpeed = 1000.0f;
	MinSpeed = 0.0f;
	Acceleration = 2.0f;
	Deceleration = 2.0f;
}

// Called when the game starts or when spawned
void AFFPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFFPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, TargetForwardSpeed, DeltaTime, Acceleration);
	CurrentRightSpeed = FMath::FInterpTo(CurrentRightSpeed, TargetRightSpeed, DeltaTime, Acceleration);

	AddMovementInput(GetActorForwardVector(), CurrentForwardSpeed);
	AddMovementInput(GetActorRightVector(), CurrentRightSpeed);

}

// Called to bind functionality to input
void AFFPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFFPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AFFPawn::MoveRight);
}

void AFFPawn::MoveForward(float NewAxisValue)  
{
	TargetForwardSpeed = NewAxisValue * MaxSpeed;
	
	//AddMovementInput(GetActorForwardVector(), ForwardSpeed);
	ABLOG(Warning, TEXT("forward : %f"), CurrentForwardSpeed);
}

void AFFPawn::MoveRight(float NewAxisValue)
{
	TargetRightSpeed = NewAxisValue * MaxSpeed;
	//AddMovementInput(GetActorRightVector(), RightSpeed);
	ABLOG(Warning, TEXT("right : %f"), CurrentRightSpeed);
}


