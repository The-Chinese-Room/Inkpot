# Inkpot
Inkpot - An container for Ink in the Unreal engine
v 0.1.20

## Requirements 
Inkpot works with version 5.3 of Unreal.
Inkpot includes a port of Ink-engine-runtime version 20.
As it compiles the Ink source directly, it should be compatible with any Ink editor version as long as you only use features from Ink version 20.

## Module Structure

Inkpot is made from three modules. 

### InkPlusPlus
InkPlusPlus is the name of the module that contains the port of the Ink Engine Runtime which can be found here
https://github.com/inkle/ink/tree/master/ink-engine-runtime

When porting the code from C# to C++ we came to the conclusion that we should keep the source as close to the original C# as possible to make updates easier. 
That said we took the code in the summer of 2021 and have not had a need to update as it works for our purposes. 
The Ink version we are running is therefore behind that of the offical Ink release being at Inkversion 20.

### Inpot 
A set of wrapper classes for InkPlusPlus to simplify Blueprint coding.
This manages the lifetime of the Ink story and provides the interface for Ink variable setting, getting and change notification.

### InpotEditor 
This is the unreal editor facing part of Ink pot.
This manages the defintion of data types in Unreal for inkpot and the compilation of new Ink scripts as they are added to the project. It also contains the unit tests for InkPlusPlus. 

## Other folders
There area couple of supporting folders that make up the Inpot distribution 

### TestInkSource
All the Unit tests for Ink. 
Currently 168 tests.

### Third Party 
This contains a copy of InkleCate and is used by the script importer to compile the scripts.

## What we did not do
You will note that 2 of the tests fail, these are the JSON serialisation tests & we simply did not need to implement them in our usage case.
We have also not implemented custom function definition.




