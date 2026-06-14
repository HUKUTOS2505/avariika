# Модели «Аварийки» — статус и промты для meshy.ai

Сгенерировано аудитом 2026-06-13. **Сеттинг — СОВРЕМЕННЫЙ, не советский.**

## Арт-бриф (общий стиль для всех промтов)

Современная (~2020-е) рабочая снаряга аварийно-ремонтной бригады: реалистичные, бренд-нейтральные предметы, заметно б/у, но НЕ антиквариат. Никаких советских/ретро/винтажных маркеров. Литой ABS-пластик, порошковая окраска и матовая краска по стали, резиновые накладки-грипсы, шлифованный (НЕ зеркальный) металл. Износ лёгкий-средний — потёртости, пыль, отпечатки, выцветшие наклейки. Палитра: safety-жёлтый и hi-vis оранжевый как акценты, графит/тёмно-серый, грязно-белый; красный — для аварийных (огнетушитель/аптечка).

**КРИТИЧНО для meshy:** упирать на МАТОВЫЙ/сатиновый пластик и порошковое покрытие, высокий roughness; избегать слов *chrome / polished / metallic / glossy / mirror* — иначе карты выбивают зеркальные пересветы в UE (на этом уже обожглись с огнетушителем). При импорте я всё равно отключаю metallic-карту.

---

## Статус всех моделей

### ✅ Готово (меш импортирован и стоит на месте)
Toilet, WeldingMachine, FireExtinguisher, FirstAidKit, Radio, Tester, Battery, Breaker, GasPipe, Generator, **Fuse** (новый, современный), **Cigarettes** (новый).

### 🚗 Импортирую из загруженного
**Hilux** (Toyota, заменяет газель) — `RawAssets/SM_Gazelle/source/.../hilux.fbx`, многоматериальный, в работе.

### 🔴 Нужны модели (заглушки на карте) — ПРИОРИТЕТ
Thermos, TrapKit, LightKit, MotionSensor.

### 🔴 Нужны модели (спавнятся в игре, сейчас куб/программно)
BioBlob (метаемый комок), Floodlight (развёрнутый прожектор), Trap (развёрнутая растяжка — можно переиспользовать TrapKit). FoamPatch (лужа пены) — делаю **декалью в UE**, meshy не нужен.

### 🟡 Опционально перегенерить (модель есть, но промт был «советский»)
Breaker, GasPipe, Generator, Battery — работают, но если хочешь единый современный стиль — промты ниже.

---

## ПРИОРИТЕТ A — нужны модели (заглушки на карте)

### SM_Thermos — термос/кофе (восстанавливает выносливость), ~25 см
```
Modern present-day stainless steel travel thermos flask, slim cylinder about 22cm tall, matte powder-coated dark teal painted body with a black screw-on cup lid that doubles as a drinking mug, satin brushed steel base ring and screw threads with high roughness and no mirror reflection, subtle silicone grip band, light-to-medium wear with fine scuffs, dust, fingerprints, a faded peeling label and a few small scratches, brand-neutral realistic used municipal worker gear, realistic proportions, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_TrapKit — носимая растяжка-шумелка, ~18 см
```
A modern compact tripwire noise-maker trap kit roughly 15 cm, a small handheld can-sized device: a matte black ABS plastic case with a safety-yellow hi-vis front panel and rounded rubber bumper pads on the corners, a coiled spool of thin steel tripwire mounted on one side with a tiny tension hook, a recessed trigger pin and a small spring-loaded clacker mechanism, a faded warning sticker and printed icons on the casing, slightly worn matte painted plastic with light scuffs, dust, fingerprints and a few faint scratches, brushed non-reflective dark metal hardware, graphite and dirty-white accents, realistic everyday proportions, contemporary present-day worn work gear, high roughness matte surfaces, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_LightKit — переносной складной LED-прожектор (носимый), ~28 см
```
A modern compact folding LED work light kit, about 25 cm folded for carrying, rectangular housing of matte safety-yellow painted ABS plastic with graphite-black accents, a brushed dark-grey metal carry handle on top, folded-up adjustable tripod legs clipped against the body, a wide rectangular non-reflective frosted LED lens panel with a textured matte bezel, rubberized grip pads, a small recessed power switch and a coiled cable tucked along one side, light realistic wear with dust, fingerprints, faint scuffs and a couple of faded warning stickers, satin painted surfaces and powder-coated metal at high roughness, realistic proportions, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_MotionSensor — датчик движения, ~16 см
```
Modern present-day PIR motion detector, roughly 18 cm tall, a rounded off-white and light-grey molded ABS plastic housing with a smooth matte semi-translucent white plastic lens dome on the front, a tiny recessed LED indicator dot below the dome, mounted on a short slim grey plastic neck rising from a small flat circular magnetic base plate, satin matte painted plastic surfaces with high roughness, neutral dirty-white and graphite-grey color scheme, light wear with faint dust, fingerprints and a small faded warning sticker, no scuffs heavier than light scratches, realistic proportions, clean contemporary contractor security device, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

