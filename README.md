# peculiarc（或 cjk‑preproc）  
**把中文写法的 C/C++ 源码转换成普通 C/C++ 的前置处理器**  

---  

| 📦 | **版本** | 0.9.0 |
|---|---|---|
| 🛠️ | **语言** | C++17 |
| 📂 | **支持平台** | Windows / Linux / macOS（任何可以使用 `cl`、`gcc`、`clang` 编译器的系统） |
| 🔧 | **依赖** | 仅 C++ 标准库，无第三方库 |
| ⚖️ | **许可证** | **Apache License 2.0** |
| 🚀 | **主页** | https://github.com/ScriptForge666/Jing-Sharp.git |

> **一句话概括**：把「全角」符号、中文的 `井` 预处理标记、以及 `def` 关键字自动转换成标准的 C/C++，随后交给系统里找到的编译器完成编译。

---

## 目录
1. [项目简介](#项目简介)  
2. [主要功能](#主要功能)  
3. [安装方式](#安装方式)  
4. [快速上手](#快速上手)  
5. [命令行参数](#命令行参数)  
6. [支持的语法转换](#支持的语法转换)  
7. [示例代码](#示例代码)  
8. [源码编译](#源码编译) 
9. [许可证](#许可证)  

---

## 项目简介
`peculiarc`（中文名字可写作 **cjk‑preproc**）读取 **中文风格** 的 C/C++ 源文件，将其中的全角符号、特殊关键字等全部替换为普通 ASCII 形式，生成一个临时文件 `<原文件>_pre.<扩展名>`，然后自动调用系统中可用的 C/C++ 编译器（`cl`、`clang`、`gcc` 中的第一个）完成编译。  

常见的中文写法包括：

| 中文写法 | 替换后 |
|----------|--------|
| `（`、`）`（全角圆括号） | `(`、`)` |
| `【`、`】`（全角方括号） | `[`、`]` |
| 行首的 `井`（可前置空格） | `#`（保留原缩进） |
| `井《include》file.h` 或 `#《include》file.h` | `#include <file.h>` |
| `井"include" "file.h"` | `#include "file.h"` |
| `def main(` | `int main(` |
| 其它行首的 `def`（不包括 `def main`）| `auto`（其余代码保持不变） |

> **注意**：本工具默认源文件为 UTF‑8 编码，这是目前大多数编辑器的默认设置。

---

## 主要功能
| ✅ | 功能描述 |
|---|----------|
| ✅ | 零运行时依赖，仅使用 C++ 标准库 |
| ✅ | 自动检测平台上可用的编译器（MSVC → Clang → GCC） |
| ✅ | 全角圆括号、方括号 → ASCII 对应字符 |
| ✅ | `井` → `#`（包括 `井《include》`、`井"include"` 两种写法） |
| ✅ | `def main` → `int main`；其余 `def` → `auto` |
| ✅ | 自动生成临时文件并在编译完成后默认删除（`--keep-temp` 可保留） |
| ✅ | 任意编译器参数均可透传，例如 `-O2 -Wall -Iinc/` |
| ✅ | 完整的错误提示（文件不存在、未检测到编译器、正则错误等） |
| ✅ | 完全基于 C++17，跨平台编译无需额外配置 |

---

## 安装方式

### 1. 直接下载二进制（推荐）

| 系统 | 下载链接 |
|------|----------|
| Windows（x64） | `peculiarc-windows-x64.zip` |
| Linux（x86_64） | `peculiarc-linux-x86_64.tar.gz` |
| macOS（Apple Silicon） | `peculiarc-macos-arm64.tar.gz` |

#### 安装步骤
1. 下载对应平台的压缩包。  
2. 解压到任意目录（建议放入系统 `PATH` 目录，如 Windows 放 `C:\Program Files\peculiarc\`，Linux/macOS 放 `/usr/local/bin/`）。  
3. 验证安装是否成功：

   ```bash
   peculiarc --version
   ```

   若出现类似 `peculiarc version 0.9.0` 的输出，则说明安装成功。

### 2. 从源码编译（适用于所有平台）

> 下面的章节 **[源码编译]** 中有详细说明，这里只给出概览。

1. 安装 **C++17 编译器**（`g++`、`clang++` 或 MSVC）。  
2. （可选）安装 **CMake ≥ 3.15**。  
3. 克隆仓库并编译：

   ```bash
   git clone https://github.com/yourname/peculiarc.git
   cd peculiarc
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build .
   ```

   编译完成后，`build/` 目录下会产生可执行文件 `peculiarc`（Windows 为 `peculiarc.exe`）。

---

## 快速上手

```bash
# 1️⃣ 编写一个中文风格的源文件（example.cj）
cat > hello.cj <<'EOF'
井《include》iostream
def main()：
    std::cout << "你好，世界！" << std::endl；
EOF

# 2️⃣ 用 peculiarc 编译
peculiarc hello.cj -O2 -Wall

# 3️⃣ 运行生成的可执行文件
./a.out            # Linux/macOS
a.exe              # Windows (MSVC/MinGW)
```

`peculiarc` 会自动完成以下转换：

```cpp
// 生成的 hello_pre.cj（临时文件）
#include <iostream>

int main() {
    std::cout << "你好，世界！" << std::endl;
}
```

随后交给系统默认的编译器完成编译，生成最终可执行文件。

---

## 命令行参数

| 参数 | 功能 |
|------|------|
| `--keep-temp` | 编译结束后 **保留** 生成的临时文件 `<源文件>_pre.*`，便于调试。 |
| `-v` / `--version` | 输出版本信息并退出。 |
| `-h` / `--help` | 显示帮助信息并退出。 |
| `<source>` | 待编译的中文风格源码路径（必填）。 |
| 其它任意不以 `-` 开头的参数 | 原样转交给底层编译器（如 `-O3 -std=c++20 -Iinclude/`）。 |

**示例**：保留临时文件并使用 Clang 编译（若系统中有 Clang）：

```bash
peculiarc --keep-temp demo.cj -std=c++23 -Wall
```

---

## 支持的语法转换

| 正则（源码中使用） | 中文写法 | 替换后 |
|-------------------|----------|--------|
| `re_left_parenthesis`  | `（` | `(` |
| `re_right_parenthesis` | `）` | `)` |
| `re_left_bracket`      | `【` | `[` |
| `re_right_bracket`     | `】` | `]` |
| `re_hash_line`         | 行首可带空格的 `井` | `#`（保留原缩进） |
| `re_include_angle`     | `井《include》file.h` / `#《include》file.h` | `#include <file.h>` |
| `re_include_quote`     | `井"include" "file.h"` / `#"include" "file.h"` | `#include "file.h"` |
| `re_def_main`          | `def main(`（可能前置空格） | `int main(` |
| `re_def_other`         | 其他行首的 `def`（不包括 `def main`） | `auto`（其余代码保持原样） |

> 只会对 **全角** 符号进行替换，半角 ASCII 本身不受影响。

---

## 示例代码

### 示例 1：最基本的“Hello World”

```c
// hello.cj
井《include》iostream

def main()：
    std::cout << "你好，世界！" << std::endl；
```

运行 `peculiarc hello.cj` 后得到的临时文件（`hello_pre.cj`）大致如下：

```cpp
#include <iostream>

int main() {
    std::cout << "你好，世界！" << std::endl;
}
```

---

### 示例 2：使用全角方括号访问数组

```c
// array.cj
井《include》cstdio

def main()：
    int nums【5】 = {1,2,3,4,5};
    for (int i = 0; i < 5; ++i)：
        std::printf("%d ", nums【i】);
    std::printf("\n");
```

编译后等价于：

```cpp
#include <cstdio>

int main() {
    int nums[5] = {1,2,3,4,5};
    for (int i = 0; i < 5; ++i) {
        std::printf("%d ", nums[i]);
    }
    std::printf("\n");
}
```

---

### 示例 3：`def` 与类型推导

```c
// utils.cj
def add(int a, int b)：
    return a + b;

def main()：
    auto sum = add(3, 4);
    std::cout << sum << std::endl;
```

转换后：

```cpp
auto add(int a, int b) {
    return a + b;
}

int main() {
    auto sum = add(3, 4);
    std::cout << sum << std::endl;
}
```

---

## 源码编译

> **前置条件**  
> - C++17 编译器（`g++`、`clang++` 或 MSVC）  
> - （可选）CMake ≥ 3.15  

### 方法 A – 单行编译（无需 CMake）

```bash
# Linux / macOS
g++ -std=c++17 -O2 -Wall -pedantic -o peculiarc main.cpp

# Windows (MSVC)
cl /std:c++17 /O2 /EHsc main.cpp /Fe:peculiarc.exe
```

> `main.cpp` 为本仓库根目录下的完整源码（即你提供的那段代码）。

### 方法 B – 使用 CMake（推荐，便于 IDE 调试）

```bash
git clone https://github.com/ScriptForge666/Jing-Sharp.git
cd peculiarc
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

编译完成后，`build/` 目录下会出现可执行文件 `peculiarc`（Windows 为 `peculiarc.exe`）。

### 运行测试（可选）

仓库自带 `tests/` 目录，里面放了一些示例源文件。执行：

```bash
cd build
ctest          # 若使用 CMake 生成了测试目标
# 或者手动跑一次
./peculiarc ../tests/sample.cj -E -Wall
```

若新增了功能，请在 `tests/` 中补充相应的测试用例。

---

### 代码风格建议

* 使用 **C++17** 标准特性，不要使用超出该标准的库。  
* 与现有代码保持相同的缩进（4 个空格），采用相同的大括号风格。  
* 正则表达式统一使用 **原始字符串字面量** `R"( … )"`。  
* 面向用户的提示文字使用中文，内部注释可使用英语。

---

## 许可证

本项目采用 **Apache License 2.0**，全文见仓库根目录的 `LICENSE.txt` 文件。简要说明如下：

```
Copyright © 2025 Scriptforge

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

> 您可以自由使用、修改、分发本代码（包括商业用途），唯一要求是保留版权声明和许可证文本。

---

## 有疑问？

* 在 **GitHub Issues** 中提交问题或建议。  
* 联系作者：📧 `scriptforgel@outlook.com`

祝您玩得开心，代码写得顺畅 🚀