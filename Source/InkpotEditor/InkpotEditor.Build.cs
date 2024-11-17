using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class InkpotEditor : ModuleRules
{
	public InkpotEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] 
            {
			}
		);
		
		PrivateIncludePaths.AddRange(
			new string[] 
            {
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",

                "Projects"
            }
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",

                "InkPlusPlus",
                "Inkpot",
                "Json",

				"Slate",
				"SlateCore",
				"UMG",
                "UMGEditor",
                "Blutility"
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
