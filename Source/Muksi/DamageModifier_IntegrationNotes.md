# DamageModifier Integration Notes

`FDamageExecutionData`는 `DamageValue`와 Instanced `DamageModifiers` 배열을 가진다.

Damage 처리 순서:
1. `DamageValue`에서 시작
2. `DamageModifiers`를 배열 순서대로 대상별 적용
3. `ApplyIncomingDamageModifiers()`로 Shield / ReduceDamage / MultiplyDamage 처리
4. 최종 HP Damage 적용

## TargetHasStatusEffectDamageModifier

`UTargetHasStatusEffectDamageModifier` 자체가 편집 데이터를 가진다.

- `StatusEffectID`
- `AdditionalDamage`

DataAsset에서 `DamageModifiers` 배열 원소에 `TargetHasStatusEffectDamageModifier`를 생성하면 위 필드가 바로 표시된다.

혈화난무 예시:

- DamageValue = 1
- DamageModifiers[0]
  - Class = TargetHasStatusEffectDamageModifier
  - StatusEffectID = Bleed
  - AdditionalDamage = 1
- DamageModifiers[1]
  - Class = TargetHasStatusEffectDamageModifier
  - StatusEffectID = SaeMaek
  - AdditionalDamage = 1
