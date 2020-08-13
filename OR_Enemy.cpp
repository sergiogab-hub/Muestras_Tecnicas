/****************** Sergio López  ******************/

/************ Base AIEnemy Class (.CPP) **************/

/********** Orion Pérsonal Project 2020 *************/


//My Classs
#include "AI/OR_Enemy.h"
#include "AI/EnemyAnimInstance.h"
#include "Character/OR_MainCharacter.h"
#include "AI/OR_AIBaseEnemyController.h"
#include "DamageTypes/MeleeDamageType.h"
#include "Components/OR_HealthComponent.h"
#include "Projectiles/OR_BulletProjectile.h"


//Engine Class
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AOR_Enemy::AOR_Enemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Weapon AI Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), "WeaponSocket");


	// Navigation points for a realisticand optimal locomotion
	AINavPoint0 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint0"));
	AINavPoint0->SetupAttachment(GetRootComponent());
	AINavPoint0->InitSphereRadius(15.f);

	AINavPoint1 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint1"));
	AINavPoint1->SetupAttachment(GetRootComponent());
	AINavPoint1->InitSphereRadius(15.f);

	AINavPoint2 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint2"));
	AINavPoint2->SetupAttachment(GetRootComponent());
	AINavPoint2->InitSphereRadius(15.f);


	AINavPoint3 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint3"));
	AINavPoint3->SetupAttachment(GetRootComponent());
	AINavPoint3->InitSphereRadius(15.f);

	AINavPoint4 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint4"));
	AINavPoint4->SetupAttachment(GetRootComponent());
	AINavPoint4->InitSphereRadius(15.f);


	AINavPoint5 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint5"));
	AINavPoint5->SetupAttachment(GetRootComponent());
	AINavPoint5->InitSphereRadius(15.f);

	AINavPoint6 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint6"));
	AINavPoint6->SetupAttachment(GetRootComponent());
	AINavPoint6->InitSphereRadius(15.f);

	AINavPoint7 = CreateDefaultSubobject<USphereComponent>(TEXT("AINavPoint7"));
	AINavPoint7->SetupAttachment(GetRootComponent());
	AINavPoint7->InitSphereRadius(15.f);


	//HealthComponent
	Health = CreateDefaultSubobject<UOR_HealthComponent>(TEXT("Health"));


	//  Navigation
	CurrentTargetLocation = FVector(0.f , 0.f ,0.f);


	// Control Combat Variables
	bIsHipsFiring = false;
	bIsIronFiringMoving = false;
	bIsIronFiringStanding = false;
	bIsFiring = false;
	bHasNeedReload = false;
	bIsRealod = false;


	//Munition Variables
	MaxMunition = 7;
	CurrentIndex = 0;


	//Defaul Shoot Ratios
	ShootStandRatio = 0.233f;
	ShootMovingRatio = 1.0f;


	//Default Shake Pitch Shoot RAtio
	ShakePitchMinRatio = -1.0f;
	ShakePitchMaxRatio = 1.0f;

	//Default Shake Yaw Shoot RAtio
	ShakeYawMinRatio = -5.0f;
	ShakeYawMaxRatio = 0.0f;
}

////////////////////////////////////////////////////////////////////
//
//   Begin Play
//
////////////////////////////////////////////////////////////////////

void AOR_Enemy::BeginPlay()
{
	Super::BeginPlay();

	// Set Up Class respective References
	SetupReferences();

	//Add Locomotion points to vector -> MANUALLY to fast edit debug edit positions
	AINavigationArrayPoints.Add(AINavPoint0->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint1->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint2->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint3->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint4->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint5->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint6->GetComponentLocation());
	AINavigationArrayPoints.Add(AINavPoint7->GetComponentLocation());

	//Current Munition
	CurrentMunition = MaxMunition;

	//Min Distance to start Run -> Dynamic distance according to Gameplay
	InitialMinDistanceToRun = MinDistanceToRun;

	// HealthDelegate
	Health->OnHealthChangeDelegate.AddDynamic(this, &AOR_Enemy::OnHealthChange);

	//Navegation System Timer To update Navigation System positions
	GetWorld()->GetTimerManager().SetTimer(UpdateNavegationSystemHandle, this, &AOR_Enemy::UpdateNavegationSystem, 0.05f, true, 0.0f);


}

