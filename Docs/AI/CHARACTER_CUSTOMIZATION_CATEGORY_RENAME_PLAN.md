# Character Customization Category Rename Plan

## Visual State Follow-up Fix - 2026-07-09

After the manual category widget rename and the first `CustomizeRenameRestore` verification, visual parity was not clean: Presets was active, but Head also looked active and a long orange line appeared down the left menu.

Root cause:

- Designer defaults still had inactive category `ActiveBG` / `ActiveLine` widgets visible after the manual rename.
- The visible inactive line widgets stacked vertically and looked like one full-height orange line.
- `UAvCharacterCustomizationRootWidget::SelectCategory()` only stored `SelectedCategory`; it did not refresh visuals even though C++ owns the category button click bindings.
- The stale Blueprint graph warning for `On Clicked (Btn_CategoryHair)` still exists, but did not break runtime after the native visual refresh.

Minimal fix completed:

- WBP Designer defaults now leave only Presets active BG/line visible on first open; all other category active BG/line widgets are collapsed.
- C++ now refreshes the eight renamed category rows from `SelectedCategory`:
  - `Btn_CategoryPresets`
  - `Btn_CategoryHead`
  - `Btn_CategoryHairHeadwear`
  - `Btn_CategoryFaceProtection`
  - `Btn_CategoryHandsAccessories`
  - `Btn_CategoryUpperGear`
  - `Btn_CategoryLowerHip`
  - `Btn_CategoryFullOutfits`
- Only active row gets active BG/line and orange text/icon; inactive rows get collapsed active BG/line and gray text/icon.
- Randomize, Apply/save/reopen, WorkerAppearance, preview/material/framing, Modular Police, and the right panel were not changed.

Verification:

- UBT build succeeded in UE 5.8 after the C++ change.
- PIE first open through `AvCustomize`: only Presets active.
- Runtime Head selection: only Head active.
- Runtime HairHeadwear selection: only HairHeadwear active.
- Screenshots:
  - `Saved/CodexScreenshots/CustomizeCategoryVisualState_20260709/pie_first_open_active_presets_fixed.png`
  - `Saved/CodexScreenshots/CustomizeCategoryVisualState_20260709/pie_category_head_active_fixed.png`
  - `Saved/CodexScreenshots/CustomizeCategoryVisualState_20260709/pie_category_hair_headwear_active_fixed.png`

Remaining TODO:

- Clean or rebind the stale WBP graph node `On Clicked (Btn_CategoryHair)` only if a later dedicated Blueprint graph cleanup is requested or if it starts blocking compile/runtime. Current compile status is success with warnings.

## Current Verified Status - 2026-07-09

This document began as a pre-rename plan. The category widget rename was later performed manually in the WBP, and the final `CustomizeRenameRestore` PIE verification has now been completed.

Verified current category button names in `WBP_CharacterCustomizationRoot_V2`:

- `Btn_CategoryPresets`
- `Btn_CategoryHead`
- `Btn_CategoryHairHeadwear`
- `Btn_CategoryFaceProtection`
- `Btn_CategoryHandsAccessories`
- `Btn_CategoryUpperGear`
- `Btn_CategoryLowerHip`
- `Btn_CategoryFullOutfits`

Verified randomize status:

- `Btn_RandomAppearance` remains outside the category list under the bottom randomize area.
- HUD automation `TriggerCustomizeRandomAppearance()` successfully changed the preview from Worker 25 to Worker 24 and changed skin tone.
- Randomize did not become an active category and did not change layout.

Verified Apply/save/reopen status:

- Applied random Worker 24 through the customization Apply path.
- Runtime body changed to `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_24.SKM_Worker_Male_24`.
- Reopen through `AvCustomize` showed Worker 24 still selected.

Screenshots:

- `Saved/CodexScreenshots/CustomizeRenameRestore_20260708/pie_first_open_after_rename_restore_final.png`
- `Saved/CodexScreenshots/CustomizeRenameRestore_20260708/pie_after_hud_random_ufunction_final.png`
- `Saved/CodexScreenshots/CustomizeRenameRestore_20260708/pie_reopen_after_apply_random_final_2.png`

Outstanding TODO:

- The log contains a stale Blueprint event graph warning from the old widget name:
  - `On Clicked (Btn_CategoryHair)  не содержит допустимый соответствующий компонент!`
