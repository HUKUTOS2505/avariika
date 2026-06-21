# OPTIMIZATION_REPORT — оптимизация проекта «Аварийка»

> Автономный read-only аудит 2026-06-21. Ничего не удалял/не менял — только анализ + рекомендации.
> Узкое звено железа: **RTX 5060, 8 ГБ VRAM** (редактор виснет при переполнении VRAM — Lumen+Nanite+тяжёлые паки).
> ⚠️ Паки 3 карт (Дом/Завод/Больница) НЕ удалять без сверки (`three-maps-plan`, `asset-deletion-method`).

---

## TL;DR — три самых ценных действия

1. **Добавить Nanite/Lumen scalability-тиры** в `DefaultScalability.ini` (сейчас Lumen ВСЕГДА на полном качестве,
   даже на Low) — это прямое лечение VRAM-зависаний. Самый важный пункт, безопасно.
2. **Безопасная чистка ~6–7 ГБ** без риска: `Intermediate` (3 ГБ), `Saved` (0.6 ГБ), `DerivedDataCache` (0.26 ГБ)
   регенерируются; demo-`*_BuiltData` непродакшн-карт (~2–3 ГБ).
3. **Разобрать дубли** Hospital/HospitalCombo и `_Packs`-дубликаты (~1.7 ГБ) — но СНАЧАЛА сверить, какой том в игре.

---

## 1. Куда уходит диск (Content ≈ 93 ГБ + служебное)

| Объект | ГБ | Статус |
|---|---|---|
| `Content/IndustrialFactory` | 21.2 | пак карты **Завод** — нужен (cherry-pick в дом, см. `FACTORY_PACK_AUDIT.md`) |
| `Content/Warehouse` | 16.0 | 2-й фабричный — донор декея; демо-карты не нужны |
| `RawAssets/` | 15.5 | исходники тулкитов (Hyper Procedural 5 ГБ, мокап 2.3 ГБ, WorldBLD 1.9 ГБ) — локально, gitignore |
| `Content/ResidentialHouses` | 11.4 | каркас **Дома** — нужен |
| `Content/_Packs` | 10.3 | **сборная солянка**: 43 демо-карты + 29 BuiltData (вендорские демки, не геймплей) |
| `Content/AtmosphericHouse` | 9.25 | меблировка/атмосфера **Дома** — нужен (4 карты-демо можно облегчить) |
| `*_BuiltData.uasset` (77 файлов) | 7.4 | запечённый свет/GI — крупные: Hospital Demonstration_2 (~2 ГБ), FactoryDocks day/overcast (1.6 ГБ ×2) |
| `Content/Hospital` | 3.0 | пак **Больницы** — нужен; ⚠️ дублируется с HospitalCombo |
| `Intermediate/` | 3.02 | шейдер-кэш — регенерируется |
| `Content/HospitalCombo` | 1.16 | ⚠️ дубль Hospital (восстановлен из git bd76ef7) |
| `Content/NiagaraExamples` | 1.19 | демо-галерея (~109 систем) — не геймплей |
| `Content/CitizenNPC` | 1.14 | бригада/жильцы — нужен (демо-overview можно убрать) |
| `Saved/` | 0.61 | логи/темп — чистимо |
| `DerivedDataCache/` | 0.26 | DDC — регенерируется |

---

## 2. Безопасная чистка СЕЙЧАС (регенерируемое, ноль риска для игры)

> Чистить при **закрытом** редакторе. Всё ниже движок пересоберёт при следующем открытии/готовке.

- `Intermediate/` (3.02 ГБ) — шейдеры/материалы. ⚠️ Первое открытие после чистки = долгая пересборка (10–15 мин на 8 ГБ) — это норма.
- `Saved/` (0.61 ГБ) — логи, автосейвы, темп (кроме нужных тебе `Saved/*.json` от наших скриптов — их оставить).
- `DerivedDataCache/` (0.26 ГБ) — локальный DDC.
- **Demo-`*_BuiltData`** непродакшн-карт (FactoryDocks, Warehouse_01/02, NiagaraExamples GalleryLevel, FluidFlux/M5VFXVOL2 демки) — ~2–3 ГБ; сами `.umap` оставить, свет перепечётся при надобности.

**Итого безопасно: ~6–7 ГБ.** Не трогает ни один геймплейный ассет.

---

## 3. Дубли и лишнее (на твоё решение — СНАЧАЛА сверить)

- **Hospital (3 ГБ) + HospitalCombo (1.16 ГБ) = 4.16 ГБ** — оба про Больницу. Решить, какой том (VOL1–6) реально в карте Больницы, второй — в архив. ⚠️ память: HospitalCombo восстановлен намеренно — не сноси вслепую.
- **`_Packs/` дубли**: `HospitalCombo` и `Construction_VOL2` лежат И в `Content/`, И в `Content/_Packs/` (~1.7 ГБ дублирования). Оставить одну копию.
- **`_Packs/` = 43 демо/showcase-карты** (Garage_Tools showcase, Survival_Horror gallery, ZombieAnimationPack showcase…) + 29 BuiltData — вендорские демки, не продакшн. Кандидат на прореживание (оставить только нужные меши, демо-карты убрать).
- **`_Packs/oldBenchPack`** — легаси-бенчмарк, без ссылок в геймплее.
- **Warehouse demo-карты** (`Warehouse_01/02_P`) — reference, не в шиппинге.
- **VFX-демки**: NiagaraExamples (1.19 ГБ), FluidFlux (0.43 ГБ), M5VFXVOL2 (0.56 ГБ), NiagaraExplosion01 (0.84 ГБ) — учебные/showcase. Нужны конкретные системы (см. `EFFECTS_INVENTORY.md`/`INCIDENT_FX_MAP.md`), не демо-карты.

