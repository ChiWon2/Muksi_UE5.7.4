#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"

#include "ProjectileExecutionData.generated.h"

class ABattleProjectileActor;

USTRUCT(BlueprintType)
struct FProjectileExecutionData
{
	GENERATED_BODY()

	// 생성할 투사체 Actor 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<ABattleProjectileActor> ProjectileActorClass;

	// 투사체의 초당 이동 속도.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (ClampMin = "0.0"))
	float MoveSpeed = 1000.0f;

	// 투사체를 생성할 공격자 SkeletalMesh 소켓 이름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FName SpawnSocketName = NAME_None;

	// 투사체가 대상에게 정상 도착했을 때 요청할 Runtime Execution Chain.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TArray<FBattleExecutionEntry> OnHitExecutionChain;
};