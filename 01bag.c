#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <direct.h> // Windows 下获取当前目录

// 物品结构体
typedef struct {
    int id;        // 物品编号
    double weight; // 物品重量
    double value;  // 物品价值
} Item;

// 生成随机物品数据
void generate_items(Item* items, int n) {
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        // 生成1.00~100.00的浮点型重量
        items[i].weight = ((rand() % 9901) / 100.0) + 1.0; // 1.00~100.00
        // 生成100.00~1000.00的浮点型价值
        items[i].value = ((rand() % 90001) / 100.0) + 100.0; // 100.00~1000.00
    }
}

// 蛮力法
void brute_force(Item* items, int n, double capacity, int* selected, double* max_value) {
    int best_combo = 0;
    double best_value = 0;
    for (int i = 0; i < (1 << n); i++) {
        double total_weight = 0, total_value = 0;
        for (int j = 0; j < n; j++) {
            if (i & (1 << j)) {
                total_weight += items[j].weight;
                total_value += items[j].value;
            }
        }
        if (total_weight <= capacity && total_value > best_value) {
            best_value = total_value;
            best_combo = i;
        }
    }
    *max_value = best_value;
    for (int j = 0; j < n; j++) {
        selected[j] = (best_combo & (1 << j)) ? 1 : 0;
    }
}

// 回溯法
void backtrack(Item* items, int n, double capacity, int idx, double curr_weight, double curr_value,
               double* max_value, int* curr_selected, int* best_selected) {
    if (idx == n) {
        if (curr_value > *max_value) {
            *max_value = curr_value;
            memcpy(best_selected, curr_selected, n * sizeof(int));
        }
        return;
    }
    // 不选当前物品
    curr_selected[idx] = 0;
    backtrack(items, n, capacity, idx + 1, curr_weight, curr_value, max_value, curr_selected, best_selected);
    // 选当前物品（如果重量允许）
    if (curr_weight + items[idx].weight <= capacity) {
        curr_selected[idx] = 1;
        backtrack(items, n, capacity, idx + 1, curr_weight + items[idx].weight,
                  curr_value + items[idx].value, max_value, curr_selected, best_selected);
    }
}

// 回溯法（剪枝：若当前重量+剩余物品最大价值<=已知最优，则剪枝）
void backtrack_prune(Item* items, int n, double capacity, int idx, double curr_weight, double curr_value,
                     double* max_value, int* curr_selected, int* best_selected, double remain_value) {
    if (idx == n) {
        if (curr_value > *max_value) {
            *max_value = curr_value;
            memcpy(best_selected, curr_selected, n * sizeof(int));
        }
        return;
    }
    // 剪枝：如果当前价值+剩余最大价值<=已知最优，则不再递归
    if (curr_value + remain_value <= *max_value) return;

    // 不选当前物品
    curr_selected[idx] = 0;
    backtrack_prune(items, n, capacity, idx + 1, curr_weight, curr_value, max_value, curr_selected, best_selected, remain_value - items[idx].value);

    // 选当前物品（如果重量允许）
    if (curr_weight + items[idx].weight <= capacity) {
        curr_selected[idx] = 1;
        backtrack_prune(items, n, capacity, idx + 1, curr_weight + items[idx].weight,
                        curr_value + items[idx].value, max_value, curr_selected, best_selected, remain_value - items[idx].value);
    }
}

