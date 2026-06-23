# Wordle 猜词游戏 — 并行编程实验

基于 Qt 6 + QML 的 Wordle 猜词游戏，在两种游戏模式中分别融入 **OpenMP 多线程** 与 **MPI 多进程** 并行编程技术。

---

## 1. 实验内容

本实验在 Wordle 猜词游戏框架下，完成一个可交互的图形界面程序，并在不改变玩家操作方式的前提下，将并行计算融入游戏逻辑：

| 游戏模式 | 玩法说明 | 并行技术 |
|----------|----------|----------|
| **经典模式** | 单机随机抽取 5 字母单词作为答案 | **OpenMP**：多线程并行查词库、并行判定字母颜色 |
| **每日猜词** | 按当天日期生成全局统一的单词 | **MPI**：`MPI_Bcast` 广播每日单词；多进程分块查词 + `MPI_Allreduce` |

游戏规则与标准 Wordle 一致：5 个字母、最多 6 次猜测；绿色表示位置正确，黄色表示字母存在但位置错误，灰色表示不存在。

**约束与要求：**

- 词库为内置英文 5 字母单词表（`resources/words.txt`，共 535 词）
- 提交猜测必须在词库中，且不能重复猜测
- 经典模式使用 OpenMP（默认开启）；每日猜词模式使用 MPI 思想（可选编译 MPI 支持）
- 界面提示保持简洁：经典模式显示「单机随机单词」，每日猜词仅显示日期

---

## 2. 实验目的

1. 通过 Wordle 游戏场景，理解 **共享内存并行（OpenMP）** 与 **分布式内存并行（MPI）** 的适用场景差异
2. 掌握 OpenMP `#pragma omp parallel for` 在词库搜索、字母判定中的应用
3. 掌握 MPI 集体通信原语 `MPI_Bcast`、`MPI_Allreduce` 在「全局一致数据」与「分块协作计算」中的用法
4. 熟悉 Qt/QML 与 C++ 混合开发，以及 CMake 中 OpenMP、MPI 的可选集成方式

---

## 3. 实验环境

### 3.1 本地开发（推荐）

| 项目 | 要求 |
|------|------|
| 操作系统 | Windows 10/11、Linux 或 macOS |
| Qt | Qt 6.10+（含 Qt Quick 模块） |
| 编译器 | MinGW 64-bit / GCC / Clang |
| 构建工具 | CMake 3.16+ |
| OpenMP | 随编译器提供（MinGW-GCC 通常已支持） |
| MPI（可选） | MS-MPI、Open MPI 或 MPICH |

### 3.2 集群环境（MPI 多进程实验）

若需真实多进程运行每日猜词模式，可在 **3 个计算节点** 组成的集群上部署（各节点 Linux），例如：

- 阿里云 3 台 ECS（4 vCPU / 8 GiB）组成 MPI 集群
- 节点间 SSH 免密互通，共享同一可执行文件与词库资源

单机运行时，每日模式会 **模拟** `MPI_Bcast`（按日期本地生成相同单词），便于无集群环境下演示。

---

## 4. 实验步骤

### 4.1 搭建并行运行环境

#### OpenMP（经典模式，默认启用）

MinGW / GCC 一般自带 OpenMP，CMake 选项 `WORDLE_ENABLE_OPENMP=ON`（默认）即可。

#### MPI（每日猜词模式，可选）

**Windows（MS-MPI 示例）：**

