// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class USphereComponent;
class UBoxComponent;
class APlayerCharacter;
class UAnimMontage;
class AAIController;

UCLASS()
class MONSTERKINDOM_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		USphereComponent* DetectSphere;

	UPROPERTY(EditAnywhere)
		USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere)
		UBoxComponent* AttackHitBox;

	UPROPERTY(BlueprintReadOnly)
		bool IsAlive = true;

	UFUNCTION()
		virtual void DetecSphereOnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void DetecSphereOnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void CombatSphereOnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void CombatSphereOnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void AttackHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	APlayerCharacter* TargetChar;

	AAIController* AI_Controller;

	void MoveToTarget();

	bool bAttacking;

	UPROPERTY(EditAnywhere)
		float MoveToTargetRadius = 10.0f;

	UPROPERTY(EditAnywhere)
		bool bTargetInAttackRange;
	
	float AttackDelayMin = 0.2;
	float AttackDelayMax = 0.7f;

	bool CanDetecDamageCollision;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnded();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere)
		UAnimMontage* CombatMontage;

	float Health = 100.0f;

	void ApplyDamage();
	void DisposeEnemy();

};
