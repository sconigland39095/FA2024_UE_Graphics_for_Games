// Fill out your copyright notice in the Description page of Project Settings.


#include "assn5ImporterLibrary.h"


Uassn5ImporterLibrary::Uassn5ImporterLibrary(class FObjectInitializer const& parent) : Super(parent)
{
	bCreateNew = false;
	bEditAfterNew = true;
	SupportedClass = UVolumeTexture::StaticClass();

	bEditorImport = true;
	bText = false;

	// NO extra spaces at start or around semicolon: file extension;descripion
	Formats.Add(TEXT("nrrd;NRRD Files"));
	Formats.Add(TEXT("nhdr;NRRD Files"));
}

Uassn5ImporterLibrary::~Uassn5ImporterLibrary()
{
}

UObject* Uassn5ImporterLibrary::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	// LOAD & PARSE FILE
	FString DataType;
	FUintVector3 DimensionSize = { 0, 0, 0 };
	FString DataFileName;

	/**
	 * Load a text file to an FString. Supports all combination of ANSI/Unicode files and platforms.
	 *
	 * @param Result       String representation of the loaded file
	 * @param Filename     Name of the file to load
	 * @param VerifyFlags  Flags controlling the hash verification behavior ( see EHashOptions )
	 * 
	 * static CORE_API bool LoadFileToString(FString & Result, const TCHAR * Filename, EHashOptions VerifyFlags = EHashOptions::None, uint32 ReadFlags = 0);
	 */
	FString LoadedFile;
	bool ifFuncPass = FFileHelper::LoadFileToString(LoadedFile, *Filename);
	if (!ifFuncPass) {
		UE_LOG(LogTemp, Error, TEXT("File Did Not Load to String"));
	}

	/**
	 * Gets an array of strings from a source string divided up by a separator and empty strings can optionally be culled.
	 * @param SourceString - The string to chop up
	 * @param Delimiter - The string to delimit on
	 * @param CullEmptyStrings = true - Cull (true) empty strings or add them to the array (false)
	 * @return The array of string that have been separated
	 * 
	 * UFUNCTION(BlueprintPure, Category = "Utilities|String")
	 * static ENGINE_API TArray<FString> ParseIntoArray(const FString & SourceString, const FString & Delimiter = FString(TEXT(" ")), const bool CullEmptyStrings = true);
	 */
	TArray<FString> FileLines;
	LoadedFile.ParseIntoArray(FileLines, TEXT("\n"), true);
	if (FileLines.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("File's Lines Did Not Parse"));
	}

	// loop through lines in file to parse
	for (auto itr : FileLines) {
		FString Key;
		FString Value;

		// Trim spaces around the key and value
		Key.TrimStartAndEndInline();
		Value.TrimStartAndEndInline();

		/**
		* Splits this string at given string position case sensitive.
		*
		* @param InStr The string to search and split at
		* @param LeftS out the string to the left of InStr, not updated if return is false
		* @param RightS out the string to the right of InStr, not updated if return is false
		* @param SearchCase		Indicates whether the search is case sensitive or not ( defaults to ESearchCase::IgnoreCase )
		* @param SearchDir			Indicates whether the search starts at the begining or at the end ( defaults to ESearchDir::FromStart )
		* @return true if string is split, otherwise false
		* 
		* UFUNCTION(BlueprintPure, Category = "Utilities|String")
		* static ENGINE_API bool Split(const FString & SourceString, const FString & InStr, FString & LeftS, FString & RightS, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase, ESearchDir::Type SearchDir = ESearchDir::FromStart);
		*/
		if (itr.Split(TEXT(":"), &Key, &Value)) {
			// https://dev.epicgames.com/documentation/en-us/unreal-engine/fstring-in-unreal-engine: If you want the comparison to ignore case, use ESearchCase::IgnoreCase, and if not, use ESearchCase::CaseSensitive.
			// TestHUDString.Equals(TEXT("Test"), ESearchCase::CaseSensitive);
			if (Key.Equals(TEXT("type"), ESearchCase::IgnoreCase)) {
				// Trim whitespaces
				FString WordBuild = "";
				for (TCHAR CurChar : Value) {
					// apparentlly \r is an endline chaaracter
					if ((CurChar == TEXT('\n') || CurChar == TEXT('\r')) && !WordBuild.Equals("")) {
						DataType = WordBuild;
					}
					else if (CurChar != TEXT(' ')) {
						//UE_LOG(LogTemp, Warning, TEXT("Cur Char: %c"), CurChar);
						WordBuild += CurChar;
					}
				}
			}
			else if (Key.Equals(TEXT("sizes"), ESearchCase::IgnoreCase)) {
				FString NumberBuild = "";
				int SizeIndexCount = 0;

				for (TCHAR CurChar : Value) {
					if (CurChar == TEXT(' ') && !NumberBuild.Equals("")) {
						DimensionSize[SizeIndexCount] = FCString::Atoi(*NumberBuild);
						// reset the number builder string & move to the next index to put the next number in
						SizeIndexCount += 1;
						NumberBuild = "";
					}
					else {
						NumberBuild += CurChar;
					}
				}

				if (SizeIndexCount == 2) {
					DimensionSize[SizeIndexCount] = FCString::Atoi(*NumberBuild);
				}
			}
			else if (Key.Equals(TEXT("data file"), ESearchCase::IgnoreCase)) {
				// Trim whitespaces
				FString WordBuild = "";
				for (TCHAR CurChar : Value) {
					//UE_LOG(LogTemp, Warning, TEXT("Cur Char: %c"), CurChar);
					// apparentlly \r is an endline chaaracter
					if ((CurChar == TEXT('\n') || CurChar == TEXT('\r')) && !WordBuild.Equals("")) {
						DataFileName = WordBuild;
					}
					else if (CurChar != TEXT(' ')) {
						WordBuild += CurChar;
					}
				}
			}
		}
	}
	
	// ERROR CHECK - PARSED VALUES
	if (DataType.Equals("")) {
		UE_LOG(LogTemp, Error, TEXT("Did not find the \'type\' line: %s"), *FString(DataType));
	}
	if (DimensionSize[0] == 0 || DimensionSize[1] == 0 || DimensionSize[2] == 0) {
		UE_LOG(LogTemp, Error, TEXT("The \'sizes\' line returned a 0 in the X, Y, or Z: %d, %d, %d"), DimensionSize[0], DimensionSize[1], DimensionSize[2]);
	}
	if (DataFileName.Equals("")) {
		UE_LOG(LogTemp, Error, TEXT("Did not find the \'data file\' line: %s"), *FString(DataFileName));
	}
	
	// Create the return value
	class UVolumeTexture* VolTexture;
	// use parmeters of factory create file
	// T* NewObject(UObject* Outer, FName Name, EObjectFlags Flags = RF_NoFlags, UObject* Template = nullptr, bool bCopyTransientsFromClassDefaults = false, FObjectInstancingGraph* InInstanceGraph = nullptr)
	VolTexture = NewObject<UVolumeTexture>(InParent, InName, Flags);
	
	// Returns the path in front of the DATA filename: static CORE_API FString GetPath(const FString & InPath);
	//FString FilePath = FPaths::GetPath(DataFileName);
	FString FilePath = FPaths::GetPath(Filename);

	// Link path & file name
	FString FullFilePath = FPaths::Combine(FilePath, DataFileName);

	// Set UTexture::SRGB to 0, since this is not color data
	VolTexture->SRGB = 0;
	// Set UTexture::MipGenSettings to TextureMipGenSettings::TMGS_NoMipmaps to avoid generating multiple resolution versions of the volume data
	VolTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

	if (DataType.Equals(TEXT("uint8"))) {
		//UE_LOG(LogTemp, Warning, TEXT("Is uint8"));
		/**
		 * Load a binary file to a dynamic array with two uninitialized bytes at end as padding.
		 *
		 * @param Result    Receives the contents of the file
		 * @param Filename  The file to read
		 * @param Flags     Flags to pass to IFileManager::CreateFileReader
		 * 
		 * static CORE_API bool LoadFileToArray(TArray<uint8>&Result, const TCHAR * Filename, uint32 Flags = 0);
		*/
		TArray<uint8> NewData;
		FFileHelper::LoadFileToArray(NewData, *FullFilePath);
		
		// Texture.Source.Init will initialize the size, format (e.g. TSF_G8), and data
		/*void FTextureSource::Init(int32 NewSizeX, int32 NewSizeY, int32 NewNumSlices, int32 NewNumMips, ETextureSourceFormat NewFormat, const uint8 * NewData)*/
		VolTexture->Source.Init(DimensionSize[0], DimensionSize[1], DimensionSize[2], 1, ETextureSourceFormat::TSF_G8, NewData.GetData());
	}
	else if (DataType.Equals(TEXT("uint16"))) {
		// Support uint16 data. You will need to scan the file for the maximum value and re-scale all of the data by that maximum.
		TArray<uint16> NewData;
		LoadFileToArray16(NewData, *FullFilePath);

		// Looping through file for max?
		uint16 MaxValue = 0;
		for (uint16 itr : NewData) {
			if (itr > MaxValue) {
				MaxValue = itr;
			}
		}

		// Scale Data
		for (uint16 itr : NewData) {
			// if (MaxValue == 0) return;
			itr /= MaxValue;
		}

		// will need G16 for unit16 file
		VolTexture->Source.Init(DimensionSize[0], DimensionSize[1], DimensionSize[2], 1, ETextureSourceFormat::TSF_G16, (const uint8*)NewData.GetData());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("A valid \'type\' was not passed"));
	}

	// Call UTexture::UpdateResource to update the UVolumeTexture object with the loaded data
	VolTexture->UpdateResource();
	return VolTexture;
}

