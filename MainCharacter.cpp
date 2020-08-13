/****************** Sergio López ******************/

/********* Mainplayer Character Class (.CPP) *********/

/********** Dungeon Temple Pérsonal Project *********/

//My Class
#include "MainCharacter.h"
#include "Enemy.h"
#include "Weapon.h"
#include "ItemStorage.h"
#include "FirstSaveGame.h"
#include "MainPlayerController.h"

//UE4 Class
#include "Engine/World.h"
#include "TimerManager.h"
#include "Containers/Array.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AMainCharacter::AMainCharacter()
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Collision "Camera" SprinArm
	CameraB = CreateDefaultSubobject <USpringArmComponent>(TEXT("CameraB"));
	CameraB->SetupAttachment(GetRootComponent()); // RootComponent
	CameraB->TargetArmLength = 600.f; // Distance to follow
	CameraB->bUsePawnControlRotation = true; //Control Pawn Rotation true


	//Capsule Default -> Adjust on Blueprints
	//GetCapsuleComponent()->SetCapsuleSize(50.f, 90.f);


	 //Follow Camera
	FollowCamera = CreateDefaultSubobject <UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraB, USpringArmComponent::SocketName); //Attack to Socket
	FollowCamera->bUsePawnControlRotation = false; //Control Pawn Rotation


	//Input Values Rates
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.F;


	// Just Camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;


	//CharacterMovement Initial Propertys
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;


	//Playe Stats
	MaxHealth = 100.f;
	Health = 100.f;
	MaxStamina = 100.f;
	Stamina = 65.f;
	Coins = 0;
	ComboCount = 0;
	DamageReceived = 0;


	//Sprint/Run States 
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	bShiftKeyDown = false;


	// Initialize Enums 
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::EMS_Normal;

	StaminaDrainRate = 30.f;
	MinSprintStamina = 20.f;

	// Weapom 
	bLMBDown = false;
	bAttacking = false;
	bBlockingHit = false;

	 
	 //InterSpeed
	 InterpSpeed = 15.f;

	 bHasInterpToEnemy = false;


	 //PauseMenu
	 bESCDown = false;

	 bDeathHUD = false;

	 bIsInCombo = false;

	 Section1 = 1;

}


////////////////////////////////////////////////////////////////////
//
//  BeginPlay
//
////////////////////////////////////////////////////////////////////

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AMainPlayerController>(GetController());

	WorldLevel= GetWorld();
	
	/* Debug Sphere Line*/
	// UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), 25.f, 8, FLinearColor::Green, 10.f, 5.f);

}

////////////////////////////////////////////////////////////////////
//
//  Event Tick
//
////////////////////////////////////////////////////////////////////

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	
	float DeltaStamina = StaminaDrainRate * DeltaTime;


	/** stamina rate, by state for smooth movement with the HUD*/

	switch (StaminaStatus)
	{
	case EStaminaStatus::EMS_Normal: //Blue State on Hud
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::EMS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::EMS_BelowMinimum: // Yellow State on Hud
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::EMS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else // When Shift Key Up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::EMS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::EMS_Exhausted: //Empty ->can't sprint until a specified percentage is recovered

		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::EMS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}

		SetMovementStatus(EMovementStatus::EMS_Normal);
			break;

	case EStaminaStatus::EMS_ExhaustedRecovering: //Red
		  
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
            SetStaminaStatus(EStaminaStatus::EMS_Normal);
            Stamina += DeltaStamina;
		}
		else
		{
		    Stamina += DeltaStamina;
		}

		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		;
	}


	/*Combat Automatic Rotation to enemy targets*/
	if (bHasInterpToEnemy && IsValid(CombatTarget))
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	/* Check on Blueprints and Code 

	if (bBlockingHit)
	{
		FVector FinalLocation = GetActorLocation();
		FinalLocation.X -= 20;
		FVector InterpLocation = FMath::VInterpTo(GetActorLocation(), FinalLocation, DeltaTime, InterpSpeed);
		SetActorRelativeLocation(InterpLocation);
		AddActorLocalOffset(FVector(-2, 0, 0));
	}

	*/

}

