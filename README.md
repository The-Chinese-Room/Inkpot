# Inkpot
**Inkpot** - A container for **Ink** within the Unreal Engine developed by [The Chinese Room](https://www.thechineseroom.co.uk/).<br><br>
This is a plugin for Unreal Engine 5.6 or later.<br>
This is version **1.30.21** of the plugin.</br>
The head revision contains work in progress towards the upcoming release.<br>

Inkpot is a wrapper for the wonderful narrative scripting language **Ink** developed by [Inkle Studios](https://www.inklestudios.com/ink/).<br>

For a demo of how this integrates with an Unreal project (& a load more documentation) see [the inkpot demo](https://github.com/The-Chinese-Room/InkpotDemo/).<br>
More docs can be found [here](https://docs.google.com/document/d/1P5p0KLc7EfqqNNqzM0VHyllNZhtMQy7Bw_Y4_0Rcpu8/edit?tab=t.0), courtesy of Milo Duclayan,

For general support and chat with other users, check out [Inkle's discord](https://discord.com/invite/inkle#unreal-projects) <br>
(You'll find Inkpot chat in #unreal-projects)

## Changelog  

### Changes in 1.30.21
Gameplay tag tables now automatically created on ink source import. Conditional on bAutogenerateGameplayTags.<br>
Gameplay tag tables automatically added to gameplay tags list with generated & save to ini file. Conditional on bAutoUpdateGameplayTagsList<br>
Added ContinueIfYouCan, combines CanContinue and Continue to simplify blueprint code.<br>
Added SwitchFlowToPath reimplemented in the story class from the demo, added GameplayTag variant, SwitchFlowToPathGT.<br>
InpotList library now has functions to add individual items to lists, AddItem & AddItemGT.<br>
Implemented scriptable import pipeline, Ink source import can now be fully customised.<br>

### Changes in 1.24.21
Fixed bad resolution of gameplay tags for getting variables. 

### Changes in 1.23.21
Fixed compilation errors when target cs has TargetRules.NativePointerMemberBehaviorOverride set to PointerMemberBehavior.Disallow (LyraStarterGame)<br>

### Changes in 1.22.21
Ink lists now report truthyness correctly.<br>

### Changes in 1.21.21
Module loading phase now set to PreDefault to resolve some blueprint function resolution errors.<br>
Fixed compilation errors on latest MSVC tool chain 14.44.35207.<br>
Fixed bad test condition parsing that was falsely failing some InkCPP unit tests.<br>
Merged support for Mac builds ( see [pull request](https://github.com/The-Chinese-Room/Inkpot/pull/118#issue-3442796120) )<br>

### Changes in 1.20.21
Variables can now be filtered in the blotter view.<br>
Variables can now be pinned in the blotter view.<br>
List entries can now be set in drop down menu in blotter view.<br>
New outliner tool, only shows story structure at the moment.<br>
Simplified version of Evaluate function, Eval, for when you have no parameters.<br>
Inkpot settings now split between project settings and editor preferences. <br>
Story factory class support, allowing projects to define their own subclass of the Inkpot story.<br>
Gameplay tag support, to streamline workflows in Blueprints. Gameplay tags can now be used instead of strings for variables and paths etc. <br>
Fixed late binding notification, which caused a few timing bugs especially for the Inkpot watch component.<br>

### Changes in 1.11.21
Added tools menu section for inkpot, for easier launching of the blotter.<br>
Blotter QoL, list value update in blotter now more streamlined, items now have check box entries.<br>
Exposed VisitCountAtPathString to Inkpot Story.<br>
Added library functions to test the type of an Inkpot value.<br>
Added delegate to allow notify when a line has finished being displayed.<br>
Line render notification, for when multiple systems are rendering your line of text.<br>
Fixed bad plugin declarations, no more dependency warnings.<br>
Refined blotter update, list values are now updated rather than being replaced for each update.<br>
Set is now SetText on blotter strings.<br>

### Changes in 1.10.21
Introducing the Blotter! or Inkpot Debugger, an Unreal editor utility widget that allows the viewing of and setting of Ink variables at runtime.<br>
Full support for Ink List creation & manipulation in blueprints.<br>
Added documentation to the headers for most blueprint facing functions, story, values & lists.<br>
Ink function evaluation now supported from Blueprints.<br> 
Added variable existence checks. All setter or getter functions from story now report success, plus generic test function IsVariableDefined.<br>
Fixed crash when calling begin story with a null asset.<br> 
Fixed crash when calling function with empty variable declarations.<br>

### Changes in 1.03.21
Added new abstract factory creation for stories, class UInkpotStory can now be subclassed on a per project basis.<br>
Switched settings back to regular UDeveloperSettings as backed by CVAR version did not seem to work.<br>
Fixed for 5.4 compilation error, template instantiation context error in InkPlusPlusTest.cpp(738).<br>
Fixed compiler warnings in 5.4.<br>
Fixed assertion when attempting to access no existent variable in story.<br>
Fixed typo in headers that would fail on certain compilers ( #include "Coreminimal.h" should have been #include "CoreMinimal.h" )<br>
FIxed Deprecation warning for GetAssetRegistryTags.<br>
Updated serialize function to match parent in UInkpotStoryAsset.<br>

### Changes in 1.02.21
Fixed assertion when accessing variables that do not exist in the ink script.<br>
Fixed relative path resolution bug that caused Ink source files to not import.<br>
Improved compilation logging.<br>

### Changes in 1.01.21
Added tag accessor methods to InkpotLine & InkpotChoice.<br>

### Changes in 1.00.21
Inkplusplus port now matches version 21 of the Ink runtime engine, Ink version 1.1.1.<br>
.net version updated to 5.0<br>

### Changes in 0.4.20
Settings backed by CVars added.<br>
First pass on auto reload and replay of ink source.<br>

### Changes in 0.3.20
JSON serialisation now functional.<br>

### Changes in 0.2.20
External functions are now implemented, along with functional tests.<br>

### Changes in 0.1.20
Initial release.<br>

## Requirements 
Inkpot works with version 5.6 of Unreal.<br>
Inkpot includes a C++ port of Ink-engine-runtime version 21, which can be found in the InkPlusPlus module.<br>

### .Net framework 5.0
InkleCate requires the .net framework 5.0 which you can download from here.<br>
https://dotnet.microsoft.com/en-us/download/dotnet/thank-you/runtime-5.0.17-windows-x64-installer<br>
(Without this you will not be able to import Ink source)<br>

### Visual Studio 2022
This plugin is distributed as source code which needs compiling.<br>
You will need to install Visual Studio 2022.<br>
https://visualstudio.microsoft.com/vs/<br>

## Installing the plugin

Make sure you have **.Net framework 5.0** and **Visual Studio 2022** installed.<br>

In your **project folder**, create a folder named **Plugins** and copy the Inkpot plugin there.<br>
Make sure the inkpot folder is called simply '**Inkpot**'.<br>

LIke this:<br>

    [Project Root Directory]/Plugins/Inkpot/

For an example of how this plugin should sit in your project see the InkpotDemo.<br>
https://github.com/The-Chinese-Room/InkpotDemo

## FAQ

### I cannot compile the plugin.
* Inkpot is distributed as C++ source code and will need compiling. <br>
Make sure you have **Visual Studio 2022** installed.<br>

* If you get this error _VisualStudioUnsupported x64 must be installed in order to build this target_.<br> 
For Unreal Engine 5.6, you need to be on v17.8 of the MSCV v143 toolchain. See here for more details.<br>
https://forums.unrealengine.com/t/visualstudiounsupported-x64-must-be-installed-trying-to-update-a-project-from-5-3-2-to-5-4-1/1835116/5

### My Ink files will not import.

* Make sure you have **.Net framework 5.0** installed.<br>
Inkpot uses Inklecate to compile the Ink files and it needs .net 5.0.<br>
Without this the Ink files will not import.<br>

* Inkpot has only been tested as an application plugin and not an engine plugin.<br>
It needs to be located in the project's plugins folder and not the engine plugin folder.<br>

* The plugin folder name should be simply 'inkpot',<br>
If you unzipped it directly to your project, it will likely be called 'inkpot-release'.<br>
eg <your project root>\plugins\inkpot<br>

* See the inkpotdemo for how it should look in your project.<br>
https://github.com/The-Chinese-Room/InkpotDemo<br>


### My Ink file changes will not update on reimport.
* Inky, the Ink script editor, will silently fail when saving to read only files.<br>
Some version control systems will make the file read only when you check them out, eg perforce.<br>
This can lead to some confusion when running Inky alongside Unreal & Inkpot.<br>


## Module Structure
Inkpot is made from three modules. <br>

### InkPlusPlus
InkPlusPlus is the name of the module that contains the C++ port of the Ink Engine Runtime which can be found here</br>
https://github.com/inkle/ink/tree/master/ink-engine-runtime

When porting the code from C# to C++ we came to the conclusion that we should keep the source as close to the original C# as possible to make updates easier. </br>
This has now been updated to Inkversion 21, Ink v1.1.1. </br>

### Inkpot 
A set of wrapper classes for InkPlusPlus to simplify Blueprint coding.<br>
Manages the lifetime of the Ink story and provides the interface for Ink variable setting, getting and change notification.<br>

### InkpotEditor 
This is the unreal editor facing part of Ink pot.</br>
This manages the defintion of data types in Unreal for inkpot and the compilation of new Ink scripts as they are added to the project.</br>
It also contains the unit tests for InkPlusPlus.</br>

## Other folders
There area couple of supporting folders that make up the Inkpot distribution.<br>

### TestInkSource
All the Unit tests for Ink.</br> 
Currently 180 tests.

### Third Party 
This contains a copy of InkleCate and is used by the script importer to compile the scripts.<br>

## Thanks to 
**[Inkle](https://www.inklestudios.com/)** </br>
for creating Ink. We've used this for much more than narrative.</br></br>
**[UnrealInk](https://github.com/DavidColson/UnrealInk)**</br>
for giving us a good basis for creating InkPlusPlus.</br></br>
**Colin, Seb & Canute at TCR**</br>
for the brilliant job in porting the Ink Engine Runtime to C++, along with the editor asset handling, unit tests and Unreal integration, most of the work here in fact.<br>
