// A9：类与对象 · 构造 / 析构 / RAII —— 配套演示
// 编译：build-class-demo.cmd
// 一句话：这一节看三件事 —— ① 构造函数和析构函数什么时候自动被调用
//          ② 默认拷贝构造干的是「浅拷贝」（成员逐个拷，指针只拷地址）
//          ③ RAII：把「资源的生命周期」绑在「对象的生命周期」上
#include <cstdio>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================
// ① struct 和 class 只差「默认访问权限」
//    struct 默认 public，class 默认 private —— 别的完全一样
// ============================================================
struct PointS { int x, y; };            // 外面能直接写 p.x
class  PointC {                         // 外面访问不了 private 的 x / y
    int x, y;
public:
    PointC(int x0, int y0) : x(x0), y(y0) {}      // 有构造函数，成员就不会是「不确定的值」
    int getX() const { return x; }
    int getY() const { return y; }
};

// ============================================================
// ② 构造 / 析构：谁在什么时候自动调用它们
// ============================================================
class Tracer {
public:
    const char* name;

    Tracer(const char* n) : name(n) { printf("      [构造] %s\n", name); }

    // 拷贝构造：用另一个 Tracer 来初始化这一个时调用（比如「传值」）
    Tracer(const Tracer& other) : name(other.name) {
        printf("      [拷贝构造] %s（说明这里发生了一次拷贝）\n", name);
    }

    ~Tracer() { printf("      [析构] %s\n", name); }
};

void byValue(Tracer t) {                 // 参数是「值」→ 进来时拷贝一次
    printf("      函数体里：%s\n", t.name);
}                                        // 离开函数，形参 t 析构

void scopeDemo() {
    printf("  进入 scopeDemo\n");
    Tracer a("a");
    {
        Tracer b("b（内层作用域）");
        printf("  内层作用域里\n");
    }                                    // b 在这里析构
    printf("  内层作用域结束，但 a 还活着\n");
    printf("  离开 scopeDemo（马上会看到 a 的析构）\n");
}                                        // a 在这里析构

// ============================================================
// ③ RAII：资源在构造函数里拿，在析构函数里还
//    好处：不管从哪条路离开作用域（正常结束、return、抛异常），析构都会跑
// ============================================================
class IntArray {
public:
    IntArray() : data_(nullptr), size_(0), cap_(0) {}                 // 构造：空数组

    ~IntArray() { delete[] data_; printf("      [析构] 释放了容量 %d 的那块内存\n", cap_); }

    int  size() const { return size_; }
    int  cap()  const { return cap_; }
    int& operator[](int i)       { return data_[i]; }                 // 可读可写
    int  operator[](int i) const { return data_[i]; }                 // const 版本：只读

    void push_back(int v) {
        if (size_ == cap_) {                                          // 满了 → 容量翻倍
            int newCap = (cap_ == 0) ? 1 : cap_ * 2;
            int* fresh = new int[newCap];                             // ① 申请新的
            for (int i = 0; i < size_; i++) fresh[i] = data_[i];      // ② 老数据搬过去
            delete[] data_;                                           // ③ 释放旧的（一定要在搬完之后）
            data_ = fresh;
            cap_ = newCap;
        }
        data_[size_] = v;                                             // ④ 放进新元素
        size_++;
    }

private:
    int* data_;
    int  size_;
    int  cap_;
};

// 演示 RAII 的用处：中途 return 也不会泄漏
void earlyReturn(int n) {
    IntArray arr;
    arr.push_back(n);
    printf("  函数里放了 %d，size=%d\n", n, arr.size());
    if (n > 0) {
        printf("  提前 return —— 这里没写任何 delete，但析构会自动跑\n");
        return;
    }
    printf("  走到函数末尾\n");
}

int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    printf("=== ① struct vs class：只差默认访问权限 ===\n");
    PointS ps{ 3, 4 };
    ps.x = 10;                                   // struct 默认 public，随便改
    PointC pc(3, 4);
    printf("   PointS 能直接改：x=%d\n", ps.x);
    printf("   PointC 的 x 是 private，只能 getX()=%d getY()=%d\n", pc.getX(), pc.getY());
    printf("   （把 class 改成 struct、或者加一句 public: 就不一样了 —— 差别只有这一处）\n");
    printf("   （另：要是像 PointC 这样不写构造函数就 PointC pc; 成员会是「不确定的值」——\n");
    printf("     这正是我们要写构造函数的原因之一）\n");

    printf("\n=== ② 构造与析构的调用时机 ===\n");
    scopeDemo();

    printf("\n   —— 再看「传值」会发生什么 ——\n");
    {
        Tracer outside("outside");
        byValue(outside);                        // 值传递 → 拷贝构造
        printf("  传值调用结束，outside 还在（拷贝出来的那个已经析构了）\n");
    }                                            // outside 在这里析构（和 a 一样，离开作用域才死）

    printf("\n=== ③ RAII：IntArray 自己管内存 ===\n");
    {
        IntArray arr;
        printf("  刚建好：size=%d cap=%d\n", arr.size(), arr.cap());
        for (int i = 1; i <= 5; i++) {
            arr.push_back(i * 10);
            printf("  push %d → size=%d cap=%d\n", i * 10, arr.size(), arr.cap());
        }
        printf("  用下标读：arr[0]=%d arr[4]=%d\n", arr[0], arr[4]);
        arr[1] = 999;                            // operator[] 返回引用，所以能写
        printf("  改过之后：arr[1]=%d\n", arr[1]);
        printf("  离开这个作用域 —— 不用写 delete，析构会释放\n");
    }

    printf("\n=== ④ 中途 return 也不泄漏 ===\n");
    earlyReturn(7);

    printf("\n记住三句话：\n");
    printf("  · 构造函数在对象出生时自动调用，析构函数在对象死亡时自动调用\n");
    printf("  · 默认拷贝构造是浅拷贝：成员逐个拷，指针只拷地址（两个对象会指向同一块内存）\n");
    printf("  · RAII = 资源在构造函数里拿、在析构函数里还，于是「忘记释放」被编译器接手了\n");
    return 0;
}
