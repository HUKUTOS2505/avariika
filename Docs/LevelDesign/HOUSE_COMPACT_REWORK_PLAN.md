# HOUSE_COMPACT_REWORK_PLAN

Основа: `/Game/Avariika/Maps/L_Dom1`  
Будущая рабочая карта: `Lvl_House_Blockout_A`  
Цель: compact survival horror mission на 15-20 минут, максимально используя уже построенный `L_Dom1`.

## 1. Принцип

`L_Dom1` уже содержит реальный дом. План не строит новый случайный лабиринт. Нужно взять существующие garage, 1F shell, stair node, B1 shell и сжать их в управляемый маршрут.

- Значимых зон: 11.
- Один key item: `Basement Service Key`.
- Один repair node: `Basement Pump Breaker`.
- Один shortcut: B1 sewer/service door обратно в garage.
- Dungeon Architect пока не использовать.
- Не расширять дом за текущие bounds `(-1487,-2400,-363)` to `(5970,1200,800)`.

## 2. Сохраняемые anchors из `L_Dom1`

| Anchor | Coordinates | Использование |
|---|---:|---|
| Garage block | `x -1397..86`, `y -1464..906` | Сохранить как garage/exit wing. |
| Garage door | around `(-877,883,-95)` | Визуальная garage identity. |
| Garage entrance | `DoorEntrance_3 (-360,428,2)` | Использовать как start/entry near garage porch или shortcut target. |
| Main entrance / side entry | `DoorEntrance_4 (99,-1352,225)` | Использовать как playable house entry. |
| Stair node | `BP_Stairs_Master (57,-610,220)` | Основная вертикаль 1F -> 2F. |
| Garage steps | `SM_Garage_Stairs2 (-367,492,2)` | Локальный переход garage/porch. |
| 1F main shell | `x 400..5000`, `y -2200..900`, `z 220` | Основной маршрут 1F. |
| B1 technical shell | `x 3344..5700`, `y -2077..-115`, `z -363..199` | Подвал: boiler/pump/sewer. |
| B1 doors | `HouseDoor_B11..B19`, `z -82` | Гейты/shortcut doors. |

## 3. Итоговые 11 зон

| ID | Зона | Existing basis | Размер target | Роль |
|---:|---|---|---|---|
| 1 | Entry / Side Porch | `DoorEntrance_4` | `300 x 240`, h `300` | Стартовая точка входа в дом. |
| 2 | Main Hall | 1F shell near `x 400..1100` | `480 x 360`, h `300` | Ориентация, вид на лестницу. |
| 3 | Common Room | 1F shell near `x 1100..2100` | `560 x 420`, h `300` | Центральная жилая зона, первый tension hub. |
| 4 | Garage | existing garage | `~1450 x 2300`, h `350` | База, выход, target для shortcut. |
| 5 | Service Corridor + Locked Descent | 1F east corridor | `900 x 140-160`, h `260` | Закрытая дверь вниз до key item. |
| 6 | 2F Landing + Narrow Corridor | stair upper shell | `760 x 130-140`, h `260` | Короткая вертикальная вылазка. |
| 7 | Bedroom/Study | upper shell | `460 x 360`, h `260` | Один key item. |
| 8 | Basement Service Corridor | B1 shell | `900 x 130`, h `220` | Технический маршрут после unlock. |
| 9 | Boiler Room | B1 west/mid room | `430 x 360`, h `220` | Шум, бойлер, опасность. |
| 10 | Pump Room / Repair Node | B1 east room | `500 x 380`, h `220` | Один repair node. |
| 11 | Sewer Hub + Garage Shortcut | B1 far route | `520 x 420`, h `220` | Разворот и shortcut в garage. |

## 4. ASCII plan

Схема показывает игровой маршрут, не все существующие комнаты `L_Dom1`.

### 1F

```text
N
^
|  Existing GARAGE wing                         Existing MAIN HOUSE shell
|
|  +-----------------------+       +--------------------------------------+
|  | [4] GARAGE           |       | [5] SERVICE CORRIDOR 140-160w       |
|  | keep walls/roof/door |<--SC--| locked descent to B1                |
|  | DoorEntrance_3       |       +---------D----------------------------+
|  +----------D------------+                 |
|             |                              |
|             |       +----------------------+---------+
|             +-------+ [2] MAIN HALL                  |
|                     | stairs visible                 |
|                     +-----D---------------+----------+
|                           |               |
|                     +-----+---------------+----------+
|                     | [3] COMMON ROOM                |
|                     | route bends, no straight sight |
|                     +-----D--------------------------+
|                           |
|                     [1] ENTRY / DoorEntrance_4
+------------------------------------------------------------> E
```

### 2F

```text
N
^
|                 from existing stair node
|                          ^
|                +---------+----------------+
|                | [6] LANDING + CORRIDOR   |
|                | 130-140w, short only     |
|                +-----------D--------------+
|                            |
|                +-----------+--------------+
|                | [7] BEDROOM / STUDY      |
|                | Basement Service Key     |
|                +--------------------------+
+------------------------------------------------------------> E
```

