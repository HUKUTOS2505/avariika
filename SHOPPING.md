# АВАРИЙКА — ИТОГ: что качаю и ставлю

Сводка `DOWNLOADS.md` (unrealmonster) + `CRAWL_FINDINGS.md` (scrydy/cgdownload), приоритизировано, без дублей.
**Источник:** (М)=unrealmonster · (S)=scrydy · (C)=cgdownload · (оба) · (MP)=Marketplace/Fab-поиск.
**Куда:** ассет-паки → `Content/<Pack>/` · плагины → `Plugins/<Plugin>/`.
⚠️ Плейсхолдеры (пиратские) — лицензии к релизу. UE 5.7.

---

## 🥇 ТИР 1 — КАЧАТЬ ПЕРВЫМ (ядро под механики)

### 🖥 UI
- [ ] **Easy Game UI Ultimate** (М) — меню/пауза/настройки
- [ ] **Survival Horror UI Kit** (PSD) (М) — скин HUD
- [ ] **Hyper Mesh to Icon Creator** (М) — иконки слотов из мешей

### 🔊 Звук
- [ ] **Ghosthack × Boom Foley** (М) — двери/шаги/фоли
- [ ] **Sonomar: Abandoned Asylum** (М) — хоррор-эмбиент
- [ ] **Dark Russian Voices** (S) — голоса диспетчер/жильцы
- [ ] **Human Vocalizations** (MP) — вокалы игрока (крик/дыхание)

### 🔌 Плагины → `Plugins/`
- [ ] **Stage Zero** (М) — загрузка/шейдеры
- [ ] **NVIDIA DLSS** (М) — перф
- [ ] **AudioToolkit Pro** (М) — звук-менеджмент
- [ ] **Narrative 3** (М) — диспетчер/цели
- [ ] **Fire Chat PRO** (S) *или* Cross-Platform Voice Chat (М) — кооп-голос

### 🧰 Меши предметов (замена кубов)
- [ ] **Industrial Fuse Box** (М) — щиток
- [ ] **Garage Tools** (М) — инструменты
- [ ] **First Aid Set** (S) — аптечка
- [ ] **Bags & Survival Kit** (C) — снаряжение
- [ ] **Radio System & Interaction** (S) — рация
- [ ] **Fire Hydrant Package** (S)

### ✨ VFX хазардов
- [ ] **Toxic Gas Pack** (S) — газ
- [ ] **Ultimate Fire Pack** (оба) — огонь/«Горит»
- [ ] **Stylized Water & Liquid** (М) — потоп
- [ ] **Lightning & Electric** (М) — ток

### 🎨 Материалы/декали
- [ ] **Industrial Rust & Corrosion Vol.7** (М)
- [ ] **1000+ Mega Decal** (S)
- [ ] **Blood Decal Bundle** (М)
- [ ] **12 Burning Wood (анимир.)** (S)

---

## 🥈 ТИР 2 — стройка (твоя зона)
- [ ] **Interior Toolkit** (оба) ⭐
- [ ] ULAT · Blockout Tools · DrCG Assistant · Simple Spline Mesh (М)
- [ ] Локации: **Abandoned Hospital · Industrial Factory · Soviet Apartment Megapack · Modular Neighborhood** (оба)

## 🥉 ТИР 3 — персонажи (жильцы/бригада)
- [ ] **Rigged NPC Creator** (оба)
- [ ] Public Service NPC (М) · **Modular Human Skeleton** (S) · Pedestrians Vol 2 (C)
- [ ] Construction NPC Animations (М)

## 👹 МОНСТР — ПОТОМ (не качать без отмашки)
- [ ] Zombie-анимы+звук (105 Mocap+281 Audio) · Animations For Monsters · Monster Library (S/М)
- [ ] Wendigo · INFECTED/Horror bundles · Mutant Monster (оба)
- [ ] Плагины: NPC Eyes Sight PRO · Enhanced AI Movement · EnGore Dismemberment (М)

---

## ❌ НЕ качать (уже есть своё)
- **EOS** → у нас **EOSCore** (стоит + настроен; `DefaultPlatformService=Null`, включим перед онлайн-тестом)
- **Сейв** → AvariikaSaveGame · **Инвентарь** → свой C++ · **Камера** → своя (V)

## ℹ️ Куда ставить
- Ассет-паки распаковать в `Content/<PackName>/` (пути `/Game/<Pack>/` должны совпасть).
- Плагины — в `Plugins/<Plugin>/` (потом включить в .uproject / редакторе).
- `*_BuiltData` >100МБ — в .gitignore.

Детали с описаниями: `DOWNLOADS.md` (монстр) · `CRAWL_FINDINGS.md` (scrydy/cgdownload).