---

## ПРИОРИТЕТ B — спавнятся в игре

### SM_BioBlob — метаемый «био-снаряд» (пикап + снаряд, один меш), ~12 см
```
A small amorphous organic blob roughly 12 cm across, a suspicious lumpy gob of muck with no defined shape, irregular sagging bulges and a few drip-like protrusions, murky brownish-green sludgy organic matter, wet matte semi-translucent surface with soft subsurface scattering and a faint inner glow, NOT metallic and not glossy, high roughness slimy skin with subtle sticky sheen, dirty and grimy with darker grime in the creases, flecks of debris and grit embedded in the mass, slightly uneven asymmetric silhouette, realistic proportions, single mesh usable as both a hand-held pickup and a thrown projectile, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Floodlight — развёрнутый прожектор на треноге, ~130 см
```
A modern deployed work floodlight on a tripod, about 130 cm tall, contemporary 2020s municipal contractor gear, a rectangular LED floodlight head mounted on an adjustable tilting bracket atop a folding three-leg tripod stand, head housing in matte safety-yellow powder-coated steel with a dark graphite painted frame and rubber-capped feet, a large flat emissive light-panel lens with a soft glowing white-blue surface behind a matte plastic bezel, a coiled black rubber power cable with a molded plug, satin painted metal tripod legs with brushed non-reflective finish, faded warning sticker and small control switch on the back of the head, light to moderate wear with scuffs, fine scratches, fingerprints, jobsite dust and a little grime, realistic proportions, matte painted surfaces and high roughness, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Trap — развёрнутая растяжка на полу, ~20 см *(или переиспользовать SM_TrapKit)*
```
A modern deployed tripwire trap, a small 20cm matte graphite and dirty-white painted ABS plastic box-style canister sitting flat on the floor, a taut thin steel trip wire stretched out from one side hooked to a small ground anchor peg, a single small glowing LED indicator lens emitting a faint red light on the top face, a hi-vis yellow warning stripe and a faded curling caution sticker, satin powder-coated steel clips and a rubber base pad, light-to-medium wear with dust, smudged fingerprints, scuffs and a few faint scratches, realistic proportions, used contractor field equipment look, matte non-reflective surfaces high roughness no chrome, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

> **SM_FoamPatch** (лужа пены из огнетушителя) — НЕ через meshy, сделаю плоской декалью/материалом в UE.

---

## ОПЦИОНАЛЬНО C — перегенерить старые «советские» в современном стиле

### SM_Breaker — электрощиток (~160 см)
```
A modern present-day wall-mounted electrical breaker panel about 160 cm tall, a molded gray-white matte plastic enclosure with a thin satin powder-coated steel hinged door, neat rows of modular miniature circuit breakers with small toggle switches and tidy color-coded wiring inside, faint graphite-gray labels and a couple of faded warning stickers, all surfaces matte painted plastic and powder-coated brushed steel with high roughness and no shine, light-to-medium wear with dust, fingerprints, scuffs and a few faint scratches, realistic contemporary proportions, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_GasPipe — газовая труба с вентилем (~230 см)
```
A modern present-day vertical gas riser pipe with a shut-off valve, about 230 cm tall, a straight steel pipe with a flanged base bracket and a side-mounted shut-off valve handwheel; matte safety-yellow powder-coated painted steel pipe body, a matte deep-red painted round valve handwheel, a small brushed brass and matte-grey pressure gauge with a glass dial, bolted flange joints, threaded couplings and a printed yellow GAS warning label and stenciled markings; clean utilitarian municipal contractor equipment, light-to-medium wear with dust, faint fingerprints, a few scuffs and a thin hairline crack near a joint suggesting a small leak, satin high-roughness painted surfaces with no shiny or mirrored finish, realistic proportions, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Generator — дизель-генератор (~200 см)
```
Modern portable diesel generator set roughly 200 cm long mounted on a matte dark-graphite welded steel frame, rounded cast safety-orange powder-coated plastic-and-steel housing panels with satin painted surfaces, an integrated control panel featuring analog gauges and a key switch, a recessed grey fuel tank cap, side air vents and a small exhaust outlet, brand-neutral faded warning stickers, moderate used wear with dust, fingerprints, light scuffs and a few faint oil streaks near the fuel cap, scratches on the frame edges, all paint matte high-roughness non-reflective, realistic proportions, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Battery — батарея фонаря (~18 см)
```
A modern compact 18 cm rechargeable headlamp battery pack, rectangular molded ABS plastic housing in matte black with safety-yellow accent end caps, a small printed product label sticker on the front, two short spring contact terminals on top, all surfaces matte painted plastic with high roughness and no metallic sheen, lightly used with faint scuffs, dust, fingerprints and a slightly faded label, realistic proportions, brand-neutral present-day equipment, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

---

## v2 — ЧИСТЫЕ промты (перегенерация шумных моделей)

Диагностика 2026-06-13: текстуры привязаны верно, НО meshy сделал их шумными/пятнистыми из-за перегруза описанием износа (scuffs/dust/stickers/scratches/grime). **Новое правило: МИНИМУМ износа, упор на чистые сплошные цвета, гладкие читаемые поверхности, «studio product render».** Хвост прежний.

### SM_Cigarettes — ПРОСТАЯ пачка (а не пачка+20 сигарет)
```
A modern brand-neutral cigarette soft pack, single small object about 9 cm tall, closed crisp rectangular paper-and-foil pack standing upright with one tan filter cigarette poking out of the top, clean matte off-white and dark graphite packaging with a simple generic warning band, smooth readable surfaces, solid clean colors, minimal wear, studio product render, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Radio
```
A modern handheld walkie-talkie radio, single clean object about 22 cm tall, matte black body with a few solid safety-orange accent panels, a short stubby antenna, a round speaker grille, two control knobs on top and a side push-to-talk button, smooth clean readable surfaces with solid colors and crisp details, minimal wear, studio product render quality, matte plastic, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_WeldingMachine
```
A modern portable inverter welding machine, single clean object about 45 cm wide, a rectangular matte blue steel case with a carry handle on top, a clean front control panel with two round dials and an amperage display, two cable sockets, a coiled welding cable with electrode holder, solid clean colors and smooth readable surfaces, minimal wear, studio product render, matte painted metal, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_MotionSensor
```
A modern PIR motion sensor, single clean object about 16 cm tall, smooth matte white ABS plastic housing with a clean semi-translucent white lens dome on the front and one small LED indicator, mounted on a short neck and a small round base, solid clean white-grey colors, smooth surfaces, minimal wear, studio product render, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_FirstAidKit
```
A modern first aid kit, single clean object about 28 cm wide, a smooth matte white hard plastic case with a bold solid red cross on the lid, two clasp latches and a carry handle, clean crisp solid colors and smooth readable surfaces, minimal wear, studio product render, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_TrapKit
```
A modern compact tripwire alarm device, single clean object about 18 cm, a smooth matte black and safety-yellow ABS plastic case with a small coiled tripwire spool on one side, a clean trigger button and a tiny LED, solid clean colors, smooth readable surfaces, minimal wear, studio product render, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Thermos
```
A modern stainless steel travel thermos, single clean object about 25 cm tall, a smooth slim cylinder with a matte dark-teal powder-coated body and a black screw-on cup lid, a brushed steel base, clean solid colors and smooth surfaces, minimal wear, studio product render, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

