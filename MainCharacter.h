/****************** Sergio López  ******************/

/********* Mainplayer Character Class (.H) *********/

/********** Dungeon Temple Pérsonal Project 2020*********/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

// UE4 Class //
class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;

// My Class //
class AWeapon;
class AItem;
class AEnemy;
class AMainPlayerController;
class UAnimInstance;
class UWorld;


////// Chacter Enum States //////
UENUM(BlueprintType)
enum class EMovementStatus :uint8
{
	EMS_Normal UMETA(DisplayName="Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX" )

};

////// Stamina Enum States //////
UENUM(BlueprintType)
enum class EStaminaStatus :uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	EMS_Exhausted UMETA(DisplayName = "Exhausted"),
	EMS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class THIRDPERSONPROJECT_API AMainCharacter : public ACharacter
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

protected:

	////////////////////////////////////////////////////////////////////
	//
	//  Player Class Components
	//
	////////////////////////////////////////////////////////////////////

						//////// Player Components///////

	/** Camera  Spring Arm to Player  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAcces = "true"))
		USpringArmComponent* CameraB;

	/** Follow Camera Player  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAcces = "true"))
		UCameraComponent* FollowCamera;

	/** Current Equip Weapon  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
		AWeapon* EquippWeapon;


	//////// Player Components Property///////

/** Save Data Storage for Weapon*/
	UPROPERTY(EditDefaultsOnly, Category = "Main|SaveData")
		TSubclassOf<class AItemStorage> WeaponStorage;

	/** Enum Movement Status*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player |Enums")
		EMovementStatus MovementStatus;

	/** Enum Stamina Status*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player |Enums")
		EStaminaStatus StaminaStatus;

	/** Current Enemy Combat Target*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player |Reference")
		AEnemy* CombatTarget;

	/** AItem Reference (Coin , Health , Trap etc)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
		AItem* ActiveOverlappingItem;


	//////////////// Class ////////////////////

/** Player Controller Reference*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Reference")
		AMainPlayerController* PlayerController;

	/** Animation Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player| Reference")
		UAnimMontage* CombatMontage;

	/** Enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Reference")
		AEnemy* Enemigo;

	/** Mesh Anim Instance Reference*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Reference")
		UAnimInstance* AnimInstance;

	/** World Reference*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Reference")
		UWorld* WorldLevel;

	/** Filter Combat Enemy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player| Combat")
		TSubclassOf<AEnemy> EnemyFilter;

protected:

	////////////////////////////////////////////////////////////////////
	//
	//  Player Variables
	//
	////////////////////////////////////////////////////////////////////

			   ////////////////** Movement Propertys *///////////////////////

	/** Left Mouse Button Key Control Variable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Variable")
		bool bLMBDown;

	/** ESC Key Control Variable*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Variable")
		bool bESCDown;

	/** Shit Key Control Variable*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player| Variable")
		bool bShiftKeyDown;

	/** Speed when character Running*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player| Variable")
		float RunningSpeed;

	/** Speed when character Sprinting*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player| Variable")
		float SprintingSpeed;

	/** Drain Rate Stamina -> "Velocity"*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player| Variable")
		float StaminaDrainRate;

	/** Min Stamina to let Sprint*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player| Variable")
		float MinSprintStamina;

	/** Turn Rate Camera/Pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Property")
		float BaseTurnRate;

	/** Look UP Camera/Pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Property")
		float BaseLookUpRate;


	////////////////** Combat Variables  *///////////////////////


/** Interpolating Animation - Focus en la spider */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Combat")
		float InterpSpeed;

	/** Interpolating Animation - Focus en la spider */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Combat")
		bool bHasInterpToEnemy;

	/**Attacking -Anim Montage*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player| Combat")
		bool bAttacking;

	/**How many enemies killed to unlock the combo */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player| Combat")
		float ComboCount;

	/** Is Blocking Hit Control Variable  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Combat")
		bool bBlockingHit;

	/* Player is in Combo control Variable*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Combat")
		bool bIsInCombo;


	///////////////////** Player Stats* /////////////////////////

/* Max CHaracter Health*/
	UPROPERTY(BlueprintReadOnly, Category = "Player|Property")
		float MaxHealth;

	/* Current Health*/
	UPROPERTY(BlueprintReadOnly, Category = "Player|Property")
		float Health;

	/* Max Character Stamina*/
	UPROPERTY(BlueprintReadOnly, Category = "Player|Property")
		float MaxStamina;

	/* Current Stamina*/
	UPROPERTY(BlueprintReadOnly, Category = "Player|Property")
		float  Stamina;

	/* Coints Count*/
	UPROPERTY(BlueprintReadOnly, Category = "Player|Property")
		int32 Coins;

	/* Damage Received */
	UPROPERTY(BlueprintReadOnly, Category = "Player|Property")
		int32 DamageReceived;

	/* Players Section MOntage*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Property")
		int32 Section1;

	/* Pick up location Array*/
	TArray<FVector> PickUpLocations;

	/*Text Timer Handle*/
	FTimerHandle Text;