### B1

```text
N
^
|             from locked descent
|                    v
|      +-------------D----------------------+
|      | [8] BASEMENT SERVICE CORRIDOR     |
|      | 130w, pipes, low ceiling          |
|      +------D------------------D---------+
|             |                  |
|      +------+-----+      +-----+---------+
|      | [9] BOILER |      | [10] PUMP     |
|      | ROOM       |      | REPAIR NODE   |
|      +------+-----+      +-----D---------+
|             |                  |
|             +---------D--------+
|                       |
|             +---------+--------+
|             | [11] SEWER HUB   |
|             | shortcut to      |
|             | garage opens here|
|             +---------D--------+
|                       |
|                 back to [4]
+------------------------------------------------------------> E
```

## 5. Маршрут игрока

1. Вход через `DoorEntrance_4` в entry.
2. Main Hall показывает лестницу и часть common room.
3. Игрок проходит common room и видит service corridor, но descent вниз locked.
4. Подъем на 2F через существующий stair node.
5. На 2F короткий landing/corridor ведет к bedroom/study.
6. В bedroom/study берется `Basement Service Key`.
7. Возврат на 1F, открытие locked descent.
8. B1 service corridor ведет к boiler room и pump room.
9. В pump room выполняется `Basement Pump Breaker`.
10. После ремонта открывается route в sewer hub.
11. Sewer hub открывает shortcut в garage.
12. Выход/возврат через garage.

## 6. Что сохранить и использовать

- Garage geometry: сохранить максимально без изменений.
- Stair node: сохранить, использовать как понятный вертикальный landmark.
- B1 shell: сохранить, но gameplay-route ограничить 4 зонами.
- Doors: использовать существующие `Door` actors как route gates.
- 1F walls/floors: сохранить большую часть, но не открывать все боковые комнаты.

## 7. Что конкретно достроить или переназначить

Не строить новый объем с нуля. Достройка должна быть компактной:

- Назначить `DoorEntrance_4` как mission entry.
- В 1F выбрать existing room cluster под `Main Hall` и `Common Room`; лишние двери обозначить как locked/blocked на плане.
- На 1F service corridor назначить конкретную дверь как `Locked Basement Door`.
- В 2F оставить только landing/corridor и одну bedroom/study room; не раздувать верхний этаж.
- В B1 выбрать три комнаты из existing lower shell:
  - boiler room;
  - pump room;
  - sewer hub.
- Сделать shortcut логически ведущим в garage wing, preferably через B1/LOW garage-adjacent shell.
- Расставить только один key item и один repair node.

## 8. Размеры и проходы

- 1F main route corridor: `150-160 cm`.
- 1F horror squeeze: `120-130 cm`, не длиннее `350 cm`.
- B1 service corridor: `130 cm`.
- Door clear width: `100-110 cm`; для coop лучше `110 cm`.
- Garage door is visual wide gate; playable pedestrian pass: `120-160 cm`.
- Stair width target: `130-140 cm`; existing stair node should be validated before final blockout.
- B1 ceiling: `220 cm`; 1F: `280-300 cm`; 2F: `250-270 cm`.

## 9. Tool split

### ULAT / Blockout Tools

- Route labels and blockers.
- Temporary walls for closing extra rooms.
- Door blockers/locked descent markers.
- Key item pedestal and repair node placeholder.
- Coop width validation blocks.

### Dungeon Architect

Do not configure now. Do not generate.

Future use only if approved:

- one hand-authored sewer hub room;
- one elbow/short connector;
- no procedural maze;
- no random seed route.

### Modular Sewers & Tunnels

- Sewer hub dressing.
- Shortcut tunnel/door from B1 to garage.
- Drainage pipe, grates, wet floor modules.

### Spline tools

- Pipes along B1 service corridor.
- Cable/wire route from pump room repair node.
- Visual breadcrumbs from locked descent to repair node.
- Not for room layout generation.

## 10. Automated build plan after approval

1. Duplicate or create separate `Lvl_House_Blockout_A`; do not work destructively in `L_Dom1`.
2. Copy relevant geometry/actors from `L_Dom1` by zone: garage, stair node, selected 1F shell, selected B1 shell.
3. Add route markers for the 11 zones.
4. Add blockers for non-route doors.
5. Add key item placeholder in bedroom/study.
6. Add one repair node placeholder in pump room.
7. Add shortcut door from sewer hub to garage.
8. Run read-only validation:
   - route exists from entry to garage exit;
   - corridor widths meet minimums;
   - exactly one key item;
   - exactly one repair node;
   - shortcut starts locked and opens from B1 side.
9. Do a player capsule walk test.
10. Only after approval, dress sewer hub with Modular Sewers & Tunnels.

## 11. Готовность к следующему шагу

План готов к созданию `Lvl_House_Blockout_A`. Основа `L_Dom1` пригодна: требуется не массовая генерация, а аккуратное выделение компактного маршрута внутри уже построенного дома.
