import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

try:
    # 设置中文字体
    matplotlib.rcParams['font.sans-serif'] = ['SimHei']
    matplotlib.rcParams['axes.unicode_minus'] = False

    # 读取数据
    df = pd.read_csv('d:/C/result_compare2.csv')
    df['快排贪心法(ms)'] = df['快排贪心法(ms)'].replace(0, 0.1)  # 解决对数坐标下0无法显示的问题
    df['选择排序贪心(ms)'] = df['选择排序贪心(ms)'].replace(0, 0.1)

    # 绘制三种算法运行时间曲线
    plt.plot(df['物品数'], df['动态规划(ms)'], marker='o', label='动态规划')
    plt.plot(df['物品数'], df['快排贪心法(ms)'], marker='s', label='快排贪心法')
    plt.plot(df['物品数'], df['选择排序贪心(ms)'], marker='^', label='选择排序贪心')

    plt.xlabel('物品数')
    plt.ylabel('运行时间 (ms)')
    plt.title('三种算法运行时间对比')

    plt.yscale('log')
    plt.yticks([0.1, 1, 10, 100, 1000, 10000, 100000], ['$10^{-1}$', '$10^0$', '$10^1$', '$10^2$', '$10^3$', '$10^4$', '$10^5$'])
    plt.gca().yaxis.set_minor_locator(matplotlib.ticker.NullLocator())
    plt.grid(axis='y', which='major', linestyle='--')

    plt.legend()
    plt.tight_layout()

    # 保存图片到指定路径
    save_path = 'd:/C/dp_greedy_selection_time_comparison.png'
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