// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Player/ShooterCharacterMovement.h"

//----------------------------------------------------------------------//
// UPawnMovementComponent
//----------------------------------------------------------------------//
UShooterCharacterMovement::UShooterCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


float UShooterCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const AShooterCharacter* ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);
	if (ShooterCharacterOwner)
	{
		if (ShooterCharacterOwner->IsTargeting())
		{
			MaxSpeed *= ShooterCharacterOwner->GetTargetingSpeedModifier();
		}
		if (ShooterCharacterOwner->IsRunning())
		{
			MaxSpeed *= ShooterCharacterOwner->GetRunningSpeedModifier();
		}
	}

	return MaxSpeed;
}

//------------------------------------------------------
//                  OVERRIDES
//------------------------------------------------------

void UShooterCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if(EMovementMode::MOVE_Walking && fCurrentFuel != JetpackMaxFuel)
    {
        fCurrentFuel = JetpackMaxFuel;
    }
}

void UShooterCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
    // if(CustomMovementMode == ECustomMovementMode::CMOVE_JETPACK)
    // {
    //     PhysJetpack(deltaTime, Iterations);
    // }

    // if(CustomMovementMode == ECustomMovementMode::CMOVE_TELEPORT)
    // {
    //     PhysTeleport(deltaTime, Iterations);
    // }

    // if(CustomMovementMode == ECustomMovementMode::CMOVE_REWIND)
    // {
    //     PhysRewind(deltaTime, Iterations);
    // }

    Super::PhysCustom(deltaTime, Iterations);
}

void UShooterCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity)
{
    // if(bWantsToJetpack) {
    //     if(CanJetpack()){
    //         SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_JETPACK);
    //     }
    // }

    // if(bWantsToTeleport){
    //     if(CanTeleport()){
    //         SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_TELEPORT);
    //     }
    // }

    // if(bWantsToRewind){
    //     if(CanRewind()){
    //         SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_REWIND);
    //     }
    // }

    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UShooterCharacterMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    // If nothing actually changed
    if(PreviousMovementMode == MovementMode && PreviousCustomMode == CustomMovementMode)
    {
        UCharacterMovementComponent::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
        return;
    }

    // If previous movement was jetpacking
    if(PreviousMovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_JETPACK)
    {
        SetJetpacking(false);
    }

    // If previous movement was teleporting
    if(PreviousMovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_TELEPORT)
    {
        SetTeleporting(false);
    }

    // If previous movement was rewinding
    if(PreviousMovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_REWIND)
    {
        SetRewinding(false);
    }

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

}

//------------------------------------------------------
//                  NETWORKING
//------------------------------------------------------

void UShooterCharacterMovement::ServerSetJetpackingRPC(bool wantsToJetpack)
{
    
}

void UShooterCharacterMovement::ServerSetTeleportingRPC(bool wantsToTeleport)
{
    
}

void UShooterCharacterMovement::ServerSetRewindingRPC(bool wantsToRewind)
{
    
}

//------------------------------------------------------
//				GENERAL FUNCTIONS
//------------------------------------------------------

bool UShooterCharacterMovement::VerifyCustomMovementMode(uint8 currentMode) const
{
    if(MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == currentMode) return true;
    return false;
}

//------------------------------------------------------
//				JETPACK FUNCTIONS
//------------------------------------------------------

// PhysJetpack is based off of the CharacterMovementComponent Phys functions (PhysWalking, PhysRunning, etc.)
void UShooterCharacterMovement::PhysJetpack(float deltaTime, int32 Iterations)
{
    // Verifies movement mode, resetting physics if inconsistent
    if(!VerifyCustomMovementMode(ECustomMovementMode::CMOVE_JETPACK)) 
    {
        SetJetpacking(false);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    if(!bWantsToJetpack)
    {
        SetJetpacking(false);
        SetMovementMode(EMovementMode::MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }
}

void UShooterCharacterMovement::execSetJetpacking(bool wantsToJetpack)
{
    bWantsToJetpack = wantsToJetpack;
}

bool UShooterCharacterMovement::CanJetpack()
{
    if(fCurrentFuel > 0) return true;
    return false;
}

void UShooterCharacterMovement::SetJetpacking(bool wantsToJetpack)
{
    if(bWantsToJetpack != wantsToJetpack) 
    {
        execSetJetpacking(wantsToJetpack);
    }
}

void UShooterCharacterMovement::IsJetpacking()
{
    
}

//------------------------------------------------------
//				TELEPORT FUNCTIONS
//------------------------------------------------------

// PhysTeleport is based off of the CharacterMovementComponent Phys functions (PhysWalking, PhysRunning, etc.)
void UShooterCharacterMovement::PhysTeleport(float deltaTime, int32 Iterations)
{
    if(!VerifyCustomMovementMode(ECustomMovementMode::CMOVE_TELEPORT))
    {
        SetTeleporting(false);
    }
}

void UShooterCharacterMovement::execSetTeleporting(bool wantsToTeleport)
{
    bWantsToTeleport = wantsToTeleport;
}

bool UShooterCharacterMovement::CanTeleport()
{
    return false;
}

void UShooterCharacterMovement::SetTeleporting(bool wantsToTeleport)
{
    if(bWantsToTeleport != wantsToTeleport)
    {
        execSetTeleporting(wantsToTeleport);
    }
}

void UShooterCharacterMovement::IsTeleporting()
{
    
}

//------------------------------------------------------
//				REWIND FUNCTIONS
//------------------------------------------------------

// PhysRewind is based off of the CharacterMovementComponent Phys functions (PhysWalking, PhysRunning, etc.)
void UShooterCharacterMovement::PhysRewind(float deltaTime, int32 Iterations)
{
    if(!VerifyCustomMovementMode(ECustomMovementMode::CMOVE_REWIND))
    {
        SetRewinding(false);
    }
}

void UShooterCharacterMovement::execSetRewinding(bool wantsToRewind)
{
    bWantsToRewind = wantsToRewind;
}

bool UShooterCharacterMovement::CanRewind()
{
    return false;
}

void UShooterCharacterMovement::SetRewinding(bool wantsToRewind)
{
    if(bWantsToRewind != wantsToRewind)
    {
        execSetRewinding(wantsToRewind);
    }
}

void UShooterCharacterMovement::IsRewinding()
{
    
}