# Аудио: что есть / кандидат / чего не хватает

Легенда статуса: ✅ ЕСТЬ — уже импортировано в `Content/` · 🟡 КАНДИДАТ — лежит в `RawAssets/`, нужен импорт · ❌ НЕТ — нет годного источника у нас.

Опорные импортированные наборы: `Content/Audio/SFX/` (12 кураторских стемов), `Content/Survival_SFX/{Movement,Survival,Craft,User_Interface}/`.

> Сводка по нашим пакам в `RawAssets/звуки/`: Ghosthack×BOOM Bundle (огромный — Garage-инструменты, Explosives, Fire/Water/Lightning foley, Whoosh/Body Fall, Hyperion-импакты, Weather), Sonomar Abandoned Asylum (эмбиент лечебницы — под больницу), FootstepSoundComponent (система шагов + бетон + Niagara-пыль), Survival SFX (уже импортирован), Rope Creak / Earthquake / HAZMAT (мелочь), и монстр-паки (SoundMorph Monster Within, Monster Sound FX, SFXtools Jumpscares, BOOM Dogs) — на будущее.

---

## 1. Движение (MOVEMENT)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Шаги ходьба по поверхности | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Movement/Walk_{grass,gravel,wood,stone}` (×4) | Есть grass/gravel/wood/stone. `stone`≈бетон. Нет **tile/metal** — добрать из FootstepSoundComponent. |
| Шаги бег | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Movement/Run_{grass,gravel,wood,stone}` (×4) | Есть и `Jog_` (средний темп). |
| Шаги краулинг/присед | ❌ НЕТ | — | Отдельного crouch-стема нет. Временно — тихий `Walk_` со сдвигом питча. |
| Прыжок (отталкивание) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Movement/Jump_{grass,gravel,stone,wood}` (×4) | — |
| Приземление (тяжёлый удар) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Body Fall` (15–39) | Бетон/кожа thud, прямое попадание. Альт — `Hyperion/Impacts`. |
| Скольжение по пене | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Movement/Slide_{stone,wood}` (×4) | Сухой слип-стем. Настоящий «мокрый» squelch пены — в шоппинг-лист. |
| Падение (споткнулся, тело) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Body Fall` (15) | Тот же набор, что и приземление; импортировать один раз. |
| Волочение раненого (скрежет+кряхт) | ❌ НЕТ | скрежет 🟡 Ghosthack `Ultimate Fighting Foley/Movement` (13, кожа/трение) | Скрежет покрываем, **кряхтение = вокал** → шоппинг-лист (мужской vocal foley). |

---

## 2. Витал (VITALS)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Сердцебиение (loop) | ✅ ЕСТЬ | `Content/Audio/SFX/Heartbeat` | — |
| Паник-вздох (gasp) | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Крик боли (ранение) | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Тяжёлое дыхание при ползании | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Бинтование/лечение | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/First_aid_*` (10) | Чистый foley бинта/аптечки. |
| Кряхтение при подъёме напарника | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Кашель | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Икота | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Запыхался (winded) | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Туалетный инцидент (комедийно, тихо) | ❌ НЕТ | жидкостной слой 🟡 Ghosthack `Magic Elements/Magic Water/Foleys` (glug) | Жидкость покрываема, реакция/«пшик» = вокал. |

> Витал — это в основном **дыра по мужскому вокалу** (подтверждается каталогами: ни в одном паке нет настоящих человеческих эффортов).

---

