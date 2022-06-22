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

    // Jetpack Resets
    if(MovementMode != EMovementMode::MOVE_Custom && fCurrentFuel != JetpackMaxFuel)
    {
        fCurrentFuel = JetpackMaxFuel;
    }

    if(fCurrentFuel >= 0)
    {
        fCurrentFuel = FMath::Clamp<float>(fCurrentFuel - .005f, 0, 1);
    }

    // Rewind Tracking
    if(fRemainingDuration <= 0 && !IsRewinding())
    {
        if(fRemainingResetDuration <= 0)
        {
            fRemainingResetDuration = RewindResetDuration;
            fRemainingDuration = RewindDuration;
        }
        fRemainingResetDuration -= RewindTickValue;
    }
    
    if(fRemainingDuration > 0 && IsRewinding())
    {
        fRemainingDuration -= RewindTickValue;
    }

    // Record movement for rewind
    if(!bWantsToRewind)
    {

        FVector tempLocation, tempVelocity;
        FRotator tempRotation;

        ACharacter* currentChar = UCharacterMovementComponent::GetCharacterOwner();

        tempLocation = currentChar->GetActorLocation();
        tempVelocity = Velocity;
        tempRotation = currentChar->GetControlRotation();

        aActorLocation.Add(tempLocation);
        aActorVelocity.Add(tempVelocity);
        aActorRotation.Add(tempRotation);

        // Attempt at garbage collection
        tempLocation.Set(NULL, NULL, NULL);
        tempVelocity.Set(NULL, NULL, NULL);
        tempRotation = FRotator(NULL, NULL, NULL);
        
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
    //Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

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

        // Give input control back
        ACharacter* currentChar = UCharacterMovementComponent::GetCharacterOwner();
        currentChar->GetController()->ResetIgnoreMoveInput();
        currentChar->GetController()->ResetIgnoreLookInput();

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
        //UE_LOG(LogTemp, Warning, TEXT("Entering Rewind Movement Mode"));
        fRemainingDuration = RewindDuration;
        fRemainingResetDuration = 0;
    }

    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

}

//------------------------------------------------------
//                  NETWORKING
//------------------------------------------------------

