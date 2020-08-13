/****************** Sergio López  ******************/

/********* Mainplayer Character Class (.CPP) *********/

/************ Orion Pérsonal Project 2020 ***********/


//My Class
#include "Pilars/OR_BasePilar.h"
#include "Character/OR_MainCharacter.h"
#include "Core/GameMode/OR_MyGameMOde.h"
#include "Components/OR_HealthComponent.h"
#include "Projectiles/OR_RocketProjectile.h"
#include "Projectiles/OR_BulletProjectile.h"
#include "Projectiles/OR_LauncherProjectile.h"


//Engine Class
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"



// Base Constructor
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Name Components
	WeaponSocketName = "R_GunSocket";
	MeleeCapsuleSocketName = "Melee";


	// Player Camera Component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh());


	// Player Spring Arm Component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Camera);


	// Player Arms Mesh
	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(SpringArm);


	// Player Weapon Mesh
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(Arms, WeaponSocketName);


	Rocket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rocket"));
	Rocket->SetupAttachment(Arms, WeaponSocketName);


	// Player Weapon Mesh
	MeleeDetector = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MeleeDetector"));
	MeleeDetector->SetupAttachment(Weapon, MeleeCapsuleSocketName);
	MeleeDetector->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeleeDetector->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//HealthComponent
	Health = CreateDefaultSubobject<UOR_HealthComponent>(TEXT("Health"));


	// Set Initial Default  Status
	MovementStatus = EMovementStatus::EMS_Idle;
	CurrentWeaponStatus = ECurrentWeapon::ECW_Rifle;


	// State Movement Variables
	RotationSpeed = 50.0f;
	NormalMaxWalkSpeed = 350.0f;
	SprintMaxWalkSpeed = 700.0F;
	bIsRuning = false;
	bIsMoving = false;
	bIsPointed = false;
	bIsJumping = false;
	bIsReload = false;
	bIsSwitching = false;


	//Set Default Current Velocity
	CurrentVelocity = 0.0f;


	// Open/Close Variables
	bIsDelay = false;
	bIsJumpCalled = false;
	bIsSprintCalled = false;
	bIsReloadCalled = false;
	bIsPointedCalled = false;
	bIsUnderShootCalled = false;
	bIsMeleeAttackCalled = false;
	bIsPointedShootCalled = false;
	bIsGrenadeLauncherCalled = false;


	//Combat Varaibles
	WeaponAmmo = 50;
	GrenadeAmmo = 3;
	bIsShooting = false;
	bIsKeyShootPressed = false;
	bIsMeleeAttack = false;
	bIsGrenadeLauncher = false;
	bIsRuningControlDelay = false;


	//Ultimate Control Varaible
	bIsUltimate = false;
	

	//Specific Ultimate Control Varaible
	bIsAttackUltimate = false;
	bIsDefenceUltimate = false;
	bIsMovilityUltimate = false;


	//Specific Ultimate Has Control Varaible
	bHasAttackUltimateReady = false;
	bHasDefenceUltimateReady = false;
	bHasMovilityUltimateReady = false;


	//Ultimate Propertys Varaibles
	PlayerRate = 1.0f;


	//Specific Ultimate Duration Varaibles
	AttackUltiMaxDuration = 20.0f;
	DefenceUltiMaxDuration = 20.0f;
	MovilityUltiMaxDuration = 20.0f;


	//Movility Ultimate Speeds
	UltimateMovilityNormalSpeed = 900.f;
	UltimateMovilitySprintSpeed = 1100.0f;
	

	//Drain Rate Ultimates
	DrainRateUltimate = 0.05f;


	//Specific Ultimates Duration
	AttackCurrentDuration = 0.f;
	DefenceCurrentDuration = 0.f;
	MovilityCurrentDuration = 0.f;
	

	//Pilar COntrol Varaibles 
	bIsOnPilarAttack = false;
	bIsOnPilarDefence = false;
	bIsOnPilarMovility = false;


	//Has to be destroy DebugVaraible 
	bHastoDestroy = false;
	
}



////////////////////////////////////////////////////////////////////
//
//   Begin Play
//
////////////////////////////////////////////////////////////////////

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	//SetUp References
	SetupMainReferences();


	//SetRocketWeaponInitialVisibility
	Rocket->SetVisibility(false);

}


//Main References
void AMainCharacter::SetupMainReferences()
{
	// Set AnimInstance
	MainAnimInstance = Arms->GetAnimInstance();

	//Set Game Mode
	GameModeReference = Cast<AOR_MyGameMOde>(GetWorld()->GetAuthGameMode());

	// Mele Combat
	MeleeDetector->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::MakeMeleeDamage);

	// HealthDelegate
	Health->OnHealthChangeDelegate.AddDynamic(this, &AMainCharacter::OnHealthChange);
}



