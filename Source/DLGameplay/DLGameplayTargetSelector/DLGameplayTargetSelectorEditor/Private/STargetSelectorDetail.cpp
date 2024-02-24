#include "STargetSelectorDetail.h"

#include "TargetSelectorEditor.h"

UObject* STargetSelectorDetail::GetObjectToObserve() const
{
	if (Editor.IsValid())
	{
		return Editor.Pin()->GetTargetObject();
	}
	return nullptr;
}

TSharedRef<SWidget> STargetSelectorDetail::PopulateSlot(const TSharedRef<SWidget> PropertyEditorWidget)
{
	return SSingleObjectDetailsPanel::PopulateSlot(PropertyEditorWidget);
}
