/****************** Sergio López  ******************/

/************ Base AIEnemy Class (.H) **************/

/********** Orion Pérsonal Project 2020 ************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OR_Enemy.generated.h"


/*Engine CLASS*/

class USkeletalMeshComponent;
class UAnimMontage;
class UAnimInstance;
class UParticleSystem;
class USphereComponent;

/*My CLASS*/

class AMainCharacter;
class AProjectile;
class AOR_AIBaseEnemyController;
class UEnemyAnimInstance;


UCLASS()
class ORIONFPS_API AOR_Enemy : public ACharacter
{

	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AOR_Enemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:
	////////////////////////////////////////////////////////////////////
	//
	//   Character Class
	//
	////////////////////////////////////////////////////////////////////


					     //////// Character Components///////


		/** Weapon Skeletal Mesh for Player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USkeletalMeshComponent* Weapon;

	   /** Health Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		UOR_HealthComponent* Health;


	                //////// Navigation Component/Variables ///////


       /** Locomotion Point 0 -> Front */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint0; //LocoPoint0;

	   /** Locomotion Point 1 -> Diagonal Right */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint1;

	  /** Locomotion Point 2 -> Right Side*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint2;

	  /** Locomotion Point 3 -> Behind*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint3;

	  /** Locomotion Point 0 -> Diagonal left*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint4;

	  /** Locomotion Point 0 -> Left Side */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint5;

	  /** Locomotion Point 0 -> Diagonal*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint6;

	  /** Locomotion Point 0 -> Diagonal*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|CharacterComponents")
		USphereComponent* AINavPoint7;

	  /** Navigation Points Array*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|CharacterVariables")
		TArray <FVector> AINavigationArrayPoints;


	                     //////////// Montages ///////////


      /** Hip Shoot Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Montages")
		UAnimMontage* ShootHipsMontage;

	  /** IronSight Shoot Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Montages")
		UAnimMontage* ShootIronSightMontage;

	  /** Reload Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Montages")
		UAnimMontage* IronSightReload;

	  /** Reload Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Montages")
		UAnimMontage* HitReactMontage;

	   /** Death Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Montages")
		UAnimMontage* DeathMontage;

                      	   
	                    ///////// References /////////


       /** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Projectil")
		TSubclassOf<AProjectile> BulletClass;

	   /** Reload Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|References")
		AMainCharacter* MainReference;

	   /** AIBaseEnemyController*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|References")
		AOR_AIBaseEnemyController* OwnController;

	   /** AnimInstance*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|References")
		UEnemyAnimInstance* EnemyAnimInstaceReference;



public:
	////////////////////////////////////////////////////////////////////
	//
	//   Getters /Setters Class 
	//
	////////////////////////////////////////////////////////////////////

	/** Pitch - Yaw */
	FORCEINLINE void SetMyPitch(float value) { AIBasePitch = value; }
	FORCEINLINE void SetMyYaw(float value) { AIBaseYaw = value; }

	/** Fire-ReloadStates */
	FORCEINLINE bool GetIsHipsFiring() { return bIsHipsFiring; }
	FORCEINLINE bool GetIsIronSightFiring() { return bIsIronFiringMoving; }
	FORCEINLINE bool GetbIsReload() { return bIsRealod; }


protected:
	////////////////////////////////////////////////////////////////////
	//
	//    AI  Character Variables
	//
	////////////////////////////////////////////////////////////////////


	        ////////////////// Navigation /Movement Variables /////////////////////// 


