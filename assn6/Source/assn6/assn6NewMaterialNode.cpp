// Fill out your copyright notice in the Description page of Project Settings.

#include "assn6NewMaterialNode.h"

#define LOCTEXT_NAMESPACE "MaterialExpression"

Uassn6NewMaterialNode::Uassn6NewMaterialNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_Math;
		FConstructorStatics()
			: NAME_Math(LOCTEXT("Math", "Math"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_Math);
#endif
}

#if WITH_EDITOR
int32 Uassn6NewMaterialNode::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (!Input.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing RGBToHSV input"));
	}

	int32 InputIndex = Input.Compile(Compiler);
	EMaterialValueType InputType = Compiler->GetType(InputIndex);
	if (InputType != EMaterialValueType::MCT_Float3 && InputType != EMaterialValueType::MCT_Float4)
	{
		return InputIndex;
	}

	int32 Result = Compiler->RgbToHsv(InputIndex);

	return Result;
}

void Uassn6NewMaterialNode::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("RGBToHSV"));
}

void Uassn6NewMaterialNode::GetExpressionToolTip(TArray<FString>& OutToolTip)
{
	ConvertToMultilineToolTip(TEXT("Convert an incoming color from RGB to HSV space."), 40, OutToolTip);
}
#endif // WITH_EDITOR

bool Uassn6NewMaterialNode::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
	using namespace UE::HLSLTree;

	FTree& Tree = Generator.GetTree();

	const FExpression* Rgb = Input.AcquireHLSLExpression(Generator, Scope);
	const FExpression* R = Tree.NewSwizzle(FSwizzleParameters(0), Rgb);
	const FExpression* G = Tree.NewSwizzle(FSwizzleParameters(1), Rgb);
	const FExpression* B = Tree.NewSwizzle(FSwizzleParameters(2), Rgb);
	// A will be 0 if Rgb is a vec3
	const FExpression* A = Tree.NewSwizzle(FSwizzleParameters(3), Rgb);

	const FExpression* P = Tree.NewExpression<FExpressionSelect>(
		Tree.NewLess(G, B),
		Tree.NewAppend(B, G, Tree.NewConstant(FVector2f(-1.f, 2.f / 3.f))),
		Tree.NewAppend(G, B, Tree.NewConstant(FVector2f(0.f, -1.f / 3.f))));

	const FExpression* Q = Tree.NewExpression<FExpressionSelect>(
		Tree.NewLess(R, Tree.NewSwizzle(FSwizzleParameters(0), P)),
		Tree.NewAppend(Tree.NewSwizzle(FSwizzleParameters(0, 1, 3), P), R),
		Tree.NewAppend(R, Tree.NewSwizzle(FSwizzleParameters(1, 2, 0), P)));
	const FExpression* Qx = Tree.NewSwizzle(FSwizzleParameters(0), Q);
	const FExpression* Qy = Tree.NewSwizzle(FSwizzleParameters(1), Q);
	const FExpression* Qz = Tree.NewSwizzle(FSwizzleParameters(2), Q);
	const FExpression* Qw = Tree.NewSwizzle(FSwizzleParameters(3), Q);

	const FExpression* Chroma = Tree.NewSub(Qx, Tree.NewMin(Qw, Qy));

	const FExpression* Epsilon = Tree.NewConstant(1e-10f);
	const FExpression* Hue = Tree.NewDiv(Tree.NewSub(Qw, Qy), Tree.NewAdd(Tree.NewMul(Tree.NewConstant(6.f), Chroma), Epsilon));
	Hue = Tree.NewAbs(Tree.NewAdd(Hue, Qz));

	const FExpression* S = Tree.NewDiv(Chroma, Tree.NewAdd(Qx, Epsilon));

	OutExpression = Tree.NewAppend(Hue, S, Qx, A);
	return true;
}

/*
//void FExpressionExternalInput::ComputeAnalyticDerivatives(FTree& Tree, FExpressionDerivatives& OutResult) const
void Uassn6NewMaterialNode::ComputeAnalyticDerivatives(FTree& Tree, FExpressionDerivatives& OutResult) const
{
	const FExternalInputDescription InputDesc = GetExternalInputDescription(InputType);
	if (InputDesc.Ddx != EExternalInput::None)
	{
		check(InputDesc.Ddy != EExternalInput::None);
		OutResult.ExpressionDdx = Tree.NewExpression<FExpressionExternalInput>(InputDesc.Ddx);
		OutResult.ExpressionDdy = Tree.NewExpression<FExpressionExternalInput>(InputDesc.Ddy);
	}
	else
	{
		const Shader::EValueType DerivativeType = Shader::MakeDerivativeType(InputDesc.Type);
		switch (InputType)
		{
		case EExternalInput::ViewportUV:
		{
			// Ddx = float2(RcpViewSize.x, 0.0f)
			// Ddy = float2(0.0f, RcpViewSize.y)
			const FExpression* RcpViewSize = Tree.NewExpression<FExpressionExternalInput>(EExternalInput::RcpViewSize);
			const FExpression* Constant0 = Tree.NewConstant(0.0f);
			OutResult.ExpressionDdx = Tree.NewExpression<FExpressionAppend>(Tree.NewExpression<FExpressionSwizzle>(MakeSwizzleMask(true, false, false, false), RcpViewSize), Constant0);
			OutResult.ExpressionDdy = Tree.NewExpression<FExpressionAppend>(Constant0, Tree.NewExpression<FExpressionSwizzle>(MakeSwizzleMask(false, true, false, false), RcpViewSize));
			break;
		}
		default:
			if (DerivativeType != Shader::EValueType::Void)
			{
				OutResult.ExpressionDdx = OutResult.ExpressionDdy = Tree.NewConstant(Shader::FValue(DerivativeType));
			}
			break;
		}
	}
}
*/