using UnrealBuildTool;

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
            }
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",

                "InkPlusPlus",
                "Inkpot",
                "Json",
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