1. 安装 [Microsoft MPI](https://learn.microsoft.com/zh-cn/message-passing-interface/microsoft-mpi)
2. 确保 `mpiexec` 在 PATH 中
3. 配置 CMake 时开启 MPI：

```bash
cmake -B build -DWORDLE_ENABLE_MPI=ON
```

**Linux 集群示例：**

1. 安装 Open MPI：`sudo apt install openmpi-bin libopenmpi-dev`
2. 配置 hostfile，确保各节点可 SSH 无密码登录
3. 在各节点同步项目并编译

### 4.2 项目结构与源代码说明

```
wordle_game/
├── main.cpp                 # 程序入口；MPI 非 0 号进程运行词库 worker
├── wordlegame.cpp/h         # 游戏逻辑、模式切换、棋盘与键盘状态
├── parallelengine.cpp/h     # 并行引擎：OpenMP 查词/判定，MPI 广播/分块查词
├── Main.qml                 # 主窗口
├── qml/                     # 棋盘、键盘、模式选择等 QML 组件
├── resources/words.txt      # 5 字母英文词库
├── CMakeLists.txt           # 构建配置（OpenMP / MPI 开关）
└── 实验模板/                # 课程实验手册与参考模板
```

**核心并行实现（`parallelengine.cpp`）：**

| 函数 | 说明 |
|------|------|
| `containsWord(..., useOpenMp, useMpi)` | 经典模式：`#pragma omp parallel for` 并行搜索词库；每日模式：MPI 分块查词 + `Allreduce` |
| `evaluateGuess(..., useOpenMp)` | 经典模式：OpenMP 并行判定字母状态 |
| `broadcastDailyWord()` | 每日模式：Rank 0 按日期生成单词，`MPI_Bcast` 广播至所有进程 |

**模式与并行映射（`wordlegame.cpp`）：**

- `normal`（经典）→ `useOpenMp() == true`
- `daily`（每日猜词）→ `useMpi() == true`

### 4.3 进行编译

**Qt Creator：**

1. 打开 `CMakeLists.txt` 所在目录
2. 选择 Kit（如 Desktop Qt 6.11 MinGW 64-bit）
3. 若需 MPI，在 CMake 配置中添加 `-DWORDLE_ENABLE_MPI=ON`
4. 点击「构建」

**命令行：**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DWORDLE_ENABLE_OPENMP=ON
cmake --build build
```

启用 MPI：

```bash
cmake -B build -DWORDLE_ENABLE_MPI=ON
cmake --build build
```

若遇 Qt 许可证检查问题，可临时设置：

```bash
set QTFRAMEWORK_BYPASS_LICENSE_CHECK=1   # Windows CMD
```

### 4.4 运行程序

**单机 GUI 运行（OpenMP 经典模式 + 模拟 MPI 每日模式）：**

```bash
# Windows
build\Desktop_Qt_6_11_1_MinGW_64_bit_Debug\appwordle_game.exe

# 或在 Qt Creator 中直接运行
```

**MPI 多进程运行（每日猜词模式，需启用 WORDLE_USE_MPI 编译）：**

```bash
mpiexec -n 4 ./appwordle_game
```

- Rank 0 进程启动 Qt 图形界面
- 其余 Rank 作为词库搜索 worker，响应 Rank 0 的 `MPI_Bcast` 查词请求

**操作说明：**

- 鼠标点击虚拟键盘，或直接用物理键盘输入 A–Z
- `Enter` 提交猜测，`Backspace` 删除字母
- 右上角图标切换明暗主题
- 「经典 / 每日猜词」按钮切换模式（自动开始新局）

---

## 5. 实验分析

### 5.1 并行策略分析

**OpenMP（经典模式）**

- **任务特点**：词库约 535 个单词，每次猜测需线性查找；字母判定为 5×5 小规模循环
- **并行方式**：`parallel for` 静态划分迭代空间，查词时使用 `critical` 保证找到即停
- **预期效果**：词库规模较小时加速不明显，但清晰展示 **共享内存、多线程协作** 的编程模型；线程数可在提交猜测后通过内部 `parallelInfo` 观察

**MPI（每日猜词模式）**

- **任务特点**：所有玩家/进程需获得 **相同的每日单词**（全局一致）；查词时可分块并行
- **并行方式**：
  - 开局：`MPI_Bcast` 由 Rank 0 广播 5 字母单词
  - 查词：各 Rank 搜索本地词库分块，`MPI_Allreduce(MAX)` 汇总是否命中
- **预期效果**：多进程下查词延迟随进程数增加而分摊；单进程时退化为本地模拟广播

### 5.2 可观察现象

| 场景 | 现象 |
|------|------|
| 经典模式提交猜测 | 内部记录 OpenMP 线程数（如「OpenMP parallel for: 8 线程并行搜索词库」） |
| 每日模式切换 / 开局 | 内部记录 MPI 广播说明（单进程为「模拟 MPI_Bcast」） |
| 每日模式 + 多进程 MPI | Rank 0 显示 GUI，后台 worker 参与分块查词 |
| 同一日期、不同机器 | 每日猜词模式得到相同答案单词 |

### 5.3 性能说明

本实验词库规模有限，**不以性能加速为主要目标**，重在理解并行原语在游戏逻辑中的嵌入方式。若需量化加速比，可扩大词库或增加查词调用次数后对比串行与并行耗时。

---

## 6. 问题与讨论

1. **为何经典模式选 OpenMP，每日模式选 MPI？**  
   OpenMP 适合单进程内多线程共享词库；MPI 适合多节点间广播全局一致数据并分块协作，与「每日一词全员同步」的场景更契合。

2. **词库很小，OpenMP 有意义吗？**  
   实际加速有限，但作为课程实验可直观看到 `parallel for` 的写法与线程协作；生产环境通常对更大数据集或更耗时任务使用并行。

3. **未安装 MPI 能否完成实验？**  
   可以。每日模式在单进程下模拟 `MPI_Bcast`；OpenMP 部分在默认编译下即可体验。

4. **MPI 模式下为何只有 Rank 0 有界面？**  
   Qt GUI 仅在主进程运行；其他 Rank 执行 `runDictionaryWorker()` 专门处理分布式查词，这是 **MPI + GUI 混合程序** 的常见架构。

5. **Windows 控制台中文乱码？**  
   程序已在 `main.cpp` 中设置 `NO_COLOR=1` 减少终端干扰；GUI 界面不受影响。

---

## 参考资料

- [Qt 6 文档](https://doc.qt.io/qt-6/)
- [OpenMP 规范与教程](https://www.openmp.org/specifications/)
- [Microsoft MPI 文档](https://learn.microsoft.com/zh-cn/message-passing-interface/microsoft-mpi)
- [Open MPI 官网](https://www.openmpi.org/)
- Wordle 游戏规则参考：[Power Language Wordle](https://www.nytimes.com/games/wordle)

---

## 仓库信息

- **GitHub**：https://github.com/yezhuqiu111/wordle_game
- **实验手册模板**：见 `实验模板/实验手册.docx`
