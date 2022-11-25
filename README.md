<div align="center">
<img src="Internal/ReadMe/HydrillLogo.png" width="600" height="600" />
</div>

<br/>

# What is Hydrill ?
**Hydrill** is a 3D game engine (for WIndows platform only) providing simple and friendly tools to realise the game that you want. At the end, you can export your game, play and share it to your friends.

<br/>

# Our HyTeam

For this project, we were 3 programmers : 

- [**Sasha Legrand**](https://github.com/saslegrand)
- [**Noé Mouchel**](https://github.com/NoeMouchel)
- [**Guillaume Chiclet**](https://github.com/GuillaumeChiclet).

<br/>

# Main Modules

Hydrill is composed of 4 main modules :
- The **Editor** (Executable)
    > *Interface to create your game. Create your scenes, place your entities and use the given tools to make the game you dream of.*
- The **Game** (Executable)
    > *Simple application to run your game. When you build your project, it will use this application to run properly.*
- The **Engine** (Dynamic library)
    > *This is the **Core module** -> This module is in charge of the management of the resources, the entities and the flow control of your game. Most of the time you will interact indirectly with it.*
- The **Scripting** (Dynamic library)
    > *Moving with inputs, react to a collision or switch to another scene, this is where you will create your own behaviors for your game entities. You can also reflect values to tweak them directly in the editor.*

<br/>

# How to use ?

You want to give it a try? - Follow the instructions below :

### **Compiling from source**
---
Go to our [**Github**](https://github.com/saslegrand/HydrillEngine) and clone the project on your Desktop. Once you are in the repository, run the **LaunchBlank.bat** file to extract *Dependencies.zip* file, clean the old binaries and launch the **HydrillProject** and **HydrillScripting** visual studio solutions.

Given the **MSVC** version you are using, be sure to have the **ATL build tools installed** (relative to the current *MSVC* version you are using). You can find them in the *Individual components* section of the *VS Installer* application:

<p align="center"><img src="Internal/ReadMe/ATL142.png"  width="450"/> 
</p>

Once you are here, you first have to compile the **HydrillProject** solution *(Editor, Engine and Game projects)*. Then you can compile the **HydrillScripting** solution (Dependency with the *Engine*).

<br/>

### -> *Debug/Release configuration*
If you compile with the **Debug** or **Release** configuration, you can run the *Editor* project on the **HydrillProject** solution and start creating your new game.

### -> *Standalone configuration*
If you compile with the **Standalone** configuration, working directories are updated. You need to directly run the *Editor.exe* in the Binaries folder to run the program. You can also run the *ReleaseBuilder.exe* to create a release of the Editor and launch the *Editor.exe* in the created *Standalone/Binaries* folder.\
/!\ If you want to use the scripting solution, you need to select the **StandaloneFinal** configuration or it won't work.

<br/>

***Notes*** : 
 - The **HydrillScripting** solution is not required to run the *Editor* or the *Game* but is necessary to use your own behaviors (if not compile, your scripts will not be detected).
  - If you want to use the scripting **Hot-reload**, you must run the program without the debugger attached (*Ctrl + F5*). Else an error will occur because you tried to override the *PDB* file attached to the *Scripting DLL* while in use.

<br/>

### **Release version**
---
Go to our [**Github Release**](https://github.com/saslegrand/HydrillEngine/releases/tag/published) and download the latest release *.ZIP* file.

Here, you simply have to extract the folder and run the **Editor.exe** shortcut to run the editor and have fun.

You can also download our game assets folder here : (https://drive.google.com/file/d/1KI3pAWKekRPo-Kmha2kKtrJ9FJslMaLF/view?usp=sharing)

<br/>

# Preview

<div align="center">
<img src="Internal/ReadMe/EditorPreview.png"/>
</div>

<br/>
<br/>

# Dependencies
- ### [**OpenGL Core 4.6**](https://glad.dav1d.de/) (Rendering library)
- ### [**GLFW 3.3.6**](https://www.glfw.org/) (OpenGL library)
- ### [**Refureku**](https://github.com/jsoysouvanh/Refureku) (Reflection library)
- ### [**ImGui**](https://github.com/ocornut/imgui) (Graphic interface library)
- ### [**PhysX SDK 4.1**](https://github.com/NVIDIAGameWorks/PhysX) (Physic library)
- ### [**STB Image**](https://github.com/nothings/stb) (Image loader library)
- ### [**Freetype**](https://github.com/freetype/freetype) (Font loader library)
- ### [**MiniAudio**](https://github.com/mackron/miniaudio) (Spatialized sound library)
- ### [**Assimp**](https://github.com/assimp/assimp) (Model loader library)
