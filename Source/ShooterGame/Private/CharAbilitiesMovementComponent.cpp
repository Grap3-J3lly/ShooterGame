// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "CharAbilitiesMovementComponent.h"


//------------------------------------------------------
//                  OVERRIDES
//------------------------------------------------------

void UCharAbilitiesMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if(EMovementMode::MOVE_Walking && fCurrentFuel != JetpackMaxFuel)
    {
        fCurrentFuel = JetpackMaxFuel;
    }
}

void UCharAbilitiesMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
    if(CustomMovementMode == ECustomMovementMode::CMOVE_JETPACK)
    {
        PhysJetpack(deltaTime, Iterations);
    }

    if(CustomMovementMode == ECustomMovementMode::CMOVE_TELEPORT)
    {
        PhysTeleport(deltaTime, Iterations);
    }

    if(CustomMovementMode == ECustomMovementMode::CMOVE_REWIND)
    {
        PhysRewind(deltaTime, Iterations);
    }

    Super::PhysCustom(deltaTime, Iterations);
}

void UCharAbilitiesMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity)
{
    if(bWantsToJetpack) {
        if(CanJetpack()){
            SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_JETPACK);
        }
    }

    if(bWantsToTeleport){
        if(CanTeleport()){
            SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_TELEPORT);
        }
    }

    if(bWantsToRewind){
        if(CanRewind()){
            SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_REWIND);
        }
    }

    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCharAbilitiesMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    // If nothing actually changed
    if(PreviousMovementMode == MovementMode && PreviousCustomMode == CustomMovementMode)
    {
        Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
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

}

//------------------------------------------------------
//                  NETWORKING
//------------------------------------------------------

void UCharAbilitiesMovementComponent::ServerSetJetpackingRPC(bool wantsToJetpack)
{
    
}

void UCharAbilitiesMovementComponent::ServerSetTeleportingRPC(bool wantsToTeleport)
{
    
}

void UCharAbilitiesMovementComponent::ServerSetRewindingRPC(bool wantsToRewind)
{
    
}

//------------------------------------------------------
//				GENERAL FUNCTIONS
//------------------------------------------------------

bool UCharAbilitiesMovementComponent::VerifyCustomMovementMode(uint8 currentMode) const
{
    if(MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == currentMode) return true;
    return false;
}

//------------------------------------------------------
//				JETPACK FUNCTIONS
//------------------------------------------------------

// PhysJetpack is based off of the CharacterMovementComponent Phys functions (PhysWalking, PhysRunning, etc.)
void UCharAbilitiesMovementComponent::PhysJetpack(float deltaTime, int32 Iterations)
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

void UCharAbilitiesMovementComponent::execSetJetpacking(bool wantsToJetpack)
{
    bWantsToJetpack = wantsToJetpack;
}

bool UCharAbilitiesMovementComponent::CanJetpack()
{
    if(fCurrentFuel > 0) return true;
    return false;
}

void UCharAbilitiesMovementComponent::SetJetpacking(bool wantsToJetpack)
{
    if(bWantsToJetpack != wantsToJetpack) 
    {
        execSetJetpacking(wantsToJetpack);
    }
}

void UCharAbilitiesMovementComponent::IsJetpacking()
{
    
}

//------------------------------------------------------
//				TELEPORT FUNCTIONS
//------------------------------------------------------

// PhysTeleport is based off of the CharacterMovementComponent Phys functions (PhysWalking, PhysRunning, etc.)
void UCharAbilitiesMovementComponent::PhysTeleport(float deltaTime, int32 Iterations)
{
    if(!VerifyCustomMovementMode(ECustomMovementMode::CMOVE_TELEPORT))
    {
        SetTeleporting(false);
    }
}

void UCharAbilitiesMovementComponent::execSetTeleporting(bool wantsToTeleport)
{
    bWantsToTeleport = wantsToTeleport;
}

bool UCharAbilitiesMovementComponent::CanTeleport()
{
    return false;
}

void UCharAbilitiesMovementComponent::SetTeleporting(bool wantsToTeleport)
{
    if(bWantsToTeleport != wantsToTeleport)
    {
        execSetTeleporting(wantsToTeleport);
    }
}

void UCharAbilitiesMovementComponent::IsTeleporting()
{
    
}

//------------------------------------------------------
//				REWIND FUNCTIONS
//------------------------------------------------------

// PhysRewind is based off of the CharacterMovementComponent Phys functions (PhysWalking, PhysRunning, etc.)
void UCharAbilitiesMovementComponent::PhysRewind(float deltaTime, int32 Iterations)
{
    if(!VerifyCustomMovementMode(ECustomMovementMode::CMOVE_REWIND))
    {
        SetRewinding(false);
    }
}

void UCharAbilitiesMovementComponent::execSetRewinding(bool wantsToRewind)
{
    bWantsToRewind = wantsToRewind;
}

bool UCharAbilitiesMovementComponent::CanRewind()
{
    return false;
}

void UCharAbilitiesMovementComponent::SetRewinding(bool wantsToRewind)
{
    if(bWantsToRewind != wantsToRewind)
    {
        execSetRewinding(wantsToRewind);
    }
}

void UCharAbilitiesMovementComponent::IsRewinding()
{
    
}