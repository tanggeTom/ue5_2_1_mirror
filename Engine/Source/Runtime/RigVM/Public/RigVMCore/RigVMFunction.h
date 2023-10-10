// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/BlueprintSupport.h"
#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "CoreTypes.h"
#include "RigVMExecuteContext.h"
#include "RigVMMemory.h"
#include "RigVMTypeIndex.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectMacros.h"

#include "RigVMFunction.generated.h"

class UScriptStruct;
struct FRigVMExtendedExecuteContext;
struct FRigVMMemoryHandle;

typedef TArrayView<FRigVMMemoryHandle> FRigVMMemoryHandleArray;

typedef void (*FRigVMFunctionPtr)(FRigVMExtendedExecuteContext& RigVMExecuteContext, FRigVMMemoryHandleArray RigVMMemoryHandles);

struct FRigVMDispatchFactory;
struct FRigVMTemplate;

/**
 * The Pin Direction is used to differentiate different kinds of 
 * pins in the data flow graph - inputs, outputs etc.
 */
UENUM(BlueprintType)
enum class ERigVMPinDirection : uint8
{
	Input, // A const input value
	Output, // A mutable output value
	IO, // A mutable input and output value
	Visible, // A const value that cannot be connected to
	Hidden, // A mutable hidden value (used for interal state)
	Invalid // The max value for this enum - used for guarding.
};

/**
 * The FRigVMFunctionArgument describes an argument necessary for the C++ invocation of the RIGVM_METHOD backed function
 */
struct RIGVM_API FRigVMFunctionArgument
{
	const TCHAR* Name;
	const TCHAR* Type;
	TSharedPtr<FString> NameString;
	TSharedPtr<FString> TypeString;

	FRigVMFunctionArgument()
		: Name(nullptr)
		, Type(nullptr)
	{
	}

	FRigVMFunctionArgument(const TCHAR* InName, const TCHAR* InType)
		: Name(InName)
		, Type(InType)
	{
	}

	FRigVMFunctionArgument(const FString& InName, const FString& InType)
		: Name(nullptr)
		, Type(nullptr)
	{
		NameString = MakeShareable(new FString(InName));
		TypeString = MakeShareable(new FString(InType));
		Name = NameString->operator*();
		Type = TypeString->operator*();
	}
};

/**
 * The FRigVMExecuteArgument describes an execute argument.
 */
struct RIGVM_API FRigVMExecuteArgument
{
	FName Name;
	ERigVMPinDirection Direction;
	TRigVMTypeIndex TypeIndex;

	FRigVMExecuteArgument()
		: Name(NAME_None)
		, Direction(ERigVMPinDirection::Invalid)
		, TypeIndex(INDEX_NONE)
	{
	}

	FRigVMExecuteArgument(const FName& InName, const ERigVMPinDirection& InDirection, const TRigVMTypeIndex& InTypeIndex = INDEX_NONE)
		: Name(InName)
		, Direction(InDirection)
		, TypeIndex(InTypeIndex)
	{
	}
};

/**
 * The FRigVMFunction is used to represent a function pointer generated by UHT
 * for a given name. The name might be something like "FMyStruct::MyVirtualMethod"
 */
struct RIGVM_API FRigVMFunction
{
	FString Name;
	UScriptStruct* Struct;
	FRigVMDispatchFactory* Factory;
	FRigVMFunctionPtr FunctionPtr;
	int32 Index;
	int32 TemplateIndex;
	TArray<FRigVMFunctionArgument> Arguments;
	mutable TArray<TRigVMTypeIndex> ArgumentTypeIndices;

	FRigVMFunction()
		: Name()
		, Struct(nullptr)
		, Factory(nullptr)
		, FunctionPtr(nullptr)
		, Index(INDEX_NONE)
		, TemplateIndex(INDEX_NONE)
		, Arguments()
		, ArgumentTypeIndices()
	{
	}

	FRigVMFunction(const TCHAR* InName, FRigVMFunctionPtr InFunctionPtr, UScriptStruct* InStruct = nullptr, int32 InIndex = INDEX_NONE, const TArray<FRigVMFunctionArgument>& InArguments = TArray<FRigVMFunctionArgument>())
		: Name(InName)
		, Struct(InStruct)
		, Factory(nullptr)
		, FunctionPtr(InFunctionPtr)
		, Index(InIndex)
		, TemplateIndex(INDEX_NONE)
		, Arguments(InArguments)
	{
	}

	FRigVMFunction(const FString& InName, FRigVMFunctionPtr InFunctionPtr, FRigVMDispatchFactory* InFactory, int32 InIndex = INDEX_NONE, const TArray<FRigVMFunctionArgument>& InArguments = TArray<FRigVMFunctionArgument>())
	: Name(InName)
	, Struct(nullptr)
	, Factory(InFactory)
	, FunctionPtr(InFunctionPtr)
	, Index(InIndex)
	, TemplateIndex(INDEX_NONE)
	, Arguments(InArguments)
	{
	}

	bool IsValid() const { return !Name.IsEmpty() && FunctionPtr != nullptr; }
	FString GetName() const;
	FName GetMethodName() const;
	FString GetModuleName() const;
	FString GetModuleRelativeHeaderPath() const;
	const TArray<FRigVMFunctionArgument>& GetArguments() const { return Arguments; }
	const TArray<TRigVMTypeIndex>& GetArgumentTypeIndices() const;
	const FRigVMTemplate* GetTemplate() const;
	const UScriptStruct* GetExecuteContextStruct() const;
	bool SupportsExecuteContextStruct(const UScriptStruct* InExecuteContextStruct) const;
	FName GetArgumentNameForOperandIndex(int32 InOperandIndex, int32 InTotalOperands) const;
};