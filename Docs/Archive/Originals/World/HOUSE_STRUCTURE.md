# HOUSE_STRUCTURE — ассеты дома (новые паки, 2026-06-16)

Старый `PostApocalypticHouse` удалён. Сейчас в проекте **2 пака реалистичных домов**:

| Пак | Путь | Ассетов | Роль |
|---|---|---|---|
| **ResidentialHouses** | `/Game/ResidentialHouses/` | 2010 | **Основа дома** — реалистичный пригород, готовые дома, мебель, кухня, электрика |
| **AtmosphericHouse** | `/Game/AtmosphericHouse/` | 1633 | **Резерв** — подвал-модули, свапер clean↔worn, worn-вид |

> Vol.1 и Vol.2 Residential оказались **идентичны** (дедуп: 2018 общих файлов, 0 уникальных) → импортирован один раз.
> Паки подхватятся при следующем запуске редактора (импорт был копией на диск, editor-free).

---

## 1. ResidentialHouses (`/Game/ResidentialHouses/`) — ОСНОВА

### Готовые дома (Blueprints/Houses) — главная ценность
- **`House01`…`House12`** — готовые дома-блюпринты (BP). Ставятся одним актором.
- **`Garage01`** — готовый гараж (BP).
- `Houses/NoInteriors/` — версии домов без интерьера (фон/оптимизация).
- `BP_SimpleDoor` + `ENUM_DoorState` — их дверь (но мы используем свой `ADoor`).
- `BP_PathSpline`, `BP_CableSpline`, `BP_TableFan` — вспомогательные.

### Архитектура (Meshes/Architecture) — для ручной сборки/правок
- `Floors`, `Roof`, `Stairs`, `Walls` (+ `Walls/Garage`, `Walls/PartitionWalls`, `Walls/Tiles`).
- `Walls/Garage/PartitionWalls/Stairs` — у гаража есть лестница (можно вести вниз).

### Аварийные точки (привязка к HOUSE_PLAN)
| Авария | Готовые ассеты в паке |
|---|---|
| 💧 **Вода** | `Meshes/Kitchen` (Stove/Fridge/**Dishwasher**, DrainingBoard, Cookware, CoffeeMachine), `Meshes/Bathroom` (раковина/туалет/ванна), краны |
| ⚡ **Электрика** | **`SM_ElectricBox`** + **`SM_ElectricMeter`** (+ MI/текстуры) — готовый щиток! Гараж под щитовую. |
| 🔥 **Газ** | прямого газового котла НЕТ ни в одном паке → плейсхолдер: котёл из props или meshy-модель позже |
| 🚪 Двери | свой `ADoor` (любой меш двери из пака) |

### Мебель (Meshes/*) — дом сразу жилой
ArmChair, Bed, BedroomFurniture, Bookshelf, Books, Couch, Dining, EntertainmentUnit, CoatRack, CeilingLamp, DeskLamp, FirePlace, Chimney, Carpets, Curtain, Cookware, FridgeMagnets, DVDPlayer и т.д. — мебель для всех комнат.

### Демо-карты (Maps) — образцы/источник домов
`ResidentialHouses.umap` (главная), `BlueprintHouses.umap` (дома-BP), `FurnitureAssets.umap`, `AssetZoo.umap` (витрина всех мешей). + сублевелы (BackgroundHouses/Landscape/Houses/Houses_Outside).

---

## 2. AtmosphericHouse (`/Game/AtmosphericHouse/`) — РЕЗЕРВ (подвал + worn)

### Зачем держим
1. **Подвал** — `SM_Basement_stairs_*`, `SM_Stairs_basement_steps`, `SM_Ceiling_stairs_basement_*`, `SM_WallIn_C_stairs_basement_*`, `Meshes/Meshes_props/Basement_props`. У Residential подвала нет → **котельную/насосную/щитовую берём отсюда**.
2. **Material Swapper** (`Blueprints/Blueprints_scripts`) — переключение всех материалов clean↔worn одной кнопкой (нужен Max Loop Iteration = 2 000 000 в Project Settings).

### Структура
- `Blueprints/` — Blueprints_building, _doors, _kitchen_cabinets, _lamps, _lights, _scripts (свапер), _additional.
- `Meshes/Meshes_building` (стены in/out/basement, полы, потолки 1x3..4x3 + broken-варианты, `Meshes_stairs`), `Meshes_doors`, `Meshes_props` (Basement/Bathroom/… props), `Meshes_decals`.
- `Materials/` — Clean / Worn / Tiled(Clean,Worn) / Master / Decals.
- `Maps/` — `House_clean_day`, `House_worn_night_baked`, `House_worn_night_lumen`, `Showcase`.

---

## 3. План сборки (как соберём дом)

**Подход: готовый дом Residential как основа + подвал из AtmosphericHouse + наши аварии.**

1. **База:** открыть `BlueprintHouses` / `ResidentialHouses` демо-карту, выбрать дом (`House0X`), у которого планировка ближе к нашему чертежу 1-го этажа (гараж + кухня + гостиная + санузел + кладовая + лестница). Гараж — `Garage01`.
2. **Перенести** выбранный дом в нашу рабочую карту (новую, `/Game/Avariika/Maps/`), компактный двор.
3. **Подвал:** пристроить из модулей AtmosphericHouse (basement stairs/walls/ceiling) под лестницей/в гараже → котельная (газ), насосная (вода), щитовая (электрика).
4. **Аварии (наши системы, пак-независимы):**
   - 💧 `Repairable_WaterPipe` у кухни/санузла/подвала (+ разлив/ток/сапоги).
   - ⚡ рубильник `APowerSwitch` у `SM_ElectricBox` в гараже/щитовой; проводка.
   - 🔥 газ-утечка в котельной (котёл-плейсхолдер).
   - 🚪 `ADoor` на дверные проёмы.
5. **Свет:** Lumen, тёмный сетап под хоррор. (Static lighting на 5.5.3/5.6 глючит — у нас 5.7+Lumen, ок.)

> Сборка идёт через ЖИВОЙ редактор+плагин с гардом (headless-pythonscript крашит на Niagara CDO). Запускаем, когда машина свободна — память: у пользователя виснет редактор.
