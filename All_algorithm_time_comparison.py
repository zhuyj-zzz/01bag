import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

try:
    # 设置中文字体
    matplotlib.rcParams['font.sans-serif'] = ['SimHei']
    matplotlib.rcParams['axes.unicode_minus'] = False

    # 读取数据
    df = pd.read_csv('d:/C/result_compare.csv')

    # 绘制五种算法运行时间曲线
    plt.plot(df['物品数'], df['蛮力法(ms)'], marker='o', label='蛮力法')
    plt.plot(df['物品数'], df['回溯法(ms)'], marker='s', label='回溯法')
    plt.plot(df['物品数'], df['回溯剪枝(ms)'], marker='x', label='回溯剪枝')
    plt.plot(df['物品数'], df['动态规划(ms)'], marker='^', label='动态规划')
    plt.plot(df['物品数'], df['贪心法(ms)'], marker='d', label='贪心法')

    plt.xlabel('物品数')
    plt.ylabel('运行时间 (ms)')
    plt.title('五种算法运行时间对比')
    plt.legend()
    plt.grid(axis='y', which='major', linestyle='--')
    plt.tight_layout()

    # 保存图片到指定路径
    save_path = 'd:/C/All_algorithm_time_comparison.png'
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"图表已成功保存到: {save_path}")

    # 显示图表
    plt.show()

except FileNotFoundError:
    print("错误: 找不到CSV文件，请检查文件路径是否正确。")
except Exception as e:
    print(f"发生未知错误: {e}")
finally:
    plt.close()