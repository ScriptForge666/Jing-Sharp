// Copyright 2025 Scriptforge
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

// ---------------------------------------------------------------
// 正则（兼容 # 与 井，已使用原始字符串）
// ---------------------------------------------------------------

// 全角左小括号
static const  std::regex re_left_parenthesis(R"(（)");

// 全角右小括号
static const  std::regex re_right_parenthesis(R"(）)");

//全角左中括号
static const  std::regex re_left_bracket(R"(【)");

//全角右中括号
static const  std::regex re_right_bracket(R"(】)");

// 井开头的行
static const std::regex re_hash_line(R"(^([ \t]*)井(?=[^\S\r\n]*\S))");

// 井《include》或 #《include》
static const std::regex re_include_angle( R"((?:井|#)《include》[\s　]*([^\s/\\]+))");

// 井"include"或 #"include"
static const std::regex re_include_quote(R""((?:井|#)"include"[\s　]*([^\s/\\]+))"");

// def main → int main
static const std::regex re_def_main(R"(^([ \t]*)def\s+main\s*\()");

// 其它 def → auto
static const std::regex re_def_other(R"(^([ \t]*)def\b)");

// ---------------------------------------------------------------
// 行处理
// ---------------------------------------------------------------
std::string process_line(const std::string& line)
{
    std::string out = line;
    std::smatch m;

    //全角括号 → 半角括号
    out = std::regex_replace(out, re_left_parenthesis, "(");
    out = std::regex_replace(out, re_right_parenthesis, ")");
	out = std::regex_replace(out, re_left_bracket, "[");
	out = std::regex_replace(out, re_right_bracket, "]");
    //def main → int main
    bool replaced_main = false;
    if (std::regex_search(out, m, re_def_main)) {
        std::string indent = m[1].str();
        out.replace(m.position(0), m.length(0), indent + "int main(");
        replaced_main = true;
    }

    //井 → #
    if (std::regex_search(out, m, re_hash_line)) {
        std::string indent = m[1].str();
        out.replace(m.position(0), m.length(0), indent + "#");
    }

    //井《include》 / #《include》
    out = std::regex_replace(out, re_include_angle, "#include <$1>");

    //井"include" / #"include"
    out = std::regex_replace(out, re_include_quote, "#include \"$1\"");

    //其它 def → auto（排除 main 行）
    if (!replaced_main && std::regex_search(out, m, re_def_other)) {
        std::string indent = m[1].str();
        out.replace(m.position(0), m.length(0), indent + "auto");
    }

    return out;
}

// ---------------------------------------------------------------
//读取文件 → 生成临时文件（假设已经是 UTF‑8）
// ---------------------------------------------------------------
fs::path translate_source(const fs::path& srcPath, bool /*keep_temp*/)
{
    std::ifstream in(srcPath, std::ios::binary);
    if (!in) {
        std::cerr << "[错误] 打不开源文件: " << srcPath << "\n";
        std::exit(1);
    }
    std::string content((std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>());

    std::istringstream iss(content);
    std::ostringstream oss;
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        oss << process_line(line) << '\n';
    }

    fs::path temp = srcPath;
    temp.replace_filename(srcPath.stem().string() + "_pre" + srcPath.extension().string());

    std::ofstream out(temp, std::ios::binary);
    out << oss.str();
    return temp;
}

// ---------------------------------------------------------------
//判断后端编译器
// ---------------------------------------------------------------
enum class Backend { MSVC, GCC, Clang };

Backend detect_backend()
{
#ifdef _WIN32
    if (std::system("where cl >nul 2>&1") == 0) return Backend::MSVC;
    if (std::system("where clang >nul 2>&1") == 0) return Backend::Clang;
    if (std::system("where gcc >nul 2>&1") == 0) return Backend::GCC;
#else
    if (std::system("command -v clang > /dev/null 2>&1") == 0) return Backend::Clang;
    if (std::system("command -v gcc > /dev/null 2>&1") == 0) return Backend::GCC;
#endif
    std::cerr << "[错误] 未检测到可用的 C/C++ 编译器 (cl/gcc/clang)。\n";
    std::exit(1);
}

// ---------------------------------------------------------------
//构造调用底层编译器的命令行
// ---------------------------------------------------------------
std::string build_command(const fs::path& tempSource,
    const std::vector<std::string>& user_args,
    Backend backend)
{
    std::ostringstream cmd;
    switch (backend) {
    case Backend::MSVC: cmd << "cl "; break;
    case Backend::GCC:  cmd << "gcc "; break;
    case Backend::Clang:cmd << "clang "; break;
    }
    if (backend == Backend::MSVC) {
        if (tempSource.extension() == ".c") cmd << "/TC ";
        else                               cmd << "/TP ";
    }

    for (const auto& a : user_args) cmd << a << ' ';
    cmd << '"' << tempSource.string() << "\" ";
    return cmd.str();
}

// ---------------------------------------------------------------
//主函数（异常捕获）
// ---------------------------------------------------------------
int main(int argc, char* argv[])
{
    const char* version_str = "peculiarc version 0.9.5";
    const char* help_str =
        "用法: peculiarc [options] <source> [compiler options]\n"
        "  --keep-temp   保留 *_pre.* 临时文件\n"
        "  -v, --version 显示版本信息并退出\n"
        "  -h, --help    显示帮助信息并退出\n";

    if (argc < 2) {
        std::cerr << help_str;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-v" || a == "--version") {
            std::cout << version_str << std::endl;
            return 0;
        }
        if (a == "-h" || a == "--help") {
            std::cout << help_str;
            return 0;
        }
    }

    bool keep_temp = false;
    std::vector<std::string> user_args;
    std::string source_file;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--keep-temp") keep_temp = true;
        else if (a.rfind('-', 0) == 0) user_args.push_back(a);
        else source_file = a;
    }

    try {
        fs::path srcPath = fs::absolute(source_file);
        if (!fs::exists(srcPath)) {
            std::cerr << "[错误] 源文件不存在: " << srcPath << "\n";
            return 1;
        }

        fs::path tmp = translate_source(srcPath, keep_temp);
        Backend be = detect_backend();
        std::string cmd = build_command(tmp, user_args, be);
https://github.com/
        std::cout << "[Info] 正在调用底层编译器:\n  " << cmd << "\n";
        int rc = std::system(cmd.c_str());

        if (!keep_temp) {
            std::error_code ec;
            fs::remove(tmp, ec);
            if (ec) std::cerr << "[Warning] 删除临时文件失败: " << ec.message() << "\n";
        }
        return rc;
    }
    catch (const std::regex_error& e) {
        std::cerr << "[Fatal] 正则错误: " << e.what()
            << "\n代码: " << e.code() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "[Fatal] 异常: " << e.what() << "\n";
        return 1;
    }
}