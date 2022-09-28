

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileData/ProjectileActor.h"
#include "HelperLibraries/HelperLibrary.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "MyProjectCharacter.generated.h"


// forward declarations
struct FProjectileDataStruct;

UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMyProjectCharacter();

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/// Replication Functions
	UFUNCTION(Server, reliable)
	void  SpawnProjectile();
	UFUNCTION(NetMulticast, reliable)
	void  SpawnProjectileClient();

		// Projectile class for spawn.
		UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectileActor> ProjectileToSpawnClass;

		// Function to initialize data table
		UFUNCTION()
		void InitDataTable();

		// This variable can hold reference to our data table, Can be used to get data table reference for later.
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	    class UDataTable* ProjectileDataTable;

		
		// This service is just used to face player towards player face.
		UFUNCTION()
		void SetFaceTowardsPlayer();


		/// BlueprintNative event. Can be called when projectile hit to player to cause damage etc.
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void TakeDamageFromProjectile(float damage);


		// Simple cooldown functions - When you press LMB for projectiles.
		FTimerHandle CoolDownTimerHandle;
		
		// Allow this variable access to blueprints, So widgets can access it and display accordingly
		UPROPERTY(replicated, BlueprintReadOnly)
			bool AllowToShoot = true;

		void StartCoolDownTimer(float ResetDelay);

		void ResetTimerValue();
		

		virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
};

