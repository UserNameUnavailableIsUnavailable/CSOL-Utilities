# CSOL 集成工具控制器

控制器是集成工具中控制执行器运行模式的模块。

## 构建

从 v1.5.3 开始，控制器构建方式改为 CMake，以提供更快的构建速度。

由于 [ONNXRuntime](https://github.com/microsoft/onnxruntime) 在 Windows 平台下仅提供 MSVC 支持，因此控制器目前仅支持使用 Visual Studio 2022 进行构建。
本项目使用 vcpkg 进行依赖管理，请确保在构建项目之前，已正确安装并配置 vcpkg。
vcpkg 已经预装到 Visual Studio 2022 中，因此若无特殊需求，可直接使用预装的 vcpkg。

> [!NOTE]
> 如果您使用 Visual Studio 中附带的 vcpkg，则无需额外配置。进入 Visual Studio 的 Developer Shell 后，`VCPKG_ROOT` 会自动设置。

首先，进入 Visual Studio 开发者 PowerShell 后，查看 `$Env:VCPKG_ROOT` 是否正确配置：

```powershell
echo $Env:VCPKG_ROOT
```

以上确认无误后即可使用 CMake 构建本项目，构建过程中将自动下载缺失的依赖。
**因此在构建时，需确保网络畅通，否则构建将会失败。**
若网络不畅，建议使用 Clash 开启 **虚拟网卡模式** 后再进行构建。
控制器提供 `Debug` 和 `Release` 两种构建类型。
以 `Release` 为例，构建方式如下：

```powershell
cd Controller
cmake -S . -B build
cmake --build build --config Release
```

> [!NOTE]
> 需要指出，本项目用到的部分依赖并未在 vcpkg 中提供，因此 `cmake -S . -B build` 会从相应的 GitHub 仓库下载这些依赖。

## 模型部署

构建完成后，控制器需要模型文件才能正常工作。
模型文件可在 [CSOL-Utilities-Controller-Models](https://huggingface.co/UserNameIsUnavailable/CSOL-Utilities-Controller-Models) 获得。
运行 `DeployModels.ps1` 脚本可将模型文件部署到构建目录中：

```powershell
.\DeployModels.ps1 -Path <模型文件所在目录> -Target Release
```

`-Path` 参数指定模型文件所在目录，`-Target` 参数指定构建类型。
