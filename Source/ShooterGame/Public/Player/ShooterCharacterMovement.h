// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Movement component meant for use with Pawns.
 */

/*
* The design below is based off of an online sample project created by 
* HarryPearce on YouTube. It has been modified to fit the context of this project.
*
* Link to Original Video: https://youtu.be/DoZyH86n_gs
* Link to HarryPearce's Sample Project Repo on GitHub: https://github.com/HarryPearce/JetpackTutorial
*/

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacterMovement.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_JETPACK = 0,
	CMOVE_TELEPORT = 1,
	CMOVE_REWIND = 2
};

UCLASS()
class UShooterCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

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
	bool bIsTeleporting = false;
	bool bInAir = false;

	float fTeleportOffsetZ = 100.0f;

	FVector vPreTeleportLocation;
	FVector vPostTeleportLocation;

	// Rewind Focus
	bool bWantsToRewind : 1;
	bool bIsRewinding = false;

	float fRemainingDuration;
	float fRemainingResetDuration;

	TArray<FVector> aActorLocation;
	TArray<FVector> aActorVelocity;

	TArray<FRotator> aActorRotation;

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
		float TeleportDistance = 250.0f;

	// Rewind Focus
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Rewind")
		float RewindDuration = 5.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Rewind")
		float RewindResetDuration = 5.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom|Rewind")
		float RewindTickValue = .01f;

	//------------------------------------------------------
    //                  OVERRIDES
    //------------------------------------------------------

private:

	virtual float GetMaxSpeed() const override;

 	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
 	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	//------------------------------------------------------
    //                  NETWORKING
    //------------------------------------------------------

private:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

public:

	// Remote Procedure Calls (RPCs)
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void ServerSetJetpackingRPC(bool wantsToJetpack);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void ServerSetTeleportingRPC(bool wantsToTeleport);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void ServerSetRewindingRPC(bool wantsToRewind);

	UFUNCTION(Client, Reliable, BlueprintCallable)
		void ClientSetJetpackingRPC(bool wantsToJetpack);
	UFUNCTION(Client, Reliable, BlueprintCallable)
		void ClientSetTeleportingRPC(bool wantsToTeleport);
	UFUNCTION(Client, Reliable, BlueprintCallable)
		void ClientSetRewindingRPC(bool wantsToRewind);

	//------------------------------------------------------
    //				GENERAL FUNCTIONS
    //------------------------------------------------------
	
private:
	
	bool VerifyCustomMovementMode(uint8 currentMode) const;
	void ToggleGravityScale(bool gravityActive);

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
		bool IsJetpacking();

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
		bool IsTeleporting();

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
		bool IsRewinding();
};

//-----------------------------------------------------------------------------------
//                  	ADDITIONAL NETWORK COMPONENT CLASSES
//-----------------------------------------------------------------------------------

// Handles saved moves from client in case Server calls to undo a move
class FSavedMove_ShooterCharacterMovement : public FSavedMove_Character
{
	friend class UShooterCharacterMovement;

public:

	//------------------------------------------------------
    //                  VARIABLES
    //------------------------------------------------------

	// Jetpack Focus
	bool savedWantsToJetpack : 1;
	float savedRemainingFuel;

	// Teleport Focus
	bool savedWantsToTeleport : 1;
	
	// Rewind Focus
	bool savedWantsToRewind : 1;
	float savedRemainingDuration;

	//------------------------------------------------------
    //                  OVERRIDES
    //------------------------------------------------------

	typedef FSavedMove_Character Super;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;

};

// Handles prediction data for a client.
class FNetworkPredictionData_Client_ShooterCharacterMovement : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_ShooterCharacterMovement(const UCharacterMovementComponent& ClientMovement);
	typedef FNetworkPredictionData_Client_Character Super;
	virtual FSavedMovePtr AllocateNewMove() override;
};