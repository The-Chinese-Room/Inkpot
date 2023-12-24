# Inkpot
**Inkpot** - An container for **Ink** within the Unreal Engine developed by [The Chinese Room](https://www.thechineseroom.co.uk/).<br><br>
This is a plugin for Unreal Engine 5.3 or later.<br>
This is version 0.3.20 of the plugin.</br>

Inkpot is a wrapper for the wonderful narrative scripting language **Ink** developed by [Inkle Studios](https://www.inklestudios.com/ink/).<br>

### Changes from 0.2.20
JSON serilaisation now functional.

### Changes from 0.1.20
External functions are now implemented, along with functional tests.

## Requirements 
Inkpot works with version 5.3 of Unreal.<br>
Inkpot includes a C++ port of Ink-engine-runtime version 20, which can be found in the InkPlusPlus module.<br>
As Inkpot compiles the Ink source directly on import, it should be compatible with any Ink editor version as long as you only use features from Ink version 20.<br>

### .Net framework 3.1
InkleCate requires the .net framework 3.1 which you can download from here.<br>
https://dotnet.microsoft.com/en-us/download/dotnet/thank-you/runtime-3.1.32-windows-x64-installer <br>
(Without this you will not be able to import Ink source)<br>

### Visual Studio 2022
This plugin is distributed as source code which needs compiling.<br>
You will need to install Visual Studio 2022.<br>
https://visualstudio.microsoft.com/vs/<br>

## Installing the plugin
Copy to the plugins folder of your project.<br>

    [Project Root Directory]/Plugins/Inkpot/

or add via git as a submodule.<br><br>
For an example of how this plugin should sit in your project see the InkpotDemo.<br>
https://github.com/The-Chinese-Room/InkpotDemo

## Module Structure
Inkpot is made from three modules. <br>

### InkPlusPlus
InkPlusPlus is the name of the module that contains the C++ port of the Ink Engine Runtime which can be found here</br>
https://github.com/inkle/ink/tree/master/ink-engine-runtime

When porting the code from C# to C++ we came to the conclusion that we should keep the source as close to the original C# as possible to make updates easier. </br>
That said we took the code in the summer of 2021 and have not had a need to update as it works for our purposes. </br>
The Ink version we are running is therefore behind that of the offical Ink release being at Inkversion 20.</br>

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
Currently 170 tests.

### Third Party 
This contains a copy of InkleCate and is used by the script importer to compile the scripts.<br>

## Thanks to 
**[Inkle](https://www.inklestudios.com/)** </br>
for creating Ink. We've used this for much more than narrative.</br></br>
**[UnrealInk](https://github.com/DavidColson/UnrealInk)**</br>
for giving us a good basis for creating InkPlusPlus.</br></br>
**Colin, Seb & Canute at TCR**</br>
for the brilliant job in porting the Ink Engine Runtime to C++, along with the editor asset handling, unit tests and Unreal integration, most of the work here in fact.<br>
