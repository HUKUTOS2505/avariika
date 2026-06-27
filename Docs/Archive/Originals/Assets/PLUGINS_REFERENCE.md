# PLUGINS_REFERENCE — плагины проекта «Аварийка»: что за что отвечает

> Составлено 2026-06-21 по `avariika.uproject` + манифестам `Plugins/*.uplugin` + движковым маркетплейс-плагинам.
> Engine 5.7.4. `Plugins/` — в `.gitignore` (локально, re-copy из RawAssets/FAB при свежем клоне).

---

## 1. Включённые в `avariika.uproject` (12)

### Встроенные UE (engine), просто включены
| Плагин | Зачем в проекте |
|---|---|
| **ModelingToolsEditorMode** | режим Modeling Tools в редакторе (правка мешей, blockout). Editor-only |
| **GameplayStateTree** | StateTree — логика поведения (AI/гейм-стейты); под будущего монстра/NPC |
| **OnlineSubsystem** + **OnlineSubsystemNull** + **OnlineSubsystemUtils** | каркас сетевого кооп; **Null** = локальный/LAN-бэкенд (когда EOS не активен) |
| **JsonBlueprintUtilities** | парсинг/сборка JSON в блупринтах |

### Сторонние / маркетплейс (ядро рабочего процесса)
| Плагин | Версия / автор | Зачем | Гочи / память |
|---|---|---|---|
| **Claudius** | v3.0/3.1 · Claudius Code | **AI-мост**: правлю UE из Claude JSON-командами (HTTP `127.0.0.1:8080`, ~230 команд, 26 категорий, кураторский индекс). Ядро автономной работы | ⚠️ порт **8080**, не 8765 (память `claudius-helper-script`); null-byte воркэраунд парс-бага. **ДУБЛЬ в движке** — см. §3 |
| **meshy** | v0.2.0 · wyn | **генерация 3D-моделей** (мебель/пропсы) через meshy.ai по тексту/картинке. «Генерация мебели» из твоего вопроса = это | Editor-only; модели генерит пользователь по промптам Claude (память `avariika-game-concept`) |
| **EOSCore** | FAB · af157f77 | **Epic Online Services** бэкенд для кооп (лобби/сессии/друзья) | ключи в `DefaultEngine.ini`; `DefaultPlatformService` пока Null; **ротировать Client Secret если репо откроют** (память `eos-plugins-batch`) |
| **WorldBLD** | v1.3.9 · WorldBLD LLC | генерация **города/дорог/окружения** (карта Завод/улица); 3 C++-модуля | ODR-фикс `GetCreditsValueFont` для сборки на 5.7.4 (память `worldbld-plugin`); локальный |
| **TickOptToolkit** | v1.5 · loonyware | **оптимизация тиков** актёров/компонентов по дистанции/видимости | установлен, **ещё не применён** — кандидаты в `OPTIMIZATION_REPORT.md` §5 |
| **Imperfecter** | v1.3.1 · Hubert Mika | пост-процесс «несовершенства камеры» (зерно/аберрация/виньетка) — **хоррор-атмосфера** | установлен, пресет ещё не настроен |
| **AudioToolkitPro** | v1.2 · UMonster | проф. редактирование аудио в редакторе | Editor-only |
| **UnrealMonsterAssetInstaller** | Editor · UMonster | инсталлер ассетов **монстра** | gated — **монстра не делать без отмашки** (память `project-status`) |

---

## 2. Лежат в `Plugins/`, но НЕ в списке `uproject` (AI-мосты — проверить статус)

| Плагин | Что | Замечание |
|---|---|---|
| **UnrealMCPServer57** | v2.0.2 · StraySpark · MCP-сервер UE5.7, **305 тулзов** по JSON-RPC/HTTP | `mcp__unreal__*` — живой канал (память `mcp-servers`). `EnabledByDefault:true` в манифесте → работает без записи в uproject |
| **NwiroIntegrationKit** | v1.0.3 · Nwiro (support Leartes) · **MCP-мост** Claude Code/Codex/Cursor/Windsurf ↔ UE5 | ещё один AI-мост; в uproject не прописан |

