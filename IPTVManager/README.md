# IPTVManager Refactored

Qt6 C++ 桌面应用，用于管理 IPTV 频道列表。

## 功能特性

- 频道导入/导出（M3U、TXT、CSV、XLSX）
- 流媒体探测（FFmpeg）
- 视频预览（OpenGL）
- 多平台支持（Windows、macOS、Linux）

## 构建要求

- CMake 3.19+
- Qt 6.8+
- FFmpeg（avformat、avutil、avcodec、swscale）
- pkg-config（Linux/macOS）或手动指定 FFmpeg 路径（Windows）

## 构建步骤

```bash
# 克隆项目（含子模块）
git clone --recursive <repo-url>
cd IPTVManagerRefactored

# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build --config Release

# 运行
./build/IPTVManager
```

### Windows 特殊配置

如果 FFmpeg 未安装在系统路径，需指定路径：

```bash
cmake -B build -DFFMPEG_DIR="C:/path/to/ffmpeg"
```

### macOS 特殊配置

使用 Homebrew 安装 FFmpeg：

```bash
brew install ffmpeg
cmake -B build
```

## 项目结构

```
IPTVManagerRefactored/
├── src/
│   ├── core/          # 配置 + 日志模块
│   ├── database/      # 数据库访问层
│   ├── network/       # 网络请求
│   ├── multimedia/    # FFmpeg 集成
│   ├── export/        # 导出功能
│   ├── logic/         # 业务逻辑
│   ├── platform/      # 平台抽象
│   └── ui/            # 界面层
├── tests/             # 单元测试
├── resources/         # 资源文件
└── third_party/       # 第三方库
```

## 测试

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest
```

## 许可证

[待定]