### SM_Fuse
```
A modern blade cartridge fuse, single clean object about 6 cm, a smooth matte colored translucent plastic body with a clear amperage number printed on it and two flat metal blade contacts, clean solid colors, smooth readable surfaces, minimal wear, studio product render, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene
```

**Совет по meshy:** генерируй в максимальном качестве, на шаге текстур выбирай вариант с чистой читаемой картой (не «камуфляжный»). Не добавляй в промт «weathered/grimy/heavily worn» — именно это даёт шумные пятна.

## Персонажи: операторы (бригада) + монстры

**Общий хвост для персонажей** (вместо «single object…»): `full body humanoid character in a relaxed T-pose, arms out to the sides, symmetrical, game-ready character, clean topology, PBR textures, single character, centered, plain background, no ground plane, no scene`.
**Совет:** если в meshy есть авто-риг (Humanoid) — включай, сэкономит риггинг; иначе экспортируй в T-позе под ручной риг на UE5 Mannequin. «gore/blood/decay» — только для монстров, не для операторов.

### SK_Operator_01 — монтёр аварийной бригады (база)
```
A modern emergency utility repair worker, adult man, wearing a clean orange high-visibility coverall with reflective grey stripes, a white hard hat with a small headlamp on the front, sturdy work gloves, a tool belt with pouches, black work boots, a simple respirator mask hanging around the neck, clean solid readable colors, matte fabric, minimal wear, realistic proportions, full body humanoid character in a relaxed T-pose, arms out to the sides, symmetrical, game-ready character, clean topology, PBR textures, single character, centered, plain background, no ground plane, no scene
```