// 动态规划法（整数化处理）
void dynamic_programming(Item* items, int n, double capacity, int* selected, double* max_value) {
    int scale = 100; // 精度：保留两位小数
    int int_capacity = (int)(capacity * scale + 0.5);
    int* int_weight = (int*)malloc(n * sizeof(int));
    double* value = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        int_weight[i] = (int)(items[i].weight * scale + 0.5);
        value[i] = items[i].value;
    }

    double** dp = (double**)malloc((n + 1) * sizeof(double*));
    int** keep = (int**)malloc((n + 1) * sizeof(int*));
    for (int i = 0; i <= n; i++) {
        dp[i] = (double*)calloc(int_capacity + 1, sizeof(double));
        keep[i] = (int*)calloc(int_capacity + 1, sizeof(int));
    }

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= int_capacity; w++) {
            if (int_weight[i-1] <= w) {
                double val_without = dp[i-1][w];
                double val_with = dp[i-1][w - int_weight[i-1]] + value[i-1];
                if (val_with > val_without) {
                    dp[i][w] = val_with;
                    keep[i][w] = 1;
                } else {
                    dp[i][w] = val_without;
                }
            } else {
                dp[i][w] = dp[i-1][w];
            }
        }
    }

    *max_value = dp[n][int_capacity];
    int w = int_capacity;
    for (int i = n; i > 0; i--) {
        if (keep[i][w]) {
            selected[i-1] = 1;
            w -= int_weight[i-1];
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
        free(keep[i]);
    }
    free(dp);
    free(keep);
    free(int_weight);
    free(value);
}

// 比较函数：按价值/重量比降序排序
int cmp(const void* a, const void* b) {
    double ratio_a = ((Item*)a)->value / ((Item*)a)->weight;
    double ratio_b = ((Item*)b)->value / ((Item*)b)->weight;
    return ratio_a < ratio_b ? 1 : -1;
}

// 选择排序实现（按价值/重量比降序）
void selection_sort(Item* items, int n) {
    for (int i = 0; i < n - 1; i++) {
        int max_idx = i;
        double max_ratio = items[i].value / items[i].weight;
        for (int j = i + 1; j < n; j++) {
            double ratio = items[j].value / items[j].weight;
            if (ratio > max_ratio) {
                max_ratio = ratio;
                max_idx = j;
            }
        }
        if (max_idx != i) {
            Item tmp = items[i];
            items[i] = items[max_idx];
            items[max_idx] = tmp;
        }
    }
}

// 贪心法
void greedy(Item* items, int n, double capacity, int* selected, double* max_value) {
    qsort(items, n, sizeof(Item), cmp);
    double total_weight = 0;
    *max_value = 0;
    for (int i = 0; i < n; i++) {
        if (total_weight + items[i].weight <= capacity) {
            selected[i] = 1;
            total_weight += items[i].weight;
            *max_value += items[i].value;
        } else {
            selected[i] = 0;
        }
    }
}

// 贪心法（选择排序版）
void greedy_selection(Item* items, int n, double capacity, int* selected, double* max_value) {
    // 复制一份数据，避免影响原数组
    Item* items_copy = (Item*)malloc(n * sizeof(Item));
    memcpy(items_copy, items, n * sizeof(Item));
    selection_sort(items_copy, n);
    double total_weight = 0;
    *max_value = 0;
    for (int i = 0; i < n; i++) {
        if (total_weight + items_copy[i].weight <= capacity) {
            selected[i] = 1;
            total_weight += items_copy[i].weight;
            *max_value += items_copy[i].value;
        } else {
            selected[i] = 0;
        }
    }
    free(items_copy);
}

// 输出结果
void print_result(Item* items, int n, int* selected, double max_value, double time_ms, double capacity) {
    printf("选择的物品编号、重量、价值：\n");
    double total_weight = 0;
    int count = 0;
    for (int i = 0; i < n && count < 10; i++) { // 限制输出前10个物品，避免过多输出
        if (selected[i]) {
            printf("物品 %d: 重量 %.2f, 价值 %.2f\n", items[i].id, items[i].weight, items[i].value);
            total_weight += items[i].weight;
            count++;
        }
    }
    if (count < n) printf("（仅显示前10个物品，实际选择可能更多）\n");
    printf("总重量: %.2f, 总价值: %.2f, 执行时间: %.2f ms\n\n", total_weight, max_value, time_ms);
}

