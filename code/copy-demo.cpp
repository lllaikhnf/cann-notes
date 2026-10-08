// A10：拷贝构造 与 三法则（深拷贝）—— 配套演示
// 编译：build-copy-demo.cmd
// 一句话：编译器默认给的"拷贝"是浅拷贝（指针只拷地址），
//         要让一个类真正能安全地拷贝，得自己写「三法则」：析构 / 拷贝构造 / 拷贝赋值
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================
// ① 浅拷贝会发生什么
//    这个类故意**不写**拷贝构造和拷贝赋值 → 用编译器默认生成的
//    ⚠️ 它的析构函数故意"什么都不做"，好让这个演示能安全跑完。
//       真实代码里析构一定会 delete[]，那样两个对象析构时就会 double free
//       —— 那个版本单独放在 copy-demo-浅拷贝会炸.cpp 里（用 ASan 跑，能看到报错）
// ============================================================
class ShallowVec {
public:
    int* data_;
    int  size_;

    ShallowVec(int n, int fill) : data_(new int[n]), size_(n) {
        for (int i = 0; i < n; i++) data_[i] = fill;
    }
    ~ShallowVec() {
        // 真实代码：delete[] data_;     ← 写了这句，下面的 b 和 a 就会 double free
        printf("      [析构] ShallowVec（这里故意没 delete[]，见注释）\n");
    }
};

// ============================================================
// ② 三法则：自己写「析构 + 拷贝构造 + 拷贝赋值」
// ============================================================
class Vec {
public:
    Vec() : data_(nullptr), size_(0) {}

    Vec(int n, int fill) : data_(nullptr), size_(0) {
        reserve(n);
        for (int i = 0; i < n; i++) data_[i] = fill;
        size_ = n;
    }

    // ---- 三法则之一：析构 ----
    ~Vec() { delete[] data_; }

    // ---- 三法则之二：拷贝构造（深拷贝）----
    // 触发时机：Vec b = a;  或者 传值 f(a);
    Vec(const Vec& other) : data_(nullptr), size_(0) {
        reserve(other.size_);                       // 自己申请一块**新的**内存
        for (int i = 0; i < other.size_; i++) data_[i] = other.data_[i];   // 内容搬过来
        size_ = other.size_;
        printf("      [拷贝构造] 新地址 %p ← 从 %p 复制内容\n",
               (void*)data_, (const void*)other.data_);
    }

    // ---- 三法则之三：拷贝赋值 ----
    // 触发时机：c = a;（两个**已经存在**的对象之间赋值）
    Vec& operator=(const Vec& other) {
        printf("      [拷贝赋值] 被调用\n");
        if (this == &other) {                       // ① 自赋值检查：v = v 时直接返回
            printf("      [拷贝赋值] 检测到自赋值（this == &other），什么都不做\n");
            return *this;
        }
        reserve(other.size_);                       // ② 申请新的、搬内容（reserve 里会释放旧的）
        for (int i = 0; i < other.size_; i++) data_[i] = other.data_[i];
        size_ = other.size_;
        return *this;                               // ③ 返回 *this，才能连写 a = b = c
    }

    // ---- 下面这些不是三法则，只是让演示方便 ----
    int  size() const { return size_; }
    int  cap()  const { return cap_; }
    const int* data() const { return data_; }
    void set(int i, int v) { data_[i] = v; }
    int  get(int i) const { return data_[i]; }

    void reserve(int newCap) {
        if (newCap <= cap_) return;
        int* fresh = new int[newCap];
        for (int i = 0; i < size_; i++) fresh[i] = data_[i];
        delete[] data_;
        data_ = fresh;
        cap_ = newCap;
    }

private:
    int* data_;
    int  size_;
    int  cap_ = 0;          // C++11 起的"成员默认初值"，构造函数可以不写它
};

int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    printf("=== ① 默认拷贝是「浅拷贝」：指针只拷地址 ===\n");
    {
        ShallowVec a(3, 7);
        ShallowVec b = a;                 // 用的是编译器默认的拷贝构造
        printf("   a.data_ = %p\n", (void*)a.data_);
        printf("   b.data_ = %p\n", (void*)b.data_);
        printf("   两个地址%s\n", a.data_ == b.data_ ? "**相同** —— 两个对象共用同一块内存！" : "不同");
        printf("   → 后果：改 b 也会改到 a；而且两个对象析构时会各自 delete[] 同一块内存 → double free\n");
    }

    printf("\n=== ② 三法则之拷贝构造：深拷贝 ===\n");
    {
        Vec a(3, 7);
        printf("   a: size=%d 地址=%p 内容=%d %d %d\n", a.size(), (const void*)a.data(), a.get(0), a.get(1), a.get(2));
        Vec b = a;                        // 走我们自己写的拷贝构造
        printf("   b: size=%d 地址=%p 内容=%d %d %d\n", b.size(), (const void*)b.data(), b.get(0), b.get(1), b.get(2));
        printf("   两个地址%s\n", a.data() == b.data() ? "相同（不对！）" : "**不同** —— 各自一块内存，互不影响");

        b.set(0, 99);
        printf("   改了 b[0] = 99 之后：a[0]=%d（应该是 7）、b[0]=%d（应该是 99）\n", a.get(0), b.get(0));
    }

    printf("\n=== ③ 三法则之拷贝赋值：两个已存在的对象之间赋值 ===\n");
    {
        Vec a(3, 7);
        Vec c(1, 1);                      // 故意让 c 的容量比 a 小，这样赋值时会真的重新申请内存
        printf("   赋值前：c 的 size=%d 地址=%p\n", c.size(), (const void*)c.data());
        c = a;                            // 走我们写的拷贝赋值
        printf("   赋值后：c 的 size=%d 地址=%p 内容=%d %d %d\n",
               c.size(), (const void*)c.data(), c.get(0), c.get(1), c.get(2));
        printf("   → 地址变了：reserve 里先释放 c 原来的内存，再申请新的（顺序不能反）\n");
        printf("   → 顺带一提：如果 c 的容量本来就够大，reserve 会直接复用，不白折腾一次\n");
    }

    printf("\n=== ④ 为什么必须写「自赋值检查」 ===\n");
    {
        Vec a(3, 7);
        printf("   执行 a = a;（看起来毫无意义，但真会发生：可能是引用/指针绕了一圈）\n");
        a = a;
        printf("   结果：size=%d 内容=%d %d %d —— 没被破坏\n", a.size(), a.get(0), a.get(1), a.get(2));
        printf("   → 没有那句 if (this == &other) return *this; 的话：\n");
        printf("      reserve 会先 delete[] 掉自己的内存，再从不存在的内存里拷 → 读到已释放的内存 → 崩\n");
    }

    printf("\n记住三句话：\n");
    printf("  · 默认拷贝 = 浅拷贝：成员逐个拷，指针只拷地址 → 两个对象共用一块内存\n");
    printf("  · 三法则：只要你写了析构函数，通常也要写拷贝构造和拷贝赋值\n");
    printf("  · 拷贝赋值里三件事：自赋值检查 → 释放旧内存 → 申请新的再拷；最后 return *this\n");
    printf("  · 不想操心这些，就用 std::vector —— 它的三法则（其实还有移动）都写对了\n");
    return 0;
}