////////////////////////////////////////////////////////////////////
//
//  Save/ Load / SwitchLevel -> Game
//
////////////////////////////////////////////////////////////////////

/////////
///////// Switch level (Check that the name is correct and exact)
/////////

void AMainCharacter::SwitchLevel(FName LevelName)
{
	if (IsValid(WorldLevel))
	{
		FString CurrentLevel = WorldLevel->GetMapName();

		FName CurrentLevelName(*CurrentLevel);//// Convert String to FName , Pointer reference
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(WorldLevel, LevelName);
		}
	}
}


/////////
/////////  Save / Load Game with stats
/////////

void AMainCharacter::SaveGame()
{
	UFirstSaveGame* SaveGameInstace = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass())); //Get My Save Game Instance , Static 

	/*
	   Current Player Stats
	*/
	if (IsValid(SaveGameInstace))
	{
		SaveGameInstace->CharacterStats.Healt = Health;
		SaveGameInstace->CharacterStats.MaxHealt = MaxHealth;
		SaveGameInstace->CharacterStats.Stamina = Stamina;
		SaveGameInstace->CharacterStats.MaxStamina = MaxStamina;
		SaveGameInstace->CharacterStats.Coins = Coins;

		/*
		   Weapon
		*/

		if (IsValid(EquippWeapon))
		{
			SaveGameInstace->CharacterStats.WeaponName = EquippWeapon->Name;  //Take by Name.
		}

		/*
		  Current Transform Location Rotation  Actor
		*/

		SaveGameInstace->CharacterStats.Locations = GetActorLocation();
		SaveGameInstace->CharacterStats.Rotation = GetActorRotation();

		/*Save Slot*/
		UGameplayStatics::SaveGameToSlot(SaveGameInstace, SaveGameInstace->PlayerName, SaveGameInstace->UserIndex);
	}

}

void AMainCharacter::LoadGame(bool SetPotions)
{
	UFirstSaveGame* LoadGameInstace = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass())); //Get My Load Game Instance , Static 

	if (IsValid(LoadGameInstace))
	{
		LoadGameInstace = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstace->PlayerName, LoadGameInstace->UserIndex)); // Get Propertys

		if (IsValid(LoadGameInstace))
		{
			/*
	        Save Player Stats
	        */

			Health = LoadGameInstace->CharacterStats.Healt;
			MaxHealth = LoadGameInstace->CharacterStats.MaxHealt;
			Stamina = LoadGameInstace->CharacterStats.Stamina;
			MaxStamina = LoadGameInstace->CharacterStats.MaxStamina;
			Coins = LoadGameInstace->CharacterStats.Coins;

			/*
			 Weapon
			*/

			if (IsValid(WeaponStorage))
			{
				AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);    //Weapon Storage  subobject class  IntemStorage.h

				if (IsValid(Weapons))
				{
					FString WeaponName = LoadGameInstace->CharacterStats.WeaponName;       //Load Name Saved

					AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);   //	Equip Weapon 

					if (IsValid(WeaponToEquip))
					{
						WeaponToEquip->Equip(this); ///Asegurese de tener solo un arma y no dos al tiempo
					}
				}
			}

			/*Load previw Save Transform Location Rotation  Actor*/

			if (SetPotions)
			{
				SetActorLocation(LoadGameInstace->CharacterStats.Locations);
				SetActorRotation(LoadGameInstace->CharacterStats.Rotation);
			}
		}
	}
	
}


////////////////////////////////////////////////////////////////////
//
//  Character class / Input
//
////////////////////////////////////////////////////////////////////

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent) //Macro Player Inpunt COmponent is valido

	PlayerInputComponent->BindAction("Jumping", IE_Pressed , this ,&AMainCharacter::Jump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::BlockingHit);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainCharacter::ESCUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUpAtRate);


}