////////////////////////////////////////////////////////////////////
//
//   Player input handling
//
////////////////////////////////////////////////////////////////////

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);  //Check Macro


	// Axis mappings
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	// Mouse Delta
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::RotatePitch);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::RotateYaw);

	//Action Mappings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::EndJump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::StopSprint);

	PlayerInputComponent->BindAction("GunPoint", IE_Pressed, this, &AMainCharacter::StartGunPoint);
	PlayerInputComponent->BindAction("GunPoint", IE_Released, this, &AMainCharacter::EndGunPoint);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::StartShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AMainCharacter::EndShoot);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainCharacter::StartReload);

	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &AMainCharacter::StartMeleeAtaack);

	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &AMainCharacter::StartGrenadeLauncher);

	PlayerInputComponent->BindAction("CurrentUltimate", IE_Pressed, this, &AMainCharacter::ActivateCurrentUltimate);

}



////////////////////////////////////////////////////////////////////
//
//   Character Player Input / Movement
//
////////////////////////////////////////////////////////////////////

//////////
////////// Player Basic Movement Inputs Handles
//////////

void AMainCharacter::MoveForward(float value)
{
	if (value == 0.0f) { return;}
	bIsMoving = true;
	UpdatePlayerProperties();
	AddMovementInput(GetActorForwardVector(), value);	
}

void AMainCharacter::MoveRight(float value)
{
	if (value == 0.0f) { return;}
	bIsMoving = true;
	UpdatePlayerProperties();
	AddMovementInput(GetActorRightVector(), value);	
}

void AMainCharacter::RotatePitch(float value)
{
	AddControllerPitchInput(value * RotationSpeed * GetWorld()->GetDeltaSeconds());	
}

void AMainCharacter::RotateYaw(float value)
{
	if (FMath::Abs(value) < 0.001f) { return; }
	AddControllerYawInput(value * RotationSpeed * GetWorld()->GetDeltaSeconds());
}


//////////
////////// Star / End - Jump Space Bar Method 
//////////

void AMainCharacter::StartJump()
{
	if (!bIsUltimate)
	{
		Jump();
		UpdatePlayerProperties();
	}
}

void AMainCharacter::EndJump()
{
	StopJumping();
}


////////// 
////////// Star / End - Sprint Left Shift Method 
//////////

void AMainCharacter::StartSprint()
{
	/** Check Reload State */
	if (GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
		bIsReload = false;
		StopMyMontage(0.5f);	
	}

	/** Set Variable */
	bIsRuning = true;
	UpdatePlayerProperties();
}

void AMainCharacter::StopSprint()
{
	/** Set Variable */
	bIsRuning = false;
	UpdatePlayerProperties();
}


//////////
////////// Star / End - Gun Pointed Right Mouse Method 
//////////

void AMainCharacter::StartGunPoint()
{
	/** Check Reload State */
	if (GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
		bIsReload = false;
		StopMyMontage(0.2f);
	}

	/* Set Variable */
	bIsPointed = true;
	UpdatePlayerProperties();
}

void AMainCharacter::EndGunPoint()
{
	/** Set Variable */
	bIsPointed = false;
	UpdatePlayerProperties();
}


//////////
////////// Star/ End SwitchWeapon 
//////////

void AMainCharacter::StartSwitchWeapon()
{
	/** Check Combat Status Conditions */
	if (GetCombatStatus() == ECombatStatus::ECS_Grenade || GetCombatStatus() == ECombatStatus::ECS_Melee || GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
		bIsReload = false;
		bIsGrenadeLauncher = false;
		bIsMeleeAttack = false;
		StopMyMontage(0.1);
	}

	if (GetCombatStatus() == ECombatStatus::ECS_FireUnder || GetCombatStatus() == ECombatStatus::ECS_PointedFire)
	{
		EndShootByOther();
	}

	bIsSwitching = true;
	UpdatePlayerProperties();

}

void AMainCharacter::EndSwitchWeapom() // Call By Anim Notify
{

	bIsSwitching = false;

	if (GetCurrentWeaponStatus() == ECurrentWeapon::ECW_Rifle)
	{
		Rocket->SetVisibility(true);
		Weapon->SetVisibility(false);
		SetCurrentWeaponStatus(ECurrentWeapon::ECW_Rocket);
	}
	else
	{
		Rocket->SetVisibility(false);
		Weapon->SetVisibility(true);
		SetCurrentWeaponStatus(ECurrentWeapon::ECW_Rifle);
	}

	UpdatePlayerProperties();
}



////////////////////////////////////////////////////////////////////
//
//   Character Player Combat Functions
//
////////////////////////////////////////////////////////////////////

///////// 
///////// Star/End Shoot 
/////////

