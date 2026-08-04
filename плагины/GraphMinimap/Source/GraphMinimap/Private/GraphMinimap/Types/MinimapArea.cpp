// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "GraphMinimap/Types/MinimapArea.h"
#include "EdGraphNode_Comment.h"
#include "MaterialGraph/MaterialGraphNode_Comment.h"
#include "Materials/MaterialExpressionComment.h"

#define LOCTEXT_NAMESPACE "MinimapArea"

namespace GraphMinimap
{
	FMinimapAreaIdentifier::FMinimapAreaIdentifier()
		: SourceType(EMinimapAreaSource::Invalid)
	{
	}

	FMinimapAreaIdentifier::FMinimapAreaIdentifier(const FString& InExportedText)
		: SourceType(EMinimapAreaSource::Invalid)
	{
		FString SourceTypeString;
		FString Data;
		if (!InExportedText.Split(TEXT(":"), &SourceTypeString, &Data))
		{
			return;
		}

		if (SourceTypeString == TEXT("WholeArea"))
		{
			SourceType = EMinimapAreaSource::WholeArea;
		}
		else if (SourceTypeString == TEXT("CommentNode"))
		{
			SourceType = EMinimapAreaSource::CommentNode;
			LexFromString(CommentNodeGuid, *Data);
		}
	}

	FMinimapAreaIdentifier::FMinimapAreaIdentifier(const TWeakObjectPtr<UEdGraphNode_Comment>& InCommentNode)
		: SourceType(EMinimapAreaSource::CommentNode)
	{
		if (InCommentNode.IsValid())
		{
			// Material nodes use the id of the associated expression because the guid is reset.
			if (const auto* MaterialCommentNode = Cast<UMaterialGraphNode_Comment>(InCommentNode.Get()))
			{
				if (UMaterialExpressionComment* MaterialExpressionComment = MaterialCommentNode->MaterialExpressionComment)
				{
					CommentNodeGuid = MaterialExpressionComment->GetMaterialExpressionId();
				}
				else
				{
					SourceType = EMinimapAreaSource::Invalid;
				}
			}
			else
			{
				CommentNodeGuid = InCommentNode->NodeGuid;
			}
		}
	}

	FMinimapAreaIdentifier FMinimapAreaIdentifier::WholeArea()
	{
		FMinimapAreaIdentifier MinimapAreaIdentifier;
		MinimapAreaIdentifier.SourceType = EMinimapAreaSource::WholeArea;
		return MinimapAreaIdentifier;
	}

	EMinimapAreaSource FMinimapAreaIdentifier::GetSourceType() const
	{
		return SourceType;
	}

	FString FMinimapAreaIdentifier::ToString() const
	{
		switch (SourceType)
		{
		case EMinimapAreaSource::WholeArea:
			return TEXT("WholeArea:");
		case EMinimapAreaSource::CommentNode:
			return FString::Printf(TEXT("CommentNode:%s"), *CommentNodeGuid.ToString(EGuidFormats::Short));
		default:
			return {};
		}
	}

	FMinimapArea::FMinimapArea()
		: SourceType(EMinimapAreaSource::Invalid)
	{
	}

	FMinimapArea::FMinimapArea(const TWeakObjectPtr<UEdGraphNode_Comment>& InCommentNode)
		: SourceType(EMinimapAreaSource::CommentNode)
		, CommentNode(InCommentNode)
	{
	}

	TSharedRef<FMinimapArea> FMinimapArea::WholeArea()
	{
		TSharedRef<FMinimapArea> MinimapArea = MakeShared<FMinimapArea>();
		MinimapArea->SourceType = EMinimapAreaSource::WholeArea;
		return MinimapArea;
	}

	FText FMinimapArea::GetName() const
	{
		switch (SourceType)
		{
		case EMinimapAreaSource::WholeArea:
			return LOCTEXT("WhileAreaText", "Whole Area");
		case EMinimapAreaSource::CommentNode:
			return CommentNode->GetNodeTitle(ENodeTitleType::EditableTitle);
		default:
			return LOCTEXT("InvalidText", "Invalid Minimap Area");
		}
	}

	FMinimapAreaIdentifier FMinimapArea::ToIdentifier() const
	{
		switch (SourceType)
		{
		case EMinimapAreaSource::WholeArea:
			return FMinimapAreaIdentifier::WholeArea();
		case EMinimapAreaSource::CommentNode:
			return FMinimapAreaIdentifier(CommentNode);
		default:
			return FMinimapAreaIdentifier();
		}
	}

	TWeakObjectPtr<UEdGraphNode_Comment> FMinimapArea::GetCommentNode() const
	{
		return CommentNode;
	}
}

#undef LOCTEXT_NAMESPACE
