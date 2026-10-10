// A10 练习：拷贝构造 与 三法则（深拷贝）（填空）
// 编译运行：双击 build-copy.cmd
// 目标：把这个 Vec 补齐「三法则 + reserve」，让它能被安全地拷贝、赋值、自赋值
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#endif

class Vec {
public:
    Vec() : data_(nullptr), size_(0), cap_(0) {}

    // 按 n 个 fill 造一个数组（这个是给你的，不用改：直接 new，不依赖 reserve）
    Vec(int n, int fill) : data_(new int[n]), size_(n), cap_(n) {
        for (int i = 0; i < n; i++) data_[i] = fill;
    }

    // ========================================================
    // TODO 1：析构函数
    //   把 data_ 指向的内存还回去（new[] 配 delete[]）
    // ========================================================
    // 在这里写 ~Vec()
    ~Vec() { delete[] data_; }


    // ========================================================
    // TODO 2：拷贝构造（深拷贝）
    //   触发时机：Vec b = a;   或者   传值 f(a);
    //   要做的事：
    //     ① 先把成员初始化成空（data_(nullptr), size_(0), cap_(0)）
    //     ② reserve(other.size_)          ← 申请一块**自己的**新内存
    //     ③ 逐个把 other 的元素拷过来
    //     ④ size_ = other.size_;
    //   关键：新对象必须有自己的内存，不能和 other 共用
    // ========================================================
    // 在这里写 Vec(const Vec& other)
    Vec(const Vec& other) :data_(nullptr), size_(0), cap_(0) {
        reserve(other.size_);
        for (int i = 0; i < other.size_; i++) {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
    }


    // ========================================================
    // TODO 3：拷贝赋值（三法则里最容易写错的一个）
    //   触发时机：c = a;   （两个**已经存在**的对象之间赋值）
    //   三件事：
    //     ① 自赋值检查：if (this == &other) return *this;
    //        （不写的话 v = v 会先释放自己的内存，再从已被释放的内存里拷 → 崩）
    //     ② reserve(other.size_) + 逐个拷 + size_ = other.size_;
    //     ③ return *this;   ← 返回引用，才能连写 a = b = c
    // ========================================================
    // 在这里写 Vec& operator=(const Vec& other)
    Vec& operator=(const Vec& other) {
        if (this == &other) return *this;
        reserve(other.size_);
        for (int i = 0; i < other.size_; i++) {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
        return *this;
    }


    // ========================================================
    // TODO 4：reserve —— 把"扩容"抽成一个函数（A9 里你在 push_back 里写过的那三步）
    //   要求：
    //     如果 newCap <= cap_，直接 return（够用就不折腾）
    //     否则：申请 new int[newCap] → 把老的 size_ 个元素搬过去
    //           → delete[] 老块 → data_ 接手新块 → cap_ = newCap
    //   为什么抽出来：拷贝构造和拷贝赋值都要用它
    // ========================================================
    // 在这里写 reserve(int newCap)
    void reserve(int newCap) {
        if (newCap <= cap_) return;
        int* fresh = new int[newCap];
        for (int i = 0; i < size_; i++) {
            fresh[i] = data_[i];
        }
        delete[] data_;
        data_ = fresh;
        cap_ = newCap;
    }


    // ---------- 以下是给你的，不用改 ----------
    int  size() const { return size_; }
    int  cap()  const { return cap_; }
    const int* data() const { return data_; }
    int  get(int i) const { return data_[i]; }
    void set(int i, int v) { data_[i] = v; }

    void push_back(int v) {
        if (size_ == cap_) reserve(cap_ == 0 ? 1 : cap_ * 2);
        data_[size_] = v;
        size_++;
    }

private:
    int* data_;
    int  size_;
    int  cap_;
};

int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    bool ok = true;

    // ---- 1) 拷贝构造：必须是深拷贝 ----
    {
        Vec a(3, 7);
        Vec b = a;                       // 走 TODO 2
        printf("=== 1) 拷贝构造 ===\n");
        printf("   a: size=%d 地址=%p 内容=%d %d %d\n", a.size(), (const void*)a.data(), a.get(0), a.get(1), a.get(2));
        printf("   b: size=%d 地址=%p 内容=%d %d %d\n", b.size(), (const void*)b.data(), b.get(0), b.get(1), b.get(2));

        if (a.data() == b.data()) { ok = false; printf("\n*** 深拷贝失败：a 和 b 指向同一块内存（还是浅拷贝）***\n"); }
        if (b.size() != 3 || b.get(0) != 7 || b.get(2) != 7) { ok = false; printf("\n*** 拷贝构造内容不对 ***\n"); }

        b.set(0, 99);
        printf("   改 b[0]=99 之后：a[0]=%d（应 7）b[0]=%d（应 99）\n", a.get(0), b.get(0));
        if (a.get(0) != 7) { ok = false; printf("\n*** 改 b 影响到了 a：说明两块内存共用了 ***\n"); }
    }

    // ---- 2) 拷贝赋值：两个已存在的对象之间 ----
    {
        Vec a(3, 7);
        Vec c(1, 1);
        printf("\n=== 2) 拷贝赋值 ===\n");
        printf("   赋值前：c 的 size=%d cap=%d 地址=%p\n", c.size(), c.cap(), (const void*)c.data());
        c = a;                           // 走 TODO 3
        printf("   赋值后：c 的 size=%d cap=%d 地址=%p 内容=%d %d %d\n",
               c.size(), c.cap(), (const void*)c.data(), c.get(0), c.get(1), c.get(2));

        if (c.size() != 3 || c.get(0) != 7 || c.get(2) != 7) { ok = false; printf("\n*** 拷贝赋值内容不对 ***\n"); }
        if (c.data() == a.data()) { ok = false; printf("\n*** 拷贝赋值也是浅拷贝：c 和 a 共用内存 ***\n"); }
    }

    // ---- 3) 自赋值：a = a ----
    {
        Vec a(3, 7);
        printf("\n=== 3) 自赋值 a = a ===\n");
        a = a;                           // 没有 TODO 3 里那句自赋值检查，这里就会崩
        printf("   结果：size=%d 内容=%d %d %d\n", a.size(), a.get(0), a.get(1), a.get(2));
        if (a.size() != 3 || a.get(0) != 7 || a.get(2) != 7) { ok = false; printf("\n*** 自赋值把自己的数据搞坏了 ***\n"); }
    }

    // ---- 4) 连写赋值 + push_back 扩容（用到 reserve）----
    {
        Vec a(2, 5);
        Vec b, c;
        printf("\n=== 4) 连写赋值 + 扩容 ===\n");
        b = c = a;                       // operator= 返回 *this 才能这么写
        printf("   b: size=%d 内容=%d %d ；c: size=%d 内容=%d %d\n",
               b.size(), b.get(0), b.get(1), c.size(), c.get(0), c.get(1));
        if (b.size() != 2 || c.size() != 2 || b.get(1) != 5 || c.get(1) != 5) { ok = false; printf("\n*** 连写赋值不对（是不是忘了 return *this?）***\n"); }

        Vec d;
        for (int i = 1; i <= 5; i++) {
            d.push_back(i * 10);
            printf("   push %d → size=%d cap=%d\n", i * 10, d.size(), d.cap());
        }
        if (d.size() != 5 || d.cap() != 8 || d.get(0) != 10 || d.get(4) != 50) {
            ok = false; printf("\n*** 扩容/内容不对：size=%d cap=%d（应 5 / 8）***\n", d.size(), d.cap());
        }
    }

    printf("\n自检：%s\n", ok ? "全部正确 ✅" : "还有问题 ❌");
    printf("（ASan 还会顺手检查有没有内存泄漏 / double free —— 正常退出就说明没有）\n");
    return 0;
}