void AMainCharacter::StartShoot()
{
	bIsKeyShootPressed = true;

	/** Rocket Shoot */
	if (GetCurrentWeaponStatus() == ECurrentWeapon::ECW_Rocket)
	{
		if (CurrentRocketAmmo > 0 && bIsAttackUltimate)
		{
			RocketShoot();
			bIsShooting = true;
			UpdatePlayerProperties();
		}
		
		return;
	}


	/** Normal Shoot Check Movement Conditions */
	if (WeaponAmmo <= 0)
	{
		StartReload();
		return;
	}

	if (GetCombatStatus() == ECombatStatus::ECS_Grenade || GetCombatStatus() == ECombatStatus::ECS_Melee || bIsSwitching)
	{
		return;
	}

	if (GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
		bIsReload = false;
		StopMyMontage(0.2f);
	}


	/** Set Variables */
	bIsShooting = true;
	UpdatePlayerProperties();
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, this, &AMainCharacter::Shoot, 0.09f, true, 0.0f);

}

void AMainCharacter::EndShoot()
{
	/** Set Variables */
	bIsShooting = false;
	bIsKeyShootPressed = false;
	UpdatePlayerProperties();
	GetWorldTimerManager().ClearTimer(ShootHandle);		
}

void AMainCharacter::EndShootByOther()
{
	/** Set Variables */
	bIsShooting = false;
	UpdatePlayerProperties();
	GetWorldTimerManager().ClearTimer(ShootHandle);
}


//////////
////////// Star/ End Reload 
//////////

void AMainCharacter::StartReload()
{
	/** Check Combat Status Conditions*/
    if (WeaponAmmo >= 50 || GetCombatStatus() == ECombatStatus::ECS_Reload || GetCombatStatus() == ECombatStatus::ECS_Grenade || bIsSwitching)
	{ 
		return;
	}
	
	if (GetCombatStatus() == ECombatStatus::ECS_FireUnder || GetCombatStatus() == ECombatStatus::ECS_PointedFire)
	{ 
		EndShootByOther(); 
	}

	if (GetCombatStatus() == ECombatStatus::ECS_Melee)
	{
		bIsMeleeAttack = false;
		StopMyMontage(0.8f);
	}


	/** Set Variables / Play montage */
	bIsReload = true;
	UpdatePlayerProperties();
	PlayMyMontage(ReloadMontage, 1.2f, FName("Reload"));

	/** Spawn Emitter */
	if (IsValid(SmokeReload))
	{
		UGameplayStatics::SpawnEmitterAttached(SmokeReload, Weapon, FName("RifleMag"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.13f, 0.01, 0.01f));
	}
}

void AMainCharacter::EndReload() //Call by Anim Notify
{
	/** Set Variables / Stop Montage */
	WeaponAmmo = 50;
	bIsReload = false;
	StopMyMontage(0.3f);
	UpdatePlayerProperties();	

	/** If Left Mouse Button Still Pressed Continue Shooting */
	if (bIsKeyShootPressed) { StartShoot(); }	
}


////////// 
////////// Star/ End MeleeAttack  and Melee Collisio/Damage
//////////

void AMainCharacter::StartMeleeAtaack()
{
	/** Check Combat Status Conditions*/
	if (GetCombatStatus() == ECombatStatus::ECS_Melee || GetCombatStatus() == ECombatStatus::ECS_Grenade || bIsSwitching)
	{ 
		return; 
	}

	if (GetCombatStatus() == ECombatStatus::ECS_FireUnder || GetCombatStatus() == ECombatStatus::ECS_PointedFire) 
	{ 
		EndShootByOther();
	}

	if (GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
		bIsReload = false;
		StopMyMontage(0.2f);
	}

	/** Set Variables / Play Montage*/
	bIsMeleeAttack = true;
	UpdatePlayerProperties();
	PlayMyMontage(MeleMontage, 1.0f, "Melee");

}

void AMainCharacter::EndMeleeAttack()
{
	/** Set Variables / End Montage*/
	bIsMeleeAttack = false;
	UpdatePlayerProperties();
	StopMyMontage(1.0f);

	/** If Left Mouse Button Still Pressed Continue Shooting */
	if (bIsKeyShootPressed) { StartShoot(); }
}

void AMainCharacter::SetEnumMeleeCollision(ECollisionEnabled::Type CollisionState) //Call By Anim Notify
{
	MeleeDetector->SetCollisionEnabled(CollisionState);
}

void AMainCharacter::MakeMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor)&& IsValid(MyDamageType))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, 20.f, SweepResult.Location, SweepResult, GetInstigatorController(), this, MyDamageType);
	}
}


//////////
////////// Star/ End GrenadeLauncher 
//////////

