# Quantum Worker: read-only аудит цвета кожи

Дата проверки: 2026-07-03.

Проверены без изменения ассетов:

- `/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Head`;
- `/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Body`;
- `/Game/Modular_Workers/Materials/Male_Body/MI_Quantum_Arms`;
- соответствующие `MI_Quantum_*_Afro` и head Alpha variants;
- master material `/Game/Modular_Workers/Materials/Quanum_MasterMaterial/M_Quantum_Master_Material`;
- material slots European/Afro head, torso, full body, hands и wrist meshes.

Проверка выполнялась через read-only Unreal API и чтение метаданных. Материалы не редактировались и не компилировались; PIE не запускался. Визуальный A/B-тест оттенков не выполнялся, поэтому отсутствие швов нельзя считать подтверждённым.

## Краткий вывод

`Color Correction` — реальный Vector Parameter типа `LinearColor` (RGB+A), общий для `MI_Quantum_Head`, `MI_Quantum_Body` и `MI_Quantum_Arms`. Но это не специализированный параметр кожи. Master material смешивает обработанный BaseColor с `Color Correction` через отдельный scalar `Color Correction Value`.

При штатных значениях instances:

- `Color Correction = (0, 0, 0, 1)`;
- `Color Correction Value = 0`;
- `Brightness = 1`;
- `Contrast = 1`;
- `Saturation = 1`.

Поэтому изменение только `Color Correction` визуально ничего не даст, пока `Color Correction Value` остаётся равным нулю.

Одинаковые значения можно технически назначить голове, телу и рукам, поскольку все три instances используют один master material и одинаковый набор параметров. Однако гарантировать отсутствие заметных швов нельзя: части используют три отдельные BaseColor/Normal/ORM texture sets. При промежуточном blend-весе исходные различия текстур сохраняются, а при весе, близком к `1`, цвет выравнивается ценой сильной потери исходной цветовой детализации.

Для production skin picker безопаснее использовать заранее проверенные согласованные наборы European/Afro. Произвольный RGB следует считать экспериментальной коррекцией до визуального теста шеи, плеч, запястий и кистей при нескольких типах освещения.

## 1. Тип и назначение `Color Correction`

`Color Correction` объявлен в `M_Quantum_Master_Material` как `MaterialExpressionVectorParameter`. Unreal API возвращает его как `LinearColor`, то есть это RGB-параметр с alpha-компонентом.

Цветовая цепочка master material устроена так:

1. `BaseColor` texture проходит через `Saturation`.
2. Результат умножается на `Brightness`.
3. Затем применяется `Contrast` через Power.
4. Полученный цвет поступает в вход A Lerp.
5. `Color Correction` поступает во вход B Lerp.
6. `Color Correction Value` управляет Alpha этого Lerp.

Следовательно, `Color Correction` не является обычным multiplicative tint. Это целевой цвет для линейного смешивания с обработанным BaseColor.

Поскольку проверенные head/body/arms instances назначены на кожные material slots, параметр действительно меняет видимый цвет кожи этих частей. Но сам параметр универсален для master material и используется также множеством материалов одежды и аксессуаров; семантически это общая цветокоррекция, а не `Skin Tone`, `Melanin` или маскированный skin-only control.

## 2. Голова, тело и руки

| Instance | BaseColor texture | Normal/ORM family | Параметры |
|---|---|---|---|
| `MI_Quantum_Head` | European Head | European Head | Общий набор master material |
| `MI_Quantum_Body` | European Body | European Body | Общий набор master material |
| `MI_Quantum_Arms` | European Arms | European Arms | Общий набор master material |
| `MI_Quantum_Head_Afro` | Afro Head | Afro Head | Тот же набор и defaults |
| `MI_Quantum_Body_Afro` | Afro Body | Afro Body | Тот же набор и defaults |
| `MI_Quantum_Arms_Afro` | Afro Arms | Afro Arms | Тот же набор и defaults |

Worker Bundle использует для открытых рук:

- `SKM_Quantum_Hands` → slot 0 `M_Quantum_Arms` → `MI_Quantum_Arms`;
- `SKM_Quantum_Hands_Afro` → slot 0 `M_Quantum_Arms` → `MI_Quantum_Arms_Afro`;
- `SKM_Quantum_FullHands` → slot 0 `M_Quantum_Arms` → `MI_Quantum_Arms`;
- `SKM_Quantum_FullHands_Afro` → slot 0 `M_Quantum_Arms` → `MI_Quantum_Arms_Afro`;
- `SKM_Quantum_Wrist` → slot 0 `M_Quantum_Arms` → `MI_Quantum_Arms`;
- `SKM_Quantum_Wrist_Afro` → slot 0 `M_Quantum_Arms` → `MI_Quantum_Arms_Afro`.

Glove meshes используют материалы перчаток, а не skin material; skin picker не должен перекрашивать такие slots.

