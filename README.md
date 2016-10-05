# PrimeAPI
PrimeAPI is an API and a build system that allows you to write custom C++ code that interfaces with classes and functions from Metroid Prime, compile it, and run it ingame. It works by compiling your code to a .rel module, and patching the game's main executable to load and link in your code module at boot. It currently only supports the NTSC 0-00 GameCube version of Prime 1, though support for other versions and other Retro games could be added by building a list of symbols for those versions.

## Requirements
Currently an installation of CodeWarrior, the compiler that comes with the Dolphin SDK, is required; this is because compilers that are typically used for homebrew (such as devkitPPC) use a different C++ ABI so they aren't directly compatible with the existing game code.

Requirements:
* An installation of the Dolphin SDK
* CodeWarrior v2.3.3 (this version comes with the SDK; must be licensed)
* Python 3.x or higher

Set the DOLPHIN_ROOT environment variable to point to the root of the Dolphin SDK, and set the CWFOLDER environment variable to point to the root of your CodeWarrior installation.

## Writing your code
These are the steps to write a custom module for the game:
* Include PrimeAPI.h.
* Write a function with the following signature: "void _prolog()". This function must contain the INIT_MODULE macro. It is called when your module is first linked into the game so you can do any one-time initialization here, though keep in mind the linking is done very early in the game's boot sequence and many of the game's subsystems aren't initialized yet.
* Write any custom functions/classes you want; you can inherit from any of the game classes that have complete headers defined for them.
* To get your functions called in game, use the PATCH_SYMBOL macro. The macro takes two arguments. The first is the name of one of the functions from the game. The other is the name of your custom function. This will cause your function to be called in place of the original one. It's recommended that your replacement function has the same return type and arguments as the original function (keeping in mind that member functions have an implicit "this" argument). Your function is also allowed to call the original function that was patched over. Note that the function you patch over doesn't need to be defined in a PrimeAPI header; it just has to be present in the symbols .lst file.

## Compiling your code
* Run scripts/BuildModule.py. Pass in the path to the directory containing all your custom C++ source files as the first argument, and the path to the game's dol file as the second. You can specify a custom output path via the -o argument, but keep in mind a current limitation is your module must be named Mod.rel.
* BuildModule.py will create a patched .dol file that can load your custom code and will compile your code to a .rel file.
* Place the compiled rel file in the game's root directory and run the patched dol file.

## Example
This is an example of a very simplistic module that updates the player's health every frame and loops it between 1 and 500.

```cpp
#include <PrimeAPI.h>
#include <CStateManager.h>
#include <CPlayerState.h>
#include <CHealthInfo.h>

// Forward decls
class CPlayer;
void _prolog();
bool UpdateHealth(CPlayer*, CStateManager&);

// Patches
PATCH_SYMBOL(CPlayer::IsUnderBetaMetroidAttack(CStateManager&) const, UpdateHealth(CPlayer*, CStateManager&))

// Impls
void _prolog()
{
	MODULE_INIT;
}

bool UpdateHealth(CPlayer *pPlayer, CStateManager& rStateMgr)
{
	// Get health
	CHealthInfo *pHealthInfo = rStateMgr.GetPlayerState()->GetHealthInfo();
	float health = pHealthInfo->GetHealth();
	
	// Update health
	health += 0.2f;
	if (health >= 500.f)
		health = 1.f;
	
	pHealthInfo->SetHealth(health);
	
	// Let the caller know we are not being attacked by a beta metroid
	return false;
}
```
