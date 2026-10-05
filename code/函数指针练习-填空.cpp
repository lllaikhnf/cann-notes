// A8 练习：函数指针 与 lambda（填空）
// 编译运行：双击 build-funcptr.cmd
// 目标：把 4 个 TODO 填完，输出应该和 README 里的"期望输出"一致
#include <cstdio>
#include <cstring>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif

struct Player {
    char name[16];
    int  score;
    int  level;
};

// ============================================================
// TODO 1：写比较规则 byLevelDesc
//   规则：等级大的排前面；等级相同时，分数大的排前面
//   签名：bool byLevelDesc(const Player& a, const Player& b)
//   提示：和上一节的 better 一个套路，只是比较的方向反过来
// ============================================================
// 在这里写 bool byLevelDesc(...) { ... }


// ---------- 通用排序：把"怎么比"当参数传进来 ----------
//   看形参：bool (*cmp)(const Player&, const Player&)
//   cmp 是一个"函数指针"，指向一个"收两个 const Player&、返回 bool"的函数
//   注意 *cmp 外面那对括号不能省
void sortBy(Player a[], int n, bool (*cmp)(const Player&, const Player&)) {
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        // ====================================================
        // TODO 2：在内层循环里用 cmp 找出"最好的"那个下标
        //   遍历 j = i+1 .. n-1，如果 cmp(a[j], a[best]) 为真，就 best = j
        //   cmp 就当普通函数来调用：cmp(元素1, 元素2)
        // ====================================================
        // 在这里写内层 for 循环

        if (best != i) { Player t = a[i]; a[i] = a[best]; a[best] = t; }
    }
}

void show(const Player a[], int n, const char* title) {
    printf("%s\n", title);
    for (int i = 0; i < n; i++)
        printf("   %-6s 分数 %3d  等级 %d\n", a[i].name, a[i].score, a[i].level);
}

int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 显示（否则中文是鬼画符）
#endif
    Player src[6] = {
        { "阿甲", 70, 3 }, { "阿乙", 95, 2 }, { "阿丙", 70, 1 },
        { "阿丁", 95, 5 }, { "阿戊", 60, 4 }, { "阿己", 95, 1 },
    };
    const int n = 6;

    // ---- 1) 函数指针当参数：同一个 sortBy，换规则只是换个函数名 ----
    Player a[6];
    memcpy(a, src, sizeof(src));
    sortBy(a, n, byLevelDesc);        // 注意：byLevelDesc 后面**没有括号**
    show(a, n, "=== 规则 byLevelDesc（等级降序，同级分数降序）===");

    // ---- 2) lambda：规则当场写（分数升序）----
    Player c[6];
    memcpy(c, src, sizeof(src));
    // ========================================================
    // TODO 3：用 lambda 让 c 按分数**升序**排
    //   语法：
    //     std::sort(c, c + n, [](const Player& p, const Player& q) {
    //         return ...;          // 让"分数小的"排前面
    //     });
    // ========================================================
    // 在这里写 std::sort(...)
    show(c, n, "\n=== lambda：分数升序 ===");

    // ---- 3) 带捕获的 lambda：规则里要用外面的变量 ----
    bool desc = false;                 // 外面这个开关决定升序还是降序
    Player d[6];
    // ========================================================
    // TODO 4：写一个"捕获 desc"的 lambda，让 d 按 desc 决定升序 / 降序
    //   要点：捕获列表里写 [desc]；不写进 [] 就用不了 desc（直接编译错）
    //         desc 为 true 时返回 p.score > q.score，否则返回 p.score < q.score
    //   下面有两个坑位，各写一次 std::sort（同一个 lambda 用两遍）
    // ========================================================
    memcpy(d, src, sizeof(src));
    // 在这里写 std::sort(...)   ← 此时 desc = false
    show(d, n, "\n=== 带捕获 [desc]，desc = false：分数升序 ===");

    desc = true;                       // 只改外面这个变量，lambda 一个字都不用动
    memcpy(d, src, sizeof(src));
    // 在这里写 std::sort(...)   ← 此时 desc = true
    show(d, n, "\n=== 同一个 lambda，desc = true：分数降序 ===");

    // ================= 自检（不用改） =================
    bool ok = true;

    // TODO 1 / TODO 2 的结果：byLevelDesc 的正确答案
    const char* expName[6]  = { "阿丁", "阿戊", "阿甲", "阿乙", "阿己", "阿丙" };
    const int   expScore[6] = {   95,     60,     70,     95,     95,     70   };
    const int   expLevel[6] = {    5,      4,      3,      2,      1,      1   };
    for (int i = 0; i < n; i++) {
        if (strcmp(a[i].name, expName[i]) != 0 || a[i].score != expScore[i] || a[i].level != expLevel[i]) {
            ok = false;
            printf("\n*** TODO 1/2 不对：第 %d 位应该是 %s %d/%d，实际是 %s %d/%d ***\n",
                   i + 1, expName[i], expScore[i], expLevel[i], a[i].name, a[i].score, a[i].level);
        }
    }

    // TODO 3：c 的分数应该是"非递减"
    for (int i = 1; i < n; i++) {
        if (c[i - 1].score > c[i].score) {
            ok = false; printf("\n*** TODO 3 不对：分数没有排成升序 ***\n"); break;
        }
    }

    // TODO 4：最后一次 desc = true，所以 d 的分数应该是"非递增"
    for (int i = 1; i < n; i++) {
        if (d[i - 1].score < d[i].score) {
            ok = false; printf("\n*** TODO 4 不对：desc = true 时分数应该降序 ***\n"); break;
        }
    }
    // 顺带保证自检本身没漏掉：TODO 4 如果只写了一次 sort，上面这条不一定报错，
    // 所以再明确要求 desc = false 那一遍也必须写过 —— 这里看 d 的降序即可，剩下靠肉眼对照输出。

    printf("\n自检：%s\n", ok ? "全部正确 ✅" : "还有问题 ❌");

    return 0;
}
