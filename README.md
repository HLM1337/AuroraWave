[点击查看中文说明](./README_CN.md)

# CS2 DX11 Hook

This project is a DirectX 11-based CS2 game assistant, integrated with an ImGui graphical interface and supporting various in-game features.

## Features
- DX11 rendering hook and ImGui interface integration
- Real-time drawing of player/enemy info (boxes, skeletons, rays, health, distance, weapon, etc.)
- Separate drawing options for teammates and enemies
- Aimbot, FOV, recoil compensation, auto anti-flash, and more
- Customizable colors, styles, corner radius, thickness, etc.
- Chinese font display support
- Display of process info, base address, coordinates, and other debugging info
- Uses MinHook for API hooking

## Directory Structure
```
├── game/
│   ├── Base.h           # Basic data structures
│   ├── Memory_LC.cpp/h  # Memory operations and features
│   ├── UI.cpp/h         # ImGui UI and drawing
│   ├── game.cpp/h       # DX11 hook main logic
│   ├── ImGui/           # ImGui and its DX11/Win32 backends
│   ├── MinHook/         # MinHook source code
│   └── output/          # Auto-generated offsets/interfaces (fetched via [cs2-dumper](https://github.com/a2x/cs2-dumper))
├── MFC_Hook.sln         # Solution file
```

## Dependencies
- Windows 10/11 x64
- Visual Studio 2019/2022
- DirectX 11 SDK
- ImGui (source included)
- MinHook (source included)

## Build Instructions
1. Open the `MFC_Hook.sln` solution in Visual Studio.
2. Select the Release x64 configuration.
3. Build the `game` project to generate the DLL.

## Usage
1. Inject the compiled DLL into the CS2 game process.
2. The DX11 hook and ImGui interface will appear automatically.
3. Customize features and parameters via the UI.
4. Exit via the UI button or by unloading the DLL.

> ⚠️ This project is for learning and technical exchange only. Do not use for illegal purposes!

## Credits
- [ocornut/imgui](https://github.com/ocornut/imgui) - GUI library
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook) - API hook library
- [a2x/cs2-dumper](https://github.com/a2x/cs2-dumper) - For generating external offsets/interfaces in the output folder

---

For questions, please submit an Issue or PR.
