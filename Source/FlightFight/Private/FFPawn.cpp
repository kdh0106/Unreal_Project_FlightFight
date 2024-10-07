// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPawn.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

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
   
    SetReplicates(true);
    bReplicates = true;
    SetReplicateMovement(true);
    bAlwaysRelevant = true; // Pawn�� �׻� �����ְ� ���� - ������ ������ ���� �Ÿ� �̻� �־����� �Ⱥ���

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
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

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

    HPBarWidget->SetupAttachment(Mesh);



    //SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
   // BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 237.5f));
    BoxCollision->InitBoxExtent(FVector(700.0f, 550.0f, 152.5f));  //237.5

    FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
    BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, BoxExtent.Z + 15.0f));

    Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -350.0f));
    Mesh_Death->SetRelativeLocation(FVector(0.0f, 0.0f, -250.0f));
    SpringArm->TargetArmLength = 4500.0f;
    SpringArm->SetRelativeLocationAndRotation(FVector(-1000.0f, 0.0f, 500.0f), FRotator(-20.0f, 0.0f, 0.0f));

    HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 480.0f));
    HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    static ConstructorHelpers::FClassFinder<UUserWidget>UI_HUD_HP(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
    if (UI_HUD_HP.Succeeded())
    {
        HPBarWidget->SetWidgetClass(UI_HUD_HP.Class);
        HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
    }

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
    Movement->SetIsReplicated(true);  //���ø�����Ʈ

    bUseControllerRotationPitch = true;  //�̰� ������ ������ ȸ���� ���ư��� ����.
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = true;

    static ConstructorHelpers::FClassFinder<AActor>BulletBPClass(TEXT("/Game/Book/Bullet/Bullet_Actor.Bullet_Actor_C"));
    if (BulletBPClass.Class != nullptr)
    {
        BulletActorClass = BulletBPClass.Class;
    }

    //HP = 100;
    MaxHP = 100.0f;
    CurrentHP = MaxHP;
    SetHP(CurrentHP);

    static ConstructorHelpers::FClassFinder<UUserWidget>AimWidget(TEXT("/Game/Book/UI/UI_Aim.UI_Aim_C"));
    if (AimWidget.Succeeded())
    {
        CrosshairWidgetClass = AimWidget.Class;
    }
}
  
// Called when the game starts or when spawned
void AFFPawn::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(CrosshairWidgetClass))
    {
        CrosshairWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), CrosshairWidgetClass));
        if (IsValid(CrosshairWidget))
        {
            CrosshairWidget->AddToViewport();
        }
    }
    
    UpdateHPBar();

    //SpawnLocation = GetActorLocation(); // FVector(4704.808635f, 4434.708254f, 432.5002f); AssignPlayerStart����(�ؿ���)!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //SpawnRotation = GetActorRotation(); // FRotator(0.0f, 45.0f, 0.0f);
    //SetActorLocation(SpawnLocation);  //Box�� Z�� ũ�⸸ŭ Mesh���� -���� ����� �Ѵ�.  / 0, 0, 320

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

   //if (FFPlayerController && FFPlayerController->IsLocalController())  //200~236 for Debug
   //{
   //    ABLOG(Warning, TEXT("Controller %s is local and now possess Pawn %s"), *FFPlayerController->GetName(), *this->GetName());
   //}
   //else
   //{
   //    ABLOG(Error, TEXT("Controller is not local or not set apd"));
   //}

   //if (HasAuthority())
   //{
   //    ABLOG(Warning, TEXT("Pawn has authority (Server)."));
   //}
   //else
   //{
   //    ABLOG(Warning, TEXT("Pawn is client-controlled"));
   //}

   //if (GetOwner())  // �����ڰ� �ִ��� Ȯ��
   //{
   //    ABLOG(Warning, TEXT("Pawn is owned by: %s"), *GetOwner()->GetName());
   //}
   //else
   //{
   //    ABLOG(Error, TEXT("Pawn does not have an owner!"));
   //}

   //// ��Ʈ�ѷ� ���� ���� Ȯ��
   //AController* LocalController = GetController();
   //if (LocalController && IsOwnedBy(LocalController))
   //{
   //    ABLOG(Warning, TEXT("Pawn is properly possessed by its controller."));
   //}
   //else
   //{
   //    ABLOG(Error, TEXT("Pawn is not possessed by a controller!"));
   //}

}