void AOR_Enemy::SetupReferences()
{

	//AI Controller Reference
	OwnController = Cast<AOR_AIBaseEnemyController>(GetController());

	// AnimInstance Reference
	EnemyAnimInstaceReference = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	// Get player reference
	MainReference = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0.f));

}

////////////////////////////////////////////////////////////////////
//
//   Event Tick 
//
////////////////////////////////////////////////////////////////////

void AOR_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Try to use tick of the behavor tree to better optimize this only for special class things

}


////////////////////////////////////////////////////////////////////
//
//  Character AI Movement 
//
////////////////////////////////////////////////////////////////////

/////////
///////// Check if Navigation points is valid  , if  is not , make interpolations to make a valid move.
/////////

FVector AOR_Enemy::CheckNavigationPoint(FVector TargetPosition)
{

	/*Get Reference NavPatch Synchronously to validate the navigation point*/
	FVector PathStart = GetActorLocation();
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), PathStart, TargetPosition, NULL);

	if (NavPath->IsValid())
	{
		return TargetPosition;
	}
	else
	{
		/*Interpolate Positions to keep fluidity of movement*/
		float InterSpeed = 90.f;
		FVector InterpVector = FMath::VInterpTo(TargetPosition, GetActorLocation(), GetWorld()->GetDeltaSeconds(), InterSpeed);
		return InterpVector;
	}

}


//////// 
//////// Get AI point to move / Player - based movement and realistic AI reaction, choose the point according to AI Yaw
////////

FVector AOR_Enemy::GetMovementDirection(float PlayerDistance) // Called By BehavorTree 
{
	/*When the player moves too far in combat, he starts the chase returning PlayerLocation*/
	if (PlayerDistance > MinDistanceToRun)
	{
		if (IsValid(MainReference))
		{
			CurrentTargetLocation = MainReference->GetActorLocation();
			return CurrentTargetLocation;
		}

	}

	/*Reach a manageable distance to stand up and shoot better*/
	if (PlayerDistance >= MinDistanceToStay && PlayerDistance <= MaxDistanceToStay)
	{
		CurrentTargetLocation = GetActorLocation();
		return CurrentTargetLocation;
	}

	/*Forward navigation When AI are a safe distance*/
	if (PlayerDistance > MaxDistanceToStay)
	{
		// Left Side Navigation/Locomotion

		if (AIBaseYaw< 0.f && AIBaseYaw >-20.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[2];
		}
		if (AIBaseYaw < -20.f && AIBaseYaw > -40.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[1];
		}
		if (AIBaseYaw < -40.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[0];
		}

		// Righ Side Navigation/Locomotion

		if (AIBaseYaw > 0.f && AIBaseYaw < 30.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[5];
		}
		if (AIBaseYaw > 30.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[0];
		}

	}
	/*Back navigation when the player gets too close to the AI*/
	else
	{
		// Left Side Navigation/Locomotion (Inverse)

		if (AIBaseYaw > 0.f && AIBaseYaw < 20.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[3];
		}
		if (AIBaseYaw > 20.f && AIBaseYaw < 40.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[4];
		}
		if (AIBaseYaw > 40.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[5];
		}

		// Righ Side Navigation/Locomotion (Inverse)

		if (AIBaseYaw < 0.f && AIBaseYaw >-30.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[3];
		}
		if (AIBaseYaw < -30.f)
		{
			CurrentTargetLocation = AINavigationArrayPoints[2];
		}
	}

	return CheckNavigationPoint(CurrentTargetLocation);
}


////////
//////// AI Adjusts speed proportional to distance to player
////////

void AOR_Enemy::GetProportionalSpeed(float distance)
{
	// Adjust speed according to distance for fluidityand realism
	if (distance > MinDistanceToRun)
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		return;
	}
	if (distance > MaxDistanceToStay)
	{
		float TempMax = MinDistanceToRun - MaxDistanceToStay;

		/*Rule of three mathematic for medium fluent navigation*/
		GetCharacterMovement()->MaxWalkSpeed = 200.f + ((distance - MaxDistanceToStay) * 100.f) / TempMax;
	}

	if (distance < MinDistanceToStay)
	{
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
	}
}


////////
//////// Update Locomotions Navigations potition
////////

