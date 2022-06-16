// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharAbilities.generated.h"

UCLASS()
class SHOOTERGAME_API ACharAbilities : public ACharacter
{
	GENERATED_BODY()

	//------------------------------------------------------
    //					CONSTRUCTORS
    //------------------------------------------------------

public:
	// Sets default values for this character's properties
	ACharAbilities();
	ACharAbilities(const FObjectInitializer& ObjectInitializer);

	//------------------------------------------------------
    //				GENERAL FUNCTIONS
    //------------------------------------------------------

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//------------------------------------------------------
    //				ABILITY FUNCTIONS
    //------------------------------------------------------

	void JumpPressed();
	void JumpReleased();

	void TeleportPressed();
	void TeleportReleased();

	void RewindPressed();
	void RewindReleased();

};
