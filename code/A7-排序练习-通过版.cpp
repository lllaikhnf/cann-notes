// A7 练习：结构体数组排序（填空）
// 编译运行：双击 build-sort.cmd
// 目标：把 4 个 TODO 填完，输出应该和 README 里的"期望输出"一致
#include <cstdio>
#include <cstring>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

struct Player {
    char name[16];
    int  score;
    int  level;
};


bool better(const Player& a, const Player& b) {
    if (a.score != b.score) {
        return a.score > b.score;
    }return 
        a.level < b.level;
}
// ============================================================
// TODO 1：写比较规则 better
//   规则：分数高的排在前面；分数相同时，等级小的排在前面
//   签名：bool better(const Player& a, const Player& b)
//   提示：先比 a.score 和 b.score，不相等就直接返回结果；
//         相等时再比等级（想清楚哪个方向返回 true）
// ============================================================
// 在这里写 bool better(...) { ... }


void swapP(Player& a, Player& b) {
    Player tem = a;
    a = b;
    b = tem;
}
// ============================================================
// TODO 2：写交换函数 swapP
//   签名：void swapP(Player& a, Player& b)
//   要求：三个赋值（用临时变量），参数必须是引用
// ============================================================
// 在这里写 void swapP(...) { ... }


void show(const Player a[], int n, const char* title) {
    printf("%s\n", title);
    for (int i = 0; i < n; i++)
        printf("   %-6s 分数 %3d  等级 %d\n", a[i].name, a[i].score, a[i].level);
}


// ============================================================
// TODO 3：补全选择排序的内层循环
//   外层已经写好了：第 i 轮要把 [i, n-1] 里"最好的"换到位置 i
//   要你写的是：在内层 for 里用 better 找出"最好的"那个下标
// ============================================================
void selectSort(Player a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        // 在这里写内层循环（遍历 j = i+1 .. n-1，用 better 更新 best）
        for (int j = i + 1; j < n; j++) {
            if (better(a[j], a[best])) {
                best = j;
            }// TODO 3（就在这个花括号里写）
        }
        if (best != i) swapP(a[i], a[best]);
    }
}

int main(void)
{
    Player src[6] = {
        { "阿甲", 70, 3 },
        { "阿乙", 95, 2 },
        { "阿丙", 70, 1 },
        { "阿丁", 95, 5 },
        { "阿戊", 60, 4 },
        { "阿己", 95, 1 },
    };
    const int n = 6;
    show(src, n, "=== 原始顺序 ===");

    // ---- 手写选择排序 ----
    Player a[6];
    memcpy(a, src, sizeof(src));
    selectSort(a, n);
    show(a, n, "\n=== 手写选择排序后 ===");

    // ---- 标准库排序，用来对答案 ----
    Player b[6];
    memcpy(b, src, sizeof(src));

    std::sort(b, b + n,better);
    // ========================================================
    // TODO 4：用 std::sort 把 b 排好
    //   提示：std::sort(起始指针, 结束指针, 比较规则)
    //         数组名 b 就是首地址；结束位置是 b + n
    //         注意比较规则那里**不要写括号**
    // ========================================================
    // 在这里写 std::sort(...);

    show(b, n, "\n=== std::sort 后（必须和上面一模一样）===");

    // 简易自检：手写和标准库结果不一致就报警
    bool same = true;
    for (int i = 0; i < n; i++) {
        if (a[i].score != b[i].score || a[i].level != b[i].level || strcmp(a[i].name, b[i].name) != 0) {
            same = false;
            printf("\n*** 第 %d 位不一样：手写是 %s，std::sort 是 %s ***\n", i, a[i].name, b[i].name);
        }
    }
    printf("\n自检：%s\n", same ? "两边结果完全一致 ✅" : "有问题 ❌");

    return 0;
}
