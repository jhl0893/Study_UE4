// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIModule/Classes/AIController.h"
#include "PlayerCharacter.h"
#include "Animation/AnimInstance.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Detect Sphere"));
	DetectSphere->SetupAttachment(GetRootComponent());
	DetectSphere->InitSphereRadius(600.0f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Combat Sphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.0f);

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	AttackHitBox->SetupAttachment(GetMesh(), TEXT("HitBoxSocket"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AI_Controller = Cast<AAIController>(GetController());

	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DetecSphereOnBeginOverlap);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DetecSphereOnEndOverlap);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnBeginOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnEndOverlap);

	AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackHitBoxOverlap);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::DetecSphereOnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsAlive)
		return;

	APlayerCharacter* MyChar = Cast<APlayerCharacter>(OtherActor);

	if (MyChar)
	{
		TargetChar = MyChar;
		MoveToTarget();	
	}
}

void AEnemy::DetecSphereOnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive)
		return;

	APlayerCharacter* MyChar = Cast<APlayerCharacter>(OtherActor);

	if (MyChar)
	{
		TargetChar = nullptr;
		
		if (AI_Controller)
			AI_Controller->StopMovement();
	}
}

void AEnemy::CombatSphereOnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsAlive)
		return;
	APlayerCharacter* MyChar = Cast<APlayerCharacter>(OtherActor);

	if (MyChar)
	{
		bTargetInAttackRange = true;

		float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);
	}
}

void AEnemy::CombatSphereOnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive)
		return;
	APlayerCharacter* MyChar = Cast<APlayerCharacter>(OtherActor);
	
	if (MyChar)
	{
		bTargetInAttackRange = false;
	}

}

void AEnemy::AttackHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanDetecDamageCollision)
	{
		APlayerCharacter* MyChar = Cast<APlayerCharacter>(OtherActor);

		if (MyChar)
		{
			CanDetecDamageCollision = false; //

			MyChar->ApplyDamage();
		
		}
	}
}

void AEnemy::MoveToTarget()
{
	if (bAttacking)
		return;

	if (AI_Controller)
	{
		FNavPathSharedPtr NavPath;
		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalActor(TargetChar);
		MoveRequest.SetAcceptanceRadius(MoveToTargetRadius);

		AI_Controller->MoveTo(MoveRequest, &NavPath);
	}
}

void AEnemy::Attack()
{
	if (!IsAlive)
		return;

	if (bAttacking)
		return;

	bAttacking = true;
	CanDetecDamageCollision = true;

	if (AI_Controller)
		AI_Controller->StopMovement();

	auto AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.15f);
		AnimInstance->Montage_JumpToSection(TEXT("Attack"), CombatMontage);
	}	
}

void AEnemy::AttackEnded()
{
	bAttacking = false;
	CanDetecDamageCollision = false;

	if (TargetChar)
	{
		if (bTargetInAttackRange)
		{
			float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);
		}
		else
		{
			MoveToTarget();
		}
	}
	else
	{
		bTargetInAttackRange = false;
	}
}

void AEnemy::ApplyDamage()
{
	Health -= 20.0f;
	if (Health <= 0.0f)
	{
		if (AI_Controller)
			AI_Controller->StopMovement();
		IsAlive = false;

		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::DisposeEnemy,2.0f);
	}

}

void AEnemy::DisposeEnemy()
{
	Destroy();
}