void AFFPawn::PostInitializeComponents()
{
    Super::PostInitializeComponents();  //�������̵� �ϰ� �̰� �����ָ� �����߻�(Super)

    OnHPIsZero.AddLambda([this]() -> void {
        MulticastSpawnDeathEffect();
        });

    if (GetLocalRole() == ROLE_Authority)  //���������� ����
    {
        NetUpdateFrequency = 60.0f; //�ʴ� 60ȸ ������Ʈ
    }
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

    CameraLocation = Camera->GetComponentLocation();

    //Ŭ���̾�Ʈ ���� �Լ� ���
    if (IsLocallyControlled())
    {
        ServerMoveForward(GetActorLocation(), Movement->Velocity, GetActorRotation());
    }
    else
    {
        //SetActorLocation(FMath::VInterpTo(GetActorLocation(), ReplicatedLocation, DeltaTime, 10.0f));
        //ABLOG(Warning, TEXT("replicated location %s"), *ReplicatedLocation.ToString());
        //SetActorRotation(ReplicatedRotation);
        Movement->Velocity = ReplicatedVelocity;
    }

    //ABLOG(Warning, TEXT("Tick print locatin "))
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

 //���ø�����Ʈ �� ������Ƽ
void AFFPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFFPawn, Movement);
    DOREPLIFETIME(AFFPawn, ReplicatedLocation);
    DOREPLIFETIME(AFFPawn, ReplicatedVelocity);
    DOREPLIFETIME(AFFPawn, ReplicatedRotation);
    DOREPLIFETIME(AFFPawn, CurrentHP);
} 


bool AFFPawn::ServerMoveForward_Validate(FVector NewLocation, FVector NewVelocity, FRotator NewRotation)
{
    //ABLOG(Warning, TEXT("Validate is it work? : %f"), Value);
    //return FMath::Abs(Value) <= 1.0f;
    return true;
}

void AFFPawn::ServerMoveForward_Implementation(FVector NewLocation, FVector NewVelocity, FRotator NewRotation)
{
    // �������� Ŭ���̾�Ʈ�� �Է��� �޾� �̵� ó��
    //if (Movement)
    //{
    //    Movement->AddInputVector(GetActorForwardVector() * Value);
    //    ABLOG(Warning, TEXT("In Movement!"));   //Ŭ���̾�Ʈ ���� �α�
    //}
    //else
    //{
    //    AddMovementInput(GetActorForwardVector(), Value);
    //    ABLOG(Warning, TEXT("In Else!"));
    //}
    ReplicatedLocation = NewLocation;
    ReplicatedVelocity = NewVelocity;
    ReplicatedRotation = NewRotation;

    SetActorLocation(NewLocation);
    SetActorRotation(NewRotation);
    Movement->Velocity = NewVelocity;
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
    /*if (CurrentSpeed >= 4000.0f)
    {
        AddControllerPitchInput(NewAxisValue);
    }*/
    AddControllerPitchInput(NewAxisValue);
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
        CameraLocation,  //shootSocketLocation_L
        CameraLocation + GetActorForwardVector() * 35000.0f,
        TraceChannel,
        CollisionParams
    );

    bool bResult_R = GetWorld()->LineTraceSingleByChannel(
        OutHit_R,
        CameraLocation,
        CameraLocation + GetActorForwardVector() * 35000.0f,
        TraceChannel,
        CollisionParams
    );

    if (bResult_L && bResult_R)
    {
        FVector ImpactPoint_L = OutHit_L.ImpactPoint;
        FVector ImpactPoint_R = OutHit_R.ImpactPoint;

        FRotator ShootRot_R = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_R, ImpactPoint_R);
        FRotator ShootRot_L = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_L, ImpactPoint_L);

        /*GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_L, ShootRot_L);
        GetWorld()->SpawnActor<AActor>(BulletActorClass, ShootSocketLocation_R, ShootRot_R);*/

        SpawnBullet(ShootSocketLocation_L, ShootRot_L);
        SpawnBullet(ShootSocketLocation_R, ShootRot_R);
    }
    else
    {
        FVector TraceEnd_L = OutHit_L.TraceEnd;
        FVector TraceEnd_R = OutHit_R.TraceEnd;

        FRotator ShootRot_R = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_R, TraceEnd_R);
        FRotator ShootRot_L = UKismetMathLibrary::FindLookAtRotation(ShootSocketLocation_L, TraceEnd_L);

        SpawnBullet(ShootSocketLocation_L, ShootRot_L);
        SpawnBullet(ShootSocketLocation_R, ShootRot_R);
    }
}

