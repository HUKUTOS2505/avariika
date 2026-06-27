# ANIM_IMPORT_PLAN — `anims_incoming` → проект (Группа B)

> Составлено автономно 2026-06-24. Источник: `RawAssets/anims_incoming` (5 суб-паков, **1436 FBX, 2.3 ГБ**).
> Цель — занести анимы на скелет игрока и сделать пригодными к использованию в `ABP_Worker`.

## 🎯 Целевой скелет (подтверждено в редакторе)

`ABP_Worker` (восстановлен после краша 2026-06-24) сидит на:

```
/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin   (UE5-маннекен)
```

Quantum-тело (`SKM_Worker_*`) гоняется этим же скелетом через compatible-skeleton (память `quantum-compat-skeleton-save`: FBL≡WorkAnimations, один UE5-mann). **Все новые анимы игрока должны в итоге лежать на этом `SK_Mannequin`.**

Скелет Quantum-меша (`/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`) — отдельный; на него ничего ретаргетить не нужно (тело анимируется через compatible с UE5-mann).

## 📦 Разбор 5 суб-паков

| Суб-пак | FBX | Скелет источника | Действие | Ценность |
|---|---:|---|---|---|
| **Motifect Injured & Exhausted** | 30 | AI-«stickman» (свой риг) | **импорт + IK-ретаргет → SK_Mannequin** | ⭐⭐⭐ диегетик-травмы/выносливость |
| **Motifect Emotes & Social** | ~? | AI-«stickman» (свой риг) | импорт + IK-ретаргет → SK_Mannequin | ⭐ кооп-социалка |
| **Getting Up 01/02** | 2 | UE4-маннекен (`AnimUE4_GettingUp`) | импорт на UE4-скелет + ретаргет UE4→UE5 | ⭐⭐ подъём после нокдауна |
| **Game Animation Sample** | ~1400 | **уже ретаргечено на UE5-mann** | **ПРЯМОЙ импорт на SK_Mannequin (ретаргет НЕ нужен)** | ⭐ библиотека локомоции под BlendSpace/MM |

### Почему ретаргет нельзя делать вслепую
Motifect — это **AI-генерённые клипы на собственном скелете** (README пака прямо: «Retarget the animation to your own character mesh… the stickman is for preview only»). Корректность ретаргета (поза не ломается, стопы не разъезжаются, таз на месте) **требует визуальной проверки** — авто-align IK-ретаргетера не трогает root/таз (память `ik-retarget-python`). Поэтому ретаргет оставлен на заход вместе с тобой, чтобы не наплодить кривых анимов рядом с активной работой над `ABP_Worker`.

## ✅ Рекомендуемый порядок (для захода с верификацией)

1. **Injured & Exhausted (30, высшая ценность).** Импорт FBX в `Content/Avariika/Anim/Motifect_Injured/` (создаётся Motifect-скелет). → IK Retargeter Motifect→SK_Mannequin → ретаргет 30 клипов → проверить позы в редакторе → привязать к состояниям (ранен/выдохся/limp/collapse/drag) в `ABP_Worker`.
2. **Getting Up (2).** Импорт на UE4-скелет (в проекте есть `UE4_Mannequin_Skeleton`) → ретаргет UE4→UE5 (есть op-stack пайплайн, память `ik-retarget-python`).
3. **Game Animation Sample (срез).** ПРЯМОЙ импорт нужного среза (idle/walk/run/crouch) на `SK_Mannequin` — без ретаргета. Полную 1400-библиотеку тащить только если строим motion-matching/BlendSpace.
4. **Emotes** — по желанию, последним.

## Засады
- FBX-импорт ≠ robocopy: нужен правильный целевой скелет в настройках импорта, иначе мусор.
- Motifect-клипы содержат preview-mesh (stickman) — импортировать как **animation**, не как gameplay-mesh.
- Game Anim Sample = ~1400 файлов; **не** булк-импортить целиком без нужды (тяжело + это в основном motion-matching-варианты AimOffset/Avoidance/Crouch).
- Лицензия Motifect: free для personal/commercial, **нельзя перепродавать сырые FBX**.

_Связано: [[anim-skeleton-truth]], [[quantum-compat-skeleton-save]], [[ik-retarget-python]], [[operator-character-anim-system]]._
