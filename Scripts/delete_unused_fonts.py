# -*- coding: utf-8 -*-
# Удаляем неиспользуемые импортированные шрифты (взяли движковый Roboto вместо них).
import unreal
for p in ["/Game/Avariika/UI/Fonts/AvCyrFont", "/Game/Avariika/UI/Fonts/arial"]:
    if unreal.EditorAssetLibrary.does_asset_exist(p):
        unreal.EditorAssetLibrary.delete_asset(p)
        unreal.log("deleted %s" % p)
