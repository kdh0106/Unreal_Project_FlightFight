// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPawn.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"

FName BoosterSocket1(TEXT("rt_thruster_jnt"));
FName BoosterSocket2(TEXT("lf_thruster_jnt"));
FName ShootSocket1(TEXT("BulletSocket_L"));
FName ShootSocket2(TEXT("BulletSocket_R"));
FName TrailSocket1(TEXT("Trail_L"));
FName TrailSocket2(TEXT("Trail_R"));
FName TrailSocketW1(TEXT("Trail_WL"));
FName TrailSocketW2(TEXT("Trail_WR"));

// Sets default values
AFFPawn::AFFPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH")); 
    Mesh_Death = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH_DEATH"));
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
    ThrusterEffect_Left = CreateDefaultSubobject<UNiagaraComponent>(TEXT("THRUSTEREFFECT_LEFT"));
    ThrusterEffect_Right = CreateDefaultSubobject<UNiagaraComponent>(TEXT("THRUSTEREFFECT_RIGHT"));
    TrailEffect_Left = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TRAIL_LEFT"));
    TrailEffect_Right = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TRAIL_RIGHT"));
    TrailEffect_WRight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TRAIL_WRIGHT"));
    TrailEffect_WLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TRAIL_WLEFT"));
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

   static ConstructorHelpers::FObjectFinder<UNiagaraSystem>TRAIL_EFFECT_LEFT(TEXT("/Game/Book/FX/NS_Flight_Trail.NS_Flight_Trail"));
    if (TRAIL_EFFECT_LEFT.Succeeded())
    {
        TrailEffect_Left->SetAsset(TRAIL_EFFECT_LEFT.Object);
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>TRAIL_EFFECT_RIGHT(TEXT("/Game/Book/FX/NS_Flight_Trail.NS_Flight_Trail"));
    if (TRAIL_EFFECT_RIGHT.Succeeded())
    {
        TrailEffect_Right->SetAsset(TRAIL_EFFECT_RIGHT.Object);
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>TRAIL_EFFECT_WRIGHT(TEXT("/Game/Book/FX/NS_Flight_Trail.NS_Flight_Trail"));
    if (TRAIL_EFFECT_WRIGHT.Succeeded())
    {
        TrailEffect_WRight->SetAsset(TRAIL_EFFECT_WRIGHT.Object);
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>TRAIL_EFFECT_WLEFT(TEXT("/Game/Book/FX/NS_Flight_Trail.NS_Flight_Trail"));
    if (TRAIL_EFFECT_WLEFT.Succeeded())
    {
        TrailEffect_WLeft->SetAsset(TRAIL_EFFECT_WLEFT.Object);
    }

    static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Big_B.P_Explosion_Big_B"));
    if (ParticleSystem.Succeeded())
    {
        DeathParticleSystem = ParticleSystem.Object;
    }

    RootComponent = BoxCollision;
    Mesh->SetupAttachment(RootComponent);
    SpringArm->SetupAttachment(RootComponent);  
    Camera->SetupAttachment(SpringArm);
    SpringArm->bUsePawnControlRotation = true;

    Mesh_Death->SetupAttachment(RootComponent);
    Mesh_Death->SetVisibility(false);



    //SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
   // BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 237.5f));
    BoxCollision->InitBoxExtent(FVector(700.0f, 550.0f, 152.5f));  //237.5

    FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
    BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, BoxExtent.Z + 15.0f));

    Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -350.0f));
    Mesh_Death->SetRelativeLocation(FVector(0.0f, 0.0f, -250.0f));
    SpringArm->TargetArmLength = 4500.0f;
    SpringArm->SetRelativeLocationAndRotation(FVector(-1000.0f, 0.0f, 500.0f), FRotator(-20.0f, 0.0f, 0.0f));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> FF_Flight(TEXT("/Game/Book/SkeletalMesh/SK_West_Fighter_Typhoon.SK_West_Fighter_Typhoon"));
    if (FF_Flight.Succeeded())
    {
        Mesh->SetSkeletalMesh(FF_Flight.Object);
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh>FF_Flight_Death(TEXT("/Game/VigilanteContent/Vehicles/West_Fighter_Typhoon/Damaged/SM_West_Fighter_Typhoon_Damaged.SM_West_Fighter_Typhoon_Damaged"));
    {
        Mesh_Death->SetStaticMesh(FF_Flight_Death.Object);
    }

    BoxCollision->SetCollisionProfileName(TEXT("Pawn"));
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
    BoxCollision->SetNotifyRigidBodyCollision(true);
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AFFPawn::OnOverlapBegin);

    Movement->MaxSpeed = 6000.0f; //FloatingPawnMovement�� �̹� �ִ� �Ӽ����̶� ���� ������� �ʿ� ����. 
    Movement->Acceleration = 2000.0f;  //Origin : 500
    Movement->Deceleration = 1000.0f;  // 50
    Movement->TurningBoost = 100.0f;   // 1   ������Ʈ ������ �Է¿��� Turn, LookUp�� 0.01 / Rolling�� 0.5 / MoveForward�� 1��

    bUseControllerRotationPitch = true;  //�̰� ������ ������ ȸ���� ���ư��� ����.
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = true;

    static ConstructorHelpers::FClassFinder<AActor>BulletBPClass(TEXT("/Game/Book/Bullet/Bullet_Actor.Bullet_Actor_C"));
    if (BulletBPClass.Class != nullptr)
    {
        BulletActorClass = BulletBPClass.Class;
    }

    HP = 100;

    SpawnLocation = FVector(4704.808635f, 4434.708254f, 432.5002f);
    SpawnRotation = FRotator(0.0f, 45.0f, 0.0f);
}

