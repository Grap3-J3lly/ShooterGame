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

//------------------------------------------------------
//                  OVERRIDES
//------------------------------------------------------

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

void UShooterCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if(MovementMode == EMovementMode::MOVE_Walking && fCurrentFuel != JetpackMaxFuel)
    {
        fCurrentFuel = JetpackMaxFuel;
    }

    if(fCurrentFuel >= 0)
    {
        fCurrentFuel = FMath::Clamp<float>(fCurrentFuel - .005f, 0, 1);
    }
}

void UShooterCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
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

void UShooterCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity)
{
    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

    if(bWantsToJetpack) {
        if(CanJetpack()){
            UE_LOG(LogTemp, Warning, TEXT("Entered Jetpack Call"));
            SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_JETPACK);
            bInAir = true;
        }
    }

    if(bWantsToTeleport){
        if(CanTeleport()){
            UE_LOG(LogTemp, Warning, TEXT("Entered Teleport Call"));
            SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_TELEPORT);
            bInAir = true;
        }
    }

    if(bWantsToRewind){
        if(CanRewind()){
            UE_LOG(LogTemp, Warning, TEXT("Entered Rewind Call"));
            SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_REWIND);
            bInAir = true;
        }
    }

}

void UShooterCharacterMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    // If nothing actually changed
    if(PreviousMovementMode == MovementMode && PreviousCustomMode == CustomMovementMode)
    {
        Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
        return;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // In Air Check
    if(PreviousMovementMode == EMovementMode::MOVE_Falling && MovementMode != EMovementMode::MOVE_Custom)
    {
        bInAir = false;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // If previous movement was jetpacking
    if(PreviousMovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_JETPACK)
    {
        UE_LOG(LogTemp, Warning, TEXT("Leaving Jetpack Movement Mode"));
        SetJetpacking(false);
    }

    // If previous movement was teleporting
    if(PreviousMovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_TELEPORT)
    {
        UE_LOG(LogTemp, Warning, TEXT("Leaving Teleport Movement Mode"));
        SetTeleporting(false);
    }

    // If previous movement was rewinding
    if(PreviousMovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_REWIND)
    {
        UE_LOG(LogTemp, Warning, TEXT("Leaving Rewind Movement Mode"));
        SetRewinding(false);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // If movement mode is now jetpack
    if(VerifyCustomMovementMode(ECustomMovementMode::CMOVE_JETPACK))
    {

    }

    // If movement mode is now teleport
    if(VerifyCustomMovementMode(ECustomMovementMode::CMOVE_TELEPORT))
    {

    }

    // If movement mode is now rewind
    if(VerifyCustomMovementMode(ECustomMovementMode::CMOVE_REWIND))
    {

    }

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

void UShooterCharacterMovement::ToggleGravityScale(bool gravityActive)
{
    if(gravityActive)
    {
        GravityScale = 0.0;
    }
    else if(!gravityActive)
    {
        GravityScale = 1.0;
    }
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

    if(!bWantsToJetpack || fCurrentFuel <= 0)
    {
        SetJetpacking(false);
        SetMovementMode(EMovementMode::MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Following logic originally outlined in Blueprint
    float newZForce = JetpackThrust * deltaTime;
    Velocity.Z += newZForce;

    UE_LOG(LogTemp, Warning, TEXT("Inside PhysJetpack, CurrentFuel: %f"), fCurrentFuel);

    PhysFalling(deltaTime, Iterations);

}

void UShooterCharacterMovement::execSetJetpacking(bool wantsToJetpack)
{
    bWantsToJetpack = wantsToJetpack;
}

bool UShooterCharacterMovement::CanJetpack()
{
    if(fCurrentFuel > 0 && MovementMode == EMovementMode::MOVE_Falling) return true;
    return false;
}

void UShooterCharacterMovement::SetJetpacking(bool wantsToJetpack)
{
    if(bWantsToJetpack != wantsToJetpack) 
    {
        execSetJetpacking(wantsToJetpack);
    }

    ToggleGravityScale(wantsToJetpack);
}

bool UShooterCharacterMovement::IsJetpacking()
{
    if(MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_JETPACK)
    {
        return true;
    }
    return false;
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
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    if(!bWantsToTeleport)
    {
        SetTeleporting(false);
        SetMovementMode(EMovementMode::MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    ACharacter* currentChar = UCharacterMovementComponent::GetCharacterOwner();
    vPreTeleportLocation = currentChar->GetActorLocation();

    FVector forwardVector;

    if(bInAir)
    {
        forwardVector = UGameplayStatics::GetPlayerCameraManager(currentChar, 0)->GetActorForwardVector();
    }
    else
    {
        forwardVector = currentChar->GetActorForwardVector();
    }

    
    forwardVector *= TeleportDistance;
    forwardVector.Z += fTeleportOffsetZ;

    vPostTeleportLocation = vPreTeleportLocation + forwardVector;

    currentChar->GetCapsuleComponent()->USceneComponent::SetWorldLocation(vPostTeleportLocation, true, 0, ETeleportType::TeleportPhysics);

}

void UShooterCharacterMovement::execSetTeleporting(bool wantsToTeleport)
{
    bWantsToTeleport = wantsToTeleport;
}

bool UShooterCharacterMovement::CanTeleport()
{
	if(!IsTeleporting()) {
		return true;
	}
    return false;
}

void UShooterCharacterMovement::SetTeleporting(bool wantsToTeleport)
{
    if(bWantsToTeleport != wantsToTeleport)
    {
        execSetTeleporting(wantsToTeleport);
    }

	if(wantsToTeleport)
	{
		bIsTeleporting = true;
	}
	else if(!wantsToTeleport)
	{
		bIsTeleporting = false;
	}
}

bool UShooterCharacterMovement::IsTeleporting()
{
    if(MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_TELEPORT)
    {
        return true;
    }
    return false;
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
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    if(!bWantsToRewind)
    {
        SetRewinding(false);
        SetMovementMode(EMovementMode::MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    UE_LOG(LogTemp, Warning, TEXT("Sitting in PhysRewind Now"));
}

void UShooterCharacterMovement::execSetRewinding(bool wantsToRewind)
{
    bWantsToRewind = wantsToRewind;
}

bool UShooterCharacterMovement::CanRewind()
{
	if(!IsRewinding())
	{
		return true;
	}
    return false;
}

void UShooterCharacterMovement::SetRewinding(bool wantsToRewind)
{
    if(bWantsToRewind != wantsToRewind)
    {
        execSetRewinding(wantsToRewind);
    }

	if(wantsToRewind)
	{
		bIsRewinding = true;
	}
	else if(!wantsToRewind)
	{
		bIsRewinding = false;
	}
}

bool UShooterCharacterMovement::IsRewinding()
{
    if(MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_REWIND)
    {
        return true;
    }
    return false;
}