- Runtime PIE checks passed despite this warning. Do not edit the WBP graph just to remove it unless a later compile/runtime failure appears or a dedicated WBP graph cleanup is requested.

## Goal

Prepare a safe rename plan for the left category menu in `WBP_CharacterCustomizationRoot_V2`.

Historical note: this was originally a documentation-only plan. No Blueprint assets, `.uasset` files, C++ code, build files, commits, or runtime behavior were changed while preparing the original plan.

## Current Problem

The left menu now visually represents the new eight Character Customization categories, but many widget names still come from the older copied category rows: `Headwear`, `Beard`, `Balaclava`, `Hands`, `Hands_1`, and copied `Head_7` fragments.

That mismatch makes future maintenance risky because a widget named `Btn_CategoryBeard` may no longer represent beard-specific UI, and copied suffixes such as `_1` do not describe the visible category purpose.

## Current Category Widget Map

This map is based on read-only inspection of:

- `Content/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2.uasset` string inventory.
- Current C++ `BindWidgetOptional` fields and `BindCategoryButtons()` usage.
- Recent runtime verification screenshot from the parity pass.

Before performing the actual rename, confirm each row visually in the Designer hierarchy because Unreal Python access to `WidgetTree` was not exposed in the current MCP binding.

| Visual Category | Current Button | Current TextBlock | Current Icon | Current ActiveBG | Current ActiveLine | Notes |
|---|---|---|---|---|---|---|
| `ПРЕСЕТЫ` | `Btn_CategoryPreset` | `Txt_CategoryPreset` | `Img_CategoryPreset` via `SB_CategoryPresetIcon` | `Border_CategoryPresetActiveBG` | `Border_CategoryPresetActiveLine` / `SB_CategoryPresetActiveLine` | Cleanest existing row; C++ binds this button. |
| `ГОЛОВА` | `Btn_CategoryHead` | `Txt_CategoryHead` | `Img_CategoryHead` via `SB_CategoryHeadIcon` | `Border_CategoryHeadActiveBG` | `Border_CategoryHeadActiveLine` / `SB_CategoryHeadActiveLine` | Mostly semantic already; C++ binds this button. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | `Btn_CategoryHair` | likely copied row child, confirm in Designer | likely copied row child, confirm in Designer | likely copied row child, confirm in Designer | likely copied row child, confirm in Designer | `Btn_CategoryHair` exists and C++ binds it, but matching `Txt_CategoryHair`, `Img_CategoryHair`, `SB_CategoryHairIcon`, and `Border_CategoryHair*` names were not found in the asset string inventory. Confirm whether the row uses `Headwear`-named children. |
| `ЛИЦО И ЗАЩИТА` | likely `Btn_CategoryHeadwear` or `Btn_CategoryBeard`, confirm in Designer | `Txt_CategoryHeadwear` or `Txt_CategoryBeard`, confirm in Designer | `Img_CategoryHeadwear` or `Img_CategoryBeard`, confirm in Designer | `Border_CategoryHeadwearActiveBG` or `Border_CategoryBeardActiveBG`, confirm in Designer | matching `*ActiveLine`, confirm in Designer | High-risk semantic mismatch: old `Headwear`/`Beard` naming may now describe face/protection. C++ currently binds both old buttons separately. |
| `РУКИ И АКСЕССУАРЫ` | likely `Btn_CategoryBalaclava`, `Btn_CategoryHands`, or copied `Btn_CategoryHands_1`, confirm in Designer | matching copied text, confirm in Designer | matching copied icon, confirm in Designer | matching copied active bg, confirm in Designer | matching copied active line, confirm in Designer | High-risk semantic mismatch: old `Balaclava` and `Hands` names may no longer match the visible category. |
| `ВЕРХ И СНАРЯЖЕНИЕ` | likely one copied old row, confirm in Designer | likely one of `Txt_CategoryBeard_1` / `Txt_CategoryHands_1`, confirm in Designer | likely matching `_1` icon, confirm in Designer | likely matching `_1` active bg, confirm in Designer | likely matching `_1` active line, confirm in Designer | Copied `_1` names are not semantic. Check whether C++ has no binding for this row before rename. |
| `НИЗ И НАБЕДРЕННОЕ` | likely one copied old row, confirm in Designer | likely one of `Txt_CategoryBeard_1` / `Txt_CategoryHands_1`, confirm in Designer | likely matching `_1` icon, confirm in Designer | likely matching `_1` active bg, confirm in Designer | likely matching `_1` active line, confirm in Designer | Copied `_1` names are not semantic. Check whether C++ has no binding for this row before rename. |
| `ЦЕЛЬНЫЕ КОМПЛЕКТЫ` | likely remaining copied old row, confirm in Designer | likely remaining copied old row text, confirm in Designer | likely remaining copied old row icon, confirm in Designer | likely remaining copied old row active bg, confirm in Designer | likely remaining copied old row active line, confirm in Designer | This category currently has no obvious semantic C++ field. Do not rename until Designer row identity is confirmed. |