## 3. Предметы (ITEMS)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Подбор предмета (металл/пластик) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_pick_up` | Пластик-вариант не критичен. |
| Бросок/дроп предмета (thud) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_drop` | — |
| Тик переключения слота | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Button_hover` | — |
| Фонарик вкл/выкл (клик) | ✅ ЕСТЬ | `Content/Audio/SFX/FlashClick` | — |
| Фонарик сел: клик + испуг-вздох | ❌ НЕТ | клик ✅ `Content/Audio/SFX/FlashClick` | Сам клик есть; **испуг-вздох = вокал** → шоппинг-лист. |
| Рация вкл (клик + static-блип) | ✅ ЕСТЬ | `Content/Audio/SFX/RadioBlip` | — |
| Рация: фон статика (loop) | ❌ НЕТ | — | Нужен loop радиошума → шоппинг-лист (comms-пак). `RadioBlip` — только one-shot. |
| Зажигалка: щелчок+затяжка (каст) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Lighter_*` (5) | Щелчок+поджиг в стеме. |
| Кофе/термос: глоток+вздох (каст) | ✅ ЕСТЬ | `Content/Audio/SFX/DrinkGlug` | Глоток есть; финальный вздох — вокал (мелочь). |
| Вставка батарейки (клик, каст) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_pick_up` | Альт точнее — Ghosthack `Garage/Nut Driver Clatter`. |
| Огнетушитель: спрей под давлением (loop) | 🟡 КАНДИДАТ | Ghosthack `Steampunk Foley/Air Burst/Pneumatic Pump` (27); альт `Garage/Tire Pressure Control` (4) | Нет настоящего CO2-пшика → импорт пневматики или докупка (шоппинг-лист). |
| Бросок: зарядка (charge) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Whoosh` (30) | Замах/нарастание. |
| Бросок: свист (whoosh) | 🟡 КАНДИДАТ | Ghosthack `Ultimate Fighting Foley/Whoosh` (30) | Тот же набор. |
| Бросок: удар (impact) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Craft/Anvil_hit_*` / `User_Interface/Metal_item_drop` | Альт — Ghosthack `Hyperion/Impacts`. |
| Ловушка: установка (клик) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Trap_place_*` (5) | — |
| Ловушка: взвод (beep) | ❌ НЕТ | — | Электронный бип → синтез/UI-пак. Времянка — `Button_press`. |
| Ловушка: срабатывание (snap) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Trap_snap_*` (5) | — |
| Прожектор-тренога: клац + гул лампы | 🟡 КАНДИДАТ | гул: Ghosthack `Magic Lightning/Foleys` (Fluorescent Lamp/Low Hum); клац: импорт. `Metal_item_drop` | Гул лампы — импортировать electric foley. |
| Био-блоб: бросок + шлепок | 🟡 КАНДИДАТ | splat: Ghosthack `Magic Water/Foleys` (Impact Splash); throw: `Whoosh` | Будущая механика; собрать слоями. |

---

## 4. Починка (REPAIR)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Вставка расходника (метал. клик) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Metal_item_pick_up` | Альт — Ghosthack `Garage/Nut Driver Clatter`. |
| Заправка топлива: глюк/налив (loop) | 🟡 КАНДИДАТ | Ghosthack `Magic Elements/Magic Water/Foleys` (Bubble Bottle Fill Glug, 15) | Времянка — `Content/Audio/SFX/DrinkGlug` (one-shot). Нужен именно loop-залив. |
| Кабель: разматывание (loop) | ❌ НЕТ | прибл. Survival_SFX `Content/Survival_SFX/Craft/Crafting_cloth_item_*` (шуршание) | Настоящего unspool нет → шоппинг-лист. |
| Тестер: контакт (beep, hit) | ❌ НЕТ | прибл. `Content/Survival_SFX/User_Interface/Button_press` | Электронный бип → синтез/UI-пак. |
| Тестер: ошибка (buzz, miss) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Error` | Прямое попадание. |
| Вентиль: трещотка (на оборот) | 🟡 КАНДИДАТ | Ghosthack `Steampunk Foley/Garage/Impact Wrench Screw` (15) | Годного импортированного нет. |
| Вентиль: срыв резьбы + шипение | 🟡 КАНДИДАТ | Ghosthack `Garage/Tire Pressure Control` (deflate, 4) | Слой шипения можно добрать из `Content/Audio/SFX/GasHiss`. |
| Генератор: натяг шнура (loop) | 🟡 КАНДИДАТ | Ghosthack `Steampunk Foley/Garage/Hydraulic Lift` (8, мех. натяг) | Идеала (pull-cord) нет → шоппинг-лист. |
| Генератор: захват шнура (snap) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Survival/Trap_snap_*` | Резкий щелчок-защёлка. |
| Обратная вспышка (dull thud) | 🟡 КАНДИДАТ | Ghosthack `Explosives/Explosion Indoor` (12, тихий) / `Body Fall` | Глухой хлоп. |
| Двигатель: запуск (1x) | ✅ ЕСТЬ | `Content/Audio/SFX/EngineStart` | — |
| Двигатель: холостой ход (loop, весь забег) | ❌ НЕТ | прибл. Ghosthack `Garage/Electric Drill Idle` (9, мотор-гул) | **EngineStart — только запуск.** Нужен idle-loop двигателя → шоппинг-лист. |
| Сварка: дуга, гул/треск (loop) | ✅ ЕСТЬ | `Content/Audio/SFX/WeldBuzz` | — |
| Починка завершена (chime) | ✅ ЕСТЬ | `Content/Audio/SFX/RepairDone` | Альт — UI `Save`. |
| Костыль-сборка: возня (loop) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Craft/Building_item_remove_*` / `Crafting_wood_item_*` | Альт точнее — Ghosthack `Garage/Tire Changing Machine` (12). |
| Костыль: провал (clank) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Craft/Anvil_hit_*` / `User_Interface/Metal_item_drop` | — |
| Электро-замыкание/zap | ✅ ЕСТЬ | `Content/Audio/SFX/ElectricZap` | — |

---