void AMainCharacter::StartGrenadeLauncher()
{
	/** Check Movement Conditions*/
	if (GrenadeAmmo <= 0 || GetCombatStatus() == ECombatStatus::ECS_Grenade || bIsSwitching)
	{ 
	     return;
	}

	if (GetCombatStatus() == ECombatStatus::ECS_FireUnder || GetCombatStatus() == ECombatStatus::ECS_PointedFire) 
	{ 
	    EndShootByOther(); 
	}

	if (GetCombatStatus() == ECombatStatus::ECS_Melee)
	{
		bIsMeleeAttack = false;
		StopMyMontage(0.8f);
	}

	if (GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
	    bIsReload = false;
		StopMyMontage(0.2f);
	}

	if (GetMovementStatus() == EMovementStatus::EMS_Sprinting)
	{
		bIsRuningControlDelay = true;
	}

	/** Set Variables / Play Montage*/
	bIsGrenadeLauncher = true;
	PlayMyMontage(GrenadeMontage, 0.8f, "Grenade");
	UpdatePlayerProperties();	
}

void AMainCharacter::SpawnGrenadeLauncher() //Call By Anim Notify
{
	if (IsValid(LauncherClass))
	{
		FVector GrenadeLocation = Arms->GetSocketLocation("GrenadeSocket");
		FRotator GrenadeRotation = Camera->GetComponentRotation();
		GrenadeRotation.Pitch = GrenadeRotation.Pitch + 10; //Adjust Pitch

		/*Spawn Projectile*/
		AOR_LauncherProjectile* Launcher = GetWorld()->SpawnActor<AOR_LauncherProjectile>(LauncherClass, GrenadeLocation, GrenadeRotation);

		/*Set Main Class Reference to Projectile*/
		Launcher->SetMain(this);
	}
}

void AMainCharacter::EndGrenadeLauncher() //Call By Anim Notify
{
	if (bIsRuningControlDelay)
	{
		bIsRuningControlDelay = false;
	}

	/** Set Variables / End Montage*/
	bIsGrenadeLauncher = false;
	UpdatePlayerProperties();
	StopMyMontage(0.2f);

	/** If Left Mouse Button Still Pressed Continue Shooting */
	if (bIsKeyShootPressed) { StartShoot(); }
}



////////////////////////////////////////////////////////////////////
//
//   Character Ultimates Functions
//
////////////////////////////////////////////////////////////////////

//////////
//////////  Activate ultimate depending on the pillars
//////////

void AMainCharacter::ActivateCurrentUltimate()
{
	/**Check if Pawn already in ultimate*/
	if (!bHasAttackUltimateReady && !bHasDefenceUltimateReady && !bHasMovilityUltimateReady ||  bIsUltimate)
	{
		return;
	}
	else
	{
		if (bHasAttackUltimateReady)
		{
			if (!bIsJumping)
			{
				bIsUltimate = true;
				bIsAttackUltimate = true;
				bHasAttackUltimateReady = false;
				AttackCurrentDuration = AttackUltiMaxDuration;

				BP_StarAttackUltimate(); //Call Camera Function -> the BP_End function is called on Tick else Jump;

				GetWorld()->GetTimerManager().SetTimer(AttackUltimateHandle, this, &AMainCharacter::StartAttackUltimate, 0.5f, false, 0.5f);// Timer to reproduce preview Animation Current Ultimate and then active ultimate
	
			}
		}

		if (bHasMovilityUltimateReady)
		{
			bIsUltimate = true;
			bIsMovilityUltimate = true;
			bHasMovilityUltimateReady = false;
			MovilityCurrentDuration = MovilityUltiMaxDuration;

			StartMovilityUltimate();
		}

		if (bHasDefenceUltimateReady)
		{
			if (IsValid(MyPilarsReference))
			{
				bIsUltimate = true;
				bIsDefenceUltimate = true;
				bHasDefenceUltimateReady = false;
				DefenceCurrentDuration = DefenceUltiMaxDuration;

				StartDefenceUltimate();

			}
		}

		/** Ultimate Counting Decrease Durations Handle */
		GetWorld()->GetTimerManager().SetTimer(UltimateDurationHandle, this, &AMainCharacter::UltimateCountingDuration, DrainRateUltimate, true, 0.0f);
	}
		
}


//////////
////////// Attack Ultimate 
//////////

void AMainCharacter::StartAttackUltimate()
{
	GetWorldTimerManager().ClearTimer(AttackUltimateHandle);
	GetCharacterMovement()->JumpZVelocity = 2200.f;
	GetCharacterMovement()->AirControl = 3.0f;
	CurrentRocketAmmo = RocketAmmo;
	Jump();
	StartSwitchWeapon();
}

void AMainCharacter::EndAttackUltimate()
{
	bIsAttackUltimate = false;
	GetCharacterMovement()->GravityScale = 1.0f;
	CurrentRocketAmmo = RocketAmmo;
}


//////////
////////// Movility Ultimate 
//////////

void AMainCharacter::StartMovilityUltimate()
{
	if (GetMovementStatus() == EMovementStatus::EMS_Sprinting)
	{
		BP_EndCameraSprint();
		bIsSprintCalled = false;
	}

	PlayerRate = MovilityUltimatePlayerRate;
	UpdatePlayerProperties();
}