Asset string inventory found 83 category-related widget names, including copied fragments:

- Buttons: `Btn_CategoryPreset`, `Btn_CategoryHead`, `Btn_CategoryHair`, `Btn_CategoryHeadwear`, `Btn_CategoryBeard`, `Btn_CategoryBeard_1`, `Btn_CategoryBalaclava`, `Btn_CategoryHands`, `Btn_CategoryHands_1`.
- Overlays: `Overlay_CategoryPreset`, `Overlay_CategoryHead`, `Overlay_CategoryHeadwear`, `Overlay_CategoryBalaclava`, `Overlay_CategoryBeard`, `Overlay_CategoryBeard_1`, `Overlay_CategoryHands`, `Overlay_CategoryHands_1`, plus copied `Overlay_CategoryHead_7`.
- Active backgrounds: `Border_CategoryPresetActiveBG`, `Border_CategoryHeadActiveBG`, `Border_CategoryHeadwearActiveBG`, `Border_CategoryBalaclavaActiveBG`, `Border_CategoryBeardActiveBG`, `Border_CategoryBeardActiveBG_1`, `Border_CategoryHandsActiveBG`, `Border_CategoryHandsActiveBG_1`, plus copied `Border_CategoryHeadActiveBG_7`.
- Active lines: `Border_CategoryPresetActiveLine`, `Border_CategoryHeadActiveLine`, `Border_CategoryHeadwearActiveLine`, `Border_CategoryBalaclavaActiveLine`, `Border_CategoryBeardActiveLine`, `Border_CategoryBeardActiveLine_1`, `Border_CategoryHandsActiveLine`, `Border_CategoryHandsActiveLine_1`.
- Icon size boxes: `SB_CategoryPresetIcon`, `SB_CategoryHeadIcon`, `SB_CategoryHeadwearIcon`, `SB_CategoryBalaclavaIcon`, `SB_CategoryBeardIcon`, `SB_CategoryBeardIcon_1`, `SB_CategoryHandsIcon`, `SB_CategoryHandsIcon_1`.
- Active line size boxes: `SB_CategoryPresetActiveLine`, `SB_CategoryHeadActiveLine`, `SB_CategoryHeadwearActiveLine`, `SB_CategoryBalaclavaActiveLine`, `SB_CategoryBeardActiveLine`, `SB_CategoryBeardActiveLine_1`, `SB_CategoryHandsActiveLine`, `SB_CategoryHandsActiveLine_1`.
- Content boxes: `HB_CategoryPresetContent`, `HB_CategoryHeadContent`, `HB_CategoryHeadwearContent`, `HB_CategoryBalaclavaContent`, `HB_CategoryBeardContent`, `HB_CategoryBeardContent_1`, `HB_CategoryHandsContent`, `HB_CategoryHandsContent_1`, plus copied `HB_CategoryHeadContent_7`.
- Images: `Img_CategoryPreset`, `Img_CategoryHead`, `Img_CategoryHeadwear`, `Img_CategoryBalaclava`, `Img_CategoryBeard`, `Img_CategoryBeard_1`, `Img_CategoryHands`, `Img_CategoryHands_1`, and copied icon fragments `Img_CategoryHead_1` through `Img_CategoryHead_7`.
- Text blocks: `Txt_CategoryPreset`, `Txt_CategoryHead`, `Txt_CategoryHeadwear`, `Txt_CategoryBalaclava`, `Txt_CategoryBeard`, `Txt_CategoryBeard_1`, `Txt_CategoryHands`, `Txt_CategoryHands_1`.

## Proposed Rename Map

Use one semantic prefix per visible row. Keep the prefix stable across button, overlay, active background, active line, content box, icon size box, icon image, and text block.

Recommended category prefixes:

