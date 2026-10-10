// A11 练习答案：移动构造 与 移动赋值（五法则的后两件）
// 与「移动语义练习-填空.cpp」逐字相同，只是把 4 个 TODO 填上了。
#include <cstdio>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

static int g_copies = 0;
static int g_moves  = 0;

class Buffer {
public:
    Buffer() : data_(nullptr), size_(0) {}

    explicit Buffer(int n, int fill) : data_(new int[n]), size_(n) {
        for (int i = 0; i < n; i++) data_[i] = fill;
    }

    Buffer(const Buffer& other) : data_(nullptr), size_(0) {
        if (other.size_ > 0) {
            data_ = new int[other.size_];
            for (int i = 0; i < other.size_; i++) data_[i] = other.data_[i];
            size_ = other.size_;
        }
        g_copies++;
    }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;
        int* fresh = (other.size_ > 0) ? new int[other.size_] : nullptr;
        for (int i = 0; i < other.size_; i++) fresh[i] = other.data_[i];
        delete[] data_;
        data_ = fresh;
        size_ = other.size_;
        g_copies++;
        return *this;
    }

    // ── TODO 1：移动构造
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        g_moves++;
    }

    // ── TODO 2：移动赋值
    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;      // 自移动保护
        delete[] data_;                        // 先还掉自己原有的
        data_ = other.data_;                   // 偷资源
        size_ = other.size_;
        other.data_ = nullptr;                 // 把对方置空
        other.size_ = 0;
        g_moves++;
        return *this;
    }

    ~Buffer() { delete[] data_; }

    int size() const { return size_; }
    const int* data() const { return data_; }

private:
    int* data_;
    int  size_;
};

// ── TODO 3：把局部对象移进 vector
void pushInto(std::vector<Buffer>& v) {
    Buffer tmp(4, 9);
    v.push_back(std::move(tmp));               // 不写 std::move 会走拷贝构造
}

// ── TODO 4：按值返回（体会 RVO）
Buffer makeBuffer(int n, int fill) {
    return Buffer(n, fill);
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

    bool ok = (c.data() != nullptr)
           && (a.data() == nullptr)
           && (a.size() == 0)
           && (d.size() == 3)
           && (b.data() == nullptr)
           && (v.size() == 1 && v[0].size() == 4 && v[0].data()[0] == 9)
           && (e.size() == 2)
           && (g_moves >= 3);

    printf("\n自检：%s\n", ok ? "全部正确 ✅" : "还有问题 ❌（对照上面的期望值）");
    printf("（ASan 会顺手检查内存问题 —— 进程正常退出就说明没有泄漏 / double free）\n");
    return ok ? 0 : 1;
}