///////
/////// Basic Input Movement
///////

void AMainCharacter::MoveForward(float value)
{
	if ((Controller != nullptr) && (value != 0.0f) && (!bAttacking) && (MovementStatus !=EMovementStatus::EMS_Dead))
	{
		// Get Rotation Values
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector  Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // Get Matrix Rotation X Direction adjust movement to the correct side
		AddMovementInput(Direction, value);
	}
}

void AMainCharacter::MoveRight(float value)
{
	// Get Rotation Values
	if ((Controller != nullptr) && (value != 0.0f) &&(!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector  Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // Get Matrix Rotation Y Direction adjust movement to the correct side
		AddMovementInput(Direction, value);

	}
}

void AMainCharacter::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


///////
/////// Rest of inputs
///////

void AMainCharacter::LMBDown() // Left Mouse Input
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (IsValid(ActiveOverlappingItem))
	{
		/*Cast Overlapping Weapon*/
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);

		/*Equiped that Preview Cast Weapon*/
		if (IsValid(Weapon))
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (IsValid(EquippWeapon))
	{
		Attack();
	}
}

void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}


// ESCInput
void AMainCharacter::ESCDown()
{
	bESCDown = true;

}
void AMainCharacter::ESCUp()
{
	bESCDown = false;
}


// Shit Key Input
void AMainCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}


////////////////////////////////////////////////////////////////////
//
//  Character Player Combat
//
////////////////////////////////////////////////////////////////////

///////
/////// Attacking Start /End and Combo 
///////

void AMainCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		bBlockingHit = false; // Can't attack and block at the same time
		SetInterpToEnemy(true); // Get Interp Enemy

		 AnimInstance = GetMesh()->GetAnimInstance();

		if (IsValid(AnimInstance) && IsValid(CombatMontage) && ComboCount <= 10)
		{
			int32 Section = FMath::RandRange(1, 4);

			//Choose Random Attack Montage
			switch (Section1)
			{
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 2:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			case 3:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
				break;
			case 4:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_4"), CombatMontage);
				break;

			default:
				;
			}

			Section1++;  //Control Slots To give Variety

			if (Section1 >= 5) 
			{
				Section1 = 1;
			}

		}
		else if (IsValid(AnimInstance) && IsValid(CombatMontage) && ComboCount >= 10) //Final Combo 
		{
			AnimInstance->Montage_Play(CombatMontage, 1.4f);
			AnimInstance->Montage_JumpToSection(FName("Taunt"), CombatMontage); //Taunt Montage 
			ComboCount = 0;
			bIsInCombo = true;
		}
	}
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;

	/*Keep Attacking if Key Still Pressed*/
	if (bLMBDown)
	{
		SetInterpToEnemy(false);
		Attack();
	}
}

void AMainCharacter::Setcombotofalse()
{
	bIsInCombo = false;

}


///////
/////// Received Hits and Damgae
///////

void AMainCharacter::receivedHit()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bBlockingHit) {

		AnimInstance = GetMesh()->GetAnimInstance();

		if (IsValid(AnimInstance)&& CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 2.0f);
			AnimInstance->Montage_JumpToSection(FName("Hit"), CombatMontage);
		}

	}	
}

void AMainCharacter::ReceiveLeftHit()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bBlockingHit) {

		 AnimInstance = GetMesh()->GetAnimInstance();

		if (IsValid(AnimInstance) && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 2.0f);
			AnimInstance->Montage_JumpToSection(FName("LeftHit"), CombatMontage);
		}
	}
}

