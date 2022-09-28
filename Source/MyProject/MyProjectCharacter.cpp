// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//

	bReplicates = true;
	SetReplicateMovement(true) ;
	

	InitDataTable();
}


void AMyProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetFaceTowardsPlayer();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);

	// Left Mouse

	PlayerInputComponent->BindAction("LeftMouse", IE_Pressed, this, &AMyProjectCharacter::SpawnProjectile);

	//

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyProjectCharacter::LookUpAtRate);

}


void AMyProjectCharacter::ResetTimerValue()
{
	AllowToShoot = true;
}

void AMyProjectCharacter::StartCoolDownTimer(float ResetDelay)
{
	AllowToShoot = false;
	GetWorld()->GetTimerManager().SetTimer(CoolDownTimerHandle, this, &AMyProjectCharacter::ResetTimerValue, ResetDelay, false, ResetDelay);
}



// Server -> Client -> RPC
void AMyProjectCharacter::SpawnProjectile_Implementation()
{
	
	SpawnProjectileClient_Implementation();

	// Can be used later, for server only event
	if (IsLocallyControlled())
	{
	}
}

void AMyProjectCharacter::SpawnProjectileClient_Implementation()
{
	// do nothing if cooldown is active
	if (AllowToShoot == false)
	{
		return;
	}

	FProjectileDataStruct* dataTableData = ProjectileDataTable->FindRow<FProjectileDataStruct>("HighScore", "Context", true);

	// cooldown timer start
	if (AllowToShoot)
	{
		StartCoolDownTimer(dataTableData->CooldownDelayForShoot);

		UWorld* p_World = GetWorld();

		bool bValid = IsValid(p_World) && IsValid(ProjectileToSpawnClass);

		if (bValid && dataTableData->bEnabledProjectileSpawnSystem)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FVector SpawnLocation = (GetActorForwardVector() * 50) + GetActorLocation();
			AProjectileActor* Projectile = p_World->SpawnActor<AProjectileActor>(ProjectileToSpawnClass, SpawnLocation, GetActorRotation(), SpawnParams);
			if (Projectile)
			{
				// Do something - 
			}
		}
	}
}



void AMyProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

// Get reference to our data table 
void AMyProjectCharacter::InitDataTable()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableToFetch(TEXT("DataTable'/Game/DataTables/ProjectileDataTable.ProjectileDataTable'"));
	if (DataTableToFetch.Succeeded())
	{
		ProjectileDataTable = DataTableToFetch.Object;

	}
	else
	{
	// Do nothing or print error message.

	}
}
// AI service
void AMyProjectCharacter::SetFaceTowardsPlayer()
{
	return;
	if (auto player = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		FVector PlayerLocation = player->GetActorLocation();
		if (!IsPlayerControlled())
		{
			// this means AI
			;
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);
			SetActorRotation(TargetRotation);
		}
	}
}


void AMyProjectCharacter::TakeDamageFromProjectile_Implementation(float damage)
{
	// this will be called in blueprint
}


void AMyProjectCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyProjectCharacter, AllowToShoot);
}
