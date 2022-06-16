// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharAbilitiesMovementComponent.generated.h"

/*
* The design below is based off of an online sample project created by 
* HarryPearce on YouTube. It has been modified to fit the context of this project.
*
* Link to Original Video: https://youtu.be/DoZyH86n_gs
* Link to HarryPearce's Sample Project Repo on GitHub: https://github.com/HarryPearce/JetpackTutorial
*/

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_JETPACK = 0,
	CMOVE_TELEPORT = 1,
	CMOVE_REWIND = 2
};

UCLASS()
class SHOOTERGAME_API UCharAbilitiesMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	//------------------------------------------------------
    //                  VARIABLES
    //------------------------------------------------------

	// General
	FVector vCharacterTransform;
	FVector vCharacterVelocity;

	FRotator rCharacterRotation;

	// Jetpack Focus

	// Had to look into bit fields to understand this declaration. 
	// Believe it is expressly stating the size, in bits, the variablle should be
	bool bWantsToJetpack : 1;

	float fCurrentFuel;

	// Teleport Focus
	bool bWantsToTeleport : 1;

	FVector vPreTeleportLocation;
	FVector vPostTeleportLocation;

	// Rewind Focus
	bool bWantsToRewind : 1;

	//------------------------------------------------------
    //                  PROPERTIES
    //------------------------------------------------------

public:

	// General

	// Jetpack Focus
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Jetpack")
		float JetpackMaxFuel = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Jetpack")
		float JetpackSlowdownVal = .35f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Jetpack")
		float JetpackThrust = 250.0f;

	// Teleport Focus
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Teleport")
		float TeleportDistance = 1000.0f;

	// Rewind Focus

	//------------------------------------------------------
    //                  OVERRIDES
    //------------------------------------------------------

private:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	//------------------------------------------------------
    //                  NETWORKING
    //------------------------------------------------------

public:

	// Remote Procedure Calls (RPCs)
	UFUNCTION(BlueprintCallable)
		void ServerSetJetpackingRPC(bool wantsToJetpack);
	UFUNCTION(BlueprintCallable)
		void ServerSetTeleportingRPC(bool wantsToTeleport);
	UFUNCTION(BlueprintCallable)
		void ServerSetRewindingRPC(bool wantsToRewind);

	//------------------------------------------------------
    //				GENERAL FUNCTIONS
    //------------------------------------------------------
	
private:
	
	bool VerifyCustomMovementMode(uint8 currentMode) const;

	//------------------------------------------------------
    //				JETPACK FUNCTIONS
    //------------------------------------------------------

private:

	void PhysJetpack(float deltaTime, int32 Iterations);

protected:

	void execSetJetpacking(bool wantsToJetpack);

public:

	bool CanJetpack();

	UFUNCTION(BlueprintCallable)
		void SetJetpacking(bool wantsToJetpack);
	UFUNCTION(BlueprintCallable)
		void IsJetpacking();

	//------------------------------------------------------
    //				TELEPORT FUNCTIONS
    //------------------------------------------------------

private:

	void PhysTeleport(float deltaTime, int32 Iterations);

protected:

	void execSetTeleporting(bool wantsToTeleport);

public:

	bool CanTeleport();

	UFUNCTION(BlueprintCallable)
		void SetTeleporting(bool wantsToTeleport);
	UFUNCTION(BlueprintCallable)
		void IsTeleporting();

	//------------------------------------------------------
    //				REWIND FUNCTIONS
    //------------------------------------------------------

private:

	void PhysRewind(float deltaTime, int32 Iterations);

protected:

	void execSetRewinding(bool wantsToRewind);

public:

	bool CanRewind();

	UFUNCTION(BlueprintCallable)
		void SetRewinding(bool wantsToRewind);
	UFUNCTION(BlueprintCallable)
		void IsRewinding();

};