> ⚠️ Все эти паки уже в `.gitignore` (локально). Удаление влияет только на локальный диск, но **меши могут быть references** в картах — перед сносом `get_referencers` (память `asset-deletion-method`).

---

## 4. ⭐ Настройки рендера — главная находка (VRAM 8 ГБ)

**Что уже хорошо** (`DefaultScalability.ini` / `DefaultEngine.ini`):
- TSR-апскейл настроен по тирам (Low 50% → Epic 100%) — правильно для 8 ГБ.
- `EffectsQuality` гейтит партиклы (0.5–0.75× на Low/Med); FluidFlux завязан на EffectsQuality≥High.
- `r.RayTracing=False`, `r.Lumen.HardwareRayTracing=False` — софт-Lumen, без аппаратного RT (бережёт 8 ГБ).
- `r.Streaming.PoolSize=2500` — поднят (был «TEXTURE STREAMING POOL OVER» от тяжёлых текстур).
- VSM (`r.Shadow.Virtual.Enable=1`), Substrate, статик-лайтинг off (только Lumen).

**🟢 ПОПРАВКА (перепроверено по движковому `BaseScalability.ini`): Lumen/текстуры УЖЕ масштабируются — добавлять тиры НЕ нужно.**
Первичная находка аудита «нет Lumen/Nanite тиров, Lumen всегда full» оказалась **ложной** (агент читал только проектный ini, не движковый).
Движок (`Engine/Config/BaseScalability.ini`) уже даёт: `[GlobalIlluminationQuality@0]` → `r.Lumen.DiffuseIndirect.Allow=0` (Lumen GI
ВЫКЛ на Low, DFAO вместо), `@1` тоже без Lumen, `@2` Lumen с урезанным бюджетом, `@3` полный; `[ReflectionQuality@N]` гейтит
`r.Lumen.Reflections.Allow`; `[TextureQuality@0]` = `r.Streaming.MipBias=16` + `PoolSize=400` + `LimitPoolSizeToVRAM=1` (агрессивно под VRAM);
Nanite гейтится своими движковыми путями. Проект это наследует (переопределяет лишь ResolutionQuality + EffectsQuality).
→ **Кастомные `[LumenQuality@N]`/`[NaniteQuality@N]` добавлять НЕ нужно и ВРЕДНО** — затрут разумные движковые значения. (Правка НЕ внесена.)

**🔴 Что РЕАЛЬНО осталось (рычаги VRAM):**
- **Нет авто-детекта качества при первом запуске** — игрок на слабом железе попадёт на Epic → VRAM overflow → плохой отзыв.
  В `EasyOptionsMenu` уже есть `DemoBenchmarkLevel` — повесить авто-бенчмарк, ставящий scalability на старте.
- **Нет per-map override** — тяжёлая карта (Завод с FluidFlux) может требовать ниже глобала. Добавить map-specific конфиг при VRAM-давлении.

---

## 5. TickOptToolkit (установлен, ещё не применён) — куда натравить

TOT снижает частоту/выключает тики по дистанции и видимости. Кандидаты:
- **`Repairable_*` акторы** (аварии): эффект/звук-тики только когда игрок близко/видит (а не каждый кадр по всему дому).
- **Ambient-эмиттеры** (скрипы, room-tone, капель) — тик по дистанции.
- **CitizenNPC** (жильцы/бригада): `ActorTickInterval`-пулинг, выключать тик пешек вне зоны игрока, упрощённый AI для фоновых.
- **FluidFlux-солверы** — пауза/пул когда вода не на экране; ниже разрешение сетки для невидимых симуляций.
- **TOD-актёры UDS** в фоновых сублевелах — гасить тик солнца/времени когда уровень не активен.
- **Destruction (Next Gen)** — пул актёров разрушения, лимит одновременных симуляций, статик-меш для неинтерактивного щебня.

---

## 6. Git / воспроизводимость (важно для бэкапа)

- Все тяжёлые паки в `.gitignore` (стратегия «локально, re-copy из RawAssets»). Плюс: репо не пухнет. **Минус: свежий клон получит пустой `Content/`** — придётся переимпортить всё вручную.
- У части паков источник = ТОЛЬКО git-история (Hospital/Construction — их нет в RawAssets). Если репо потеряется — паки не восстановить.
- **Рекомендация:** в `WORKLOG.md` вести список «какой пак откуда переимпортить» (FAB-ссылка / RawAssets / git-история). Для ядра (IndustrialFactory, ResidentialHouses) рассмотреть **Git LFS**, чтобы не качать заново.

---

## 7. Приоритеты (что я бы сделал по порядку)

| # | Действие | Риск | Выигрыш |
|---|---|---|---|
| 1 | Nanite/Lumen scalability-тиры + авто-бенчмарк | низкий | **прямое лечение зависаний** |
| 2 | Безопасная чистка Intermediate/Saved/DDC/demo-BuiltData (закрыть редактор) | нет | ~6–7 ГБ |
| 3 | TOT на Repairable/ambient/NPC | низкий | FPS/стабильность в игре |
| 4 | Разобрать Hospital/HospitalCombo + `_Packs`-дубли (сверив тома) | средний | ~1.7–4 ГБ |
| 5 | Demo-карты паков → локальный архив | низкий | ~6 ГБ BuiltData |
| 6 | Отрезать дальнее вост. крыло L_Dom1 (см. `FLOOR1_DESIGN.md` §7) | средний | VRAM + плотность хоррора |

_Связано: `FACTORY_PACK_AUDIT.md`, `FLOOR1_DESIGN.md`, `three-maps-plan`, `water-tech-decision`, `ue-build-workflow`._
