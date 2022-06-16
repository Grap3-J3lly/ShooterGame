// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "CharAbilities.h"
#include "CharAbilitiesMovementComponent.h"

	//------------------------------------------------------
    //					CONSTRUCTORS
    //------------------------------------------------------

ACharAbilities::ACharAbilities()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Sets default values
ACharAbilities::ACharAbilities(const FObjectInitializer& ObjectInitializer)
	//: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharAbilitiesMovementComponent>(ACharacter::CharacterMovementComponentName))
	// ^ Specifies what subclass the ObjectInitializer will initialize instances from when called for a CharacterMovementComponent
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

	//------------------------------------------------------
    //				GENERAL FUNCTIONS
    //------------------------------------------------------

// Called when the game starts or when spawned
void ACharAbilities::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharAbilities::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharAbilities::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharAbilities::JumpPressed);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharAbilities::JumpReleased);
	PlayerInputComponent->BindAction("Teleport", EInputEvent::IE_Pressed, this, &ACharAbilities::TeleportPressed);
	PlayerInputComponent->BindAction("Teleport", EInputEvent::IE_Released, this, &ACharAbilities::TeleportReleased);
	PlayerInputComponent->BindAction("Rewind", EInputEvent::IE_Pressed, this, &ACharAbilities::RewindPressed);
	PlayerInputComponent->BindAction("Rewind", EInputEvent::IE_Released, this, &ACharAbilities::RewindReleased);
}

	//------------------------------------------------------
    //				ABILITY FUNCTIONS
    //------------------------------------------------------

void ACharAbilities::JumpPressed()
{
	Super::Jump();

	UCharAbilitiesMovementComponent* abilityComponent = Cast<UCharAbilitiesMovementComponent>(GetCharacterMovement());
	if(abilityComponent)
	{
		if(!HasAuthority()) abilityComponent->ServerSetJetpackingRPC(1.0);
		abilityComponent->SetJetpacking(1.0);
	}
}

void ACharAbilities::JumpReleased()
{
	Super::StopJumping();

	UCharAbilitiesMovementComponent* abilityComponent = Cast<UCharAbilitiesMovementComponent>(GetCharacterMovement());
	if(abilityComponent)
	{
		if(!HasAuthority()) abilityComponent->ServerSetJetpackingRPC(0.0);
		abilityComponent->SetJetpacking(0.0);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ACharAbilities::TeleportPressed()
{
	UCharAbilitiesMovementComponent* abilityComponent = Cast<UCharAbilitiesMovementComponent>(GetCharacterMovement());
	if(abilityComponent)
	{
		if(!HasAuthority()) abilityComponent->ServerSetTeleportingRPC(1.0);
		abilityComponent->SetTeleporting(1.0);
	}
}

void ACharAbilities::TeleportReleased()
{
	UCharAbilitiesMovementComponent* abilityComponent = Cast<UCharAbilitiesMovementComponent>(GetCharacterMovement());
	if(abilityComponent)
	{
		if(!HasAuthority()) abilityComponent->ServerSetTeleportingRPC(0.0);
		abilityComponent->SetTeleporting(0.0);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ACharAbilities::RewindPressed()
{
	UCharAbilitiesMovementComponent* abilityComponent = Cast<UCharAbilitiesMovementComponent>(GetCharacterMovement());
	if(abilityComponent)
	{
		if(!HasAuthority()) abilityComponent->ServerSetRewindingRPC(1.0);
		abilityComponent->SetRewinding(1.0);
	}
}

void ACharAbilities::RewindReleased()
{
	UCharAbilitiesMovementComponent* abilityComponent = Cast<UCharAbilitiesMovementComponent>(GetCharacterMovement());
	if(abilityComponent)
	{
		if(!HasAuthority()) abilityComponent->ServerSetRewindingRPC(0.0);
		abilityComponent->SetRewinding(0.0);
	}
}

