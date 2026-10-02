// A6：结构体 struct 与 引用 & —— 配套演示
// 编译：build-struct-demo.cmd   （MSVC + /W4，故意把该报的警告暴露出来）
#include <cstdio>
#include <cstring>

// ---------- 1. struct 是什么：把"一个记录"打包成一个类型 ----------
struct Student {
    char name[16];      // 注意：数组不能整体赋值，初始化可以，之后要用 strcpy
    int  score;
    int  id;
};

// ---------- 2. 内存对齐：sizeof 不等于"成员加起来" ----------
struct Bad  { char c; int i; char d; };    // 顺序不好
struct Good { int i; char c; char d; };    // 顺序调一下，可能省 4 字节

// ---------- 3. 三种传参方式，改不改得动原对象一目了然 ----------
void byValue(Student s)        { s.score += 100; }              // 拷贝一份，改的是副本
void byPointer(Student* p)     { p->score += 100; }             // 能改，但要解引用/可能为空
void byReference(Student& r)   { r.score += 100; }              // 能改，语法像值
void byConstRef(const Student& r) {                             // 只读、不拷贝：大赛标准写法
    printf("      （只读）%s 的分数 = %d\n", r.name, r.score);
}

int main(void)
{
    // ---- 定义与初始化 ----
    Student a = { "小明", 90, 1 };      // 聚合初始化：按顺序给
    Student b{};                        // 全零初始化
    printf("=== 1. 定义与初始化 ===\n");
    printf("  a = {%s, %d, %d}\n", a.name, a.score, a.id);
    printf("  b（零初始化）= {\"%s\", %d, %d}\n", b.name, b.score, b.id);

    // ---- 复制：struct 可以整体拷贝（数组做不到） ----
    Student c = a;                      // 逐成员值拷贝
    c.score = 60;
    printf("\n=== 2. 复制语义 ===\n");
    printf("  c = a 之后改 c.score = 60：a.score = %d（没被影响），c.score = %d\n", a.score, c.score);

    // ---- sizeof 与对齐 ----
    printf("\n=== 3. sizeof 与内存对齐 ===\n");
    printf("  sizeof(Student) = %zu （char[16] + int + int = %zu 才是最理想的，但对齐会补）\n",
           sizeof(Student), sizeof(char)*16 + sizeof(int)*2);
    printf("  struct Bad  { char; int; char; } = %zu 字节\n", sizeof(struct Bad));
    printf("  struct Good { int; char; char; } = %zu 字节\n", sizeof(struct Good));
    printf("  → 成员一样多，只是顺序不同，大小就差了 %zu 字节\n",
           sizeof(struct Bad) - sizeof(struct Good));

    // ---- 指针与 -> ----
    printf("\n=== 4. 指针访问成员 ===\n");
    Student* p = &a;
    printf("  (*p).score = %d   和   p->score = %d   是一回事\n", (*p).score, p->score);

    // ---- 三种传参 ----
    printf("\n=== 5. 传参三兄弟 ===\n");
    byValue(a);      printf("  值传递后 a.score = %d（没变）\n", a.score);
    byPointer(&a);   printf("  指针传递后 a.score = %d（变了）\n", a.score);
    byReference(a);  printf("  引用传递后 a.score = %d（变了，语法还是 a.score）\n", a.score);
    byConstRef(a);   // 只读、零拷贝

    // ---- 结构体数组：比赛里最常用的形态 ----
    printf("\n=== 6. 结构体数组 ===\n");
    Student cls[3] = { { "甲", 70, 1 }, { "乙", 85, 2 }, { "丙", 95, 3 } };
    int sum = 0;
    for (int i = 0; i < 3; i++) {
        printf("  #%d %s : %d\n", cls[i].id, cls[i].name, cls[i].score);
        sum += cls[i].score;
    }
    printf("  总分 = %d，平均 = %.1f\n", sum, sum / 3.0);

    // ---- 引用是什么：就是"变量的别名" ----
    printf("\n=== 7. 引用 = 别名 ===\n");
    int x = 5;
    int& rx = x;        // rx 是 x 的别名，不是新变量
    rx = 42;
    printf("  x = %d（通过引用改的），&x = %p, &rx = %p（同一个地址）\n", x, (void*)&x, (void*)&rx);

    return 0;
}
