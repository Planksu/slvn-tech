# slvn-tech
slvn-tech is a game engine utilizing Vulkan

# Status
Current main status: [![MSBuild Actions Status](https://github.com/Planksu/slvn-tech/workflows/MSBuild/badge.svg)](https://github.com/Planksu/slvn-tech/actions)

# Configuring / building
slvn-tech uses Premake5 as a build configuration tool. You need to download it from:
[Premake's website](https://premake.github.io/ "premake.github.io")

Move the premake5.exe into the cloned folder "slvn-tech". Using the cmd/powershell/any other terminal environment, run the command:
``` premake5.exe vs2019 ```

This will configure some Visual Studio 2019 project files for you to use. Then you can open the solution from the "project-files"
folder.