/**
 * COPUED FROM: FileHelper.cpp & changed the TArray<uint8>& Result -> TArray<uint16>& Result
 * Load a binary file to a dynamic array with two uninitialized bytes at end as padding.
 */
bool Uassn5ImporterLibrary::LoadFileToArray16(TArray<uint16>& Result, const TCHAR* Filename, uint32 Flags)
{
	FScopedLoadingState ScopedLoadingState(Filename);

	FArchive* Reader = IFileManager::Get().CreateFileReader(Filename, Flags);
	if (!Reader)
	{
		if (!(Flags & FILEREAD_Silent))
		{
			UE_LOG(LogStreaming, Warning, TEXT("Failed to read file '%s' error."), Filename);
		}
		return false;
	}
	int64 TotalSize64 = Reader->TotalSize();
	if (TotalSize64 + 2 > MAX_int32)
	{
		if (!(Flags & FILEREAD_Silent))
		{
			UE_LOG(LogStreaming, Error, TEXT("File '%s' is too large for 32-bit reader (%lld), use TArray64."), Filename, TotalSize64);
		}
		return false;
	}
	int32 TotalSize = (int32)TotalSize64;
	// Allocate slightly larger than file size to avoid re-allocation when caller null terminates file buffer
	Result.Reset(TotalSize + 2);
	Result.AddUninitialized(TotalSize);
	Reader->Serialize(Result.GetData(), Result.Num());
	bool Success = Reader->Close();
	delete Reader;
	return Success;
}