void AOR_Enemy::UpdateNavegationSystem()
{
	AINavigationArrayPoints[0] = AINavPoint0->GetComponentLocation();
	AINavigationArrayPoints[1] = AINavPoint1->GetComponentLocation();
	AINavigationArrayPoints[2] = AINavPoint2->GetComponentLocation();
	AINavigationArrayPoints[3] = AINavPoint3->GetComponentLocation();
	AINavigationArrayPoints[4] = AINavPoint4->GetComponentLocation();
	AINavigationArrayPoints[5] = AINavPoint5->GetComponentLocation();
	AINavigationArrayPoints[6] = AINavPoint6->GetComponentLocation();
	AINavigationArrayPoints[7] = AINavPoint7->GetComponentLocation();
}



////////////////////////////////////////////////////////////////////
//
//  Delegates / Respectives Delegate Functions
//
////////////////////////////////////////////////////////////////////

//////// 
//////// On Health Change Delegate -> Death / TypeDamage and Death 
////////

void AOR_Enemy::OnHealthChange(UOR_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsValid(DamageType)) // Bullet has a NullPtr of damage type, therefore it needs to be checked once
	{ 
		const UMeleeDamageType* MeleeDamage = Cast<UMeleeDamageType>(DamageType); //My Melee Damage Type not Needed Interface

		{
			if (IsValid(MeleeDamage) && IsValid(OwnController))
			{
				OwnController->UnPossess();
				BP_Death();
				EnemyAnimInstaceReference->SetBlendIronSightWeigth(1.0f);  //Adjust Skeleton Animations Values
				EnemyAnimInstaceReference->Montage_Play(DeathMontage, 1.0f);
				EnemyAnimInstaceReference->Montage_JumpToSection("Slot6", DeathMontage); //Montage Receive Melee Hit
				AddActorLocalOffset(FVector(-120.0f, 0.0f, 0.0f));  //Move Actor for Realism
				return;
			}
		}
	}

	if (IsValid(DamageCauser))
	{
		AProjectile* Bullet = Cast<AProjectile>(DamageCauser);

		if (IsValid(Bullet))
		{
        	if (CurrentHealthComponent->GetIsDead())
			{
				if (IsValid(OwnController) && DeathMontage)
				{
					OwnController->UnPossess();
					PlayMontageRandomDeathAnimation();
					StopIronFire();
					BP_Death();
				}
			}
		}
		else //Explotion for default (Launcher , Rocket , Torret) ->Impulse Getting By Respective Projectile Actor
		{
			if (IsValid(OwnController))
			{
				OwnController->UnPossess();
				StopIronFire();
				GetMesh()->SetCollisionProfileName("Pawn");
				GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
				GetMesh()->SetSimulatePhysics(true);
				BP_Death();

			}
		}
	}
}


////////
//////// DeathMontages
////////

void AOR_Enemy::PlayMontageRandomDeathAnimation()
{
	if (IsValid(EnemyAnimInstaceReference))
	{
		EnemyAnimInstaceReference->SetBlendIronSightWeigth(1.0f); //Adjust Skeleton Animation Mesh

		int32 Section = FMath::RandRange(1, 5); //Random Animation for Death
		switch (Section)
		{
		case 1:
			EnemyAnimInstaceReference->Montage_Play(DeathMontage, 1.0f);
			EnemyAnimInstaceReference->Montage_JumpToSection("Slot1", DeathMontage);
			break;
		case 2:
			EnemyAnimInstaceReference->Montage_Play(DeathMontage, 1.0f);
			EnemyAnimInstaceReference->Montage_JumpToSection("Slot2", DeathMontage);

			break;
		case 3:
			EnemyAnimInstaceReference->Montage_Play(DeathMontage, 1.0f);
			EnemyAnimInstaceReference->Montage_JumpToSection("Slot3", DeathMontage);

			break;
		case 4:
			EnemyAnimInstaceReference->Montage_Play(DeathMontage, 1.0f);
			EnemyAnimInstaceReference->Montage_JumpToSection("Slot4", DeathMontage);

			break;
		case 5:
			EnemyAnimInstaceReference->Montage_Play(DeathMontage, 1.0f);
			EnemyAnimInstaceReference->Montage_JumpToSection("Slot5", DeathMontage);

			break;
		default:
			break;
		}
	}

}


////////////////////////////////////////////////////////////////////
//
//  Character Combat 
//
////////////////////////////////////////////////////////////////////

