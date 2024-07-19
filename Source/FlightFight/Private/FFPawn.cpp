// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPawn.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

FName BoosterSocket1(TEXT("rt_thruster_jnt"));
FName BoosterSocket2(TEXT("lf_thruster_jnt"));

// Sets default values
AFFPawn::AFFPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));  //->GetMesh()로 대체 가능
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	ThrusterEffect_Left = CreateDefaultSubobject<UNiagaraComponent>(TEXT("THRUSTEREFFECT_LEFT"));
	ThrusterEffect_Right = CreateDefaultSubobject<UNiagaraComponent>(TEXT("THRUSTEREFFECT_RIGHT"));

	//Mesh->SetCollisionProfileName(TEXT("Pawn"));


	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>THRUSTER_EFFECT_LEFT(TEXT("/Game/Book/FX/NS_West_Fighter_Typhoon_Jet.NS_West_Fighter_Typhoon_Jet"));
	if (THRUSTER_EFFECT_LEFT.Succeeded())
	{
		ThrusterEffect_Left->SetAsset(THRUSTER_EFFECT_LEFT.Object);
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>THRUSTER_EFFECT_RIGHT(TEXT("/Game/Book/FX/NS_West_Fighter_Typhoon_Jet.NS_West_Fighter_Typhoon_Jet"));
	if (THRUSTER_EFFECT_RIGHT.Succeeded())
	{
		ThrusterEffect_Right->SetAsset(THRUSTER_EFFECT_RIGHT.Object);
	}

	RootComponent = Mesh; 
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 1200.0f; 
	SpringArm->SetRelativeLocationAndRotation(FVector(-1000.0f, 0.0f, 500.0f), FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FF_Flight(TEXT("/Game/Book/SkeletalMesh/SK_West_Fighter_Typhoon.SK_West_Fighter_Typhoon"));
	if (FF_Flight.Succeeded())
	{
		Mesh->SetSkeletalMesh(FF_Flight.Object);
	}   
	//Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//Mesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//Mesh->SetGenerateOverlapEvents(true);

	Movement->MaxSpeed = 6000.0f; //FloatingPawnMovement에 이미 있는 속성들이라 새로 만들어줄 필요 없음. 
	Movement->Acceleration = 500.0f;
	Movement->Deceleration = 50.0f;
	Movement->TurningBoost = 1.0f;
		 
	bUseControllerRotationPitch = true;  //이걸 해주지 않으면 회전이 돌아가지 않음.
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	//Mesh->OnComponentBeginOverlap.AddDynamic(this, &AFFPawn::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AFFPawn::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));

	Mesh->SetCollisionProfileName(TEXT("Pawn"));
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AFFPawn::OnOverlapBegin);

	if (ThrusterEffect_Left && ThrusterEffect_Right)
	{
		ThrusterEffect_Left->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, BoosterSocket1);
		ThrusterEffect_Right->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, BoosterSocket2);
	}
}

// Called every frame
void AFFPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentSpeed = this->GetVelocity().Size();
	  
}

FVector AFFPawn::CalculateCurrentVelocity() const
{
	if (Movement)
	{
		return Movement->Velocity;
	}
	return FVector::ZeroVector;
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
		if (NewAxisValue > 0.0f)
		{

			//ThrusterEffect_Left->Activate();
			//ThrusterEffect_Right->Activate();
			ABLOG(Warning, TEXT("WOW!!!!!!!!!!On"));
		}
		else
		{
			//ThrusterEffect_Left->Deactivate();
			//ThrusterEffect_Right->Deactivate();
			ABLOG(Warning, TEXT("WOW!!!!!!!!!!Off"));
		}
	}
}

void AFFPawn::Turn(float NewAxisValue)
{
	//TargetRightSpeed = NewAxisValue * MaxSpeed;
	AddControllerYawInput(NewAxisValue);
	//ABLOG(Warning, TEXT("turn : %f"), NewAxisValue);
}

void AFFPawn::LookUp(float NewAxisValue)
{
	if (CurrentSpeed >= 4000.0f)
	{
		AddControllerPitchInput(NewAxisValue);
		//ABLOG(Warning, TEXT("lookup : %f"), NewAxisValue);
	}
}

void AFFPawn::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ABLOG(Warning, TEXT("Overlap with Actor %s"), *OtherActor->GetName());
	}
}