void AMainCharacter::EndMovilityUltimate()
{
	bIsMovilityUltimate=false;
	bIsUltimate = false;
	PlayerRate = 1.0f;

	BP_EndMovilityUltimate();

	/**Check if pawn are running when the ultimate ends and continue sprinting fluently*/
	if (GetMovementStatus() == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintMaxWalkSpeed;
		BP_StarCameraSprint();
	}
	else
	{
		
		GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
	}
	
	UpdatePlayerProperties();
}


//////////
////////// Defence Ultimate 
//////////

void AMainCharacter::StartDefenceUltimate()
{
	BP_StartDefenceUltimate();

	if (IsValid(MyPilarsReference))
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), MyPilarsReference, MyPilarsReferencesArray);

		// Direct Direction
		for (auto& Pi : MyPilarsReferencesArray)
		{
			AOR_BasePilar* CurrentPilarObject = Cast<AOR_BasePilar>(Pi);

			if (IsValid(CurrentPilarObject))
			{
				CurrentPilarObject->StartUltimate();
			}
		}

	}

}

void AMainCharacter::EndDefenceUltimate()
{
	BP_EndDefenceUltimate();

	bIsDefenceUltimate = false;
	bIsUltimate = false;

	if (IsValid(MyPilarsReference))
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), MyPilarsReference, MyPilarsReferencesArray);

		// Direct Direction
		for (auto& Pi : MyPilarsReferencesArray)
		{
			AOR_BasePilar* CurrentPilarObject = Cast<AOR_BasePilar>(Pi);

			if (IsValid(CurrentPilarObject))
			{
				CurrentPilarObject->StopUltimate();
			}
		}

	}

}


//////////
////////// Ultimate Counting Duration depends of current ultimante
//////////

void AMainCharacter::UltimateCountingDuration()
{
	if (bIsAttackUltimate)
	{
		if (AttackCurrentDuration-DrainRateUltimate <= 0)
		{
			AttackCurrentDuration = 0;
			EndAttackUltimate();
			GetWorldTimerManager().ClearTimer(UltimateDurationHandle);
			return;
		}
		else
		{
			AttackCurrentDuration -= DrainRateUltimate;
			return;
		}
		
	}

	if (bIsMovilityUltimate)
	{
		if (MovilityCurrentDuration- DrainRateUltimate <= 0)
		{
			MovilityCurrentDuration = 0;
			EndMovilityUltimate();
			GetWorldTimerManager().ClearTimer(UltimateDurationHandle);
			return;
		}
		else
		{
			MovilityCurrentDuration-= DrainRateUltimate;
			return;
		}	
	}

	if (bIsDefenceUltimate)
	{
		if (DefenceCurrentDuration - DrainRateUltimate <= 0)
		{
			DefenceCurrentDuration = 0;
			EndDefenceUltimate();
			GetWorldTimerManager().ClearTimer(UltimateDurationHandle);
		}
		else
		{
			DefenceCurrentDuration-= DrainRateUltimate;
		}	
	}

}


//////////
////////// One Heatl Change Delegate 
//////////

void AMainCharacter::OnHealthChange(UOR_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	BP_ReceiveDamage();

	/*Check If Pawn is already dead with the new damage*/
	if (CurrentHealthComponent->GetIsDead())
	{
		if (IsValid(GameModeReference))
		{
			UE_LOG(LogTemp, Warning, TEXT("Ta Rigus Mortis"));
			BP_Death();
			GameModeReference->GameOver(this);
		}
		
	}

}



////////////////////////////////////////////////////////////////////
//
//  Shots Function
//
////////////////////////////////////////////////////////////////////

//////////
////////// Normal Weapon Shoot Function 
//////////

