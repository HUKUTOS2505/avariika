МОДЕЛИ ИЗ MESHY.AI — КУДА КЛАСТЬ

Экспортируй из meshy.ai FBX с текстурами и клади в папку с ТОЧНЫМ именем ниже
(одна модель = одна папка, имя файла любое). Потом скажи Claude: «импортируй модели».
Claude прогонит Scripts/import_models.py (редактор должен быть ЗАКРЫТ): импорт в
/Game/Avariika/Meshes/, автоназначение на актор/предмет, автомасштаб под габарит.

Промпты для генерации — в WORKLOG.md (таблица, хвост «single object… no scene» уже вшит).

СЕТТИНГ: условно больница. Туалет = ОБЫЧНЫЙ унитаз (не уличная кабинка!).

ПРИОРИТЕТ (сначала первый модуль, его и тестируем):
  1) SM_Toilet      — унитаз (больничный санузел)
  2) SM_Breaker     — электрощиток
  3) SM_Tester      — тестер (мультиметр)
Потом крупняк:
  4) SM_GasPipe     — газовая труба с вентилем
  5) SM_Generator   — дизель-генератор
  6) SM_Gazelle     — ГАЗель (декор у зоны выхода)
Потом мелочёвка (предметы):
  7) SM_WeldingMachine, 8) SM_FireExtinguisher, 9) SM_FirstAidKit,
  10) SM_Cigarettes, 11) SM_Radio, 12) SM_Battery, 13) SM_Fuse

Куда что встанет (для справки):
  актор уровня:  SM_Toilet→Toilet, SM_Breaker→Repairable_Breaker,
                 SM_GasPipe→Repairable_GasPipe, SM_Generator→Repairable_Generator,
                 SM_Tester→Tester, SM_Radio→Radio, SM_Gazelle→декор у ExitZone
  CDO предмета:  SM_WeldingMachine, SM_FireExtinguisher, SM_FirstAidKit,
                 SM_Cigarettes, SM_Battery, SM_Fuse → /Game/Avariika/Items/BP_*

Можно приносить по одной-две модели — импортирую частями, не обязательно все сразу.
