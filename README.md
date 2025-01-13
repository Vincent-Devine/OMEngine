# OMEngine
One More Engine is a 3D engine made with Vulkan

## Table of content
- [Getting Started](#getting-started)
- [Technology](#technology)
- [Credit](#credit)

## Getting Started
### Clone
```bash
git clone git@github.com:Vincent-Devine/OMEngine.git
```

### Setup Vulkan
If you don't have the Vulkan SDK installed on your PC. You can download [here](https://vulkan.lunarg.com/sdk/home#windows). <br>
After installing it, **make sure** that the SDK installation has added a system variable named ```VULKAN_SDK```, which should point
directly to the SDK path.<br>
**If not**, you have two options:
1. Add the system variable manually.
2. Modify the project properties to set your path to the Vulkan SDK.

## Technology
- Engine Language: C++ *20*
- OS: Windows
- Versionning: [Github](https://github.com/Vincent-Devine/OMEngine)
- Editor: [Visual Studio 2022](https://visualstudio.microsoft.com/fr/vs/)

### Library Used
- Graphic API: [Vulkan](https://vulkan.lunarg.com/home/welcome) *v1.3.236.0*
- Window: [GLFW](https://github.com/glfw/glfw) *v3.4*
- Texture loader: [stb_image](https://github.com/nothings/stb) *v2.28*
- Logger: [OMLogger](https://github.com/Vincent-Devine/OMLogger) *v1.0*

## Credit
Author: **Vincent DEVINE**<br>
Project Start: 05/01/2025

### Specials Thanks
- [Vulkan-Tutorial.com](https://vulkan-tutorial.com/Introduction)
- [Introduction to vulkan by Intel](https://github.com/GameTechDev/IntroductionToVulkan)
- [Youtuber: The Cherno](https://www.youtube.com/@TheCherno)
- [Youtuber: Prog'Z](https://www.youtube.com/@ProgZ)