// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/DataTable.h"


#include "HelperLibrary.generated.h"



UENUM(BlueprintType)
enum  EffectType 
{
	Health,
	Destructible,
	Nothing
};

USTRUCT(BlueprintType, Blueprintable)

struct FProjectileDataStruct :  public FTableRowBase
{

	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bEnabledProjectileSpawnSystem;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ProjectileCollisonProfileName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bEnabledProjectileCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* ProjectileMesh;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ProjectileSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector ProjectileVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ProjectileGravityInFloat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector ProjectileSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bDestroyOnHit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bSendDamageCallbackToBlueprint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float DamageAmoutForEnemy;

};

UCLASS()
class MYPROJECT_API UHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:


	static FProjectileDataStruct* GetProjectileDataRow(class UDataTable* DataTable_ptr, FName RowToFind )
	{
		if (RowToFind != "" && DataTable_ptr != nullptr)
		{
			return DataTable_ptr->FindRow<FProjectileDataStruct>(RowToFind, "Context", true);
		}
		
		return nullptr;
	} 

};
