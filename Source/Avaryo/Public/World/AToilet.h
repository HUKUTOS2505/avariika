#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AToilet.generated.h"

class AAvaryoCharacter;
class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * Биотуалет — мини-игра. E начинает «процесс»: шкала туалета медленно уходит сама,
 * а по нижней полоске бегает курсор. Жми E в зелёной зоне — уходит быстро (-30),
 * в жёлтой — средне (-12), мимо — почти ничего (-2) и громкий конфуз на всю карту.
 * Зоны каждый раз в новом месте, курсор ускоряется. Шкала в ноль — визит зачтён.
 * Движение срывает процесс.
 */
UCLASS()
class AVARYO_API AToilet : public AActor
{
	GENERATED_BODY()

public:
	AToilet();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Кабинка (куб-заглушка; меш можно заменить в Blueprint). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Toilet")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Табличка над кабинкой. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Toilet")
	TObjectPtr<UTextRenderComponent> Label;

	// ---------- Настройки мини-игры ----------

	/** Скорость курсора на старте, проходов полоски в секунду. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float CursorSpeed;

	/** Полуширина зелёной зоны (доля полоски). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float GreenHalfWidth;

	/** Полуширина жёлтой зоны (доля полоски, включает зелёную). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float YellowHalfWidth;

	/** Снятие шкалы: зелёная / жёлтая / мимо. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float GreenDrain;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float YellowDrain;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float MissDrain;

	/** Пассивный слив, %/сек — процесс идёт, даже если не жать. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Toilet|Minigame")
	float PassiveDrainPerSecond;

	// ---------- API (сервер) ----------

	/** Есть ли игроку смысл садиться (шкала не пустая, не ранен, свободно). */
	UFUNCTION(BlueprintPure, Category="Toilet")
	bool CanUseBy(const AAvaryoCharacter* Who) const;

	/** Начать процесс (нажал E у кабинки). Только сервер. */
	bool BeginUseBy(AAvaryoCharacter* Who);

	/** Попытка попадания (E во время процесса). Только сервер. */
	void TryHitBy(AAvaryoCharacter* Who);

	/** Прервать процесс (ушёл/ранен). Только сервер. */
	void EndUseBy(AAvaryoCharacter* Who);

	// ---------- Геттеры для HUD ----------

	UFUNCTION(BlueprintPure, Category="Toilet") bool IsOccupied() const { return Occupant != nullptr; }
	UFUNCTION(BlueprintPure, Category="Toilet") AAvaryoCharacter* GetOccupant() const { return Occupant; }
	UFUNCTION(BlueprintPure, Category="Toilet") float GetCursorPos() const { return CursorPos; }
	UFUNCTION(BlueprintPure, Category="Toilet") float GetGreenCenter() const { return GreenCenter; }

protected:
	/** Кто сейчас «в процессе». */
	UPROPERTY(Replicated)
	TObjectPtr<AAvaryoCharacter> Occupant;

	/** Позиция курсора 0..1 (пинг-понг), реплицируется для HUD. */
	UPROPERTY(Replicated)
	float CursorPos;

	/** Центр зелёной зоны 0..1 — после каждого нажатия переезжает. */
	UPROPERTY(Replicated)
	float GreenCenter;

	/** Фаза курсора и текущий множитель скорости (сервер). */
	float CursorPhase;
	float SpeedMultiplier;

	/** Завершить успешно: шкала в ноль, визит в статистику. Только сервер. */
	void FinishSession(AAvaryoCharacter* Who);

	/** Перекинуть зелёную зону в случайное место. */
	void RerollGreenZone();
};