////////
//////// SetTimer depends of Moving
////////

void AOR_Enemy::StartIroSightFire(bool Moving)
{
	if (bIsIronFiringMoving || bIsIronFiringStanding) { return;}

	if (!Moving) //if  AI are Standing his firing rate is faster and precise. (with Montage)

	{
		GetWorld()->GetTimerManager().SetTimer(ShootIronHandle, this, &AOR_Enemy::Firing, ShootStandRatio, true, 0.0f);
		bIsIronFiringStanding = true;
	}
	else //if AI are moving his firing rate is lowerand more impressive. (Not Montage)
	{
		GetWorld()->GetTimerManager().SetTimer(ShootIronHandle, this, &AOR_Enemy::Shoot, ShootMovingRatio, true, ShootMovingRatio);
		bIsIronFiringMoving = true;
	}

	bIsFiring = true;
		
}


////////
//////// Play Monntage Firing
////////

void AOR_Enemy::Firing()
{
	if (IsValid(EnemyAnimInstaceReference))
	{

	  if (IsValid(ShootHipsMontage) && IsValid(ShootIronSightMontage))
		{
			if (bIsHipsFiring) //->> Not Used Yet
			{
				EnemyAnimInstaceReference->Montage_Play(ShootHipsMontage, 1.0f);
				EnemyAnimInstaceReference->Montage_JumpToSection("Default", ShootHipsMontage);
			}
			else
			{

				if (CurrentMunition <= 0) //Check Munition
				{
					bHasNeedReload = true;
					return;
				}
				else
				{
					EnemyAnimInstaceReference->Montage_Play(ShootIronSightMontage, 1.0f);
					EnemyAnimInstaceReference->Montage_JumpToSection("Default", ShootIronSightMontage);
					CurrentMunition--;
				}
				
			}
		}
	  
	}

}


////////
//////// Stop Firing timer and montage
////////

void AOR_Enemy::StopIronFire()
{
	/*Clean Timer and Stop Firing*/
	GetWorld()->GetTimerManager().ClearTimer(ShootIronHandle);

	if (bIsIronFiringMoving)
	{
		bIsIronFiringMoving = false;
	}
	if (bIsIronFiringStanding)
	{
		bIsIronFiringStanding = false;
	}

	bIsFiring = false;
}


////////
//////// Shoot Spawn Projectile and discount Munition
////////

void AOR_Enemy::Shoot() //Called by Anim Notify
{
	if (CurrentMunition <= 0)
	{
		bHasNeedReload = true;
		return;
	}
	if (IsValid(BulletClass))
	{
		/*Get Socket Transforms*/
		FVector MuzzleLocation = Weapon->GetSocketLocation("muzzlee");
		FRotator MuzzleRotation = Weapon->GetSocketRotation("muzzlee");
		MuzzleRotation.Pitch = MuzzleRotation.Pitch + FMath::RandRange(ShakePitchMinRatio, ShakePitchMaxRatio);  //Shake Pitch
		MuzzleRotation.Yaw = MuzzleRotation.Yaw + FMath::RandRange(ShakeYawMinRatio, ShakeYawMaxRatio);  //Shake Yaw
	

		/*Spawn Projectile*/
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, MuzzleLocation, MuzzleRotation);

		BP_Shoot();
	}

	CurrentMunition--;
}

//  Start Reload Function
void AOR_Enemy::StartReload()
{
	if (IsValid(IronSightReload) && IsValid(EnemyAnimInstaceReference))
	{
		EnemyAnimInstaceReference->Montage_Play(IronSightReload, 1.0f);
		EnemyAnimInstaceReference->Montage_JumpToSection("Default", IronSightReload);
		bIsRealod = true;
		
	}
}

//  End Reload Function called by Anim Nofity 
void AOR_Enemy::EndReload()
{
	bIsRealod = false;
	bHasNeedReload = false;
	CurrentMunition = MaxMunition;
}

//Not Yet
void AOR_Enemy::HitBulletReact()
{
	if (IsValid(HitReactMontage) && IsValid(EnemyAnimInstaceReference))
	{
		EnemyAnimInstaceReference->Montage_Play(HitReactMontage, 1.0f);
		EnemyAnimInstaceReference->Montage_JumpToSection("Default", HitReactMontage);
	}
}
