// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileActor.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetupProjectileProperties();
	SetupProjectileMovementComponent();
	AddProjectileMesh();
	BindEventOnHit();

	// Make sure to enable replication.
	bReplicates = true;
	SetReplicateMovement(true);

}

// Get Data table values.
UDataTable* AProjectileActor::GetProjectileDataTable()
{
	return Cast<AMyProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->ProjectileDataTable;
}


void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Get Data table row from struct and set required values
	auto dataTableData = UHelperLibrary::GetProjectileDataRow(GetProjectileDataTable(), "HighScore");
	SetActorScale3D(dataTableData->ProjectileSize);
	const FString CollisionProfileNameOfProjectile = dataTableData->ProjectileCollisonProfileName;
	CollisionComponent->SetCollisionProfileName(FName(CollisionProfileNameOfProjectile));
	ProjectileMovementComponent->InitialSpeed = dataTableData->ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = dataTableData->ProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = dataTableData->ProjectileGravityInFloat;
	ProjectileMeshComponent->SetStaticMesh(dataTableData->ProjectileMesh);
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileActor::SetupProjectileProperties()
{
	// Add CollisionComponent as root component
	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->SetIsReplicated(true);
		CollisionComponent->InitSphereRadius(15.0f);
		RootComponent = CollisionComponent;
	}
}

		void AProjectileActor::SetupProjectileMovementComponent()
		{
			// Add projectile movement component.
			if (!ProjectileMovementComponent)
			{
				ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
				ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
				ProjectileMovementComponent->SetIsReplicated(true);
				ProjectileMovementComponent->bRotationFollowsVelocity = true;
				ProjectileMovementComponent->bShouldBounce = true;
				ProjectileMovementComponent->Bounciness = 0.3f;
				ProjectileMovementComponent->ProjectileGravityScale = 1.f;
			}
		}


		void AProjectileActor::AddProjectileMesh()
		{
			// Add projectile mesh component. Can be sphere or cube etc.
			if (!ProjectileMeshComponent)
			{
				ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
				ProjectileMeshComponent->SetupAttachment(CollisionComponent);
				ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				ProjectileMeshComponent->SetIsReplicated(true);
			}
		}

		// This function will be used to bind delegate onHit.
		void AProjectileActor::BindEventOnHit()
		{
			CollisionComponent->OnComponentHit. AddDynamic(this, &AProjectileActor::OnHit);
		}
	
		// RPC- Functions for server and client communication
		void AProjectileActor::OnProjectileHit_Server_Implementation( AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult Hit)
		{

			OnProjectileHit_Client_Implementation(OtherActor, OtherComp, Hit);
			
		}

		// RPC- Functions for server and client communication
		void AProjectileActor::OnProjectileHit_Client_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult Hit)
		{
			auto dataTableData = UHelperLibrary::GetProjectileDataRow(GetProjectileDataTable(), "HighScore");

			if (auto character = Cast<AMyProjectCharacter>(OtherActor))
			{
				if (!character->IsPlayerControlled())
				{
					character->TakeDamageFromProjectile(dataTableData->DamageAmoutForEnemy);
				}
			}
			if (OtherActor->ActorHasTag("destructible"))
			{
				OtherActor->Destroy();
			}
		}


		void AProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
		{
			auto dataTableData = UHelperLibrary::GetProjectileDataRow(GetProjectileDataTable(), "HighScore");

			if (OtherActor != this)
			{
				OnProjectileHit_Server_Implementation(OtherActor, OtherComp, Hit);
			}
			if (dataTableData->bDestroyOnHit)
			{
				Destroy();
			}
		}