- `CategoryPreset`
- `CategoryHead`
- `CategoryHairHeadwear`
- `CategoryFaceProtection`
- `CategoryHandsAccessories`
- `CategoryUpperGear`
- `CategoryLowerHip`
- `CategoryFullSets`

| Visual Category | Old Name | New Name | Widget Type | Risk | Notes |
|---|---|---|---|---|---|
| `ПРЕСЕТЫ` | `Btn_CategoryPreset` | `Btn_CategoryPreset` | Button | Low | Already semantic enough; can keep to avoid needless C++ churn. |
| `ПРЕСЕТЫ` | `Overlay_CategoryPreset` | `Overlay_CategoryPreset` | Overlay | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `Border_CategoryPresetActiveBG` | `Border_CategoryPresetActiveBG` | Border | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `Border_CategoryPresetActiveLine` | `Border_CategoryPresetActiveLine` | Border | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `SB_CategoryPresetActiveLine` | `SB_CategoryPresetActiveLine` | SizeBox | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `HB_CategoryPresetContent` | `HB_CategoryPresetContent` | HorizontalBox | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `SB_CategoryPresetIcon` | `SB_CategoryPresetIcon` | SizeBox | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `Img_CategoryPreset` | `Img_CategoryPreset` | Image | Low | Already semantic enough. |
| `ПРЕСЕТЫ` | `Txt_CategoryPreset` | `Txt_CategoryPreset` | TextBlock | Low | Already semantic enough. |
| `ГОЛОВА` | `Btn_CategoryHead` | `Btn_CategoryHead` | Button | Low | Already semantic enough; can keep to avoid needless C++ churn. |
| `ГОЛОВА` | `Overlay_CategoryHead` | `Overlay_CategoryHead` | Overlay | Low | Already semantic enough. |
| `ГОЛОВА` | `Border_CategoryHeadActiveBG` | `Border_CategoryHeadActiveBG` | Border | Low | Already semantic enough. |
| `ГОЛОВА` | `Border_CategoryHeadActiveLine` | `Border_CategoryHeadActiveLine` | Border | Low | Already semantic enough. |
| `ГОЛОВА` | `SB_CategoryHeadActiveLine` | `SB_CategoryHeadActiveLine` | SizeBox | Low | Already semantic enough. |
| `ГОЛОВА` | `HB_CategoryHeadContent` | `HB_CategoryHeadContent` | HorizontalBox | Low | Already semantic enough. |
| `ГОЛОВА` | `SB_CategoryHeadIcon` | `SB_CategoryHeadIcon` | SizeBox | Low | Already semantic enough. |
| `ГОЛОВА` | `Img_CategoryHead` | `Img_CategoryHead` | Image | Low | Already semantic enough. |
| `ГОЛОВА` | `Txt_CategoryHead` | `Txt_CategoryHead` | TextBlock | Low | Already semantic enough. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | `Btn_CategoryHair` | `Btn_CategoryHairHeadwear` | Button | Medium | Requires C++ `BindWidgetOptional` field rename and handler binding update. Confirm row children in Designer first. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | `Overlay_CategoryHeadwear` or copied row overlay | `Overlay_CategoryHairHeadwear` | Overlay | Medium | Use the overlay that is actually under the visual hair/headwear row. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old active bg | `Border_CategoryHairHeadwearActiveBG` | Border | Medium | Confirm exact old name before rename. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old active line border | `Border_CategoryHairHeadwearActiveLine` | Border | Medium | Confirm exact old name before rename. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old active line size box | `SB_CategoryHairHeadwearActiveLine` | SizeBox | Medium | Confirm exact old name before rename. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old content box | `HB_CategoryHairHeadwearContent` | HorizontalBox | Medium | Confirm exact old name before rename. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old icon size box | `SB_CategoryHairHeadwearIcon` | SizeBox | Medium | Confirm exact old name before rename. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old image | `Img_CategoryHairHeadwear` | Image | Medium | Confirm exact old name before rename. |
| `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` | matching old text | `Txt_CategoryHairHeadwear` | TextBlock | Medium | Confirm exact old name before rename. |
| `ЛИЦО И ЗАЩИТА` | confirmed visual row old prefix | `Btn_CategoryFaceProtection` | Button | High | Requires C++ field/handler enum cleanup. Old `Beard`, `Balaclava`, or `Headwear` naming is misleading. |
| `ЛИЦО И ЗАЩИТА` | confirmed visual row old prefix children | `Overlay_CategoryFaceProtection`, `Border_CategoryFaceProtectionActiveBG`, `Border_CategoryFaceProtectionActiveLine`, `SB_CategoryFaceProtectionActiveLine`, `HB_CategoryFaceProtectionContent`, `SB_CategoryFaceProtectionIcon`, `Img_CategoryFaceProtection`, `Txt_CategoryFaceProtection` | Mixed | High | Rename as a group after confirming row identity. |
| `РУКИ И АКСЕССУАРЫ` | confirmed visual row old prefix | `Btn_CategoryHandsAccessories` | Button | High | Requires C++ field/handler enum cleanup. Do not confuse with old `Hands` if it now maps to another row. |
| `РУКИ И АКСЕССУАРЫ` | confirmed visual row old prefix children | `Overlay_CategoryHandsAccessories`, `Border_CategoryHandsAccessoriesActiveBG`, `Border_CategoryHandsAccessoriesActiveLine`, `SB_CategoryHandsAccessoriesActiveLine`, `HB_CategoryHandsAccessoriesContent`, `SB_CategoryHandsAccessoriesIcon`, `Img_CategoryHandsAccessories`, `Txt_CategoryHandsAccessories` | Mixed | High | Rename as a group after confirming row identity. |
| `ВЕРХ И СНАРЯЖЕНИЕ` | confirmed copied old prefix | `Btn_CategoryUpperGear` | Button | High | Likely copied `_1` row; C++ may currently have no binding for the copied button. |
| `ВЕРХ И СНАРЯЖЕНИЕ` | confirmed copied old prefix children | `Overlay_CategoryUpperGear`, `Border_CategoryUpperGearActiveBG`, `Border_CategoryUpperGearActiveLine`, `SB_CategoryUpperGearActiveLine`, `HB_CategoryUpperGearContent`, `SB_CategoryUpperGearIcon`, `Img_CategoryUpperGear`, `Txt_CategoryUpperGear` | Mixed | High | Rename as a group only after Designer confirmation. |
| `НИЗ И НАБЕДРЕННОЕ` | confirmed copied old prefix | `Btn_CategoryLowerHip` | Button | High | Likely copied `_1` row; C++ may currently have no binding for the copied button. |
| `НИЗ И НАБЕДРЕННОЕ` | confirmed copied old prefix children | `Overlay_CategoryLowerHip`, `Border_CategoryLowerHipActiveBG`, `Border_CategoryLowerHipActiveLine`, `SB_CategoryLowerHipActiveLine`, `HB_CategoryLowerHipContent`, `SB_CategoryLowerHipIcon`, `Img_CategoryLowerHip`, `Txt_CategoryLowerHip` | Mixed | High | Rename as a group only after Designer confirmation. |
| `ЦЕЛЬНЫЕ КОМПЛЕКТЫ` | confirmed remaining copied old prefix | `Btn_CategoryFullSets` | Button | High | Needs explicit C++ binding if click behavior is expected. |
| `ЦЕЛЬНЫЕ КОМПЛЕКТЫ` | confirmed remaining copied old prefix children | `Overlay_CategoryFullSets`, `Border_CategoryFullSetsActiveBG`, `Border_CategoryFullSetsActiveLine`, `SB_CategoryFullSetsActiveLine`, `HB_CategoryFullSetsContent`, `SB_CategoryFullSetsIcon`, `Img_CategoryFullSets`, `Txt_CategoryFullSets` | Mixed | High | Rename as a group only after Designer confirmation. |