void AMainCharacter::Shoot()
{
	if (WeaponAmmo <= 0)
	{
		EndShootByOther();
		StartReload();
		return;
	}

	if (IsValid(BulletClass))
	{
		if (IsValid(MuzzleShoot1) && IsValid(MuzzleShoot2) && IsValid(ShellEject) && IsValid(SmokeMuzzle) && IsValid(SmokeShell))
		{
			if (GetMovementStatus() == EMovementStatus::EMS_Pointing)
			{
				/*Play Random Section*/
				int32 Section = FMath::RandRange(1, 2);
				switch (Section)
				{
				case 1:
					PlayMyMontage(PointedShoot_Montage, 2.0F, FName("PShoot01"));
					break;
				case 2:
					PlayMyMontage(PointedShoot_Montage, 2.0F, FName("PShoot02"));
					break;
				default:
					break;
				}

				/*Get Socket Transforms*/
				FVector MuzzleLocation = Weapon->GetSocketLocation("Muzzle");
				FRotator MuzzleRotation = Weapon->GetSocketRotation("Muzzle");
				MuzzleRotation.Pitch = MuzzleRotation.Pitch - 4.5f; //Adjust Socket Rotation for pointed

				/*Spawn Projectile*/
				AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, MuzzleLocation, MuzzleRotation);
				Projectile->SetMain(this); // Set This to Projectile

				/** Spawn Emitters */
				UGameplayStatics::SpawnEmitterAttached(MuzzleShoot1, Weapon, FName("Muzzle"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.3f));
				UGameplayStatics::SpawnEmitterAttached(MuzzleShoot2, Weapon, FName("Muzzle"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.15f));
				UGameplayStatics::SpawnEmitterAttached(SmokeMuzzle, Weapon, FName("Muzzle"), FVector(ForceInitToZero), GetActorLocation().ToOrientationRotator(), FVector(0.02f, 0.02f, 0.15f));
				UGameplayStatics::SpawnEmitterAttached(SmokeShell, Weapon, FName("Smoke"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.2f, 0.6f, 0.6f));
				UGameplayStatics::SpawnEmitterAttached(ShellEject, Weapon, FName("Shell"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.6f));
			}
			else
			{
				/*Play Random Section*/
				int32 Section = FMath::RandRange(1, 2);
				switch (Section)
				{
				case 1:
					PlayMyMontage(ShootMontage, 2.0f, FName("Shoot01"));
					break;
				case 2:
					PlayMyMontage(ShootMontage, 2.0f, FName("Shoot02"));
					break;
				default:
					break;
				}

				/*Get Socket Transforms*/
				FVector MuzzleLocation = Weapon->GetSocketLocation("Muzzle");
				FRotator MuzzleRotation = Weapon->GetSocketRotation("Muzzle");
				MuzzleRotation.Pitch = MuzzleRotation.Pitch - FMath::FRandRange(2.f , 5.0f);  // FMath::FRandRange(0, 2.5); // 2.5f; //Adjust Socket Rotation for Fire Under 
				MuzzleRotation.Yaw = MuzzleRotation.Yaw + FMath::FRandRange(2.f , 5.0f); //FMath::FRandRange(0, 2.0); //2;//Adjust Socket Rotation Fire Under


				/*Spawn Projectile*/
				AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, MuzzleLocation, MuzzleRotation);
				Projectile->SetMain(this); // Set This to Projectile


				/** Spawn Emitters */
				UGameplayStatics::SpawnEmitterAttached(MuzzleShoot1, Weapon, FName("Muzzle"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.6f));
				UGameplayStatics::SpawnEmitterAttached(MuzzleShoot2, Weapon, FName("Muzzle"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.3f));
				UGameplayStatics::SpawnEmitterAttached(SmokeMuzzle, Weapon, FName("Muzzle"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.02f, 1.5f, 0.15f));
				UGameplayStatics::SpawnEmitterAttached(SmokeShell, Weapon, FName("Smoke"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.15f, 0.2f, 0.7f));
				UGameplayStatics::SpawnEmitterAttached(ShellEject, Weapon, FName("Shell"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(0.6f));
			}
		}

		WeaponAmmo--;
	}
}


//////////
////////// Rocket Shoot Function 
//////////

void AMainCharacter::RocketShoot()
{
	if (IsValid(RocketClass) && IsValid(Weapon) && IsValid(FinalGun))
	{
		/*Get Socket Transforms*/
		FVector MuzzleLocation = Weapon->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = Weapon->GetSocketRotation("Muzzle");
		MuzzleRotation.Pitch = MuzzleRotation.Pitch - 4.f; //	Adjust Projectiel Direction

		/*Spawn Projectile*/
		AOR_RocketProjectile* MyRocket = GetWorld()->SpawnActor<AOR_RocketProjectile>(RocketClass, MuzzleLocation, MuzzleRotation);
		MyRocket->SetMain(this);

		CurrentRocketAmmo--;

		/*Check State*/
		if (GetMovementStatus() != EMovementStatus::EMS_Pointing)
		{
			PlayMyMontage(ShootMontage, 1.0f, FName("Shoot01"));
			UGameplayStatics::SpawnEmitterAttached(FinalGun, Weapon, FName("Muzzle"), FVector(ForceInitToZero), FRotator::ZeroRotator, FVector(2.0f));
		}

		/*Check Ammo and State*/
		if (CurrentRocketAmmo == 0 && bIsAttackUltimate)
		{
			EndAttackUltimate();
		}

	}
}



