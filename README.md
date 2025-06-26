# 01bag

# 项目简介
本项目主要针对经典的0/1背包问题，用 C 语言实现了四种经典算法（蛮力、贪心、回溯、动态规划），并用 Python 对算法效率进行对比分析和可视化。项目包含所有算法的实现、批量测试数据、性能对比图表和结果数据，便于学习和分析不同算法在背包问题上的表现。

# 文件结构
01bag.c
四种背包算法（蛮力、贪心、回溯、动态规划）的 C 语言实现及测试入口。
All_algorithm_time_comparison.py
用于比较所有算法运行时间的 Python 脚本，并绘制对比图。
All_algorithm_time_comparison.png
所有算法运行时间对比图。
dp_greedy_time_comparison.py
动态规划与贪心法效率对比的 Python 绘图脚本。
dp_greedy_selection_time_comparison.png
动态规划与贪心法运行时间对比图。
items.csv
背包问题的测试用例数据，物品及其属性。
result_compare.csv、result_compare2.csv
各算法运行结果与效率的对比数据。
README.md
项目说明文件。

# 主要功能
支持用四种算法解决 0/1 背包问题
统一生成并读取测试数据
批量测试与统计各算法的运行时间与解的质量
Python 脚本自动生成效率对比图，直观展示算法优劣

# 使用方法
1.编译并运行 C 代码
用 GCC 或其他 C 编译器编译 01bag.c，按提示输入或调用数据进行测试。
gcc 01bag.c -o 01bag
./01bag
2.准备测试数据
items.csv 为批量测试数据，可自行扩展生成。
3.运行 Python 可视化脚本
需先安装 matplotlib 和 pandas：
pip install matplotlib pandas
python All_algorithm_time_comparison.py
python dp_greedy_time_comparison.py
运行后会自动读取 CSV 结果并输出对比图片。

# 结果展示
项目内已包含部分运行时间对比的图片，例如：
All_algorithm_time_comparison.png 展示四种算法效率对比曲线
dp_greedy_selection_time_comparison.png 展示动态规划与贪心算法对比

# 数据文件说明
items.csv：每行一个物品，包含重量和价值等信息
result_compare.csv/result_compare2.csv：不同算法的运行时间和解的价值对比

# 适用场景
适合学习 0/1 背包算法、性能分析、算法对比，以及数据可视化等相关内容。
