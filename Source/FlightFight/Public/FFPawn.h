 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "FFPlayerController.h"
#include "FFGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "FFHPBarWidget.h"
#include "FFHUDWidget.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FFPawn.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate)

UCLASS()
 class FLIGHTFIGHT_API AFFPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFFPawn();

	UFUNCTION(Server, UnReliable, WithValidation)
	void ServerMoveForward(FVector NewLocation, FVector NewVelocity, FRotator NewRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTakeDamage(float Damage, AController* InstigatorController);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnDeathEffect();

	// 네트워크 관련성 오버라이드
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override; 

	/*void CallOpenLevel(const FString& Address);
	void CallClientTravel(const FString& Address);

	void OpenLobby();*/

	void CreateGameSession();
	void JoinGameSession();

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	void PrintToScreen(const FString& Message);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	IOnlineSessionPtr OnlineSessionInterface;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	void OnFindSessionComplete(bool bWasSuccessful);

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;  //프로퍼티 리플리케이션
	//virtual void Destroyed();

	// Called to bind functionality to input 
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FOnHPIsZeroDelegate OnHPIsZero;

	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Visual)
	UStaticMeshComponent* Mesh_Death;

	UPROPERTY(VisibleAnywhere, Replicated, Category = Movement)
	UFloatingPawnMovement* Movement;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* ThrusterEffect_Left;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* ThrusterEffect_Right;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* TrailEffect_Right;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* TrailEffect_Left;

	UPROPERTY(VisibleAnywhere, Category = Effect)  //#
	UNiagaraComponent* TrailEffect_WRight;

	UPROPERTY(VisibleAnywhere, Category = Effect)  //#
	UNiagaraComponent* TrailEffect_WLeft;

	UPROPERTY(EditAnywhere, Category = FX)
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> BulletActorClass;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	FVector CameraLocation;

	UPROPERTY(VisibleAnywhere, Category = "Socket")
	FVector ShootSocketLocation_L;

	UPROPERTY(VisibleAnywhere, Category = "Socket")
	FVector ShootSocketLocation_R;

	UPROPERTY(VisibleAnywhere, Category = "Socket")
	FRotator ShootSocketRotation_L;

	UPROPERTY(VisibleAnywhere, Category = "Socket")
	FRotator ShootSocketRotation_R;

	UPROPERTY(VisibleAnywhere, Category = "Socket")
	FVector BulletSocket_Location;

	UPROPERTY(VisibleAnywhere, Category = "Spawn")
	FVector SpawnLocation;

	UPROPERTY(VisibleAnywhere, Category = "Spawn")
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UUserWidget* CrosshairWidget;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> ScoreWidgetClass;

	UUserWidget* ScoreTextWidget;

	UPROPERTY(ReplicatedUsing = OnRep_ThrusterFXActive)
	bool bIsThrusterFXActive;

	UFUNCTION()
	void OnRep_ThrusterFXActive();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetThrusterFXActive(bool bNewState);

	UFUNCTION()
	void ActivateThrusterFX();

	UFUNCTION()
	void DeactivateThrusterFX();

	UPROPERTY(ReplicatedUsing = OnRep_TrailFXActive)
	bool bIsTrailFXActive;

	UFUNCTION()
	void OnRep_TrailFXActive();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetTrailFXActive(bool bNewState);

	UFUNCTION()
	void ActivateTrailFX();

	UFUNCTION()
	void DeactivateTrailFX();

	/*UPROPERTY(VisibleAnywhere, Category = "Stats", ReplicatedUsing = OnRep_CurrentScore, Meta = (AllowPrivateAccess = true))
	int32 CurrentScore;*/
	 
private:
	void MoveForward(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Rolling(float NewAxisValue);
	
	void Fire();
	void ShootBullet();

	UFUNCTION(Server, Reliable)
	void SpawnBullet(const FVector& Location, const FRotator& Rotation);

	void StopShooting();
	void SpawnDeathEffect();

	void RespawnActor();
	void UpdateHPBar();
	void SetHP(float NewHP);

	//void UpdateScore();
	//void SetScore(int32 NewScore);

	UPROPERTY(Transient, VisibleInstanceOnly)
	int32 HP;

	UPROPERTY(EditAnywhere, Category = "Stats", Meta = (AllowPrivateAccess = true))
	float MaxHP;

	UPROPERTY(VisibleAnywhere, Category = "Stats", ReplicatedUsing = OnRep_CurrentHP, Meta = (AllowPrivateAccess = true))
	float CurrentHP;

	UFUNCTION()
	void OnRep_CurrentHP();

	/*UPROPERTY(EditAnywhere, Category = "Stats", Meta = (AllowPrivateAccess = true))
	int32 HighScore;

	

	UFUNCTION()
	void OnRep_CurrentScore();*/

	

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* DeathParticleSystem;

	UPROPERTY()
	class AFFPlayerController* FFPlayerController;

	FTimerHandle ShootingTimerHandle;
	FTimerHandle DestroyTimerHandle;
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(Replicated)
	FVector ReplicatedLocation;

	UPROPERTY(Replicated)
	FVector ReplicatedVelocity;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;

	FRotator LastRotation;

	//FString PathToLevel{ TEXT("/Game/Book/Maps/LandTest.LandTest?listen") };

};