////////////////////////////////////////////////////////////////////
//
//   Basic Event Tick
//
////////////////////////////////////////////////////////////////////

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get CurretVelocity 
	CurrentVelocity = GetVelocity().Size();

	// Check Moving False
	if (CurrentVelocity == 0.f)
	{
		bIsMoving = false;
		UpdatePlayerProperties();
	}



	///// Set Jumping Control / Attack Ultimate ////  
	if (GetVelocity().Z != 0.f)
	{
		if (!bIsJumpCalled)
		{
			bIsJumping = true;
			bIsJumpCalled = true;
			bIsDelay = true;   

			BP_StarCameraJump();
		}
	}
	else
	{
		if (bIsJumpCalled)
		{
			bIsJumping = false;
			bIsJumpCalled = false;
			BP_EndCameraJump();
			
			//Check Equip Rocket and Attack Ultimate
			if (GetCurrentWeaponStatus() == ECurrentWeapon::ECW_Rocket)
			{
				BP_EndAttackUltimate();

				if (bIsUltimate)
				{
					bIsUltimate = false;
				}

				//Set Normal Walk Propertys
				GetCharacterMovement()->JumpZVelocity = 600.f;
				GetCharacterMovement()->AirControl = 0.02f;
				StartSwitchWeapon();
			}
			
		}
	}

	///// Set Camera Rotation Movement with Pawn //// 
	if(!Camera->bUsePawnControlRotation)
	{
		CameraRotationWithPawn.Pitch = GetControlRotation().Pitch;
	}

}



////////////////////////////////////////////////////////////////////
//
//   Character Update Propertys and BP Camera Functions
//
////////////////////////////////////////////////////////////////////

//////////
//////////UpdatePlayerPropertys // Status-Camera
//////////

