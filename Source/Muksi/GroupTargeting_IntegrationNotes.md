
## Group-aware Execution update

- `ProjectileExecution` now launches one projectile for every targeting group with a valid path.
- Each projectile resolves its hit target from that group's `Targets` and passes that character as `ExecutionTarget` to `OnHitExecutionEntries`.
- `ProjectileExecution` waits until every projectile and every accepted nested on-hit runner has completed before finishing.
- `DamageExecution`, `StatusEffectExecution`, `KnockbackExecution`, and target-based runtime modifiers continue to consume `GetAllTargets()` across every group.
- `RushExecution` intentionally keeps using the primary group because a single attacker can only follow one movement path per execution.
