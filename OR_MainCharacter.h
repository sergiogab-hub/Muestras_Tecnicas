/****************** Sergio López ******************/

/********* Mainplayer Character Class (.H) *********/

/********** Orion Pérsonal Project 2020 **+********/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OR_MainCharacter.generated.h"


/*Engine Class*/

class UCameraComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UAnimMontage;
class UAnimInstance;
class UParticleSystem;


/*MY Class*/

class AProjectile;
class AOR_LauncherProjectile;
class AOR_RocketProjectile;
class UOR_HealthComponent;
class AOR_MyGameMOde;
class AOR_BasePilar;



////// Movement Enum States //////
UENUM(BlueprintType)
enum class EMovementStatus :uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Walking UMETA(DisplayName = "Walking"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Pointing UMETA(DisplayName = "Pointing"),
	EMS_Switch UMETA(DisplayName = "Switch"),
	

	EMS_MAX UMETA(DisplayName = "DefaultMAX")

};


/////// Combat Enum States /////////
UENUM(BlueprintType)
enum class ECombatStatus :uint8
{
	ECS_NoCombat UMETA(DisplayName = "NoCombat"),
	ECS_FireUnder UMETA(DisplayName = "FireUnder"),
	ECS_PointedFire UMETA(DisplayName = "PointedFire"),
	ECS_Reload UMETA(DisplayName = "Reload"),
	ECS_Melee UMETA(DisplayName = "Melee"),
	ECS_Grenade UMETA (DisplayName="Grenade"),
	
	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};



/////// Weapon Enum States /////////
UENUM(BlueprintType)
enum class ECurrentWeapon :uint8
{
	ECW_Rifle UMETA(DisplayName = "Rifle"),
	ECW_Rocket UMETA(DisplayName = "Rocket"),


	ECW_MAX UMETA(DisplayName = "DefaultMAX")
};



///////// Main Class /////////
UCLASS()
class ORIONFPS_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AMainCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	////////////////////////////////////////////////////////////////////
	//
	//  Player Input / Functions
	//
	////////////////////////////////////////////////////////////////////

	/**  Forward/Backward control input */
	void MoveForward(float value);

	/**  Right/Left control input */
	void MoveRight(float value);

	/**  Up/Down control mouse input */
	void RotatePitch(float value);

	/**  Right/Left control mouse input */
	void RotateYaw(float value);


	/**  Jump Space Bar input */
	void StartJump();
	void EndJump();

	/**  Sprint Left Shift Input */
	void StartSprint();
	void StopSprint();

	/**  Pointed Weapon Right Mouse Input */
	void StartGunPoint();
	void EndGunPoint();

	/**  Shoot Left Mouse Input */
	void StartShoot();
	void EndShoot();
	void EndShootByOther();

	/**  Star Reload Left Mouse */
	void StartReload(); //EndReload() -> BP Function

	/**  Star Melee Attack */
	void StartMeleeAtaack(); //EndMeleeAttack() -> BP Function

	/**  Star Grenade Launcher */
	void StartGrenadeLauncher(); //EndGrenadeLauncher() -> BP Function

	/**  Star Switch Weapon */
	void StartSwitchWeapon(); //EndSwitch() -> BP Function

	/**  Activate Current Ultimate */
	void ActivateCurrentUltimate();

	/** BeginPlay Call Function to Set Player Object/Class References */
	void SetupMainReferences();



protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Character Class
	//
	////////////////////////////////////////////////////////////////////


				       //////// Character Components///////


	/** Main Camera Component for Player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		UCameraComponent* Camera;

	/** Arms Skeletal Mesh for Player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		USkeletalMeshComponent* Arms;

	/** Weapon Skeletal Mesh for Player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		USkeletalMeshComponent* Weapon;

	/** Rocket Skeletal Mesh for Player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		USkeletalMeshComponent* Rocket;

	/** Spring Arm (Camera-Arms) for Player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		USpringArmComponent* SpringArm;

	/** Health Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		UOR_HealthComponent* Health;

	/** Mele Capsule Collision*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterComponents")
		UCapsuleComponent* MeleeDetector;


	                    //////// Character Propertys///////


    /** Main Camera RelativeRotationVariable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterPropertys")
		FRotator CameraRotationWithPawn;

	/** Weapon Socket Name*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterPropertys")
		FName WeaponSocketName;

	/** Melle Socket Name*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterPropertys")
		FName MeleeCapsuleSocketName;

	/** Has to Destroy When he dies?*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterPropertys")
		bool bHastoDestroy;

      
	                    ///////////  Montages ///////////


    /** Under Shoot Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|Montages")
		UAnimMontage* ShootMontage;

	/** Pointed Shoot Montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|Montages")
		UAnimMontage* PointedShoot_Montage;

	/** Reload Montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|Montages")
		UAnimMontage* ReloadMontage;

	/** Mele Montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|Montages")
		UAnimMontage* MeleMontage;

	/** Mele Montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|Montages")
		UAnimMontage* GrenadeMontage;


	                  /////////// Particle System ///////////


    /** Muzzle Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem")
		UParticleSystem* MuzzleShoot1;

	/** Muzzle Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem")
		UParticleSystem* MuzzleShoot2;

	/** Muzzle Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem")
		UParticleSystem* SmokeMuzzle;

	/** Shell Eject Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem")
		UParticleSystem* ShellEject;

	/** Shell Eject Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem")
		UParticleSystem* SmokeShell;

	/** SmokeReload ParticleSystem */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem")
		UParticleSystem* SmokeReload;

	/** Ultimate Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|ParticleSystem") 
		UParticleSystem* FinalGun;


	                ///////////  References /SubClass ///////////


    /** Game Mode Reference*/
	UPROPERTY(BlueprintReadOnly, Category = "Main|References")
		AOR_MyGameMOde* GameModeReference;

	/** Anim Instance */
	UPROPERTY(BlueprintReadOnly, Category = "Main|References")
		UAnimInstance* MainAnimInstance;

	/** Projectile Rocket to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Main|References")
		TSubclassOf<AOR_BasePilar> MyPilarsReference;

    /** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Main|Projectiles")
		TSubclassOf<AProjectile> BulletClass;

	/** Projectile Grenade to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Main|Projectiles")
		TSubclassOf<AOR_LauncherProjectile> LauncherClass;

	/** Projectile Rocket to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Main|Projectiles")
		TSubclassOf<AOR_RocketProjectile> RocketClass;

	/** Damage Type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterPropertys")
		TSubclassOf<UDamageType> MyDamageType;



protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Character Variables
	//
	////////////////////////////////////////////////////////////////////


	           ////////////////Movement Character Variables//////////////


    /** Rotation Turn Speed Pitch/Yaw*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterMovement")
		float RotationSpeed;

	/** Normal MaxWalkSpeed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterMovement")
		float NormalMaxWalkSpeed;

	/** Sprint MaxWalkSpeed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterMovement")
		float SprintMaxWalkSpeed;

	/** Control Variable Run State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsRuning;

	/** Control Variable Jump State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsJumping;

	/** Control Variable Moving/Idle State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsMoving;

	/** Control Variable Pointed Weapon State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsPointed;

	/** Open/Close BP_Pointed Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsPointedCalled;

	/** Open/Close BP_Sprint Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsSprintCalled;

	/** Open/Close Jump Camera Logic*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsJumpCalled;

	/** Jump Camera Lag Delay*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main|CharacterMovement")
		bool bIsDelay;

	/** Pawn Current Velocity  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		float CurrentVelocity;

	/** IsSwitching Control Varaiblñe  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		float bIsSwitching;

	/** Control Variable Reload State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterMovement")
		bool bIsReload;

	/** Launcher Pad Variable Key*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main|CharacterMovement") 
		bool Key = false;

	/**Tarea */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main|CharacterMovement") 
		int32 Count = 0;


	            ////////////////////Combat Character/////////////////////


	/** Control Variable Reload State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsMeleeAttack;

	/** Shooting Control Variable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsShooting;

	/** Shooting Control Variable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsGrenadeLauncher;

	/** Shooting Control Variable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsRuningControlDelay;

	/** Shooting Control Variable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsKeyShootPressed;

	/** Open/Close BP_UnderShoot Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsUnderShootCalled;

	/** Open/Close BP_PointedShoot Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsPointedShootCalled;

	/** Open/Close BP_UnderShoot Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsReloadCalled;

	/** Open/Close BP_PointedShoot Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsGrenadeLauncherCalled;

	/** Open/Close BP_PointedShoot Function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		bool bIsMeleeAttackCalled;

	/** CountAmmo*/
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		int32 WeaponAmmo;

	/** CountAmmo*/
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Main|CharacterCombat")
		int32 GrenadeAmmo;


	           ////////////////////Ultimate Character/////////////////////


    /** Is ultimate Active */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bIsUltimate;

	 /** Rockry AmmoCount */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate|AttackUltimate")
		int32 CurrentRocketAmmo;

	/** Rockry AmmoCount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate|AttackUltimate")
		int32 RocketAmmo;

	/** Attack Ultimate */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate|AttackUltimate")
		float AttackUltiMaxDuration;

	/** Defence Ultimate */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate|DefenceUltimate")
		float DefenceUltiMaxDuration;

	/** Movility Ultimate */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate|MovilityUltimate")
		float MovilityUltiMaxDuration;

	/** Ultimate Movility SprintSpeed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate|MovilityUltimate")
		float UltimateMovilitySprintSpeed;

	/** Ultimate Movility NormalSpeed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate|MovilityUltimate")
		float UltimateMovilityNormalSpeed;

	/** Player Rate Anim */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate|MovilityUltimate")
		float PlayerRate;

	/** Player Rate Anim */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate|MovilityUltimate")
		float MovilityUltimatePlayerRate;

	/** Is ultimate Active */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bIsAttackUltimate;

	/** Is ultimate Active */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bIsDefenceUltimate;

	/** Is ultimate Active */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bIsMovilityUltimate;

	/** Has Ultimate Ready Pilar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bHasAttackUltimateReady;

	/** Has Ultimate Ready Pilar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bHasDefenceUltimateReady;

	/** Has Ultimate Ready Pilar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		bool bHasMovilityUltimateReady;

	/** AttackUltimateDuration */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		float AttackCurrentDuration;

	/** Pilars Max State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		float MovilityCurrentDuration;

	/** Pilars Max State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		float DefenceCurrentDuration;

	/** Drain Rate Ultimate Control  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		float DrainRateUltimate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|CharacterUltimate")
		TArray<AActor*> MyPilarsReferencesArray;


	                  ////////////////////Pilars/////////////////////


	/** Pilar Attack Control Overlap */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|Pilars")
		bool bIsOnPilarAttack;

	/** Pilar Defence Control Overlap */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|Pilars")
		bool bIsOnPilarDefence;

	/** Pilar Movility Control Overlap */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|Pilars")
		bool bIsOnPilarMovility;

	/** Attack Pilar Percent */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|Pilars")
		float PilarAttackPercent;

	/** Defence Pilar Percent */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|Pilars")
		float PilarDefencePercent;

	/** Movility Pilar Percent */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main|Pilars")
		float PilarMovilityPercent;



