// A11 练习：移动构造 与 移动赋值（五法则的后两件）
// 编译运行：双击 build-move.cmd
// 目标：给 Buffer 补上「移动」两件套，让它在转移所有权时不复制内存
//      （A10 你已经写过拷贝构造 / 拷贝赋值，这次是它们的"省事版"）
#include <cstdio>
#include <utility>      // std::move
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

static int g_copies = 0;    // 统计拷贝次数
static int g_moves  = 0;    // 统计移动次数

class Buffer {
public:
    Buffer() : data_(nullptr), size_(0) {}

    explicit Buffer(int n, int fill) : data_(new int[n]), size_(n) {
        for (int i = 0; i < n; i++) data_[i] = fill;
    }

    // ---------- 以下是给你的，不用改 ----------
    Buffer(const Buffer& other) : data_(nullptr), size_(0) {   // 拷贝构造：深拷贝
        if (other.size_ > 0) {
            data_ = new int[other.size_];
            for (int i = 0; i < other.size_; i++) data_[i] = other.data_[i];
            size_ = other.size_;
        }
        g_copies++;
    }

    Buffer& operator=(const Buffer& other) {                   // 拷贝赋值
        if (this == &other) return *this;
        int* fresh = (other.size_ > 0) ? new int[other.size_] : nullptr;
        for (int i = 0; i < other.size_; i++) fresh[i] = other.data_[i];
        delete[] data_;
        data_ = fresh;
        size_ = other.size_;
        g_copies++;
        return *this;
    }

    ~Buffer() { delete[] data_; }

    int size() const { return size_; }
    const int* data() const { return data_; }
    // ---------- 给你结束 ----------


    // ============================================================
    // TODO 1：移动构造   Buffer(Buffer&& other) noexcept
    //   要点：
    //     ① 直接把 other 的指针"拿过来"：初始化列表写 data_(other.data_)
    //        —— 不要 new！移动的意义就是"不复制"
    //     ② 必须把 other 置空：other.data_ = nullptr; other.size_ = 0;
    //        —— 不置空的话，other 析构时会把你刚拿走的内存 delete 掉 → 崩
    //     ③ 记一次数：g_moves++;
    //     ④ 函数签名末尾写 noexcept（为什么必须写？看 README 第 3 节）
    // ============================================================


    // ============================================================
    // TODO 2：移动赋值   Buffer& operator=(Buffer&& other) noexcept
    //   要点：
    //     ① 自移动检查：if (this == &other) return *this;
    //        —— 不写的话，下面的 delete[] 会先把资源删掉，再偷到"空"
    //     ② 先 delete[] data_;  把自己原有的内存还回去（不然泄漏）
    //     ③ 偷 other 的指针 + 把 other 置空
    //     ④ g_moves++;  return *this;
    // ============================================================


    // ============================================================
    // TODO 3：把局部 Buffer "移" 进 vector
    //   在下面这个函数体里写：
    //       Buffer tmp(4, 9);
    //       v.push_back(???);      ← ??? 用 std::move(tmp)
    //   为什么这里必须用 std::move：tmp 是具名的左值，
    //   不写 std::move 就会调用拷贝构造（白复制一次）
    // ============================================================

private:
    int* data_;
    int  size_;
};

void pushInto(std::vector<Buffer>& v) {
    // TODO 3：在这里写三行（或用你的写法，效果一样就行）
}


// ============================================================
// TODO 4：写一个按值返回的函数（体会 RVO：连移动都不会发生）
//   要求：函数名 makeBuffer，参数 (int n, int fill)，返回 Buffer
//   函数体只写一行：直接构造并 return
// ============================================================

Buffer makeBuffer(int /*n*/, int /*fill*/) {
    // TODO 4：把参数名前的注释去掉，然后写出那一行
    return Buffer(0, 0);      // ← 先让它能编译；写完请改成正确的实现
}


int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    printf("=== 1) 拷贝构造（已经给你了）===\n");
    Buffer a(3, 7);
    Buffer b = a;
    printf("   a: size=%d 地址=%p\n", a.size(), (void*)a.data());
    printf("   b: size=%d 地址=%p   ← 两个地址应【不同】（各有一块内存）\n",
           b.size(), (void*)b.data());

    printf("\n=== 2) 移动构造  c = std::move(a) ===\n");
    Buffer c = std::move(a);
    printf("   c: size=%d 地址=%p\n", c.size(), (void*)c.data());
    printf("   被移动后的 a: size=%d 地址=%p   ← 地址应与 c【相同】，且 a 已空\n",
           a.size(), (void*)a.data());

    printf("\n=== 3) 移动赋值  d = std::move(b) ===\n");
    Buffer d(6, 1);
    d = std::move(b);
    printf("   d: size=%d 地址=%p\n", d.size(), (void*)d.data());
    printf("   被移动后的 b: size=%d 地址=%p   ← 同上\n", b.size(), (void*)b.data());

    printf("\n=== 4) 自移动  d = std::move(d) ===\n");
    d = std::move(d);
    printf("   结果：d.size=%d   ← 应保持 3（没被清空、也没崩）\n", d.size());

    printf("\n=== 5) 移进 vector ===\n");
    std::vector<Buffer> v;
    pushInto(v);
    printf("   v[0]: size=%d  首元素=%d\n",
           v.empty() ? -1 : v[0].size(),
           (!v.empty() && v[0].size() > 0) ? v[0].data()[0] : -1);

    printf("\n=== 6) 按值返回（RVO）===\n");
    Buffer e = makeBuffer(2, 8);
    printf("   e: size=%d 地址=%p\n", e.size(), (void*)e.data());

    printf("\n拷贝 %d 次，移动 %d 次\n", g_copies, g_moves);

    bool ok = (c.data() != nullptr)          // 移动构造拿到了资源
           && (a.data() == nullptr)          // 被移动的对象已置空
           && (a.size() == 0)
           && (d.size() == 3)                // 移动赋值：拿到了 b 的 3 个元素
           && (b.data() == nullptr)          // b 也被置空
           && (v.size() == 1 && v[0].size() == 4 && v[0].data()[0] == 9)
           && (e.size() == 2)                // 按值返回
           && (g_moves >= 3);                // 至少移动了 3 次

    printf("\n自检：%s\n", ok ? "全部正确 ✅" : "还有问题 ❌（对照上面的期望值）");
    printf("（ASan 会顺手检查内存问题 —— 进程正常退出就说明没有泄漏 / double free）\n");
    return ok ? 0 : 1;
}
