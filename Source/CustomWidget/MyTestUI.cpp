#include "MyTestUI.h"
#include "Components/SlateWrapperTypes.h"

#define LOCTEXT_NAMESPACE "UMG"

void UMyTestUI::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	MySlice->SetBrush(&Brush);
	MySlice->SetImage(&Brush2);
}

void UMyTestUI::ReleaseSlateResources(bool bReleaseChildren)
{
	MySlice.Reset();
}


FReply UMyTestUI::OnClickOk()
{
	OnClickTestUI.Broadcast();

	return FReply::Handled();
}

#if WITH_EDITOR
const FText UMyTestUI::GetPaletteCategory()
{
	return LOCTEXT("CustomPaletteCategory", "My custom category2!");
}
#endif

TSharedRef<SWidget> UMyTestUI::RebuildWidget()
{
	MySlice = SNew(SSlateMyTest)
		.Brush(&Brush)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, OnClickOk));
	return MySlice.ToSharedRef();
}