// 主函数
int main() {
    srand(time(NULL));
    while (1) {
        int algo;
        printf("请选择算法：\n1. 蛮力法\n2. 回溯法\n3. 动态规划法\n4. 贪心法\n5. 四种算法一起运行\n6. 小规模四算法对比\n7. 大规模只对比DP和贪心\n0. 退出\n输入编号：");
        scanf("%d", &algo);

        if (algo == 0) {
            printf("程序已退出。\n");
            break;
        }

        int n;
        if (algo == 1 || algo == 2) {
            printf("请输入物品数量（建议20以内）：");
            scanf("%d", &n);
            if (n > 20) {
                printf("警告：数量过大会导致运行极慢甚至崩溃！\n");
                n = 20;
            }
        } else if (algo == 3 || algo == 4) {
            printf("请输入物品数量（如1000、2000、5000等）：");
            scanf("%d", &n);
        } else if (algo == 5) { // 四种算法一起运行
            printf("请输入物品数量（如1000、2000、5000等）：");
            scanf("%d", &n);

            double capacity;
            printf("请输入背包容量（如10000.00、100000.00、1000000.00）：");
            scanf("%lf", &capacity);

            // 生成一份相同的物品数据
            Item* items_all = (Item*)malloc(n * sizeof(Item));
            generate_items(items_all, n);

            // 蛮力法（最多20个物品）
            int n_bf = n > 20 ? 20 : n;
            int* selected_bf = (int*)calloc(n_bf, sizeof(int));
            double max_value_bf = 0;
            clock_t start_bf = clock();
            brute_force(items_all, n_bf, capacity, selected_bf, &max_value_bf);
            clock_t end_bf = clock();
            double time_bf = ((double)(end_bf - start_bf) * 1000) / CLOCKS_PER_SEC;
            printf("\n【蛮力法】(物品数:%d)\n", n_bf);
            double total_weight_bf = 0;
            int count_bf = 0;
            for (int i = 0; i < n_bf; i++) {
                if (selected_bf[i]) {
                    if (count_bf < 20)
                        printf("物品 %d: 重量 %.2f, 价值 %.2f\n", items_all[i].id, items_all[i].weight, items_all[i].value);
                    total_weight_bf += items_all[i].weight;
                    count_bf++;
                }
            }
            if (count_bf > 20) printf("（仅显示前20个物品，实际选择可能更多）\n");
            printf("总重量: %.2f, 总价值: %.2f\n", total_weight_bf, max_value_bf);
            printf("算法执行时间: %.2f ms\n\n", time_bf);
            free(selected_bf);

            // 回溯法（最多20个物品）
            int* selected_bt = (int*)calloc(n_bf, sizeof(int));
            int* curr_selected_bt = (int*)calloc(n_bf, sizeof(int));
            double max_value_bt = 0;
            clock_t start_bt = clock();
            backtrack(items_all, n_bf, capacity, 0, 0, 0, &max_value_bt, curr_selected_bt, selected_bt);
            clock_t end_bt = clock();
            double time_bt = ((double)(end_bt - start_bt) * 1000) / CLOCKS_PER_SEC;
            printf("\n【回溯法】(物品数:%d)\n", n_bf);
            double total_weight_bt = 0;
            int count_bt = 0;
            for (int i = 0; i < n_bf; i++) {
                if (selected_bt[i]) {
                    if (count_bt < 20)
                        printf("物品 %d: 重量 %.2f, 价值 %.2f\n", items_all[i].id, items_all[i].weight, items_all[i].value);
                    total_weight_bt += items_all[i].weight;
                    count_bt++;
                }
            }
            if (count_bt > 20) printf("（仅显示前20个物品，实际选择可能更多）\n");
            printf("总重量: %.2f, 总价值: %.2f\n", total_weight_bt, max_value_bt);
            printf("算法执行时间: %.2f ms\n\n", time_bt);
            free(selected_bt);
            free(curr_selected_bt);

            // 动态规划法（n个物品）
            int* selected_dp = (int*)calloc(n, sizeof(int));
            double max_value_dp = 0;
            clock_t start_dp = clock();
            dynamic_programming(items_all, n, capacity, selected_dp, &max_value_dp);
            clock_t end_dp = clock();
            double time_dp = ((double)(end_dp - start_dp) * 1000) / CLOCKS_PER_SEC;
            printf("\n【动态规划法】(物品数:%d)\n", n);
            double total_weight_dp = 0;
            int count_dp = 0;
            for (int i = 0; i < n; i++) {
                if (selected_dp[i]) {
                    if (count_dp < 20)
                        printf("物品 %d: 重量 %.2f, 价值 %.2f\n", items_all[i].id, items_all[i].weight, items_all[i].value);
                    total_weight_dp += items_all[i].weight;
                    count_dp++;
                }
            }
            if (count_dp > 20) printf("（仅显示前20个物品，实际选择可能更多）\n");
            printf("总重量: %.2f, 总价值: %.2f\n", total_weight_dp, max_value_dp);
            printf("算法执行时间: %.2f ms\n\n", time_dp);
            free(selected_dp);

            // 贪心法（n个物品）
            int* selected_gr = (int*)calloc(n, sizeof(int));
            double max_value_gr = 0;
            clock_t start_gr = clock();
            greedy(items_all, n, capacity, selected_gr, &max_value_gr);
            clock_t end_gr = clock();
            double time_gr = ((double)(end_gr - start_gr) * 1000) / CLOCKS_PER_SEC;
            printf("\n【贪心法】(物品数:%d)\n", n);
            double total_weight_gr = 0;
            int count_gr = 0;
            for (int i = 0; i < n; i++) {
                if (selected_gr[i]) {
                    if (count_gr < 20)
                        printf("物品 %d: 重量 %.2f, 价值 %.2f\n", items_all[i].id, items_all[i].weight, items_all[i].value);
                    total_weight_gr += items_all[i].weight;
                    count_gr++;
                }
            }
            if (count_gr > 20) printf("（仅显示前20个物品，实际选择可能更多）\n");
            printf("总重量: %.2f, 总价值: %.2f\n", total_weight_gr, max_value_gr);
            printf("算法执行时间: %.2f ms\n\n", time_gr);
            free(selected_gr);

            // 生成Excel（CSV）文件
            int save_count = n > 1000 ? 1000 : n;
            FILE* fp = fopen("d:\\C\\items.csv", "w");
            if (fp) {
                fprintf(fp, "物品编号,物品重量,物品价值\n");
                for (int i = 0; i < save_count; i++) {
                    fprintf(fp, "%d,%.2f,%.2f\n", items_all[i].id, items_all[i].weight, items_all[i].value);
                }
                fclose(fp);
                printf("已生成 d:\\C\\items.csv 文件（保存前%d个物品数据，可用Excel打开）。\n", save_count);
            } else {
                printf("无法创建 d:\\C\\items.csv 文件！\n");
            }

            char cwd[260];
            _getcwd(cwd, sizeof(cwd));
            printf("当前工作目录: %s\n", cwd);

            free(items_all);
            continue;
        } else if (algo == 6) { // 小规模四算法对比
            int n_list[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
            int n_case = sizeof(n_list) / sizeof(n_list[0]);
            double capacity = 1000.0;
            FILE* fpr = fopen("d:\\C\\result_compare.csv", "w");
            if (fpr) {
                fprintf(fpr, "物品数,蛮力法(ms),回溯法(ms),回溯剪枝(ms),动态规划(ms),贪心法(ms)\n");
                for (int idx = 0; idx < n_case; idx++) {
                    int n = n_list[idx];
                    Item* items = (Item*)malloc(n * sizeof(Item));
                    generate_items(items, n);

                    // 蛮力法
                    int* selected_bf = (int*)calloc(n, sizeof(int));
                    double max_value_bf = 0;
                    clock_t start_bf = clock();
                    brute_force(items, n, capacity, selected_bf, &max_value_bf);
                    clock_t end_bf = clock();
                    double time_bf = ((double)(end_bf - start_bf) * 1000) / CLOCKS_PER_SEC;
                    free(selected_bf);

                    // 回溯法
                    int* selected_bt = (int*)calloc(n, sizeof(int));
                    int* curr_selected_bt = (int*)calloc(n, sizeof(int));
                    double max_value_bt = 0;
                    clock_t start_bt = clock();
                    backtrack(items, n, capacity, 0, 0, 0, &max_value_bt, curr_selected_bt, selected_bt);
                    clock_t end_bt = clock();
                    double time_bt = ((double)(end_bt - start_bt) * 1000) / CLOCKS_PER_SEC;
                    free(selected_bt);
                    free(curr_selected_bt);

                    // 回溯剪枝
                    int* selected_btp = (int*)calloc(n, sizeof(int));
                    int* curr_selected_btp = (int*)calloc(n, sizeof(int));
                    double max_value_btp = 0;
                    double remain_value = 0;
                    for (int i = 0; i < n; i++) remain_value += items[i].value;
                    clock_t start_btp = clock();
                    backtrack_prune(items, n, capacity, 0, 0, 0, &max_value_btp, curr_selected_btp, selected_btp, remain_value);
                    clock_t end_btp = clock();
                    double time_btp = ((double)(end_btp - start_btp) * 1000) / CLOCKS_PER_SEC;
                    free(selected_btp);
                    free(curr_selected_btp);

                    // 动态规划法
                    int* selected_dp = (int*)calloc(n, sizeof(int));
                    double max_value_dp = 0;
                    clock_t start_dp = clock();
                    dynamic_programming(items, n, capacity, selected_dp, &max_value_dp);
                    clock_t end_dp = clock();
                    double time_dp = ((double)(end_dp - start_dp) * 1000) / CLOCKS_PER_SEC;
                    free(selected_dp);

                    // 贪心法
                    int* selected_gr = (int*)calloc(n, sizeof(int));
                    double max_value_gr = 0;
                    clock_t start_gr = clock();
                    greedy(items, n, capacity, selected_gr, &max_value_gr);
                    clock_t end_gr = clock();
                    double time_gr = ((double)(end_gr - start_gr) * 1000) / CLOCKS_PER_SEC;
                    free(selected_gr);

                    fprintf(fpr, "%d,%.3f,%.3f,%.3f,%.3f,%.3f\n", n, time_bf, time_bt, time_btp, time_dp, time_gr);
                    free(items);
                }
                fclose(fpr);
                printf("已生成 d:\\C\\result_compare.csv 文件。\n");
            }
            continue;
        } else if (algo == 7) { // 大规模只对比DP和贪心
            // 增加采样点
            int n_list[] = {50, 100, 200, 500, 1000, 2000, 3000, 4000, 5000, 6000, 8000, 10000, 12000, 16000, 20000, 24000, 28000, 32000};
            int n_case = sizeof(n_list) / sizeof(n_list[0]);
            double capacity = 1000.0;
            FILE* fpr = fopen("d:\\C\\result_compare2.csv", "w");
            if (fpr) {
                // 修改表头
                fprintf(fpr, "物品数,动态规划(ms),快排贪心(ms),选择排序贪心(ms)\n");
                // 先生成最大规模的数据
                int max_n = n_list[n_case - 1];
                Item* items_all = (Item*)malloc(max_n * sizeof(Item));
                generate_items(items_all, max_n);

                for (int idx = 0; idx < n_case; idx++) {
                    int n = n_list[idx];
                    Item* items = items_all; // 直接用指针，不再malloc/free

                    // 动态规划法
                    int* selected_dp = (int*)calloc(n, sizeof(int));
                    double max_value_dp = 0;
                    clock_t start_dp = clock();
                    dynamic_programming(items, n, capacity, selected_dp, &max_value_dp);
                    clock_t end_dp = clock();
                    double time_dp = ((double)(end_dp - start_dp) * 1000) / CLOCKS_PER_SEC;
                    free(selected_dp);

                    // 快排贪心法（qsort）
                    int* selected_gr = (int*)calloc(n, sizeof(int));
                    double max_value_gr = 0;
                    clock_t start_gr = clock();
                    greedy(items, n, capacity, selected_gr, &max_value_gr);
                    clock_t end_gr = clock();
                    double time_gr = ((double)(end_gr - start_gr) * 1000) / CLOCKS_PER_SEC;
                    free(selected_gr);

                    // 选择排序贪心法
                    int* selected_gr_sel = (int*)calloc(n, sizeof(int));
                    double max_value_gr_sel = 0;
                    clock_t start_gr_sel = clock();
                    greedy_selection(items, n, capacity, selected_gr_sel, &max_value_gr_sel);
                    clock_t end_gr_sel = clock();
                    double time_gr_sel = ((double)(end_gr_sel - start_gr_sel) * 1000) / CLOCKS_PER_SEC;
                    free(selected_gr_sel);

                    fprintf(fpr, "%d,%.3f,%.3f,%.3f\n", n, time_dp, time_gr, time_gr_sel);
                }
                free(items_all); // 只需释放一次
                fclose(fpr);
                printf("已生成 d:\\C\\result_compare2.csv 文件。\n");
            }
            continue;
        }

        double capacity;
        printf("请输入背包容量（如10000.00、100000.00、1000000.00）：");
        scanf("%lf", &capacity);

        // 单算法运行（原有逻辑）
        Item* items = (Item*)malloc(n * sizeof(Item));
        int* selected = (int*)calloc(n, sizeof(int));
        generate_items(items, n);

        double max_value = 0;

        // 记录算法开始时间
        clock_t start = clock();

        if (algo == 1) {
            brute_force(items, n, capacity, selected, &max_value);
        } else if (algo == 2) {
            int* curr_selected = (int*)calloc(n, sizeof(int));
            backtrack(items, n, capacity, 0, 0, 0, &max_value, curr_selected, selected);
            free(curr_selected);
        } else if (algo == 3) {
            dynamic_programming(items, n, capacity, selected, &max_value);
        } else if (algo == 4) {
            greedy(items, n, capacity, selected, &max_value);
        }

        // 记录算法结束时间
        clock_t end = clock();
        double time_ms = ((double)(end - start) * 1000) / CLOCKS_PER_SEC;

        // 输出结果
        printf("选择的物品编号、重量、价值：\n");
        double total_weight = 0;
        int count = 0;
        for (int i = 0; i < n; i++) {
            if (selected[i]) {
                printf("物品 %d: 重量 %.2f, 价值 %.2f\n", items[i].id, items[i].weight, items[i].value);
                total_weight += items[i].weight;
                count++;
            }
        }
        printf("总重量: %.2f, 总价值: %.2f\n", total_weight, max_value);
        printf("算法执行时间: %.2f ms\n\n", time_ms);

        // 生成Excel（CSV）文件
        int save_count = n > 1000 ? 1000 : n;
        FILE* fp = fopen("d:\\C\\items.csv", "w");
        if (fp) {
            fprintf(fp, "物品编号,物品重量,物品价值\n");
            for (int i = 0; i < save_count; i++) {
                fprintf(fp, "%d,%.2f,%.2f\n", items[i].id, items[i].weight, items[i].value);
            }
            fclose(fp);
            printf("已生成 d:\\C\\items.csv 文件（保存前%d个物品数据，可用Excel打开）。\n", save_count);
        } else {
            printf("无法创建 d:\\C\\items.csv 文件！\n");
        }

        char cwd[260];
        _getcwd(cwd, sizeof(cwd));
        printf("当前工作目录: %s\n", cwd);

        free(items);
        free(selected);
    }
    return 0;
}