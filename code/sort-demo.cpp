// A7：结构体数组 + 排序入门 —— 配套演示
// 编译：build-sort-demo.cmd
// 一句话：排序 = 「比较规则」+「怎么把顺序摆对」两件事，结构体排序还要先定"按哪个字段比"
#include <cstdio>
#include <algorithm>      // std::sort / std::swap
#include <cstring>

struct Player {
    char name[16];
    int  score;
    int  level;
};

// ---------- 1. 比较规则：这一节的核心 ----------
// 排序键：分数高的在前；分数相同则等级小的在前（多关键字）
bool better(const Player& a, const Player& b) {
    if (a.score != b.score) return a.score > b.score;   // 主键：分数大
    return a.level < b.level;                           // 次键：等级小
}

// ---------- 2. 交换：引用传参第一次派上大用场 ----------
void swapP(Player& a, Player& b) {
    Player t = a;      // struct 能整体拷贝（第十五节），所以交换只要三行
    a = b;
    b = t;
}

void show(const Player a[], int n, const char* title) {
    printf("%s\n", title);
    for (int i = 0; i < n; i++)
        printf("   %-6s 分数 %3d  等级 %d\n", a[i].name, a[i].score, a[i].level);
}

// ---------- 3. 选择排序：每轮找"最好的"换到前面 ----------
// 比较次数固定 n(n-1)/2 → O(n^2)
long long selectSort(Player a[], int n) {
    long long cmp = 0;
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            cmp++;
            if (better(a[j], a[best])) best = j;
        }
        if (best != i) swapP(a[i], a[best]);
    }
    return cmp;
}

// ---------- 4. 冒泡排序：相邻比较，带"提前退出" ----------
long long bubbleSort(Player a[], int n) {
    long long cmp = 0;
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - pass; j++) {
            cmp++;
            if (better(a[j + 1], a[j])) { swapP(a[j], a[j + 1]); swapped = true; }
        }
        if (!swapped) break;          // 这一轮没换过 → 已经有序，提前收工
    }
    return cmp;
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
    Player a[6]; memcpy(a, src, sizeof(src));
    long long c1 = selectSort(a, n);
    show(a, n, "\n=== 选择排序后（比较次数永远 = n(n-1)/2 = 15）===");
    printf("   实际比较次数 = %lld（和 n(n-1)/2 = 15 一致：它不看数据长什么样，永远这么多）\n", c1);

    // ---- 手写冒泡（带提前退出）----
    Player b[6]; memcpy(b, src, sizeof(src));
    long long c2 = bubbleSort(b, n);
    show(b, n, "\n=== 冒泡排序后 ===");
    printf("   实际比较次数 = %lld（这组数据没触发提前退出，所以和选择排序一样）\n", c2);

    // ---- 比赛里真正该用的：std::sort ----
    Player c[6]; memcpy(c, src, sizeof(src));
    std::sort(c, c + n, better);        // 传一个"比较规则"进去
    show(c, n, "\n=== std::sort 后（三者结果必须完全一致）===");

    // ---- 提前退出有多值：换一组"几乎已经有序"的数据 ----
    Player s2[6] = {
        { "甲", 95, 1 }, { "乙", 95, 2 }, { "丙", 90, 1 },
        { "丁", 80, 1 }, { "戊", 70, 1 }, { "己", 60, 1 },
    };
    Player d1[6], d2[6];
    memcpy(d1, s2, sizeof(s2)); memcpy(d2, s2, sizeof(s2));
    long long e1 = selectSort(d1, n);
    long long e2 = bubbleSort(d2, n);
    printf("\n=== 换一组「本来就基本有序」的数据 ===\n");
    printf("   选择排序比较 %lld 次（照样死板）\n", e1);
    printf("   冒泡排序比较 %lld 次（提前退出生效了）\n", e2);

    // ---- 复杂度感受：为什么比赛不能用 O(n^2) ----
    printf("\n=== 复杂度对比（只算比较次数，不真跑）===\n");
    printf("   n = 1000：选择排序 ≈ %lld 次比较；O(n log n) ≈ %d 次\n", 1000LL * 999 / 2, 1000 * 10);
    printf("   n = 100000：选择排序 ≈ %lld 次（10^10 量级，必然超时）\n", 100000LL * 99999 / 2);
    printf("   → 这就是为什么比赛里一律 std::sort（O(n log n)）\n");

    return 0;
}
