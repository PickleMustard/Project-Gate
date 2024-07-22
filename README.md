# Gate of Scalad (WIP Name)
Adventure through procedurally generated rooms and fight unique enemies in this rogue-lite tactical rpg. Command your units across the different geographic features that generate in specialized levels. Equip them with varied and fun equipment to best the grunts and bosses you encounter. Can you help them escape?

This project is not accepting contributions. It is a personal project to better my skills in C++ and C#, better my understanding of Godot, and reinforce computer graphics and linear algebra concepts.

## Contents
* Main Godot project files under the "game-files" directory
* GDExtension files under /Gate-of-Scalad/src
* Sconstruct python make system to build the C++ GDExtension tool

## Usage
### Building
** Note: This has only been tested on Linux **
1. To build the GDExtension for level generation, ensure python3 is installed on the system with the pip module SCons:latest installed
2. At the top level of the directory, use the command
```bash scons platform={platform_name}```
to build the GDextension for your current platform
3. Open the project in Godot. If the objects defined under src are available to add to a scene, the extension has compiled properly

Attribution: Godot-Foundation for forked [godot-cpp-template repository](https://github.com/godotengine/godot-cpp-template)

### Godot
There is not a fully built release available to download and play. To test the game in its current state, please follow the following instructions:
1. Download the latest Godot Engine - .NET build. Ensure it is the .NET build as the project makes extensive use of the language
2. Open the repository through Godot using either the GUI Project Manager or use the command `godot --path <directory> --editor` to get to the editor
3. This will allow you to play with terrain generation variables and customize it
4. To interact with the game, either click the play button in the top right of the editor or use the command `godot --path <directory>`