## C++ Binding Impact

Current C++ only binds category buttons. It does not bind category text, icons, active backgrounds, active lines, overlays, or content boxes.

| Current C++ Field | Current Widget Name | Proposed Widget Name | File | Notes |
|---|---|---|---|---|
| `Btn_CategoryPreset` | `Btn_CategoryPreset` | keep `Btn_CategoryPreset` | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryPresetClicked()`. |
| `Btn_CategoryHead` | `Btn_CategoryHead` | keep `Btn_CategoryHead` | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryHeadClicked()`. |
| `Btn_CategoryHair` | `Btn_CategoryHair` | `Btn_CategoryHairHeadwear` | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryHairClicked()`. Rename requires C++ field and function binding update. |
| `Btn_CategoryHeadwear` | `Btn_CategoryHeadwear` | likely `Btn_CategoryFaceProtection`, confirm row | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryHeadwearClicked()`. Old semantic no longer matches new menu. |
| `Btn_CategoryBeard` | `Btn_CategoryBeard` | likely one of `Btn_CategoryFaceProtection` / `Btn_CategoryUpperGear` / `Btn_CategoryLowerHip`, confirm row | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryBeardClicked()`. High-risk stale semantic. |
| `Btn_CategoryBalaclava` | `Btn_CategoryBalaclava` | likely one of `Btn_CategoryFaceProtection` / `Btn_CategoryHandsAccessories`, confirm row | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryBalaclavaClicked()`. High-risk stale semantic. |
| `Btn_CategoryHands` | `Btn_CategoryHands` | likely `Btn_CategoryHandsAccessories` or copied later row, confirm row | `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h` | Bound in `BindCategoryButtons()` to `HandleCategoryHandsClicked()`. High-risk stale semantic. |
| none currently found | `Btn_CategoryBeard_1` | likely `Btn_CategoryUpperGear` / `Btn_CategoryLowerHip` / `Btn_CategoryFullSets`, confirm row | none currently bound | Button exists in WBP strings but has no current `BindWidgetOptional` field. |
| none currently found | `Btn_CategoryHands_1` | likely `Btn_CategoryUpperGear` / `Btn_CategoryLowerHip` / `Btn_CategoryFullSets`, confirm row | none currently bound | Button exists in WBP strings but has no current `BindWidgetOptional` field. |

