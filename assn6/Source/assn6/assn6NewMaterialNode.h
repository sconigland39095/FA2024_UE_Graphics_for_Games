// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpression.h"

#include "MaterialCompiler.h"
//#include "MaterialHLSLTree.h"
#include "MaterialHLSLGenerator.h"
//#include "HLSLTree/HLSLTreeCommon.h"

#include "assn6NewMaterialNode.generated.h"

//namespace UE::HLSLTree{
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class Uassn6NewMaterialNode : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()
	
	Uassn6NewMaterialNode();
	//~Uassn6NewMaterialNode();

	// MINE
	UPROPERTY()
	FVector2D UV;
	UPROPERTY(EditAnywhere)
	int32 XEdges;
	UPROPERTY(EditAnywhere)
	int32 YEdges;

	// GIVEN
	UPROPERTY()
	FExpressionInput Input;

	//~ Begin UMaterialExpressionMaterialX Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual void GetExpressionToolTip(TArray<FString>& OutToolTip) override;
	virtual FText GetKeywords() const override { return FText::FromString(TEXT("rgbtohsv")); }

	virtual bool GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const override;
#endif
	//~ End UMaterialExpressionMaterialX Interface

	//virtual void ComputeAnalyticDerivatives(FTree& Tree, FExpressionDerivatives& OutResult) const override;
	//void ComputeAnalyticDerivatives(FTree& Tree, FExpressionDerivatives& OutResult) const;
};

/*
class FExpressionSelect : public FExpression
{
public:
	FExpressionSelect(const FExpression* InCondition, const FExpression* InTrue, const FExpression* InFalse)
		: ConditionExpression(InCondition)
		, TrueExpression(InTrue)
		, FalseExpression(InFalse)
	{
		check(ConditionExpression);
	}

	const FExpression* ConditionExpression;
	const FExpression* TrueExpression;
	const FExpression* FalseExpression;

	virtual void ComputeAnalyticDerivatives(FTree& Tree, FExpressionDerivatives& OutResult) const override;
	virtual const FExpression* ComputePreviousFrame(FTree& Tree, const FRequestedType& RequestedType) const override;
	virtual bool PrepareValue(FEmitContext& Context, FEmitScope& Scope, const FRequestedType& RequestedType, FPrepareValueResult& OutResult) const override;
	virtual void EmitValueShader(FEmitContext& Context, FEmitScope& Scope, const FRequestedType& RequestedType, FEmitValueShaderResult& OutResult) const override;
	virtual void EmitValuePreshader(FEmitContext& Context, FEmitScope& Scope, const FRequestedType& RequestedType, FEmitValuePreshaderResult& OutResult) const override;
	virtual bool EmitValueObject(FEmitContext& Context, FEmitScope& Scope, const FName& ObjectTypeName, void* OutObjectBase) const override;
};
*/

/*
#include "Materials/MaterialExpression.h"
#include "MaterialExpressionRgbToHsv.generated.h"

UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UMaterialExpressionRgbToHsv : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FExpressionInput Input;

	//~ Begin UMaterialExpressionMaterialX Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual void GetExpressionToolTip(TArray<FString>& OutToolTip) override;
	virtual FText GetKeywords() const override { return FText::FromString(TEXT("rgbtohsv")); }

	virtual bool GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const override;
#endif
	//~ End UMaterialExpressionMaterialX Interface
};
*/
