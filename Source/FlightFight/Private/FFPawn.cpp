// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPawn.h"

FName BoosterSocket1(TEXT("rt_thruster_jnt"));
FName BoosterSocket2(TEXT("lf_thruster_jnt"));
FName ShootSocket1(TEXT("lf_wheelMain_2_jnt"));
FName ShootSocket2(TEXT("rt_wheelMain_2_jnt"));

// Sets default values
AFFPawn::AFFPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH")); 
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
    ThrusterEffect_Left = CreateDefaultSubobject<UNiagaraComponent>(TEXT("THRUSTEREFFECT_LEFT"));
    ThrusterEffect_Right = CreateDefaultSubobject<UNiagaraComponent>(TEXT("THRUSTEREFFECT_RIGHT"));
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BOXCOLLISION"));

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

    RootComponent = BoxCollision;
    Mesh->SetupAttachment(RootComponent);
    SpringArm->SetupAttachment(RootComponent);  
    Camera->SetupAttachment(SpringArm);
    SpringArm->bUsePawnControlRotation = true;


    //SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
   // BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 237.5f));
    BoxCollision->InitBoxExtent(FVector(700.0f, 550.0f, 152.5f));  //237.5

    FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
    BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, BoxExtent.Z + 15.0f));

    Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -250.0f)); 
    SpringArm->TargetArmLength = 2000.0f;
    SpringArm->SetRelativeLocationAndRotation(FVector(-1000.0f, 0.0f, 500.0f), FRotator(-20.0f, 0.0f, 0.0f));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> FF_Flight(TEXT("/Game/Book/SkeletalMesh/SK_West_Fighter_Typhoon.SK_West_Fighter_Typhoon"));
    if (FF_Flight.Succeeded())
    {
        Mesh->SetSkeletalMesh(FF_Flight.Object);
    }

    BoxCollision->SetCollisionProfileName(TEXT("Pawn"));
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
    BoxCollision->SetNotifyRigidBodyCollision(true);

    Movement->MaxSpeed = 6000.0f; //FloatingPawnMovement에 이미 있는 속성들이라 새로 만들어줄 필요 없음. 
    Movement->Acceleration = 500.0f;
    Movement->Deceleration = 50.0f;
    Movement->TurningBoost = 1.0f;

    bUseControllerRotationPitch = true;  //이걸 해주지 않으면 회전이 돌아가지 않음.
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = true;

    static ConstructorHelpers::FClassFinder<AActor>BulletBPClass(TEXT("/Game/Book/Bullet/Bullet_Actor.Bullet_Actor_C"));
    if (BulletBPClass.Class != nullptr)
    {
        BulletActorClass = BulletBPClass.Class;
        ABLOG(Warning, TEXT("Suiii"));
    }

    ShootSocketLocation_L = Mesh->GetSocketLocation(ShootSocket1);
    ShootSocketLocation_R = Mesh->GetSocketLocation(ShootSocket2);
    ShootSocketRotation_L = Mesh->GetSocketRotation(ShootSocket1);
    ShootSocketRotation_R = Mesh->GetSocketRotation(ShootSocket2);
}

// Called when the game starts or when spawned
void AFFPawn::BeginPlay()
{
    Super::BeginPlay();

    SetActorLocation(FVector(0.0f, 0.0f, 250.0f));  //Box의 Z축 크기만큼 Mesh에서 -값을 해줘야 한다.

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

void AFFPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFFPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFFPawn::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFFPawn::LookUp);
    PlayerInputComponent->BindAxis(TEXT("Rolling"), this, &AFFPawn::Rolling);
    PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AFFPawn::Fire);
}

void AFFPawn::MoveForward(float NewAxisValue)
{
    if (Movement)
    {
        AddMovementInput(GetActorForwardVector(), NewAxisValue);
        if (NewAxisValue > 0.0f)
        {
            ThrusterEffect_Left->Activate();
            ThrusterEffect_Right->Activate();
        }
        else
        {
            ThrusterEffect_Left->Deactivate();
            ThrusterEffect_Right->Deactivate();
        }
    }
}

void AFFPawn::Turn(float NewAxisValue)
{
    AddControllerYawInput(NewAxisValue);
}

void AFFPawn::LookUp(float NewAxisValue)
{
    if (CurrentSpeed >= 4000.0f)
    {
        AddControllerPitchInput(NewAxisValue);
    }
}

void AFFPawn::Rolling(float NewAxisValue)
{
    AddControllerRollInput(NewAxisValue);
}

void AFFPawn::Fire()
{
    ShootBullet();    //Timer를 넣어줘야하나?? 일단 그냥 넣어보기
    ABLOG(Warning, TEXT("Shoot!!"));
}

void AFFPawn::ShootBullet()
{
    FHitResult OutHit;
    ECollisionChannel TraceChannel = ECC_Visibility; // Visibility채널의 의미???
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);  // 자신은 콜리전 반응이 일어나지 않게끔.

    bool bResult = GetWorld()->LineTraceSingleByChannel(
        OutHit,
        GetActorLocation(),
        GetActorLocation() + GetActorForwardVector() * 35000.0f,
        TraceChannel,
        CollisionParams
    );

    if (bResult)
    {
        GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_L, ShootSocketRotation_L);
        GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_R, ShootSocketRotation_R);
    }
}
//void AFFPawn::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//    if (OtherActor && (OtherActor != this) && OtherComp)
//    {
//        ABLOG(Warning, TEXT("Overlap with Actor %s"), *OtherActor->GetName());
//    }
//}
