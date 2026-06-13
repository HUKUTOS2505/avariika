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

## Как присылать модели
Экспортируй из meshy FBX с текстурами, положи в `RawAssets/SM_<Имя>/` (точное имя из заголовков выше, напр. `SM_Thermos`). Скажи «импортируй» — прогоню скрипт (без metallic-карты), назначу на актор/предмет и его инстансы, отмасштабирую.