## 5. Опасности (HAZARDS)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Утечка газа: ровное шипение (loop, 3D) | ✅ ЕСТЬ | `Content/Audio/SFX/GasHiss` | ⚠️ Вероятно слоёный плейсхолдер (чистого газ-хисса в RawAssets нет) — проверить качество, при релизе докупить настоящий. |
| Газовое облако: усиленное шипение | ✅ ЕСТЬ (прибл.) | `Content/Audio/SFX/GasHiss` (питч/слой) | Альт — Ghosthack `Garage/Tire Pressure Control`. |
| Газовый ВЗРЫВ (boom+rumble+debris) | ✅ ЕСТЬ | `Content/Audio/SFX/Explosion` | Для разнообразия — Ghosthack `Explosives/Explosion Gas` (40). |
| Пожар после взрыва (crackle, loop) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/Survival/Campfire_*` (5) | Альт — Ghosthack `Medieval.../Fire` (9). |
| Перегруз сети: хлопок реле + спад гула | 🟡 КАНДИДАТ | `Content/Audio/SFX/ElectricZap` + Ghosthack `Magic Lightning/Foleys` (Low Hum) | Собрать слоями; хлопок реле — из металл-клика. |
| Восстановление питания: гул + свет | 🟡 КАНДИДАТ | Ghosthack `Magic Lightning/Foleys` (Low Hum Sequence, Fluorescent Lamp) | Импортировать electric foley. |
| Хоррор-скрипы/стуки/капли (рандом) | 🟡 КАНДИДАТ | Sonomar `Abandoned Asylum/Ambience - Room Tone/...Basement_Water_Drips` (3) | Скрипы — Ghosthack `Fire`/дерево; импорт Sonomar. |
| Капель (ambient drip) | 🟡 КАНДИДАТ | Sonomar `...Basement_Water_Drips_SC-AA_026` (3) | Альт — Ghosthack `Magic Water` drip. |

---

## 6. Диспетчер / соц (DISPATCHER/SOCIAL)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Одиночный бип рации | ✅ ЕСТЬ | `Content/Audio/SFX/RadioBlip` | — |
| Паник-скрим игрока | ❌ НЕТ | — | Мужской вокал. Шоппинг-лист. |
| Толчок напарника: thud + кряхт | ❌ НЕТ | thud ✅ Survival_SFX `Content/Survival_SFX/Survival/Punch_*` (5) | Удар есть; **кряхтение = вокал**. |
| Оператор: тумблер монитора/экран | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/User_Interface/Button_press` | Альт — `FlashClick`. |

---

## 7. UI

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Наведение (hover tick) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Button_hover` | — |
| Клик кнопки | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Button_press_*` (2) | — |
| Открытие/закрытие меню (swoosh) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/{Open,Close}_menu_*` | — |
| Покупка (ka-ching) | ❌ НЕТ | прибл. `Content/Survival_SFX/User_Interface/Save` | Кассы нет → UI/casino-пак. |
| Не хватает денег (error buzz) | ✅ ЕСТЬ | Survival_SFX `Content/Survival_SFX/User_Interface/Error` | — |
| Старт забега (sting) | 🟡 КАНДИДАТ | Ghosthack `Hyperion - Trailer Hits & Atmospheres/Impacts` (72) | Импорт или короткий муз-стинг-пак. |
| Победа (sting) | 🟡 КАНДИДАТ | Ghosthack `Hyperion/Impacts` | Времянка — `RepairDone`. |
| Поражение (sting) | 🟡 КАНДИДАТ | Ghosthack `Hyperion/Drones & Atmospheres` (20) | — |
| Акт о работе (шуршание бумаги) | ✅ ЕСТЬ (прибл.) | Survival_SFX `Content/Survival_SFX/Craft/Crafting_cloth_item_*` | Ткань вместо бумаги; настоящий paper rustle — мелочь в шоппинг-лист. |

---

## 8. Эмбиент (AMBIENT)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Фоновый дрон уровня (больница/котельная, вент) | ✅ ЕСТЬ | `Content/Audio/SFX/Ambient_Boiler` | Для палат/коридоров добрать Sonomar `Large Hall` / `Long Corridor` (🟡). |
| Ночной ветер | 🟡 КАНДИДАТ | Ghosthack `Medieval.../Wind` (9) | — |
| Дождь + гром | 🟡 КАНДИДАТ | Ghosthack `Medieval.../Weather` (35–44) | Medium/Strong Rain, Thunder Distant/Close. |
| Электрический гул | 🟡 КАНДИДАТ | Ghosthack `Magic Lightning/Foleys` (Low Hum, Fluorescent Lamp) | Частично покрывает `Ambient_Boiler`. |

---

## 9. Монстр — будущее (MONSTER, только каталог)

| Событие | Статус | Источник (pack + path/stem, variants) | Заметка |
|---|---|---|---|
| Тяжёлые шаги | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Footsteps` (272) | Импорт только при старте работ по монстру. |
| Дыхание/рык | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Breathing` (55) | Альт-орг. — BOOM `DOGS.WAV/Growl` (обработать). |
| Стинг «заметил» (alerted) | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Reaction` (156) | — |
| Джампскейр-скрим | 🟡 КАНДИДАТ | `SFXtools Jumpscares` (41, JS_Jumpscare_*) | Готовые стинги. |
| Музыка погони | 🟡 КАНДИДАТ | SoundMorph `Monster Within/_(WAVs)_Charge` (34) | Передаёт преследование, но это **не муз-трек** → отдельная музыка позже. |