## 3. Можно ли использовать одно значение без швов

Технически — да: на динамических instances головы, тела и рук можно установить одинаковые `Color Correction` и `Color Correction Value`.

Визуально бесшовный результат не доказан по следующим причинам:

- Head, Body и Arms имеют разные BaseColor textures.
- У частей отдельные Normal и ORM textures, поэтому граница может проявляться освещением даже при близком BaseColor.
- `Color Correction Value < 1` сохраняет вклад разных исходных BaseColor.
- `Color Correction Value = 1` заменяет цветовой результат на почти сплошной RGB и подавляет исходную цветовую детализацию кожи.
- European и Afro — отдельные согласованные texture families, а не один base texture с общим melanin-параметром.

Практический вывод: использовать один RGB можно только после отдельной визуальной калибровки. Следует хранить не произвольный цвет, а проверенный skin preset, содержащий как минимум variant family, `Color Correction` и `Color Correction Value`. Нельзя смешивать European head с Afro arms/body или наоборот.

## 4. Material slots European/Afro

### Отдельные head meshes

У обоих head variants кожный material находится в slot index `8`, slot name `M_Quantum_Head`:

| Mesh | Skin slot | Назначенный material |
|---|---:|---|
| `SKM_Quantum_Head` | 8 | `MI_Quantum_Head` |
| `SKM_Quantum_Head_Afro` | 8 | `MI_Quantum_Head_Afro` |

Slots 0–7 относятся к cartilage, facial details, eye edge, eyes, eye shell, saliva и teeth. Их skin picker менять не должен.

`MI_Quantum_Head_Alpha` и `MI_Quantum_Head_Afro_Alpha` имеют те же skin textures и параметры, но дополнительно используют Alpha Map. Если конкретный head mesh/вариант назначает Alpha instance, менять нужно именно его skin instance, сохраняя Alpha Map и static-switch конфигурацию; подмена обычным `MI_Quantum_Head*` может сломать маскирование.

### Combined torso/full-body meshes

`SKM_Quantum_Torso` и `SKM_Quantum_Torso_Afro`:

| Slot index | Slot name | European | Afro |
|---:|---|---|---|
| 0 | `M_Quantum_Head` | `MI_Quantum_Head` | `MI_Quantum_Head_Afro` |
| 1 | `M_Quantum_Arms` | `MI_Quantum_Arms` | `MI_Quantum_Arms_Afro` |
| 2 | `M_Quantum_Body` | `MI_Quantum_Body` | `MI_Quantum_Body_Afro` |

У full-body meshes порядок различается:

- `SKM_Quantum_Body_Full`: Head = 8, Arms = 9, Body = 10.
- `SKM_Quantum_Body_Full_Afro`: Head = 0, Arms = 1, Body = 2; остальные служебные face/eye/teeth slots идут после них.

Следовательно, production-код не должен полагаться на одинаковые числовые индексы между meshes. Надёжнее находить slots по именам `M_Quantum_Head`, `M_Quantum_Arms`, `M_Quantum_Body` и дополнительно проверять parent material.

## 5. Параметры, которые нельзя связывать со skin picker

На всех проверенных `MI_Quantum_Head`, `MI_Quantum_Body`, `MI_Quantum_Arms` и Afro variants доступны отдельные параметры:

- `Brightness` — scalar, default `1`;
- `Contrast` — scalar, default `1`;
- `Saturation` — scalar, default `1`.

Они входят в обработку BaseColor до `Color Correction` Lerp. Для обычного skin picker их менять нельзя: они меняют тональную характеристику и насыщенность независимо от выбранного RGB, усложняют согласование частей и повышают риск швов.

Также не относятся к выбору оттенка кожи:

- `Roughness Intensity`;
- `Roughness Contrast`;
- `Metallic Intensity`;
- `Normal Strength`.

Для skin picker допустимы только согласованный выбор material family European/Afro и, после визуальной валидации, пара `Color Correction` + `Color Correction Value`. `Brightness`, `Contrast`, `Saturation`, roughness, metallic и normal controls должны оставаться на штатных значениях соответствующего material instance.

## Итоговая рекомендация для Avariika

1. Считать European и Afro отдельными skin families и переключать Head/Body/Arms комплектом.
2. Не назначать один material на все части: сохранить родные `MI_Quantum_Head*`, `MI_Quantum_Body*`, `MI_Quantum_Arms*` и создавать MID для каждого используемого skin slot.
3. При экспериментальном tint передавать одинаковые `Color Correction` и `Color Correction Value` всем трём MID.
4. Искать skin slots по именам, а не по фиксированным индексам.
5. Не затрагивать eye, teeth, facial-detail, cartilage, glove и clothing slots.
6. Не экспонировать `Brightness`, `Contrast` и `Saturation` в skin picker.
7. До визуального теста считать произвольный RGB неподтверждённой функцией; безопасный первый этап — дискретные European/Afro presets.
