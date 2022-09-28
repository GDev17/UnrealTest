
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include "../MyProject.h"
#include "../HelperLibraries/HelperLibrary.h"
#include "ProjectileActor.generated.h"



// forward declarations
struct FProjectileDataStruct;


UCLASS()
class MYPROJECT_API AProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = Movement)
		UProjectileMovementComponent* ProjectileMovementComponent;

	// Projectile mesh component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UStaticMeshComponent* ProjectileMeshComponent;

	// Projectile mesh
	UPROPERTY(EditAnywhere, Category = Projectile)
		UStaticMesh* ProjectileMesh;

	// Functions to setup and initialize everything
public:
	UFUNCTION()
		void SetupProjectileProperties();

	UFUNCTION()
		void SetupProjectileMovementComponent();

	UFUNCTION()
		void AddProjectileMesh();

	UFUNCTION()
		void BindEventOnHit();
	
	// this function will be called when projectile will hit some other object.
	UFUNCTION()
	 void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) ;

	// reference to player character. can be used later.
	UPROPERTY()
	class AMyProjectCharacter* character_ptr;

	// Pointer to our data table. Which is reference to struct inside helper library.
	UFUNCTION()
	UDataTable* GetProjectileDataTable();


	// Networking functions
	UFUNCTION(Server, Reliable)
	void OnProjectileHit_Server(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult Hit);

	UFUNCTION(Client, Reliable)
	void OnProjectileHit_Client(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult Hit);

};