void AFFPawn::SpawnBullet_Implementation(const FVector& Location, const FRotator& Rotation)
{
    AActor* SpawnedBullet = GetWorld()->SpawnActor<AActor>(BulletActorClass, Location, Rotation);
    if (SpawnedBullet)
    {
        SpawnedBullet->SetInstigator(this);

        FTimerHandle BulletTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle, [SpawnedBullet]()  //�ð��� ������ ������ �Ѿ��� �Ҹ��Ŵ
            {
                if (IsValid(SpawnedBullet))
                {
                    SpawnedBullet->Destroy();
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
        if(OtherActor && BulletActorClass && OtherActor->IsA(BulletActorClass))
        {
            AActor* Bullet = Cast<AActor>(OtherActor);
            if (Bullet && Bullet->GetInstigator() != this) {
                ServerTakeDamage(5.0f);
            }
        }
        else if (OtherActor && OtherActor->GetClass()->GetName().Contains(TEXT("Landscape")))
        {
            MulticastSpawnDeathEffect(); 
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

            if (Movement)  //-> �̵��� �ȵŵ�, ȸ���� �� 
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
    ////���⼭ HasAuthority �ߴ��� Ŭ���̾�Ʈ���� �������ǰ� ������ �ȵ�.
    //if (GetController())
    //{
    //    GetController()->SetControlRotation(SpawnRotation);  //SetActorRotation���δ� �ȵ�.
    //}

    ////SetActorLocation(SpawnLocation);
    ////SetActorRotation(SpawnRotation); //�̰� ������ ������ �������ɶ� ��񵿾� ��� ����� Rotation�� �����ؼ�, ���������ڸ��� �浹�� �߻���.

    //Mesh->SetVisibility(true);
    //Mesh_Death->SetVisibility(false);
    //SetActorEnableCollision(true);
    //SetHP(MaxHP);

    //Movement->SetActive(true);
    //Movement->Velocity = FVector::ZeroVector;  //�̰� �����ִ�, ���� �����ִ� �ӵ� ������ ���������ڸ��� ȥ�ڼ� ������.
    //EnableInput(FFPlayerController);
    AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
    if (GameMode)
    {
        GameMode->RestartPlayer(GetController());
    }
}

void AFFPawn::UpdateHPBar()
{
    auto HPWidget = Cast<UFFHPBarWidget>(HPBarWidget->GetUserWidgetObject()); 
    if (HPWidget)
    {
        HPWidget->UpdateHPBarWidget(CurrentHP, MaxHP);
    }  
}

void AFFPawn::SetHP(float NewHP)
{
    CurrentHP = NewHP;
    OnRep_CurrentHP();
}

void AFFPawn::OnRep_CurrentHP()
{
    UpdateHPBar(); 
}

void AFFPawn::ServerTakeDamage_Implementation(float Damage)
{
    if (GetLocalRole() == ROLE_Authority) 
    {
        float NewHP = FMath::Max(CurrentHP - Damage, 0.0f);
        SetHP(NewHP);
        if (CurrentHP <= KINDA_SMALL_NUMBER)
        {
            OnHPIsZero.Broadcast();
        }
        ABLOG(Warning, TEXT("Damage!"));
    }
}

bool AFFPawn::ServerTakeDamage_Validate(float Damage)
{
    return Damage > 0;
}

void AFFPawn::MulticastSpawnDeathEffect_Implementation()
{
    SpawnDeathEffect();
}

bool AFFPawn::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
    return true; //�׻� ��Ʈ��ũ ���ü��� �ִٰ� ��ȯ
}

void AFFPawn::SetSpawnLocationAndRotation(FVector Location, FRotator Rotation)
{
    if (HasAuthority())
    {
        SpawnLocation = Location;
        SpawnRotation = Rotation;
    }
}