void AMainCharacter::ReceiveRightHit()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bBlockingHit) {

		AnimInstance = GetMesh()->GetAnimInstance();

		if (IsValid(AnimInstance) && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 2.0f);
			AnimInstance->Montage_JumpToSection(FName("RightHit"), CombatMontage);
		}
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount > 0.f)
	{
	  DrecrementHealth(DamageAmount);
	}

	if (IsValid(DamageCauser))
	{
		DamageReceived += DamageAmount;

		AEnemy* Enemy = Cast <AEnemy>(DamageCauser);

		if (IsValid(Enemy))
		{
			if (Health <= 0)
			{
				//Died
				Enemy->bHasValidTarget = false;
				Enemy->SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			}
		}
	}

	//Timer to Set Cero Damage
	GetWorld()->GetTimerManager().SetTimer(Text, this, &AMainCharacter::CeroDamage, 0.8, true);

	return DamageAmount;
}

void AMainCharacter::BlockingHit()
{
	bBlockingHit = true;

	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		 AnimInstance = GetMesh()->GetAnimInstance();

		if (IsValid(AnimInstance))
		{
			AnimInstance->Montage_Play(CombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Block"), CombatMontage);
		}
	
	}
}

void AMainCharacter::UpDateCombatTarget()
{
	TArray <AActor*> OverlappingActors;

	if (IsValid(EnemyFilter))
	{
		GetOverlappingActors(OverlappingActors, EnemyFilter); //I get all Enemy actors it overlaps -> enemy subclass to filter

		if (OverlappingActors.Num() == 0) return;  

		AEnemy* ClosesEnemy = Cast<AEnemy>(OverlappingActors[0]); //Casteo to enemy

		if (IsValid(ClosesEnemy))
		{
			FVector Location = GetActorLocation();

			FVector MinDistance1 = ClosesEnemy->GetActorLocation() - Location; // Minimal distance

			float MinDistance = MinDistance1.Size();


			for (auto Actor : OverlappingActors) //Loop the arrangement of the actors that I'm fighting
			{
				AEnemy* Enemy = Cast<AEnemy>(Actor);

				if (IsValid(Enemy))
				{
					FVector DistanceToActor1 = (Enemy->GetActorLocation() - Location);
					float DistanceToActor = DistanceToActor1.Size();

					/*Pick Closes Enemy*/
					if (DistanceToActor < MinDistance)
					{
						MinDistance = DistanceToActor;
						ClosesEnemy = Enemy;
					}
				}
				
			}

			SetCombatTarget(ClosesEnemy);
		}
	}
}

////////////////////////////////////////////////////////////////////
//
//  Character Movement / State Propertys
//
////////////////////////////////////////////////////////////////////

void AMainCharacter::ShowPickUpLocations()
{
	for (FVector Location : PickUpLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 5.f);

	}

	// BUSQUE EN REDDIT Y ANALISE CUAL ES MAS OPTIMA DEPENDIENDO DE LOS CASOS(NO OLVIDAR!)

	/*for (int32 i = 0; i <= PickUpLocations.Num(); i++)       //Clasica
	{
	   UKismetSystemLibrary::DrawDebugSphere(this, PickUpLocations[1], 25.f, 8, FLinearColor::Green, 10.f, 5.f); )
												
	}*/

	/* for (auto Location :PickUpLocations)       //Auto
	{
	   UKismetSystemLibrary::DrawDebugSphere(this, Locations, 25.f, 8, FLinearColor::Green, 10.f, 5.f);
	}
	*/
}

// Set Movement Status Speed
void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;

	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::IncrementCoins()
{
	Coins += 1;
}

// Interp to Enemy
void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bHasInterpToEnemy = Interp;
}

//Rotation LookAtYaw by Target
FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}


///////
/////// Decrement Health/ Damage and Die 
///////

void AMainCharacter::DrecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMainCharacter::Die()
{
	 AnimInstance = GetMesh()->GetAnimInstance(); 

	if (IsValid(AnimInstance) && IsValid(CombatMontage))
	{
		AnimInstance->Montage_Play(CombatMontage, 1.8f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
		
	}

	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}


void AMainCharacter::CeroDamage() 
{
	DamageReceived = 0;
}