// Called when the game starts or when spawned
void AFFPawn::BeginPlay()
{
    Super::BeginPlay();

    SetActorLocation(SpawnLocation);  //Box�� Z�� ũ�⸸ŭ Mesh���� -���� ����� �Ѵ�.  / 0, 0, 320

    if (ThrusterEffect_Left && ThrusterEffect_Right)
    {
        ThrusterEffect_Left->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, BoosterSocket1);
        ThrusterEffect_Right->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, BoosterSocket2);
    }

   if (TrailEffect_Left && TrailEffect_Right)
    {
        TrailEffect_Left->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket1);
        TrailEffect_Right->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket2);
        TrailEffect_WLeft->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocketW1);
        TrailEffect_WRight->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocketW2);
    }

   FFPlayerController = Cast<AFFPlayerController>(GetController());
}

void AFFPawn::PostInitializeComponents()
{
    Super::PostInitializeComponents();  //�������̵� �ϰ� �̰� �����ָ� �����߻�

    OnHPIsZero.AddLambda([this]() -> void {
        ABLOG(Warning, TEXT("HP is Zero!!!"));
        SpawnDeathEffect();
        });
}

// Called every frame
void AFFPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CurrentSpeed = this->GetVelocity().Size();

    ShootSocketLocation_L = Mesh->GetSocketLocation(ShootSocket1);
    ShootSocketLocation_R = Mesh->GetSocketLocation(ShootSocket2);
    ShootSocketRotation_L = Mesh->GetSocketRotation(ShootSocket1);
    ShootSocketRotation_R = Mesh->GetSocketRotation(ShootSocket2);
}

void AFFPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFFPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFFPawn::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFFPawn::LookUp);
    PlayerInputComponent->BindAxis(TEXT("Rolling"), this, &AFFPawn::Rolling);
    PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AFFPawn::Fire);
    PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &AFFPawn::StopShooting);
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
            TrailEffect_Left->Activate();
            TrailEffect_Right->Activate();
            TrailEffect_WRight->Activate();
            TrailEffect_WLeft->Activate();
        }
        else
        {
            ThrusterEffect_Left->Deactivate();
            ThrusterEffect_Right->Deactivate();
            TrailEffect_Left->Deactivate();
            TrailEffect_Right->Deactivate();
            TrailEffect_WRight->Deactivate();
            TrailEffect_WLeft->Deactivate();
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
    // Ÿ�̸Ӹ� �����Ͽ� ���� �ð� �������� ShootBullet �Լ��� ȣ���ϵ��� �Ѵ�.
    GetWorld()->GetTimerManager().SetTimer(
        ShootingTimerHandle,        // Ÿ�̸� �ڵ�: Ÿ�̸Ӹ� �����ϰ� �����ϴ� �� ���ȴ�.
        this,                       // Ÿ�̸Ӱ� ȣ���� �Լ��� ���� ��ü (���� ��ü: 'this')
        &AFFPawn::ShootBullet,      // Ÿ�̸Ӱ� ȣ���� �Լ�: ShootBullet
        0.2f,                       // �Լ� ȣ�� ����: 0.2��
        true                        // �ݺ� ����: true (�ݺ� ȣ��)
    );
}