void AMainCharacter::UpdatePlayerProperties()
{

	/////////// Player Status////////////

	//Combat Status

	if (!bIsShooting && !bIsReload)
	{
		SetCombatStatus(ECombatStatus::ECS_NoCombat);
	}
	if (bIsGrenadeLauncher && !bIsMeleeAttack && !bIsReload && !bIsShooting)
	{
		SetCombatStatus(ECombatStatus::ECS_Grenade);
	}
	if (bIsShooting && bIsPointed)
	{
		SetCombatStatus(ECombatStatus::ECS_PointedFire);
	}
	if (bIsShooting && !bIsPointed)
	{
		SetCombatStatus(ECombatStatus::ECS_FireUnder);
	}
	if (bIsReload && !bIsShooting)
	{
		SetCombatStatus(ECombatStatus::ECS_Reload);
	}
	if (bIsMeleeAttack && !bIsReload && !bIsShooting)
	{
		SetCombatStatus(ECombatStatus::ECS_Melee);
	}
	
	

	//Movement Status

	if ((bIsMoving && !bIsRuning && !bIsPointed && !bIsSwitching) || (bIsMoving && !bIsPointed && GetCombatStatus() == ECombatStatus::ECS_FireUnder && !bIsSwitching) || (bIsMoving && (GetCombatStatus() == ECombatStatus::ECS_Reload || GetCombatStatus() == ECombatStatus::ECS_Melee || GetCombatStatus() == ECombatStatus::ECS_Grenade) && !bIsSwitching))
	{
		SetMovementStatus(EMovementStatus::EMS_Walking);
	}
	if (bIsMoving && bIsRuning && !bIsPointed && !bIsDelay && GetCombatStatus() != ECombatStatus::ECS_FireUnder && !bIsReload && !bIsGrenadeLauncher && !bIsSwitching)
	{
		SetMovementStatus(EMovementStatus::EMS_Sprinting);
	}
	if (bIsPointed && GetCombatStatus() != ECombatStatus::ECS_Reload && GetCombatStatus() != ECombatStatus::ECS_Melee && GetCombatStatus() != ECombatStatus::ECS_Grenade && !bIsSwitching)
	{
		SetMovementStatus(EMovementStatus::EMS_Pointing);
	}
	if (!bIsMoving && !bIsPointed || bIsDelay && !bIsPointed || !bIsMoving && ( GetCombatStatus() == ECombatStatus::ECS_Reload || GetCombatStatus() == ECombatStatus::ECS_Melee || GetCombatStatus() == ECombatStatus::ECS_Grenade) && !bIsSwitching)
	{
		SetMovementStatus(EMovementStatus::EMS_Idle);
	}
	if (bIsSwitching)
	{
		SetMovementStatus(EMovementStatus::EMS_Switch);
	}

	
	              /////////////Camera Movements//////////////

	//Activa/Deactive -> Camera Shoot Under
	if (GetCombatStatus() == ECombatStatus::ECS_FireUnder)
	{
		if (!bIsUnderShootCalled)
		{
			BP_StarUnderShoot();
			bIsUnderShootCalled = true;
		}
	}
	else
	{
		if (bIsUnderShootCalled)
		{
			BP_EndUnderShoot();
			bIsUnderShootCalled = false;

		}
	}

	//Activa/Deactive -> Camera Shoot Pointed
	if (GetCombatStatus() == ECombatStatus::ECS_PointedFire)
	{
		if (!bIsPointedShootCalled)
		{
			BP_StarPointedShoot();
			bIsPointedShootCalled = true;

		}
	}
	else
	{
		if (bIsPointedShootCalled)
		{
			BP_EndPointedShoot();
			bIsPointedShootCalled = false;
		}
	}


	//Activa/Deactive -> Camera Sprinting Movements
	if (GetMovementStatus() == EMovementStatus::EMS_Sprinting )
	{
		if (!bIsSprintCalled)
		{
			bIsSprintCalled = true;

			//Ultimate Control
			if (!bIsMovilityUltimate)
			{
				BP_StarCameraSprint();
				GetCharacterMovement()->MaxWalkSpeed = SprintMaxWalkSpeed;
			}
			else
			{
				BP_StartMovilityUltimate();
				GetCharacterMovement()->MaxWalkSpeed = UltimateMovilitySprintSpeed;
			}
			
		}

	}
	else
	{
		if (bIsSprintCalled)
		{
			bIsSprintCalled = false;

			//Ultimate Control
			if (!bIsMovilityUltimate)
			{
				BP_EndCameraSprint();
				GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
			}
			else
			{
				BP_EndMovilityUltimate();
				GetCharacterMovement()->MaxWalkSpeed = UltimateMovilityNormalSpeed;
			}
			
		}
	}


	//Activa/Deactive -> Camera Pointed Movements
	if (GetMovementStatus() == EMovementStatus::EMS_Pointing)
	{
		if (!bIsPointedCalled)
		{
			BP_StartCameraPointed();
			bIsPointedCalled = true;
			SpringArm->bEnableCameraLag = false;

			/**Rocket Scoop Logic*/
			if (GetCurrentWeaponStatus() == ECurrentWeapon::ECW_Rocket)
			{
				SetScoopVisibility(true);
				Rocket->SetVisibility(false);
			}

		}
	}
	else
	{
		if (bIsPointedCalled)
		{
			BP_EndCameraPointed();
			bIsPointedCalled = false;
			SpringArm->bEnableCameraLag = true;

			/**Rocket Scoop Logic*/
			if (GetCurrentWeaponStatus() == ECurrentWeapon::ECW_Rocket)
			{
				SetScoopVisibility(false);
				Rocket->SetVisibility(true);
			}

		}
	}

	//Activa/Deactive -> Camera Grenade MOvement
	if (GetCombatStatus() == ECombatStatus::ECS_Grenade)
	{
		if (!bIsGrenadeLauncherCalled)
		{
			
			bIsGrenadeLauncherCalled = true;

			/**Get freedom of rotation without affecting movement*/
			CameraRotationWithPawn = Camera->GetRelativeRotation(); //Complementing in BP
			Camera->bUsePawnControlRotation = false;
			BP_StarGrenadeLauncher();
		}
	}
	else
	{
		if (bIsGrenadeLauncherCalled)
		{
			bIsGrenadeLauncherCalled = false;
			BP_EndGrenadeLauncher();
		}

	}


	//Activa/Deactive -> Camera Melee Attack
	if (GetCombatStatus() == ECombatStatus::ECS_Melee)
	{
		if (!bIsMeleeAttackCalled)
		{

			bIsMeleeAttackCalled = true;

			/**Get freedom of rotation without affecting movement*/
			CameraRotationWithPawn = Camera->GetRelativeRotation(); //Complementing in BP
			Camera->bUsePawnControlRotation = false;
			BP_StarMeleeAttack();
		}
	}
	else
	{
		if (bIsMeleeAttackCalled)
		{
			bIsMeleeAttackCalled = false;
			BP_EndMeleeAttack();
		}

	}


	//Activa/Deactive -> Camera Reload Movement
	if (GetCombatStatus() == ECombatStatus::ECS_Reload)
	{
		if (!bIsReloadCalled)
		{

			bIsReloadCalled = true;

			/**Get freedom of rotation without affecting movement*/
			CameraRotationWithPawn = Camera->GetRelativeRotation();//Complementing in BP
			Camera->bUsePawnControlRotation = false;
			BP_StarReload();
		}
	}
	else
	{
		if (bIsReloadCalled)
		{
			if (!Camera->bUsePawnControlRotation)
			{
				Camera->bUsePawnControlRotation = true;
				BP_EndReload();
			}

			bIsReloadCalled = false;
		}
	}
}


//////////////////////////// ///////
// My  Custom Play - Stop Montage //
///////////////////////////////////

void AMainCharacter::PlayMyMontage(UAnimMontage* MontageToPlay, float Ratio, FName Section)
{
	if (IsValid(MainAnimInstance))
	{
		if (IsValid(MontageToPlay))
		{
			MainAnimInstance->Montage_Play(MontageToPlay, Ratio);
			MainAnimInstance->Montage_JumpToSection(Section, MontageToPlay);
		}	
	}	
}

void AMainCharacter::StopMyMontage(float RatioStop)
{
	if (IsValid(MainAnimInstance))
	{
		MainAnimInstance->StopAllMontages(RatioStop);
	}
}





