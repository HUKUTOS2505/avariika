# Разбор импортированных паков — что брать, что нет

Параллельный аудит (2026-06-14): отдельные агенты прочитали структуру/зависимости каждого пака и оценили применимость к Аварийке. Каталог самих паков — см. [ASSETS.md](../Assets/ASSETS.md).

## ГЛАВНОЕ: динамическая погода (HyperDynamicWeatherSky)

**Вердикт: фреймворк целиком — SKIP. Cherry-pick только VFX грозы/дождя + статичное ночное небо.**

### Стоит ли брать саму «динамическую погоду»?
Нет. Это система для open-world/survival: погода, биомы, сезоны, часовые пояса. В закрытой больнице/доме всё это почти не видно, а нашу ночь+туман+свет↔паника мы УЖЕ сделали. Из 6.3 ГБ реально полезно ~45–50 МБ. Полный движок погоды только продублирует и поломает нашу архитектуру.

### Что именно мешает (компонентность на GameState/Controller)
Погода живёт как компоненты на GameState и PlayerController, со своим ReplicationSubSystem и интерфейсами (BPI_WeatherGameState/BPI_Biome). Если внедрить:
- получаем **второй параллельный мир-стейт** со своей репликацией поверх нашего C++ ARunState/диспетчера/фаз забега;
- **авторитет хоста на погоду конкурирует** с авторитетом на фазы забега — конфликт коопной репликации;
- тянет BML_*-макробиблиотеки и весь слой енумов/структур (DayStates/WeatherStates/Biomes/Seasons);
- UI пака требует CommonUI+CommonInput (не включены), а у нас Canvas-HUD.
Поэтому НЕ навешивать AC_GameState_Time / AC_GameState_DayNightCycle / AC_Biome_WeatherManager на наш ARunState и AC_PlayerController_WeatherManager на наш PC.

### Как сделать нашу ночь/дождь/грозу без 5.7 ГБ природного мусора
1. **VFX (нужен только Niagara, уже включён).** Самодостаточны, без менеджера, из `ResourcePack/Effects/Weather`:
   - `NS_ThunderStormWithRain`, `NS_Rain_Heavy`, `NS_Rain_Light` — дождь/гроза за окнами палат;
   - `NS_Heavy_Wind`, `NS_Blizzard` — по желанию.
   - Спавнить из ARunState как ambient FX или ставить вручную за окнами. Молнию позже подвязать к шумовой системе (раскат грома = маскировка шагов / всплеск для будущего монстра-слухача).
2. **Ночное небо — статично, без динамики:**
   - положить `BP_DayNight_SunMoon` и зафиксировать ночь (НЕ запускать day-night менеджер) — он несёт SkyAtmosphere+SkyLight+DirectionalLight+VolumetricCloud+ExpHeightFog+PostProcess готовым ригом;
   - либо чище: скопировать эти компоненты в расстановку света уровня + `SM_SkySphere`/`M_SkySphere`/`T_Moon`/`T_Stars`/`MI_SimpleVolumetricClouds` как ночной купол.

### Порядок действий (Hyper)
1. При закрытом редакторе вычистить балласт (~6 ГБ): удалить `ResourcePack/Environments` (3.1 ГБ), `ResourcePack/Tileable_Materials` (2.3 ГБ), `Locomotion` (390 МБ), `Food/Props/Epic`. Оставить `ResourcePack/Effects/Weather`, `Icons/Weather_States`, `ResourcePack/Post_Process_Effects`.
2. **Перед удалением Locomotion** обнулить хард-ссылку `BP_DayNight_SunMoon` → `/Game/Hyper/Locomotion/Chars` (и на Effects), иначе ошибка загрузки/кука.
3. Учесть `*_BuiltData` и ссылки перед чисткой.
4. Поставить ночной небо-риг статично в тест-уровень, проверить связку с туманом/паникой.
5. Подключить 1–2 грозовых NS_* за окнами, проверить производительность в коопе.

**Усилия на полезную часть — низкие. На полное внедрение — высокие и не оправданы.**

---

## Остальные паки (краткие вердикты)

- **City Modular Hospital v.2** — **USE (cherry-pick), флагман-окружение.** Собрать СВОЙ уровень из модульного кита; прямое попадание ремонт-целей: щиток `SM_Electrical_Shield`, генератор `SM_Generator`. Шаг: перевести двери-BP на наше взаимодействие, под «трубу» искать меш отдельно, текстуры ужать до 2K.
- **Modular Haunted House** — **USE (cherry-pick), карта «дом» для прототипа 0.1.** Взять `LV_House` + кит и особенно `LV_Horror_Light` (готовый ночь+туман+PostProcess+LUT, ложится на нашу панику-от-света). Шаг: дублировать LV_House с `__ExternalActors__`, поставить наш GameMode; для швыряемого дебриса снять Nanite/дать fallback.
- **Citizens_Pack** — **cherry-pick, мирные NPC (посетители/пациенты).** Шаг: реассайн скелета на наш Mannequin → бесплатная анимация. Медперсонала (халаты) НЕТ — форму искать отдельно; текстуры до 1–2K.
- **Easy Options Menu** — **USE, готовый экран настроек на чистом UMG.** JsonBlueprintUtilities уже включён. Шаг: открывать `WBP_EasyOptionsMenuMain`, привязать ремап к нашим Avaryo IMC/IA, выкинуть Demo/Benchmark/Maps.
- **OG Main Menu** — **SKIP (как фронт).** Тяжёлый CommonUI-фреймворк со своим GI/GM/PC/HUD/Sessions, конфликтует с нашим стеком. Своё меню уже сделали.

---

## Что делать дальше (по приоритету)

1. **Флагман-уровень больницы (City Hospital):** playable-коробка из модулей под ARunState, ремонт-цели через set_static_mesh на `SM_Electrical_Shield`/`SM_Generator`, меш под «трубу», динамический ночной свет (Lumen/movable). Ядро игры.
2. **Хоррор-свет даром:** перенести светориг `LV_Horror_Light` (DirLight+SkyLight+ExpHeightFog+PPV+LUT+RectLights) в геймплейный уровень — самый дешёвый способ закрыть «ночь+туман+свет↔паника».
3. **Hyper — мини-задача cherry-pick:** статичное ночное небо в окнах + 1–2 грозовых NS_* за окнами; затем вычистить 6 ГБ балласта (при закрытом редакторе, обнулив хард-ссылки BP_DayNight_SunMoon). Фреймворк погоды не трогать.
4. **Меню настроек (EOM):** вкрутить `WBP_EasyOptionsMenuMain` в паузу, ремап → наши Enhanced Input ассеты, аудио-слайдеры → наши SoundClass/SoundMix (согласовать с AudioToolkitPro).
5. **NPC из Citizens_Pack:** реассайн скелета на Mannequin, базовый NPC-BP (пациенты — спящая поза, посетители — MoveTo), даунскейл текстур. После того, как уровень и цикл починок стоят.

