// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "CharAbilities.h"

// Sets default values
ACharAbilities::ACharAbilities()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

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
	PlayerInputComponent->BindAction(TEXT("Rewind"), IE_Pressed, this, &ACharAbilities::Teleport);
}

void ACharAbilities::Teleport()
{
	// Attempt to see if Input Binding worked
	UE_LOG(LogTemp, Warning, TEXT("Pressed Teleport Key"));
}