Current C++ usage locations:

- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`: `UPROPERTY(meta=(BindWidgetOptional))` declarations for category buttons and stale enum values `Headwear`, `Beard`, `Balaclava`, `Hands`.
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`: `BindCategoryButtons()` binds the seven current C++ button fields.
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`: category click handlers call `SelectCategory(...)`.

## Safe Rename Order

1. Open `WBP_CharacterCustomizationRoot_V2` in Unreal Editor only when screen/control is available.
2. Do not use automated rename until the Designer hierarchy is visually confirmed row by row.
3. In Designer, identify the exact widget subtree for each visual row in this order: `ПРЕСЕТЫ`, `ГОЛОВА`, `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ`, `ЛИЦО И ЗАЩИТА`, `РУКИ И АКСЕССУАРЫ`, `ВЕРХ И СНАРЯЖЕНИЕ`, `НИЗ И НАБЕДРЕННОЕ`, `ЦЕЛЬНЫЕ КОМПЛЕКТЫ`.
4. Start with non-C++-bound child widgets: overlays, active backgrounds, active lines, size boxes, images, text blocks.
5. Rename one full row subtree at a time and compile the Blueprint after each row.
6. Rename button widgets last, because button names affect C++ `BindWidgetOptional` fields.
7. After button renames, update C++ field names, enum names, and handler names in one narrow C++ patch.
8. Build after C++ changes.
9. Run PIE through the real `AvCustomize` path and verify the first-open menu before touching any other customization behavior.

## Do Not Rename Yet

- Do not rename anything in `WBP_CharacterCustomizationRoot_V2.uasset` until Designer hierarchy confirmation is possible.
- Do not rename `Overlay_CategoryHead_7`, `HB_CategoryHeadContent_7`, `Border_CategoryHeadActiveBG_7`, or `Img_CategoryHead_7` until confirming whether they belong to the randomize button copy rather than a left menu row.
- Do not rename `Btn_RandomAppearance`, `Txt_RandomAppearanceLabel`, `Img_RandomAppearanceIcon`, `Overlay_RandomAppearance`, or `SB_BottomArea` as part of category cleanup.
- Do not rename Apply/save/reopen widgets.
- Do not rename skin swatch widgets.
- Do not rename preview/framing/material widgets.
- Do not rename inventory, Modular Police, or WorkerAppearance widgets/classes.
- Do not delete copied widgets just because their names look stale; first verify whether they are live rows.

## Validation Checklist

After the future rename implementation, verify:

- PIE opens customization through the real `AvCustomize` path.
- First-open visual state still matches Designer.
- All 8 category rows are visible in the correct order.
- All 8 category rows click without log errors.
- Active category visual remains Designer-authored.
- Inactive category visual remains Designer-authored.
- `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` still wraps exactly as intended.
- Randomize button layout and behavior did not move or change.
- Apply/save/reopen still works.
- No `BindWidget` or Blueprint compile warnings for missing renamed widgets.
- C++ build succeeds after any future C++ field rename.