void AFFPawn::ShootBullet()
{
    FHitResult OutHit_L, OutHit_R;
    ECollisionChannel TraceChannel = ECC_Visibility; 
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);  // �ڽ��� �ݸ��� ������ �Ͼ�� �ʰԲ�.

    bool bResult_L = GetWorld()->LineTraceSingleByChannel(
        OutHit_L,
        ShootSocketLocation_L,
        ShootSocketLocation_L + GetActorForwardVector() * 35000.0f,
        TraceChannel,
        CollisionParams
    );

    bool bResult_R = GetWorld()->LineTraceSingleByChannel(
        OutHit_R,
        ShootSocketLocation_R,
        ShootSocketLocation_R + GetActorForwardVector() * 35000.0f,
        TraceChannel,
        CollisionParams
    );

    if (bResult_L && bResult_R)
    {
        FVector ImpactPoint_L = OutHit_L.ImpactPoint;
        FVector ImpactPoint_R = OutHit_R.ImpactPoint;

        FRotator ShootRot_R = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_R, ImpactPoint_R);
        FRotator ShootRot_L = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_L, ImpactPoint_L);

        GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_L, ShootRot_L);
        GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_R, ShootRot_R);
    }
    else
    {
        FVector TraceEnd_L = OutHit_L.TraceEnd;
        FVector TraceEnd_R = OutHit_R.TraceEnd;

        FRotator ShootRot_R = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_R, TraceEnd_R);
        FRotator ShootRot_L = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_L, TraceEnd_L);

        AActor* SpawnedBullet_L = GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_L, ShootRot_L);
        AActor* SpawnedBullet_R = GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_R, ShootRot_R);

        FTimerHandle BulletTimerHandle_L;
        GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle_L, [SpawnedBullet_L]()  //�ð��� ������ ������ �Ѿ��� �Ҹ��Ŵ
            {
                if (IsValid(SpawnedBullet_L))
                {
                    SpawnedBullet_L->Destroy();
                    ABLOG(Warning, TEXT("Destroy!!"));
                }
            }, 3.0f, false);

        FTimerHandle BulletTimerHandle_R;
        GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle_R, [SpawnedBullet_R]()
            {
                if (IsValid(SpawnedBullet_R))
                {
                    SpawnedBullet_R->Destroy();
                    ABLOG(Warning, TEXT("Destroy!!"));
                }
            }, 3.0f, false);
    }
}

void AFFPawn::StopShooting()
{
    if (ShootingTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);  //Ÿ�̸Ӹ� ����
        ShootingTimerHandle.Invalidate();                               //�ڵ��� ��ȿȭ�Ͽ� Ÿ�̸Ӱ� �� �̻� ������� �ʵ���
    }
}

void AFFPawn::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        if (OtherActor && OtherActor->GetClass()->GetName().Contains(TEXT("Bullet")))  //BulletActorClass
        { 
            HP -= 5;
            ABLOG(Warning, TEXT("HP : %d"), HP);
            if (HP <= 0)
            {
                OnHPIsZero.Broadcast();
            }
        }
        else if (OtherActor && OtherActor->GetClass()->GetName().Contains(TEXT("Landscape")))
        {
            SpawnDeathEffect();
        }
        //���� �� ��ü���� �浹 �� ��� �߰� ����
    }
}

void AFFPawn::SpawnDeathEffect()
{
    if (DeathParticleSystem)
    {
        UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticleSystem, GetActorLocation(), GetActorRotation());
        if (ParticleSystemComponent)
        {
            FVector EffectScale(5.0f, 5.0f, 5.0f);  //���� ����Ʈ�� ũ�⸦ �� ũ��
            ParticleSystemComponent->SetWorldScale3D(EffectScale); 
            
            Mesh->SetVisibility(false);
            Mesh_Death->SetVisibility(true);
            SetActorEnableCollision(false);

            if (Movement)  //-> �̵��� �ȵǵ�, ȸ���� �� 
            {  
                Movement->SetActive(false); //������ �������� ����
            }
            DisableInput(FFPlayerController); //�ƹ� �Էµ� ���� ���ϵ���

            GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AFFPawn::RespawnActor, 5.0f, false);
        }
    }
}

void AFFPawn::RespawnActor()  //Destroy�� �����Ϸ��ٰ� Hidden�� ������.
{
    GetController()->SetControlRotation(SpawnRotation);  //SetActorRotation���δ� �ȵ�.
    SetActorLocation(SpawnLocation);

    Mesh->SetVisibility(true);
    Mesh_Death->SetVisibility(false);
    SetActorEnableCollision(true);
    HP = 100;

    Movement->SetActive(true); 
    Movement->Velocity = FVector::ZeroVector;  //�̰� �����ִ�, ���� �����ִ� �ӵ� ������ ���������ڸ��� ȥ�ڼ� ������.
    EnableInput(FFPlayerController);

}