FNetworkPredictionData_Client*
UShooterCharacterMovement::GetPredictionData_Client() const
{
    check(PawnOwner != NULL);

    if(!ClientPredictionData)
    {
        UShooterCharacterMovement* MutableThis = const_cast<UShooterCharacterMovement*>(this);
        MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_ShooterCharacterMovement(*this);
    }
    return ClientPredictionData;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UShooterCharacterMovement::ServerSetJetpackingRPC_Validate(bool wantsToJetpack)
{
    return true;
}
void UShooterCharacterMovement::ServerSetJetpackingRPC_Implementation(bool wantsToJetpack)
{
    execSetJetpacking(wantsToJetpack);
}

bool UShooterCharacterMovement::ServerSetTeleportingRPC_Validate(bool wantsToTeleport)
{
    return true;
}
void UShooterCharacterMovement::ServerSetTeleportingRPC_Implementation(bool wantsToTeleport)
{
    execSetTeleporting(wantsToTeleport);
}

bool UShooterCharacterMovement::ServerSetRewindingRPC_Validate(bool wantsToRewind)
{
    return true;
}
void UShooterCharacterMovement::ServerSetRewindingRPC_Implementation(bool wantsToRewind)
{
    execSetRewinding(wantsToRewind);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UShooterCharacterMovement::ClientSetJetpackingRPC_Implementation(bool wantsToJetpack)
{
    execSetJetpacking(wantsToJetpack);
}

void UShooterCharacterMovement::ClientSetTeleportingRPC_Implementation(bool wantsToTeleport)
{
    execSetTeleporting(wantsToTeleport);
}

void UShooterCharacterMovement::ClientSetRewindingRPC_Implementation(bool wantsToRewind)
{
    execSetRewinding(wantsToRewind);
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
    if(fCurrentFuel > 0 && MovementMode != EMovementMode::MOVE_Custom) return true;
    return false;
}

void UShooterCharacterMovement::SetJetpacking(bool wantsToJetpack)
{
    if(bWantsToJetpack != wantsToJetpack) 
    {
        execSetJetpacking(wantsToJetpack);
    }

    ToggleGravityScale(wantsToJetpack);


    // - - - - - - - - - Network Elements - - - - - - - - - - - - -

    if(!GetOwner() || !GetPawnOwner())
    {return;}

    if(!GetOwner()->HasAuthority() && GetPawnOwner()->IsLocallyControlled())
    {
        ServerSetJetpackingRPC(bWantsToJetpack);
    }
    else if(GetOwner()->HasAuthority() && !GetPawnOwner()->IsLocallyControlled())
    {
        ClientSetJetpackingRPC(bWantsToJetpack);
    }

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
    // Exit Conditions

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

    // Attempted to account for when teleport is activated while looking at the floor.
    // Ideally, teleport would still work but instead of teleporting in the direction that the camera is facing,
    // it would teleport forward based off of where the character is facing. 
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

    // - - - - - - - - - Network Elements - - - - - - - - - - - - -

    if(!GetOwner() || !GetPawnOwner())
    {return;}

    if(!GetOwner()->HasAuthority() && GetPawnOwner()->IsLocallyControlled())
    {
        ServerSetTeleportingRPC(bWantsToTeleport);
    }
    else if(GetOwner()->HasAuthority() && !GetPawnOwner()->IsLocallyControlled())
    {
        ClientSetTeleportingRPC(bWantsToTeleport);
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
        UE_LOG(LogTemp, Warning, TEXT("Sitting in PhysRewind First If"));

        SetRewinding(false);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    if(!bWantsToRewind || fRemainingDuration <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sitting in PhysRewind Second If"));

        SetRewinding(false);
        SetMovementMode(EMovementMode::MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    ACharacter* currentChar = UCharacterMovementComponent::GetCharacterOwner();
    APlayerCameraManager* thisCharacterCameraManager = UGameplayStatics::GetPlayerCameraManager(currentChar, 0);

    // Temporarily lock move/look input
    currentChar->GetController()->SetIgnoreMoveInput(true);
    currentChar->GetController()->SetIgnoreLookInput(true);


    // Get most recent position and details
    FVector tempLocation = aActorLocation.Last();
    FVector tempVelocity = aActorVelocity.Last();
    FRotator tempRotation = aActorRotation.Last();

    // Set location, velocity, and camera rotation to previous value
    currentChar->GetCapsuleComponent()->USceneComponent::SetWorldLocation(tempLocation, false, 0, ETeleportType::None);
    Velocity = tempVelocity;
    //thisCharacterCameraManager->SetActorRotation(tempRotation, ETeleportType::None);
    //currentChar->GetController()->SetControlRotation(tempRotation);
    GetPawnOwner()->Controller->SetControlRotation(tempRotation);

    // Remove last value from list
    if(aActorLocation.Num()-1 > 0) 
    {aActorLocation.RemoveAt(aActorLocation.Num()-1);}
    else
    {bWantsToRewind = false;}

    if(aActorVelocity.Num()-1 > 0)
    {aActorVelocity.RemoveAt(aActorVelocity.Num()-1);}
    else
    {bWantsToRewind = false;}

    if(aActorRotation.Num()-1 > 0)
    {aActorRotation.RemoveAt(aActorRotation.Num()-1);}
    else
    {bWantsToRewind = false;}

    // Attempt at garbage collection
        tempLocation.Set(NULL, NULL, NULL);
        tempVelocity.Set(NULL, NULL, NULL);
        tempRotation = FRotator(NULL, NULL, NULL);

}

void UShooterCharacterMovement::execSetRewinding(bool wantsToRewind)
{
    bWantsToRewind = wantsToRewind;
}

bool UShooterCharacterMovement::CanRewind()
{
    //UE_LOG(LogTemp, Warning, TEXT("Sitting in CanRewind Now: %f"), fRemainingResetDuration);
	if(!IsRewinding() && fRemainingDuration > 0)
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

    // - - - - - - - - - Network Elements - - - - - - - - - - - - -

    if(!GetOwner() || !GetPawnOwner())
    {return;}

    if(!GetOwner()->HasAuthority() && GetPawnOwner()->IsLocallyControlled())
    {
        ServerSetRewindingRPC(bWantsToRewind);
    }
    else if(GetOwner()->HasAuthority() && !GetPawnOwner()->IsLocallyControlled())
    {
        ClientSetRewindingRPC(bWantsToRewind);
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

//-----------------------------------------------------------------------------------
//                ADDITIONAL NETWORK COMPONENT CLASS FUNCTIONS
//-----------------------------------------------------------------------------------

FNetworkPredictionData_Client_ShooterCharacterMovement::FNetworkPredictionData_Client_ShooterCharacterMovement(const UCharacterMovementComponent& ClientMovement)
    : Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_ShooterCharacterMovement::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_ShooterCharacterMovement());
}

void FSavedMove_ShooterCharacterMovement::Clear()
{
    Super::Clear();
    savedWantsToJetpack = false;
    savedRemainingFuel = 1.0f;

    savedWantsToTeleport = false;

    savedWantsToRewind = false;
    savedRemainingDuration = 0.0f;
}

uint8 FSavedMove_ShooterCharacterMovement::GetCompressedFlags() const
{
    return Super::GetCompressedFlags();
}

bool FSavedMove_ShooterCharacterMovement::CanCombineWith(const FSavedMovePtr & NewMove, ACharacter * Character, float MaxDelta) const
{
    if(savedWantsToJetpack != ((FSavedMove_ShooterCharacterMovement*)&NewMove)->savedWantsToJetpack)
    {return false;}
    if(savedRemainingFuel != ((FSavedMove_ShooterCharacterMovement*)&NewMove)->savedRemainingFuel)
    {return false;}
    if(savedWantsToTeleport != ((FSavedMove_ShooterCharacterMovement*)&NewMove)->savedWantsToTeleport)
    {return false;}
    if(savedWantsToRewind != ((FSavedMove_ShooterCharacterMovement*)&NewMove)->savedWantsToRewind)
    {return false;}
    if(savedRemainingDuration != ((FSavedMove_ShooterCharacterMovement*)&NewMove)->savedRemainingDuration)
    {return false;}

    return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_ShooterCharacterMovement::SetMoveFor(ACharacter * Character, float InDeltaTime, FVector const & NewAccel, FNetworkPredictionData_Client_Character & ClientData)
{
    Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
    UShooterCharacterMovement* CharacterMovement = Cast<UShooterCharacterMovement>(Character->GetCharacterMovement());
    if(CharacterMovement)
    {
        savedWantsToJetpack = CharacterMovement->bWantsToJetpack;
        savedRemainingFuel = CharacterMovement->fCurrentFuel;

        savedWantsToTeleport = CharacterMovement->bWantsToTeleport;

        savedWantsToRewind = CharacterMovement->bWantsToRewind;
        savedRemainingDuration = CharacterMovement->fRemainingDuration;
    }
}

void FSavedMove_ShooterCharacterMovement::PrepMoveFor(ACharacter * Character)
{
    Super::PrepMoveFor(Character);
    UShooterCharacterMovement* CharacterMovement = Cast<UShooterCharacterMovement>(Character->GetCharacterMovement());
    if(CharacterMovement)
    {
        CharacterMovement->bWantsToJetpack = savedWantsToJetpack;
        CharacterMovement->fCurrentFuel = savedRemainingFuel;

        CharacterMovement->bWantsToTeleport = savedWantsToTeleport;

        CharacterMovement->bWantsToRewind = savedWantsToRewind;
        CharacterMovement->fRemainingDuration = savedRemainingDuration;
    }
}