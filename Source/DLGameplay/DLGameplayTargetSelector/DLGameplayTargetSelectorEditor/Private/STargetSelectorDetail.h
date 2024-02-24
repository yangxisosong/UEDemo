#pragma once
#include "SSingleObjectDetailsPanel.h"

class FTargetSelectorEditor;

class STargetSelectorDetail
	: public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(STargetSelectorDetail)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedPtr<FTargetSelectorEditor> InEditor)
	{
		SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments());
		Editor = InEditor;
	}

private:
	TWeakPtr<FTargetSelectorEditor> Editor;

protected:
	virtual UObject* GetObjectToObserve() const override;
	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override;
};
