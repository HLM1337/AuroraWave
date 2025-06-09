# AuroraWave

本项目为基于 DirectX 11 的 CS2 游戏辅助，集成了 ImGui 图形界面，支持多种游戏功能。

## 功能特性
- DX11 渲染劫持与 ImGui 界面集成
- 实时绘制敌我方信息（方框、骨骼、射线、血量、距离、武器等）
- 支持队友与敌人独立绘制选项
- 自瞄、FOV、后坐力补偿、自动防闪等功能
- 支持自定义颜色、风格、圆角、粗细等参数
- 支持中文字体显示
- 进程信息、基址、坐标等调试信息展示
- 采用 MinHook 进行 API Hook

## 目录结构
```
├── game/
│   ├── Base.h           # 基础数据结构
│   ├── Memory_LC.cpp/h  # 内存操作与功能实现
│   ├── UI.cpp/h         # ImGui 界面与绘制
│   ├── game.cpp/h       # DX11 Hook 主逻辑
│   ├── ImGui/           # ImGui 及其 DX11/Win32 后端
│   ├── MinHook/         # MinHook 源码
│   └── output/          # 自动生成的偏移/接口等数据（由 [cs2-dumper](https://github.com/a2x/cs2-dumper) 获取）
├── MFC_Hook.sln         # 解决方案文件
```

## 依赖环境
- Windows 10/11 x64
- Visual Studio 2019/2022
- DirectX 11 SDK
- ImGui（已集成源码）
- MinHook（已集成源码）

## 编译方法
1. 使用 Visual Studio 打开 `MFC_Hook.sln` 解决方案。
2. 选择 Release x64 配置。
3. 编译 `game` 项目，生成 DLL。

## 使用说明
1. 编译生成的 DLL 需以注入方式加载到 CS2 游戏进程中。
2. 注入后会自动 Hook DX11 并弹出 ImGui 界面。
3. 可通过界面自定义各项功能参数。
4. 退出可通过界面按钮或卸载 DLL。

> ⚠️ 本项目仅供学习与技术交流，请勿用于非法用途！

## 鸣谢
- [ocornut/imgui](https://github.com/ocornut/imgui) - 图形界面库
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook) - API Hook 库
- [a2x/cs2-dumper](https://github.com/a2x/cs2-dumper) - 用于获取 output 文件夹内的外部偏移/接口数据

---

如有问题欢迎提交 Issue 或 PR。
