#include "SSlateMyTestClip.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Runtime/SlateCore/Public/Fonts/FontMeasure.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSlateMyTestClip::Construct(const FArguments& InArgs)
{
	Brush = FInvalidatableBrushAttribute(InArgs._Brush);

	OnClicked = InArgs._OnClicked;

	m_clipNum = InArgs._ClipNum;

	bIsFocusable = 1;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

int32 SSlateMyTestClip::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	//控件位置
	const FVector2D Pos = AllottedGeometry.GetAbsolutePosition();

	auto pos2 = AllottedGeometry.Position;
	//获取大小
	const FVector2D Size = AllottedGeometry.GetAbsoluteSize();
	//计算中心坐标
	const FVector2D Center = Pos + 0.5 * Size;

	const float Radius = FMath::Min(Size.X, Size.Y) * 0.5f;

	const FSlateBrush* SlateBrush = Brush.GetImage().Get();
	FLinearColor LinearColor = ColorAndOpacity.Get() * InWidgetStyle.GetColorAndOpacityTint() * SlateBrush->GetTint(InWidgetStyle);
	FColor FinalColorAndOpacity = LinearColor.ToFColor(true);

	TArray<FVector2D> posarry;
	posarry.Add(FVector2D(-0.5, -0.5));
	posarry.Add(FVector2D(-0.5, 0.5));
	posarry.Add(FVector2D(0.5, 0.5));
	posarry.Add(FVector2D(0.5, -0.5));

	TArray<FSlateVertex> Vertices;

	TArray<SlateIndex> Indices;

	FSlateVertex centerinfo;
	centerinfo.Position= Center;
	centerinfo.Color = FinalColorAndOpacity;
	centerinfo.TexCoords[0] = 0.5;
	centerinfo.TexCoords[1] = 0.5;
	centerinfo.TexCoords[2] = 1;
	centerinfo.TexCoords[3] = 1;
	Vertices.Push(centerinfo);
	for (int i = 0; i < posarry.Num(); i++)
	{
		FSlateVertex pos;
		pos.Position = FVector2D(Center.X+posarry[i].X*(Radius/0.5), Center.Y+posarry[i].Y * (Radius / 0.5));
		pos.Color = FinalColorAndOpacity;

		//计算UV
		float uvx = posarry[i].X * 2 < 0 ? posarry[i].X * 2 + 1 : posarry[i].X * 2;
		float uvy = posarry[i].Y * 2 < 0 ? posarry[i].Y * 2 + 1 : posarry[i].Y * 2;
		// xy zw
		pos.TexCoords[0] = uvx;
		pos.TexCoords[1] = uvy;
		pos.TexCoords[2] = 1;
		pos.TexCoords[3] = 1;

		//pos.MaterialTexCoords= FVector2D(0.0f, 0.0f);

		Vertices.Push(pos);


		Indices.Push(0);
		Indices.Push(i + 1);

		int index = (i + 1);
		if (index == posarry.Num())
		{
			Indices.Push(1);
		}
		else
		{
			Indices.Push(i + 2);
		}
	};

	const FSlateBrush* SlateBrush2 = Image.GetImage().Get();
	const FSlateResourceHandle Handle2 = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*SlateBrush2);
	//FSlateDrawElement::MakeCustomVerts(
	//	OutDrawElements,
	//	LayerId,
	//	Handle2,
	//	Vertices,
	//	Indices,
	//	nullptr,
	//	0,
	//	0
	//);

	//顶点数组
	TArray<FSlateVertex> vertexs;
	//渲染索引
	TArray<SlateIndex> indexs;

	//需要画的三个顶点
	FSlateVertex v1;
	FSlateVertex v2;
	FSlateVertex v3;


	vertexs.Add(v1);
	vertexs.Add(v2);
	vertexs.Add(v3);

	//绘画顶点的顺序
	indexs.Add(0);
	indexs.Add(1);
	indexs.Add(2);

	long time= Args.GetCurrentTime()*100;
	float time2= time%100000;
	float p = 0;
	//p=0.5*FMath::Sin(time2/50)+0.5;
	//UE_LOG(LogTemp, Warning, TEXT("Your message %f time :%f"),p, time);
	//顶点的UV 数组分别表示 xy zw  zw=1时uv正常 还不清楚这个有什么用
	vertexs[0].TexCoords[0] = 0 + p;
	vertexs[0].TexCoords[1] = 0;
	vertexs[0].TexCoords[2] = 1;
	vertexs[0].TexCoords[3] = 1;
	vertexs[1].TexCoords[0] = 0 + p;
	vertexs[1].TexCoords[1] = 1;
	vertexs[1].TexCoords[2] = 1;
	vertexs[1].TexCoords[3] = 1;
	vertexs[2].TexCoords[0] = 1 + p;
	vertexs[2].TexCoords[1] = 1;
	vertexs[2].TexCoords[2] = 1;
	vertexs[2].TexCoords[3] = 1;

	//给顶点颜色
	//vertexs[0].Color = FinalColorAndOpacity;
	//vertexs[1].Color = FinalColorAndOpacity;
	//vertexs[2].Color = FinalColorAndOpacity;

	vertexs[0].Color = FColor::Red;
	vertexs[1].Color = FColor::Green;
	vertexs[2].Color = FColor::Blue;

	//下面是给三个顶点位置
	//vertexs[0].Position = FVector2D(500, 500);
	//vertexs[1].Position = FVector2D(pos2.X, pos2.Y);
	//vertexs[0].Position = FVector2D(Center.X - Size.X * 0.5, Center.Y - Size.Y * 0.5);
	//vertexs[1].Position = FVector2D(Center.X - Size.X * 0.5, Center.Y + Size.Y * 0.5);
	//vertexs[2].Position = FVector2D(Center.X + Size.X * 0.5, Center.Y + Size.Y * 0.5);

	const FSlateRenderTransform pRenderTransform = AllottedGeometry.GetAccumulatedRenderTransform();

	FVector2D TopLeft(0, 0);
	FVector2D BotRight(AllottedGeometry.GetLocalSize());
	const FVector2D TopRight = FVector2D(BotRight.X, TopLeft.Y);
	const FVector2D BotLeft = FVector2D(TopLeft.X, BotRight.Y);

	FVector2D StartUV = FVector2D(0.0f, 0.0f);
	FVector2D EndUV = FVector2D(1.0f, 1.0f);
	auto LocalSize = AllottedGeometry.GetLocalSize();
	auto DrawScale = AllottedGeometry.Scale;
	auto Tiling= FVector2D(1.0f, 1.0f);

	vertexs[0] = FSlateVertex::Make<ESlateVertexRounding::Disabled>(pRenderTransform, TopLeft, LocalSize, DrawScale, FVector4(StartUV, Tiling), FColor::Red);
	vertexs[1] = FSlateVertex::Make<ESlateVertexRounding::Disabled>(pRenderTransform, BotLeft, LocalSize, DrawScale, FVector4(FVector2D(StartUV.X, EndUV.Y), Tiling), FColor::Green);
	vertexs[2] = FSlateVertex::Make<ESlateVertexRounding::Disabled>(pRenderTransform, BotRight, LocalSize, DrawScale, FVector4(EndUV, Tiling), FColor::Blue);
	
	
	//auto transform = this->GetRenderTransform();
	//this->GetContentScale();
	//;
	//vertexs[0]=FSlateVertex::Make<ESlateVertexRounding::Enabled>(AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform(),
	//FVector2D(0, 0), Size, AllottedGeometry.Scale, 
	//FVector4(0, 0, 1.0f, 1.0f), FinalColorAndOpacity); //0
	//vertexs[1]=FSlateVertex::Make<ESlateVertexRounding::Enabled>(AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform(),
	//	FVector2D(0 ,  Size.Y), Size, AllottedGeometry.Scale,
	//	FVector4(0, 1, 1.0f, 1.0f), FinalColorAndOpacity); //0
	//vertexs[2]=FSlateVertex::Make<ESlateVertexRounding::Enabled>(AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform(),
	//	FVector2D(Size.X , Size.Y), Size, AllottedGeometry.Scale,
	//	FVector4(1, 1, 1.0f, 1.0f), FinalColorAndOpacity); //0

	//添加裁剪
	const FSlateClippingZone ClippingZone(
		FVector2D(Center.X - Size.X * 0.5 * (1 - m_clipNum), Center.Y - Size.Y * 0.5 * (1 - m_clipNum)),
		FVector2D(Center.X + Size.X * 0.5 * (1 - m_clipNum), Center.Y - Size.Y * 0.5 * (1 - m_clipNum)),
		FVector2D(Center.X - Size.X * 0.5 * (1 - m_clipNum), Center.Y + Size.Y * 0.5 * (1 - m_clipNum)),
		FVector2D(Center.X + Size.X * 0.5 * (1 - m_clipNum), Center.Y + Size.Y * 0.5 * (1 - m_clipNum)));



	if (!ClippingZone.HasZeroArea())
	{
		OutDrawElements.PushClip(ClippingZone);
	}

	const FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*SlateBrush);
	FSlateDrawElement::MakeCustomVerts(
		OutDrawElements,
		LayerId,
		Handle,
		vertexs,
		indexs,
		nullptr,
		0,
		0
	);

	FString str = Text.ToString();
	FTextBlockStyle DefaultTextStyle;
	//渲染文字所占用的总大小
	const FVector2D TextSize = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(Text, Font);
	//绘制区域大小
	auto boxsize = AllottedGeometry.GetLocalSize();
	//每个字符的平均宽度
	int onesieze = FMath::CeilToInt(TextSize.X / str.Len());
	//将字符分几行
	int row = FMath::CeilToInt(TextSize.X / boxsize.X);
	//一行最大可以容纳的字符
	int maxstr = FMath::FloorToInt(boxsize.X / onesieze);
	for (int i = 0; i <= row; i++)
	{
		FString str2 = str.Mid(i * maxstr, maxstr);
		FSlateLayoutTransform testtrans(1, FVector2D(0, TextSize.Y * i));
		FSlateDrawElement::MakeText(
			OutDrawElements,
			++LayerId,
			AllottedGeometry.ToPaintGeometry(TextSize, testtrans),
			str2,
			0,
			str2.Len(),
			Font,
			ESlateDrawEffect::None,
			InWidgetStyle.GetColorAndOpacityTint()
		);
	}

	//FSlateDrawElement::MakeBox(
	//	OutDrawElements,
	//	LayerId++,
	//	AllottedGeometry.ToPaintGeometry(
	//		FVector2D::ZeroVector,
	//		FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)),
	//	SlateBrush,
	//	ESlateDrawEffect::None,
	//	LinearColor
	//);

	if (!ClippingZone.HasZeroArea())
	{
		OutDrawElements.PopClip();
	}
	

	//FSlateDrawElement::MakeText(
	//	OutDrawElements,
	//	++LayerId,
	//	AllottedGeometry.ToPaintGeometry(),
	//	str,
	//	0,
	//	str.Len(),
	//	DefaultTextStyle.Font,
	//	ESlateDrawEffect::None,
	//	InWidgetStyle.GetColorAndOpacityTint()
	//);





	const FSlateRenderTransform& Transform = AllottedGeometry.GetAccumulatedRenderTransform();

	const FVector2D MaxSize = AllottedGeometry.GetLocalSize();

	//const FSlateClippingZone ClippingZone(Transform.TransformPoint((ProgressOrigin - FVector2D(Progress.Left, Progress.Top)) * MaxSize),
	//	Transform.TransformPoint( FVector2D(ProgressOrigin.X + Progress.Right, ProgressOrigin.Y - Progress.Top) * MaxSize),
	//	Transform.TransformPoint( FVector2D(ProgressOrigin.X - Progress.Left, ProgressOrigin.Y + Progress.Bottom) * MaxSize),
	//	Transform.TransformPoint( (ProgressOrigin + FVector2D(Progress.Right, Progress.Bottom)) * MaxSize));






	//FSlateDrawElement::MakeDebugQuad
	//(
	//	OutDrawElements,
	//	LayerId,
	//	AllottedGeometry.ToPaintGeometry()
	//);

	const FVector2D Start(10, 10);
	const FVector2D StartDir(AllottedGeometry.GetLocalSize().X * 1000 / 600, 0);
	const FVector2D End(AllottedGeometry.GetLocalSize().X / 4, AllottedGeometry.GetLocalSize().Y - 10);
	const FVector2D EndDir(AllottedGeometry.GetLocalSize().X * 1000 / 600, 0);

	/*FSlateDrawElement::MakeSpline(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		Start, StartDir,
		End, EndDir,
		AllottedGeometry.Scale,
		0 ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
		FColor::White
	);*/


	return LayerId - 1;
}