⚠️ **Итого 4 AI-моста**: Claudius (проектный) + Claudius (движковый дубль) + UnrealMCPServer + NwiroIntegrationKit.
Избыточно — стоит оставить 1–2 рабочих (Claudius на 8080 + UnrealMCP), остальные выключить, чтобы не плодить
порты/тики редактора. На решение пользователя.

---

## 3. ⚠️ Дубль Claudius в движке

В `C:\Program Files\Epic Games\UE_5.7\Engine\Plugins\Marketplace\Untitledca7e22d4545aV1\` установлен
**второй Claudius** (`Claudius.uplugin`, v3.1.0, «External Automation Framework»). То есть Claudius стоит
дважды: проектный `Plugins/Claudius` + движковый. Два экземпляра одного editor-плагина → возможный конфликт
(двойная регистрация команд/портов). Рекомендация: оставить ОДИН (проектный), движковый отключить/удалить.
Папка с безымянным GUID (`Untitledca7e22d4545aV1`) — артефакт установки с FAB без имени.

Движковые маркетплейс-плагины (для справки): `FabLauncher` (магазин FAB) + этот безымянный Claudius.

---

## 4. Скачано в `RawAssets/`, но НЕ установлено в проект

Тулкиты/паки на полке (вердикты — память `obshchee-asset-catalog`, WORKLOG):

| В RawAssets | Что | Под что |
|---|---|---|
| **Level Design Tools — DrCG Assistant** | Editor-виджет: Align/Distribute/Scatter/Array/Randomize | **расстановка мебели/объектов** (твоя ручная стройка) — НЕ установлен |
| **Hyper Procedural Spline Toolkit v4** | кабели/трубы по сплайну (5 ГБ) | провода/трубы дома (`ATMOSPHERE_PLAN.md` §3) |
| **Next Gen Destruction Toolkit** | разрушение мешей (Geometry Collection) | взрыв газа рушит стену (gated по EffectsQuality) |
| **Flashlight** | фонарь-в-руке | старт-инструмент → каска-апгрейд |
| **InteractionToolkit** | взаимодействие (референс) | образец для нашей E-механики |
| **Boss AI Toolkit 5.3** + **слухач** | AI босса + монстр-слухач | **монстр — отложен/gated** |
| **Luoss Particle Toolkit Vol 1** | VFX-партиклы | эффекты аварий (резерв) |
| **Multiplayer FPS Procedural Animation Toolkit 5.0** | проц-анимация рук/оружия FPS | апгрейд от 1-го лица (нужен ребилд, 5.0) |
| **Pro HUD Pack V2 / Survival_UI_Kit / Easy Game UI Ultimate** | HUD/UI-наборы | переход HUD на UMG (задумка WORKLOG) |
| **TranslationToolkit** | editor-перевод (4.27) | НЕ рантайм-локализация |
| **TOT57 / Imperfecter57 / WorldBLD_1.3.9_UE57** | исходники уже установленных | бэкап под re-copy |
| **[SCANS] Warehouse** | исходник пака Warehouse | re-copy фабрики |
| **звуки / anims_incoming** | сырые WAV / FBX-анимы | курация под механики |

> «Interior Toolkit» (спец. меблировка интерьеров ⭐) — в списках `СКАЧАТЬ/DOWNLOADS/SHOPPING.md`, но **ещё не скачан**.

---

## 5. Карта «что чем делаю»

- **Править UE из Claude** → Claudius (8080) / UnrealMCP (`mcp__unreal__*`).
- **Генерить мебель/пропсы** → meshy (по промпту) → импорт скриптом.
- **Расставлять объекты** → вручную DrCG-виджетом (после установки) ИЛИ скриптом `level.spawn_actor` (Claude).
- **Кооп-сеть** → EOSCore (+ OnlineSubsystem Null как фолбэк).
- **Окружение Завод/улица** → WorldBLD.
- **Атмосфера-камера** → Imperfecter; **перф** → TickOptToolkit.
- **Монстр** → UnrealMonsterAssetInstaller + Boss AI + слухач — **только по отмашке**.

_Связано: `mcp-servers`, `claudius-helper-script`, `eos-plugins-batch`, `worldbld-plugin`, `obshchee-asset-catalog`._
