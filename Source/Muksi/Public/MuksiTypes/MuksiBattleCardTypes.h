#pragma once

/*
이동
공격
방어

그냥 공격 -> 데미지 가장 높음 / 코스트 낮음
이동 공격(돌진) -> 데미지 낮음 / 코스트 높음 / 이동거리 짧음
이동 방어(회피) -> 코스트 높음/ 이동거리 짧음
장거리 공격(직선) -> 코스트 비교적 낮음
장거리 공격(추적) -> 코스트 비교적 높음 / 데미지 비교적 낮음
광역기 공격 -> 코스트 매우 높음 / 데미지 비교적 매우 낮음
방어 -> 다음 합 안아프게 맞음 / 코스트 낮음
호흡(숨 고르기) -> 코스트 회복
명상 -> 드로우


신속 -> 속도 값 상승
허약 -> 속도 값 하락

변초를 상대(AI)만 사용하는 느낌으로? 마공느낌으로? 마교/사파 쪽에서 쓰는 느낌으로?

전투 파이프라인

각 전투 패시브 효과는 해당 시간에 사용됨 
전투 매니저 담당
(전투 시작 시, 국 시작 시, 합 시작 시, 공격 시작 시, 공격 종료 시, 합 종료 시, 국 종료 시, 전투 종료 시)
캐릭터 담당
(ex) 피해 받을 시, 체력이 50% 미만이 되었을 시 등등)

1. 각 캐릭터 패시브 (아군) 효과 적용 (전투 전)

1국(局) 시작 (혹은 1판 시작)

2. 각 캐릭터 속도 값 제공 
3. 카드 선택 (3장)
4. 속도 값 공개
5. 속도 값이 높은 캐릭터 카드 사용 (각자 카드 공개) (합 시작 시) (변초 확인 가능시 실제 카드공개)

5 - 1. 전투 효과 등등은 3합이 다 진행 된 이후 반영 -> 처음 속도 값, 출혈, 등등의 효과가 3합 까지 유지 ex) 처음 속도가 4가 나와서 선공을 잡으면 3합 동안 선공
5 - 2. 전투 효과를 매 합마다 진행 -> 속도 값을 매 합 마다 정하기 <- 일단 이걸로

6. 선공 1합 (공격 시작)
6-1. 선공 첫번째 선택한 카드 사용(변초면 실제 카드 공개)
6-1.선공 1합 (공격 종료)
6-2. 후공 1합 (공격 시작)
6-2. 후공 첫번째 선택한 카드 사용(변초면 실제 카드 공개)
6-2. 후공 1합 (공격 종료)

7. 각 카드 효과, 패시브 등등 적용 후 각 캐릭터 속도 값 공개 후 선공 후공 확정

8. 선공 2합 (공격 시작)
8-1. 선공 두번째 선택한 카드 사용(변초면 실제 카드 공개)
8-1.선공 2합 (공격 종료)
8-2. 후공 2합 (공격 시작)
8-2. 후공 두번째 선택한 카드 사용(변초면 실제 카드 공개)
8-2. 후공 2합 (공격 종료)

9. 각 카드 효과, 패시브 등등 적용 후 각 캐릭터 속도 값 공개 후 선공 후공 확정

10. 선공 3합 (공격 시작)
10-1. 선공 세번째 선택한 카드 사용(변초면 실제 카드 공개)
10-1.선공 3합 (공격 종료)
10-2. 후공 3합 (공격 시작)
10-2. 후공 세번째 선택한 카드 사용(변초면 실제 카드 공개)
10-2. 후공 3합 (공격 종료)

1국(局) 종료 (혹은 1판 종료)
*/

UENUM(BlueprintType)
enum class ECardEffectType : uint8
{
	None,
	Move,
	Damage,
	ApplyStatus,
	
};

UENUM(BlueprintType)
enum class EStatusEffectType : uint8
{
	None,
	Rush,
	Bleed,
	Poison,
};




/*USTRUCT(BlueprintType)
struct FCardEffectSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECardEffectType EffectType = ECardEffectType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Value = 0;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EStatusEffectType StatusEffectType = EStatusEffectType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Duration = 0;
};*/
