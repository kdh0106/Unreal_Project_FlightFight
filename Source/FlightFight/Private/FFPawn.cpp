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
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	//CurrentForwardSpeed = 0.0f;
	//CurrentRightSpeed = 0.0f;
	//TargetForwardSpeed = 0.0f;
	//TargetRightSpeed = 0.0f;
	//MaxSpeed = 1000.0f;
	//MinSpeed = 0.0f;
	//Acceleration = 2.0f;
	//Deceleration = 2.0f;

	Movement->MaxSpeed = 6000.0f;
	Movement->Acceleration = 500.0f;
	Movement->Deceleration = 50.0f;
	Movement->TurningBoost = 1.0f;
		
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

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

	//CurrentForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, TargetForwardSpeed, DeltaTime, Acceleration);
	//CurrentRightSpeed = FMath::FInterpTo(CurrentRightSpeed, TargetRightSpeed, DeltaTime, Acceleration);

	//AddMovementInput(GetActorForwardVector(), CurrentForwardSpeed);
	//AddMovementInput(GetActorRightVector(), CurrentRightSpeed);
	//auto Pawn = this;
	CurrentSpeed = this->GetVelocity().Size();
	ABLOG(Warning, TEXT("speed : %f"), CurrentSpeed);

}

// Called to bind functionality to input
void AFFPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFFPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFFPawn::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFFPawn::LookUp);
}

void AFFPawn::MoveForward(float NewAxisValue)  
{
	//TargetForwardSpeed = NewAxisValue * MaxSpeed;
	
	if (Movement)
	{
		AddMovementInput(GetActorForwardVector(), NewAxisValue);
	} 
}

void AFFPawn::Turn(float NewAxisValue)
{
	//TargetRightSpeed = NewAxisValue * MaxSpeed;
	AddControllerYawInput(NewAxisValue);
	ABLOG(Warning, TEXT("turn : %f"), NewAxisValue);
}

void AFFPawn::LookUp(float NewAxisValue)
{
	if (CurrentSpeed >= 4000.0f)
	{
		AddControllerPitchInput(NewAxisValue);
		ABLOG(Warning, TEXT("lookup : %f"), NewAxisValue);
	}
}