### SK_Operator_02 — монтёр (вариант, для разнообразия бригады)
```
A modern maintenance technician, adult man, wearing a dark navy-blue work coverall with a hi-vis yellow vest over it, a grey beanie instead of a helmet, a headlamp strap on the forehead, fingerless gloves, knee pads, heavy boots, a coiled cable over one shoulder, clean solid readable colors, matte fabric, slightly stocky build, realistic proportions, full body humanoid character in a relaxed T-pose, arms out to the sides, symmetrical, game-ready character, clean topology, PBR textures, single character, centered, plain background, no ground plane, no scene
```

### SK_Operator_03 — монтёр-женщина (вариант)
```
A modern emergency repair technician, adult woman, wearing a grey-and-teal work coverall with reflective stripes, sleeves rolled up, a hard hat with headlamp over a tied-back ponytail, work gloves, a tool belt, sturdy boots, clean solid readable colors, matte fabric, athletic realistic proportions, full body humanoid character in a relaxed T-pose, arms out to the sides, symmetrical, game-ready character, clean topology, PBR textures, single character, centered, plain background, no ground plane, no scene
```

### SK_Monster_Listener — монстр-слухач (охотится на звук; в геймплее ОТЛОЖЕН, модель делать можно)
```
A tall gaunt humanoid horror creature that hunts by sound, completely eyeless smooth face, enormous oversized human-like ears and exposed sensory flaps on the sides of the head, long thin limbs, hunched posture, pale grey veined skin stretched over the ribs, sharp slender fingers, no clothing, unsettling but not gory, dark muted colors, matte skin, realistic creature anatomy, full body humanoid character in a relaxed T-pose, arms out to the sides, symmetrical, game-ready character, clean topology, PBR textures, single character, centered, plain background, no ground plane, no scene
```

### SK_Monster_Patient — монстр-«пациент» (медицинский хоррор, вариант)
```
A disturbing humanoid horror creature themed as a former hospital patient, emaciated body in a tattered dirty hospital gown, head wrapped in stained bandages with no visible eyes, elongated jaw, twisted long limbs, pale sickly mottled skin, an IV tube still attached to one arm, unsettling but restrained without heavy gore, dark muted desaturated colors, matte surfaces, realistic creature anatomy, full body humanoid character in a relaxed T-pose, arms out to the sides, symmetrical, game-ready character, clean topology, PBR textures, single character, centered, plain background, no ground plane, no scene
```

## Как присылать модели
Экспортируй из meshy FBX с текстурами, положи в `RawAssets/SM_<Имя>/` (пропсы) или `RawAssets/SK_<Имя>/` (персонажи/монстры). Скажи «импортируй» — прогоню скрипт (без metallic-карты), назначу/отмасштабирую. Для персонажей риг под UE5 Mannequin делаешь ты (или meshy auto-rig), потом подключим AnimBP.