public:
	////////////////////////////////////////////////////////////////////
	//
	//   Getters /Setters Class
	//
	////////////////////////////////////////////////////////////////////


                             ////////// Movement Status //////////


	 /** Player Movement Status Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main|Enums")
		EMovementStatus MovementStatus;

	/** Set the New Movement Status*/
	FORCEINLINE void SetMovementStatus(EMovementStatus Status) { MovementStatus = Status; } 

	/** Get Current Movement Status*/
	FORCEINLINE EMovementStatus GetMovementStatus()  const { return MovementStatus; } 


	                        /////////// Combat Status //////////


	/** Player Combat Status Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main|Enums")
		ECombatStatus CombatStatus;

	/** Set the New Combat Status*/
	FORCEINLINE void SetCombatStatus(ECombatStatus Status) { CombatStatus = Status; } 

	/** Get Current Combat Status*/
	FORCEINLINE ECombatStatus GetCombatStatus() const { return CombatStatus; } 


	                         ///////// Weapon Status ////////


    /** Player Current Weapon Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main|Enums")
		ECurrentWeapon CurrentWeaponStatus;

	/** Set the New Combat Status*/
	FORCEINLINE void SetCurrentWeaponStatus (ECurrentWeapon Status) { CurrentWeaponStatus = Status; }

	/** Get Current Combat Status*/
	FORCEINLINE ECurrentWeapon  GetCurrentWeaponStatus() const { return CurrentWeaponStatus; }


	                        ///////// Character Variables ////////


	FORCEINLINE bool GetKeyBoolValue() const { return Key; }

	FORCEINLINE bool GetIsRuning() const { return bIsRuning; }

	FORCEINLINE bool GetIsPointed() const { return bIsPointed; }

	FORCEINLINE bool HasToDestroy() const { return bHastoDestroy; }

	FORCEINLINE UOR_HealthComponent* GetHealthComponent() { return Health;}

	void SetEnumMeleeCollision(ECollisionEnabled::Type CollisionState);


	                       //////// Ultimate States/Variables ////////


	FORCEINLINE float GetPlayerRate() const { return PlayerRate; }


	FORCEINLINE bool GetIsAttackUltimate() const { return bIsAttackUltimate; }

	FORCEINLINE bool GetIsDefenceUltimate() const { return bIsDefenceUltimate; }

	FORCEINLINE bool GetIsMovilityUltimate() const { return bIsMovilityUltimate; }


	FORCEINLINE bool GetHasAttackUltimateReady() const { return bHasAttackUltimateReady; }

	FORCEINLINE bool GetHasDefenceUltimateReady() const { return bHasDefenceUltimateReady; }

	FORCEINLINE bool GetHasMovilityUltimateReady() const { return bHasMovilityUltimateReady; }


	FORCEINLINE void SetHasAttackUltimateReady(bool MyBool) { bHasAttackUltimateReady = MyBool; }

	FORCEINLINE void SetHasDefenceUltimateReady(bool MyBool) { bHasDefenceUltimateReady = MyBool; }

	FORCEINLINE void SetHasMovilityUltimateReady(bool MyBool) { bHasMovilityUltimateReady = MyBool; }


	                        /////// Pilars States/Variables //////


	FORCEINLINE void SetPilarAttackPercent(float Percent)  { PilarAttackPercent=Percent; }

	FORCEINLINE void SetPilarDefencePercent(float Percent) { PilarDefencePercent = Percent; }

	FORCEINLINE void SetPilarMovilityPercent(float Percent) { PilarMovilityPercent = Percent; }


	FORCEINLINE void SetIsOnPilarAttack(bool MyBool)  { bIsOnPilarAttack = MyBool; }

	FORCEINLINE void SetIsOnPilarDefence(bool MyBool) { bIsOnPilarDefence = MyBool; }

	FORCEINLINE void SetIsOnPilarMovility(bool MyBool) { bIsOnPilarMovility = MyBool; }


	FORCEINLINE bool GetIsOnPilarAttack() const { return bIsOnPilarAttack;}

	FORCEINLINE bool GetIsOnPilarDefence() const { return bIsOnPilarDefence;}

	FORCEINLINE bool GetIsOnPilarMovility() const{ return bIsOnPilarMovility;}


