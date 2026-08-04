// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

class UEdGraphNode_Comment;

namespace GraphMinimap
{
	struct FMinimapArea;
	
	/**
	 * An enum that defines kind of what the minimap area is defined by.
	 */
	enum class EMinimapAreaSource : uint8
	{
		Invalid,
		WholeArea,
		CommentNode,
	};
	
	/**
	 * An abstract structure for identifying a minimap area.
	 */
	struct FMinimapAreaIdentifier
	{
	public:
		// Constructor.
		FMinimapAreaIdentifier();
		explicit FMinimapAreaIdentifier(const FString& InExportedText);
		explicit FMinimapAreaIdentifier(const TWeakObjectPtr<UEdGraphNode_Comment>& InCommentNode);
		static FMinimapAreaIdentifier WholeArea();

		// Returns the kind of what the minimap area is defined by.
		EMinimapAreaSource GetSourceType() const;
		
		// Exports identifier as string.
		FString ToString() const;
		
		// Overload comparison operators.
		FORCEINLINE bool operator==(const FMinimapAreaIdentifier& Other) const
		{
			return ToString().Equals(Other.ToString());
		}
		FORCEINLINE bool operator!=(const FMinimapAreaIdentifier& Other) const
		{
			return !(*this == Other);
		}
		
	private:
		// Kind of what the minimap area is defined by.
		EMinimapAreaSource SourceType;
		
		// The guid of comment node to use as minimap area.
		FGuid CommentNodeGuid;
	};
	
	/**
	 * A wrapper structure considering the possibility of specifying the minimap area other than comment nodes in the future.
	 */
	struct FMinimapArea
	{
	public:
		// Constructor.
		FMinimapArea();
		explicit FMinimapArea(const TWeakObjectPtr<UEdGraphNode_Comment>& InCommentNode);
		static TSharedRef<FMinimapArea> WholeArea();

		// Returns the name of the minimap area.
		FText GetName() const;
		
		// Exports as an abstracted identifier.
		FMinimapAreaIdentifier ToIdentifier() const;

		// Returns comment node to use as a minimap area.
		TWeakObjectPtr<UEdGraphNode_Comment> GetCommentNode() const;

		// Overload comparison operators.
		FORCEINLINE bool operator==(const FMinimapArea& Other) const
		{
			return (ToIdentifier() == Other.ToIdentifier());
		}
		FORCEINLINE bool operator!=(const FMinimapArea& Other) const
		{
			return !(*this == Other);
		}
		
	private:
		// Kind of what the minimap area is defined by.
		EMinimapAreaSource SourceType;
		
		// The comment node to use as a minimap area.
		TWeakObjectPtr<UEdGraphNode_Comment> CommentNode;
	};
}
