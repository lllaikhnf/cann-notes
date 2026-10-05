// A8：函数指针 与 lambda —— 配套演示
// 编译：build-funcptr-demo.cmd
// 一句话：前面 std::sort(a, a+n, better) 里那个 better，就是"把函数当参数传"；这一节把这个机制拆开看
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

// ---------- 1. 三个"比较规则"，长一个样，只是内容不同 ----------
bool byScore(const Player& a, const Player& b) {          // 分数降序，同分等级升序
    if (a.score != b.score) return a.score > b.score;
    return a.level < b.level;
}
bool byLevel(const Player& a, const Player& b) {          // 等级升序，同级分数降序
    if (a.level != b.level) return a.level < b.level;
    return a.score > b.score;
}
bool byLevelDesc(const Player& a, const Player& b) {      // 等级降序（和 byLevel 正好相反）
    if (a.level != b.level) return a.level > b.level;
    return a.score > b.score;
}

// ---------- 2. 通用排序：把"怎么比"当成一个参数 ----------
//     看这个形参：bool (*cmp)(const Player&, const Player&)
//        · 括号必须把 *cmp 括起来，否则会变成"返回函数指针的函数"
//        · 这就是"函数指针"类型：指向一个 (const Player&, const Player&)->bool 的函数
void sortBy(Player a[], int n, bool (*cmp)(const Player&, const Player&)) {
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++)
            if (cmp(a[j], a[best])) best = j;      // 像普通函数一样调用它
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
    SetConsoleOutputCP(CP_UTF8);
#endif
    Player src[6] = {
        { "阿甲", 70, 3 }, { "阿乙", 95, 2 }, { "阿丙", 70, 1 },
        { "阿丁", 95, 5 }, { "阿戊", 60, 4 }, { "阿己", 95, 1 },
    };
    const int n = 6;
    Player a[6];

    // ---- 3. 同一个函数，换规则就是换个函数名 ----
    memcpy(a, src, sizeof(src));
    sortBy(a, n, byScore);
    show(a, n, "=== 规则① byScore（分数降序，同分等级升序）===");

    memcpy(a, src, sizeof(src));
    sortBy(a, n, byLevel);
    show(a, n, "=== 规则② byLevel（等级升序，同级分数降序）===");

    memcpy(a, src, sizeof(src));
    sortBy(a, n, byLevelDesc);
    show(a, n, "=== 规则③ byLevelDesc（等级降序，同级分数降序）===");

    // ---- 4. 函数指针变量：自己拿着一个"函数" ----
    printf("\n=== 函数指针变量本身 ===\n");
    bool (*cmp)(const Player&, const Player&) = byScore;   // 定义并指向 byScore
    Player x = { "甲", 90, 1 }, y = { "乙", 80, 2 };
    printf("   cmp(甲, 乙) = %s  →  说明 90 分应该排在 80 分前面\n", cmp(x, y) ? "true" : "false");
    printf("   (*cmp)(甲, 乙) = %s（加 * 也能用，两写法等价）\n", (*cmp)(x, y) ? "true" : "false");
    printf("   sizeof(cmp) = %zu 字节（指针就是 8 字节，跟它指向的函数多大无关）\n", sizeof(cmp));
    cmp = byLevel;                                          // 指针可以改指向
    printf("   改成指向 byLevel 后：cmp(甲, 乙) = %s\n", cmp(x, y) ? "true" : "false");

    // ---- 5. lambda：规则"当场写"，不用另起一个函数 ----
    memcpy(a, src, sizeof(src));
    std::sort(a, a + n, [](const Player& p, const Player& q) {
        return p.score < q.score;            // 分数升序（和 byScore 正好相反）
    });
    show(a, n, "\n=== lambda：分数升序（当场写的规则）===");

    // ---- 6. 带捕获的 lambda：规则里要用外面的变量，就写进 [] ----
    bool desc = false;                       // 外面这个开关决定升序还是降序
    memcpy(a, src, sizeof(src));
    std::sort(a, a + n, [desc](const Player& p, const Player& q) {
        return desc ? p.score > q.score : p.score < q.score;   // 用到了外面的 desc
    });
    show(a, n, "\n=== 带捕获 [desc]，当前 desc = false：分数升序 ===");

    desc = true;                             // 只改外面这个变量，lambda 一个字没动
    memcpy(a, src, sizeof(src));
    std::sort(a, a + n, [desc](const Player& p, const Player& q) {
        return desc ? p.score > q.score : p.score < q.score;
    });
    show(a, n, "\n=== 同一个 lambda，desc 改成 true：分数降序 ===");
    printf("   规则没变，变的是它捕获到的外部状态 —— 这就是捕获的意义\n");

    // ---- 7. auto 存 lambda（lambda 的真实类型是编译器造的，写不出来，所以用 auto）----
    auto twice = [](int v) { return v * 2; };
    printf("\n=== auto 存 lambda ===\n   twice(21) = %d\n", twice(21));

    // ---- 8. 别踩：lambda 里想用外面的变量，必须写进捕获列表 ----
    // std::sort(a, a + n, [](const Player& p, const Player& q) { return p.score >= limit; });  // ❌ limit 没捕获，编译错
    printf("\n记住：lambda 的 [] 里不写，外面的变量就用不了（这是设计，不是 bug）\n");

    // ---- 9. 一个容易被坑的点：比较规则"区分不出元素"时，排序看起来没效果 ----
    printf("\n=== 反例：规则区分不出元素 ===\n");
    memcpy(a, src, sizeof(src));
    std::sort(a, a + n, [](const Player& p, const Player& q) {
        return strlen(p.name) < strlen(q.name);   // 6 个人的名字都是 2 个汉字 = 6 字节，永远相等
    });
    show(a, n, "   按名字长度排（全都一样长）→ 顺序没变");
    printf("\n   看起来像【排序没生效】，其实是规则压根没区分度。\n");
    printf("   所以写比较规则时先问自己一句：这条规则能把这些元素排出先后吗？\n");

    return 0;

}
