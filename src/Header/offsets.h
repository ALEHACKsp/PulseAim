//
// love fn sucks
// paulgamer stop pasteing plz
// hehe cracked
//
// YTMcGamer#0131
//
#pragma once
#include "../Incl.h"
namespace Offsets {
	typedef struct {
		LPCWSTR Name;
		DWORD& Offset;
	} OFFSET;

	extern PVOID* uWorld;

	namespace Engine {
		namespace World {
			extern DWORD OwningGameInstance;
			extern DWORD Levels;
		}

		namespace Level {
			extern DWORD AActors;
		}

		namespace GameInstance {
			extern DWORD LocalPlayers;
		}

		namespace Player {
			extern DWORD PlayerController;
		}

		namespace Controller {
			extern DWORD ControlRotation;
			extern PVOID SetControlRotation;
			extern PVOID ClientSetRotation;
			extern PVOID invisible;
		}

		namespace PlayerController {
			extern DWORD PlayerCameraManager;
			extern DWORD AcknowledgedPawn;
		}

		namespace Pawn {
			extern DWORD PlayerState;
		}

		namespace PlayerState {
			extern PVOID GetPlayerName;
		}

		namespace Actor {
			extern DWORD RootComponent;
			extern DWORD CustomTimeDilation;
		}

		namespace Character {
			extern DWORD Mesh;
		}

		namespace SceneComponent {
			extern DWORD RelativeLocation;
			extern DWORD ComponentVelocity;
		}

		namespace StaticMeshComponent {
			extern DWORD ComponentToWorld;
			extern DWORD StaticMesh;
		}

		namespace SkinnedMeshComponent {
			extern DWORD CachedWorldSpaceBounds;
		}
	}

	namespace FortniteGame {
		namespace FortPawn {
			extern DWORD bIsDBNO;
			extern DWORD bIsDying;
			extern DWORD CurrentWeapon;
		}

		namespace FortPickup {
			extern DWORD PrimaryPickupItemEntry;
		}

		namespace FortItemEntry {
			extern DWORD ItemDefinition;
		}

		namespace FortItemDefinition {
			extern DWORD DisplayName;
			extern DWORD Tier;
		}

		namespace FortPlayerStateAthena {
			extern DWORD TeamIndex;
		}

		namespace FortWeapon {
			extern DWORD WeaponData;
			extern DWORD LastFireTime;
			extern DWORD LastFireTimeVerified;
		}

		namespace FortWeaponItemDefinition {
			extern DWORD WeaponStatHandle;
		}

		namespace FortProjectileAthena {
			extern DWORD FireStartLoc;
		}
		namespace test
		{
			extern DWORD test1;
		}

		namespace FortBaseWeaponStats {
			extern DWORD ReloadTime;
		}

		namespace BuildingContainer {
			extern DWORD bAlreadySearched;
		}
	}

	BOOLEAN Initialize();
}