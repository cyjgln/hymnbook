# 诗歌本 HymnBookApp

![license](https://img.shields.io/badge/license-MIT-blue)
![version](https://img.shields.io/badge/version-1.2.0-green)

教会诗歌本软件 — 用于聚会、主日敬拜、祷告会、小组聚会等场景。

主要功能：浏览目录、查看歌谱、搜索诗歌、全屏投影、新增/修改/删除歌谱、批量导入。

---

## 编译方法

### 环境要求

- **Qt 6.x**（最低 6.2）
- **CMake** ≥ 3.16
- **C++17** 编译器

### 编译步骤

```bash
# 克隆或进入项目目录
cd HymnBookApp

# 配置
mkdir build && cd build
cmake ..

# 编译
cmake --build .

# 运行
./HymnBookApp
```

### Windows 下编译

1. 安装 [Qt 6](https://www.qt.io/download)（选择 MSVC 或 MinGW 版本）
2. 安装 [CMake](https://cmake.org/download/)
3. 打开 "Qt 6.x (MSVC 20xx)" 命令行提示符
4. 进入项目目录，执行：

```bat
cd HymnBookApp
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc20xx_64
cmake --build .
release\HymnBookApp.exe
```

---

## 运行方法

### 首次运行

程序启动时会自动在用户数据目录创建 `hymns.json`。如果是首次运行，会创建空数据文件。

### 目录结构

```
HymnBookApp/
├── HymnBookApp.exe         # 可执行文件
├── data/
│   └── hymns.json           # 诗歌数据（JSON 格式）
└── (其他库文件)
```

---

## 功能说明

| 功能 | 操作方法 |
|------|----------|
| 浏览目录 | 启动后默认显示目录页，点击歌名进入歌谱页 |
| 查看歌谱 | 歌谱页显示图片，支持缩放、拖动 |
| 搜索诗歌 | Ctrl+F 打开搜索，输入关键字实时过滤 |
| 上一首/下一首 | 歌谱页点击按钮，或按 ← / → 方向键 |
| 返回目录 | 点击「返回目录」按钮，或按 Esc |
| 放大/缩小 | 鼠标滚轮，或 Ctrl++ / Ctrl+-，或点击工具栏按钮 |
| 全屏投影 | 按 F11 切换全屏 |
| 新增歌谱 | 目录页点击「新增歌谱」，输入歌名、选择图片 |
| 修改歌谱 | 歌谱页点击「编辑」，修改歌名或替换图片 |
| 删除歌谱 | 编辑弹窗中点击「删除歌谱」|

---

## 快捷键一览

| 快捷键 | 功能 |
|--------|------|
| `Ctrl + F` | 打开搜索 |
| `←` (左方向键) | 上一首 |
| `→` (右方向键) | 下一首 |
| `Esc` | 返回目录 |
| `Ctrl + +` | 放大图片 |
| `Ctrl + -` | 缩小图片 |
| 鼠标滚轮 | 放大 / 缩小图片 |
| `F11` | 全屏切换 |

---

## 数据维护

### hymns.json 格式

```json
[
  {
    "id": 1,
    "title": "奇异恩典",
    "imagePath": "images/amazing_grace.png",
    "pageNumber": 1
  }
]
```

| 字段 | 说明 |
|------|------|
| `id` | 唯一标识（自动生成） |
| `title` | 歌名 |
| `imagePath` | 图片路径（相对于 data/ 目录的父目录） |
| `pageNumber` | 页码（自动维护） |

### 运行时数据存储

数据（`hymns.json`）和歌谱图片默认保存在操作系统的用户应用数据目录：

| 系统 | 路径 |
|------|------|
| Linux | `~/.local/share/HymnBook/HymnBookApp/` |
| Windows | `C:/Users/\<用户名\>/AppData/Local/HymnBook/HymnBookApp/` |

目录结构：
```
├── data/
│   └── hymns.json          # 诗歌数据
└── images/                 # 歌谱图片（新增时自动放入）
    ├── amazing_grace.png
    └── ...
```

### 添加图片

在软件中使用「新增歌谱」或「批量导入」功能选择图片文件，程序会自动复制到上述 `images/` 目录。

### 注意事项

- 图片路径使用相对路径，程序会自动根据 `hymns.json` 所在位置解析
- 新增歌谱时，图片会自动复制到用户数据目录的 `images/` 下，不会影响源码目录
- 修改 `hymns.json` 后需重启程序或触发数据刷新

---

## 技术栈

- **框架**: Qt 6 (Widgets)
- **语言**: C++17
- **构建**: CMake
- **数据**: JSON (本地文件)
- **图片**: QGraphicsView + QPixmap

## 许可证

本项目仅供教会内部使用。