---

## ❌ Чего не хватает (купить/сгенерить)

**A. Мужской вокальный foley (самая большая дыра — у нас нет ни одного человеческого эффорта):**
паник-вздох, крик боли, тяжёлое дыхание при ползании, кряхтение при подъёме, кашель, икота, «запыхался», туалетная реакция, испуг-вздох (севший фонарик), вздох после кофе, кряхтение при волочении тела и толчке напарника, паник-скрим игрока.
→ Купить **male vocal efforts / breaths / pain pack** (напр. «Male Character Voices: Efforts, Pain, Breathing» на Fab/asset-store) **или записать своего актёра** — самый дешёвый и качественный путь для коопа.

**B. Настоящий газ/давление/двигатель (индустриальные loop'ы):**
огнетушитель CO2-спрей (loop), **проверить/заменить `GasHiss`** на настоящее газовое шипение под давлением, **холостой ход двигателя (loop на весь забег)**, разматывание кабеля (loop), трещотка вентиля.
→ Купить **automotive/garage + pressurized gas & steam pack** и **engine idle loop pack** (часть закрывается импортом Ghosthack Garage — см. ниже, но idle двигателя и CO2 лучше докупить отдельно).

**C. Электроника / UI-стинги / радио:**
покупка (ka-ching), взвод ловушки (beep), контакт тестера (beep), фон радиошума (loop), стинги старт/победа/поражение.
→ **UI/casino SFX pack** (ka-ching, бипы) + **comms/radio static pack** (loop рации) + короткий **musical sting pack** (старт/победа/поражение можно временно собрать из Ghosthack Hyperion).

**D. Мелочь:**
crouch-шаги (можно из Walk питчем), «мокрый» squelch скольжения по пене, шуршание бумаги (акт).
→ Низкий приоритет, закрыть импровизацией из имеющихся стемов.

---

## 📦 Импортировать в проект (приоритет)

Конвенция: разовые SFX → `Content/Audio/SFX/<Группа>/`, эмбиенты → `Content/Audio/Ambient/<Группа>/`. Импорт = robocopy дерева пака в целевую папку (см. memory: asset import method), либо headless `UnrealEditor-Cmd`.

1. **Sonomar Abandoned Asylum** (эмбиент + капель + котельная + подвал-шаги) → `Content/Audio/Ambient/Asylum/`.
   Закрывает: фоновый дрон палат/коридоров, капель, хоррор-фон, опасный газ/мех-фон. Высший приоритет — атмосфера уровня.
2. **FootstepSoundComponent** (бетон + Blueprint-логика поверхностей + Niagara-пыль) → `Content/FootstepSoundComponent/` (сохранить пути `/Game/FootstepSoundComponent/`).
   Закрывает: бетонные шаги + переиспользуемая детекция поверхностей под недостающие **tile/metal**, и пыль-VFX.
3. **Ghosthack Steampunk Garage** (Impact Wrench, Tire Pressure Control, Hydraulic Lift, Pneumatic Pump, Electric Drill Idle, Tire Changing Machine, Nut Driver Clatter) → `Content/Audio/SFX/Garage/`.
   Закрывает: трещотка/срыв вентиля, шнур генератора, пневмо-спрей огнетушителя, возня костыля, мотор-гул (времянка под idle).
4. **Ghosthack Magic Fire/Water/Lightning Foleys** (реалистичные слои, игнор спелл-вушей) → `Content/Audio/SFX/Elemental/`.
   Закрывает: огонь после взрыва, налив топлива (glug-loop), капель, электро-гул/zap/флуоресцент, восстановление питания.
5. **Ghosthack Whoosh + Body Fall + Hyperion Impacts** → `Content/Audio/SFX/Foley/`.
   Закрывает: бросок (charge/whoosh/impact), приземление/падение, UI-стинги старт/победа/поражение.
6. **Ghosthack Medieval Weather + Wind** → `Content/Audio/Ambient/Weather/`.
   Закрывает: дождь+гром, ночной ветер.
7. **(Будущее, по отмашке)** SoundMorph Monster Within + SFXtools Jumpscares → `Content/Audio/Monster/`.
   Только когда стартуют работы по монстру (см. project-status: монстра не делать без отмашки).