	/** Character Pitch*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|NavMomevent|Variables")
		float AIBasePitch;

	/** Character Yaw*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|NavMomevent|Variables")
		float AIBaseYaw;

	/** Initial Min DIntance to Run */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = " AI|NavMomevent|Variables")
		float InitialMinDistanceToRun;

	/** Current Min DIntance to Run */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = " AI|NavMomevent|Variables")
		float MinDistanceToRun;

	/** Max Distance to Stay */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = " AI|NavMomevent|Variables")
		float MaxDistanceToStay;

	/** MinDistancetoStay */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = " AI|NavMomevent|Variables")
		float MinDistanceToStay;

	/** MinDistancetoStay */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = " AI|NavMomevent|rVariables")
		FVector CurrentTargetLocation;

	/**CurrentIndexNotYet */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|NavMomevent|Variables")
		int32 CurrentIndex; //Temporal Not Use


	      /////////////////////////  Combat Variables     ///////////////////////////// 


	   /** HIPS Firing State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|Combat|Variables")
		bool bIsHipsFiring;
  
	   /** Iron Firing State Moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|Combat|Variables")
		bool bIsIronFiringMoving;

	   /** Iron Firing State Not Moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|Combat|Variables")
		bool bIsIronFiringStanding;

	   /** Is Firin State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Combat|Variables")
		bool bIsFiring;
 
	   /** Has to reload state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|Combat|Variables")
		bool bHasNeedReload;

	   /** On Reload State */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|Combat|Variables")
		bool bIsRealod;

	   /** Current Munition */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " AI|Combat|Variables")
		int32 CurrentMunition;

	   /** Max Munition */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " AI|Combat|Variables")
		int32 MaxMunition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " AI|Combat|Variables" , meta = (ClampMin = 0.0, UMin = 0.0))
		float ShootMovingRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " AI|Combat|Variables", meta = (ClampMin = 0.0, UMin = 0.0))
		float ShootStandRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " AI|Combat|Variables")
		float ShakePitchMinRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " AI|Combat|Variables")
		float ShakePitchMaxRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " AI|Combat|Variables")
		float ShakeYawMinRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " AI|Combat|Variables")
		float ShakeYawMaxRatio;


	       /////////////////////////  Handles   ///////////////////////////// 


		/*Handles*/
	FTimerHandle ShootHipsHandle;
	FTimerHandle ShootIronHandle;
	FTimerHandle UpdateNavegationSystemHandle;


	


protected:
	////////////////////////////////////////////////////////////////////
	//
	//  AI Character BP_Functions
	//
	////////////////////////////////////////////////////////////////////

	/*Shoot BP Function*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable , Category = "AI|BP_Functions")
		void BP_Shoot();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AI|BP_Functions")
		void BP_Death();

protected:
	////////////////////////////////////////////////////////////////////
	//
	//  AI Character Functions
	//
	////////////////////////////////////////////////////////////////////


	     ////////////////// Class Function ///////////////////////


		/*Setup MainReference*/
	void SetupReferences();


	   ////////////////// Combat Functions ///////////////////////


	/*Start Fire with IronSights*/
	UFUNCTION(BlueprintCallable)
		void StartIroSightFire(bool Moving);

	/*Stop Iron Fire*/
	UFUNCTION(BlueprintCallable)
		void StopIronFire();

	/*Shoot Spawn proyectile called by Anim notificy*/
	UFUNCTION(BlueprintCallable)
		void Shoot();

	/*Start Reload*/
	UFUNCTION(BlueprintCallable)
		void StartReload();

	/* Start end Reload */
	UFUNCTION(BlueprintCallable)
		void EndReload();

	/*Hit React */  //Temporal not use
	UFUNCTION(BlueprintCallable)
		void HitBulletReact();

	/*Firing timer function Montage*/
	void Firing();


	       ////////////////// Movement Functions ///////////////////////


	/* Get Navigation point to move */
	UFUNCTION(BlueprintCallable)
		FVector GetMovementDirection(float PlayerDistance);

	/*Proporcional speed to movement */
	UFUNCTION(BlueprintCallable)
		void GetProportionalSpeed(float distance);

	/* Check valid navigation point */
	FVector CheckNavigationPoint(FVector TargetPosition);

	/* Update points position */
	void UpdateNavegationSystem();


	     ////////////////// Delegate Component/FUnction //////////////////


	 /** Health Delegate Component */
	UFUNCTION(BlueprintCallable, Category = "AI|Component")
		void OnHealthChange(UOR_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/*Choose Montage Play Animation*/
	void PlayMontageRandomDeathAnimation();


	




};