public:
	////////////////////////////////////////////////////////////////////
	//
	//  Character BP_Functions and Camera
	//
	////////////////////////////////////////////////////////////////////


                     ///////////// Movement BP Functions ///////////////////


	/** Star Camera Pointed BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_MovementEvents")
		void BP_StartCameraPointed();

	/** End Camera Pointed BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_MovementEvents")
		void BP_EndCameraPointed();

	/** Star Camera Sprint BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_MovementEventss")
		void BP_StarCameraSprint();

	/** End Camera Sprint BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_MovementEvents")
		void BP_EndCameraSprint();

	/** Star Camera Jump BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_MovementEvents")
		void BP_StarCameraJump();

	/** End Camera Jump BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_MovementEvents")
		void BP_EndCameraJump();


	                  /////////////Combat BP Functions /////////////////


	/** Star Camera Under Shoot BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_StarUnderShoot();

	/** End Camera Under Shoot BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_EndUnderShoot();

	/** Star Camera Pointed SHoot BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_StarPointedShoot();

	/** End Camera Pointed Shoot BP Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_EndPointedShoot();

	/** Star Grenade Launcher Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_StarGrenadeLauncher();

	/** End Grenade Launcher Camera Logi*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_EndGrenadeLauncher();

	/** Star Grenade Launcher Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_StarMeleeAttack();

	/** End Grenade Launcher Camera Logi*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_EndMeleeAttack();

	/** Star Reload BP Function*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_StarReload();

	/** End Reload BP Function*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main|BP_CombatEvents")
		void BP_EndReload();


	                   /////////////Ultimate BP Functions /////////////////


	/**Scoop Function*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEvents")
		void SetScoopVisibility(bool Visibilty);

	/**Start Attack Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEvents")
		void BP_StarAttackUltimate();

	/**End Attack Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEventss")
		void BP_EndAttackUltimate();

	/**ShootRocketCameraLogic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEvents")
		void BP_ShootRocket();

	/**Start Movility Camera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEvents")
		void BP_StartMovilityUltimate();

	/**End MovilitCamera Logic*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEventss")
		void BP_EndMovilityUltimate();

	/**Start Defence Ultimate*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEvents")
		void BP_StartDefenceUltimate();

	/**End Defence Ultimate*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_UltimateEventss")
		void BP_EndDefenceUltimate();


	             /////////////Death / Damage State BP Functions /////////////////


	/**Deast*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_DamageEvent")
		void BP_Death();

	/**Deast*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Main | BP_DamageEvent")
		void BP_ReceiveDamage();



public:
	////////////////////////////////////////////////////////////////////
	//
	//  Character Functions
	//
	////////////////////////////////////////////////////////////////////


                             ///////////Movement Functions////////////


	     /** Update Player Properies()*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterMovement")
		void UpdatePlayerProperties();

	    /*** End Switch Weapon*/
	UFUNCTION(BlueprintCallable, Category = "Main|haracterMovement")
		void EndSwitchWeapom();


                            ////////////Combat Functions //////////////


	    /** Shoot Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterCombat")
		void Shoot();

	    /** Shoot FTimerHablde */
	   FTimerHandle ShootHandle;
   
	     /*** End Reload Function*/
	UFUNCTION(BlueprintCallable , Category= "Main|CharacterCombat")
	    void EndReload();

		/*** End MeleeAttack Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterCombat")
		void EndMeleeAttack();

	    /*** Make Mele Damage*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterCombat")
	    void MakeMeleeDamage( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		/*** End Grenade Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterCombat")
		void EndGrenadeLauncher();

	   /*** End Reload Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterCombat")
		void SpawnGrenadeLauncher();


	                        ////////////Ultimates Functions ////////////


	    /***Star Attack Ultimate Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		void  StartAttackUltimate();

	   /***End Attack Ultimate Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		void  EndAttackUltimate();

	   /***Star Movility Ultimate Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		void  StartMovilityUltimate();

	    /***End Movility Ultimate Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		void  EndMovilityUltimate();

         /***Start Defence Ultimate*/
	 UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		 void StartDefenceUltimate();

	     /***End Defence Ultimate*/
	 UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		 void EndDefenceUltimate();

	     /** Shoot Rocket Function*/
	 UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		 void RocketShoot();

	     /***Ultimate Counting Duration*/
	 UFUNCTION(BlueprintCallable, Category = "Main|CharacterUltimate")
		 void  UltimateCountingDuration();

	 /** Ultimate Duration Handle FTimerHablde */
	     FTimerHandle UltimateDurationHandle;

	  /** Attack Ultimate Handle FTimerHablde */
	     FTimerHandle AttackUltimateHandle;


	                        //////////// Delegate Component ////////////


	     /** Health Delegate Component */
	 UFUNCTION(BlueprintCallable, Category = "Main|Component")
		 void OnHealthChange(UOR_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	                        //////////// My Custom Montage Functions ////////////


		/*** Play Montage Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|Montage")
		void PlayMyMontage(UAnimMontage* MontageToPlay, float Ratio, FName Section);

	  /*** Stop Montage Function*/
	UFUNCTION(BlueprintCallable, Category = "Main|Montage")
		void StopMyMontage(float RatioStop);

	  

};