public:

	////////////////////////////////////////////////////////////////////
	//
	//  Getters /Setters Class
	//
	////////////////////////////////////////////////////////////////////

				   ///////////////////** Enemy * /////////////////////////

	FORCEINLINE void SetEnemy(AEnemy* status) { Enemigo = status; }

	FORCEINLINE AEnemy* GetEnemy() { return Enemigo; }

	void SetInterpToEnemy(bool Interp);


	///////////////////** Weapon * /////////////////////////

	FORCEINLINE void SetEquippedWeapon(AWeapon* Weapon) { EquippWeapon = Weapon; }

	FORCEINLINE void DeleteCurrentEquipWeapon() { EquippWeapon = nullptr; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippWeapon; }


	///////////////////**Components* /////////////////////////

	FORCEINLINE class USpringArmComponent* GetCameraB() const { return CameraB; } //Actuan como Getters

	FORCEINLINE class UCameraComponent* GetFollowCamara() const { return FollowCamera; }//Actuan como Getters


				///////////////////**Movement Statys* /////////////////////////

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	void SetMovementStatus(EMovementStatus Status);

	///////////////////**Combat* /////////////////////////

	FORCEINLINE void IncreaseComboCount() { ComboCount++; }

	FORCEINLINE bool GetbIsCombo() { return bIsInCombo; }

	FORCEINLINE bool GetBlokingHit() { return bBlockingHit; }

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	///////////////////**PickUps* /////////////////////////

	FORCEINLINE void SetHealth(float value) { Health += value; }

	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE void SetPickUpLocation(FVector LocationItem) { PickUpLocations.Add(LocationItem); }



protected:

	////////////////////////////////////////////////////////////////////
	//
	//  Player Class Input /Save/Propertys
	//
	////////////////////////////////////////////////////////////////////


	 ///////////////////**Inputs * /////////////////////////

	/** Inputs Forward(Front/ Behind) */
	void MoveForward(float value);

	/** Inputs Right/ LeFT*/
	void MoveRight(float value);

	/** Turn Rate Funciton */
   /*@param Normalized between 0 and 1o*/
	void TurnAtRate(float rate);

	/** Look Up Rate Function */
	/*@param Normalized between 0 and 1 */
	void LookUpAtRate(float rate);

	/* Sprint Key Down*/
	void ShiftKeyDown();

	/* Sprint Key Up*/
	void ShiftKeyUp();

	/* ESC key Down*/
	void ESCDown();

	/* ESC key Up*/
	void ESCUp();

	/* Left Mouse button Up*/
	void LMBDown();

	/* Left Mouyse button Up*/
	void LMBUp();

	///////////////////**Save/Load Game * /////////////////////////

	/* Save Game*/
	UFUNCTION(BlueprintCallable)
		void SaveGame();

	/* Load Game*/
	UFUNCTION(BlueprintCallable)
		void LoadGame(bool SetPotitions);

	///////////////////**Propertys * ///////////////////////// 

	/* Get Cero Damage*/
	void CeroDamage();

	/* ShowPickUpLocation Debug*/
	UFUNCTION(BlueprintCallable)
		void ShowPickUpLocations();
	
	/* Rotation Yaw By Target*/
	FRotator GetLookAtRotationYaw(FVector Target);

public:	

	////////////////////////////////////////////////////////////////////
    //
    //  Player Class public /Functions
    //
    ////////////////////////////////////////////////////////////////////



	///////////////////** Combat Callables * ///////////////////////// 

	/* DeathEnd Called By Notify*/
	UFUNCTION(BlueprintCallable)
	   void DeathEnd();

	/* Player Attack End*/
	UFUNCTION(BlueprintCallable)
	   void AttackEnd();

	/* OnBlocking Enemy Hits Event*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnBlockingEvent"))
	   void OnBlockingEvent();

	/* Combo Finisht Set False*/
	UFUNCTION(BlueprintCallable)
	   void Setcombotofalse();

	/* Update Respective Combat Enemy Targe*/
	UFUNCTION(BlueprintCallable)
	   void UpDateCombatTarget();


	///////////////////** Combat Natives * ///////////////////////// 

	/* Start Attack*/
	void Attack();

	/* Received Hit*/
	void receivedHit();

	/* Received Left Hit*/
	void ReceiveLeftHit();

	/* Received Right Hit*/
	void ReceiveRightHit();

	/* Decrement Health Depends of Amount*/
	void DrecrementHealth(float Amount);

	/* Call Blocking Hit */
	void BlockingHit();

	/*Not Use -> Direct to BP*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable , Category="Player| BP_Funcition")
		void BP_BlockingHit();

	/*Take Damage Function*/
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	///////////////////** Propertys * ///////////////////////// 

	/*Switch Level*/
	void SwitchLevel(FName LevelName);

	/*Increment Coins*/
	void IncrementCoins();

	/*When Player Die*/
	void Die();

	/*Clean Hud*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") //****Pasela Getter Public y varaible protegida*/
		bool bDeathHUD;



};