void SSlateMyTestClip::SetBrush(FSlateBrush* InBrush)
{
	Brush.SetImage(*this, InBrush);
}
void SSlateMyTestClip::SetImage(TAttribute<const FSlateBrush*> InImage)
{
	Image.SetImage(*this, InImage);
}
void SSlateMyTestClip::SetClip(float num)
{
	m_clipNum = num;
}
void SSlateMyTestClip::TestSpline(const FGeometry& InParams)
{

}


void SSlateMyTestClip::SetFont(const FSlateFontInfo& InFont)
{
	Font = InFont;
}

bool SSlateMyTestClip::SupportsKeyboardFocus() const
{
	return true;
}

FReply SSlateMyTestClip::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	UE_LOG(LogTemp, Warning, TEXT("OnMouseButtonUp"));

	auto bEventOverButton = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());
	auto mousepos =	MouseEvent.GetScreenSpacePosition();
	auto clip = GetCurrentClippingState();
	auto box = clip->ScissorRect.GetValue();
	if (mousepos.X< box.TopRight.X && mousepos.X > box.TopLeft.X &&
		mousepos.Y< box.BottomLeft.Y && mousepos.Y > box.TopLeft.Y)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMouseButtonUp OK"));
	}

	if (IsEnabled())
	{
		if (OnClicked.IsBound())
		{
			Reply = OnClicked.Execute();
		}

		if (Reply.IsEventHandled() == false)
		{
			Reply = FReply::Handled();
		}
	}

	if (Reply.GetMouseCaptor().IsValid() == false && HasMouseCapture())
	{
		Reply.ReleaseMouseCapture();
	}

	Invalidate(EInvalidateWidget::Layout);

	return Reply;


	//const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
	//const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;

	//auto bEventOverButton = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());

