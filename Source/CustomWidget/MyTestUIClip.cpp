#include "MyTestUIClip.h"
#include "Components/SlateWrapperTypes.h"

#define LOCTEXT_NAMESPACE "UMG"

void UMyTestUIClip::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	MySlice->SetBrush(&Brush);
	MySlice->SetImage(&Brush2);
	MySlice->SetFont(m_Font);
	MySlice->SetClip(ClipNum);
	MySlice->SetText(m_Text);
}

void UMyTestUIClip::ReleaseSlateResources(bool bReleaseChildren)
{
	MySlice.Reset();
}


FReply UMyTestUIClip::OnClickOk()
{
	OnClickTestUI.Broadcast();

	return FReply::Handled();
}

#if WITH_EDITOR
const FText UMyTestUIClip::GetPaletteCategory()
{
	return LOCTEXT("CustomPaletteCategory", "My custom ClipUI!");
}
#endif

TSharedRef<SWidget> UMyTestUIClip::RebuildWidget()
{
	MySlice = SNew(SSlateMyTestClip)
		.Brush(&Brush)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, OnClickOk))
		.ClipNum(0);
	return MySlice.ToSharedRef();
}