//	if (bEventOverButton)
//	{
//		// If we asked for a precise tap, all we need is for the user to have not moved their pointer very far.
//		const bool bTriggerForTouchEvent = InputClickMethod == EButtonClickMethod::PreciseClick;
//
//		// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
//		// pressed the button down first, then we'll allow the click to proceed without an active capture
//		const bool bTriggerForMouseEvent = (InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture());
//
//		if ((bTriggerForTouchEvent || bTriggerForMouseEvent))
//		{
//			if (OnClicked.IsBound())
//			{
//				FReply Reply = OnClicked.Execute();
//#if WITH_ACCESSIBILITY
//				FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(AsShared(), EAccessibleEvent::Activate);
//#endif
//				return Reply;
//			}
//			else
//			{
//				return FReply::Handled();
//			}
//		}
//	}


}

FReply SSlateMyTestClip::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	UE_LOG(LogTemp, Warning, TEXT("OnMouseButtonDown"));
	Reply = FReply::Handled().CaptureMouse(AsShared());
	Invalidate(EInvalidateWidget::Layout);
	return Reply;
	//if (IsEnabled() && (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton || MouseEvent.IsTouchEvent()))
	//{

	//	if (InputClickMethod == EButtonClickMethod::MouseDown)
	//	{
	//		//get the reply from the execute function
	//		Reply = ExecuteOnClick();

	//		//You should ALWAYS handle the OnClicked event.
	//		ensure(Reply.IsEventHandled() == true);
	//	}
	//	else if (InputClickMethod == EButtonClickMethod::PreciseClick)
	//	{
	//		// do not capture the pointer for precise taps or clicks
	//		// 
	//		Reply = FReply::Handled();
	//	}
	//	else
	//	{
	//		//we need to capture the mouse for MouseUp events
	//		Reply = FReply::Handled().CaptureMouse(AsShared());
	//	}
	//}

	//Invalidate(EInvalidateWidget::Layout);

	//return the constructed reply
}


void SSlateMyTestClip::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bIsHovered = false;

	Invalidate(EInvalidateWidget::Layout);
}

void SSlateMyTestClip::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsHovered = true;

	Invalidate(EInvalidateWidget::Layout);
}

void SSlateMyTestClip::SetText(FText info)
{
	Text = info;
}
