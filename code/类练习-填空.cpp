// A9 练习：类 · 构造 / 析构 / RAII（填空）
// 编译运行：双击 build-class.cmd
// 目标：把 4 个 TODO 填完，让 IntArray 自己管内存，输出和 README 的"期望输出"一致
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================
// 这一节就是把之前练的 grow 收进一个类：
//   data_ / size_ / cap_ 从"三个散着的变量"变成"一个对象的内部状态"；
//   而 new / delete 的配对责任，从"调用者"转移到"这个类自己"——这就是 RAII。
// ============================================================
class IntArray {
public:
    // ========================================================
    // TODO 1：写构造函数 + 析构函数
    //   构造函数：data_ 置空、size_ 和 cap_ 置 0（写在初始化列表里最标准）
    //     参考形状：  IntArray() : data_(...), size_(...), cap_(...) {}
    //   析构函数：把 data_ 指向的数组释放掉（注意 new[] 配 delete[]）
    //     参考形状：  ~IntArray() { ... }
    // ========================================================
    // 在这里写构造 / 析构


    void push_back(int v) {
        // ====================================================
        // TODO 2：满了就翻倍扩容（三步顺序不能错），然后把 v 放进去
        //   ① 申请新块：      int* fresh = new int[newCap];
        //   ② 老数据搬过去：   for (i < size_) fresh[i] = data_[i];
        //   ③ 释放老块再接手： delete[] data_;  data_ = fresh;  cap_ = newCap;
        //   最后：data_[size_] = v;  size_++;
        //   提示：初始 cap_ 是 0，所以 newCap 用 (cap_ == 0 ? 1 : cap_ * 2)
        // ====================================================
        // 在这里写扩容 + 放入

        (void)v;   // ← 填完 TODO 2 就把这一行删掉（它只是让骨架能编过）
    }

    // ========================================================
    // TODO 3：写 size() / cap() 和两个 operator[]
    //   int  size() const            → 返回 size_
    //   int  cap()  const            → 返回 cap_（main 里要看扩容轨迹）
    //   int& operator[](int i)       → 返回 data_[i]（不带 const，所以能写）
    //   int  operator[](int i) const → 返回 data_[i]（带 const，只读）
    //   为什么写两个？因为 const IntArray& 只能调用带 const 的那个版本
    // ========================================================
    // 在这里写 size / cap / operator[]


private:
    int* data_;
    int  size_;
    int  cap_;
};

// ============================================================
// TODO 4：写 sum —— 用 const 引用收参数，证明"只读也能用这个类"
//   签名：int sum(const IntArray& a)
//   里面：从 0 到 a.size() 累加 a[i]
//   注意：这里 a 是 const 引用，只能调到 TODO 3 里那个带 const 的 operator[]
// ============================================================
// 在这里写 sum


int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 显示（否则中文是鬼画符）
#endif

    bool ok = true;
    int  gotCap[6] = { 0 };

    {
        IntArray arr;

        printf("=== 1) 刚建好 ===\n");
        printf("   size=%d cap=%d\n", arr.size(), arr.cap());
        gotCap[0] = arr.cap();

        printf("\n=== 2) push_back 的扩容轨迹 ===\n");
        for (int i = 1; i <= 5; i++) {
            arr.push_back(i * 10);
            gotCap[i] = arr.cap();
            printf("   push %d → size=%d cap=%d\n", i * 10, arr.size(), arr.cap());
        }

        printf("\n=== 3) 读 和 写 ===\n");
        printf("   arr[0]=%d arr[4]=%d\n", arr[0], arr[4]);
        arr[1] = 999;                                  // operator[] 返回引用，所以能改
        printf("   改了 arr[1] 之后：arr[1]=%d\n", arr[1]);
        printf("   用 const 引用求和：sum(arr)=%d\n", sum(arr));

        printf("\n=== 4) 出作用域（不用写 delete）===\n");
        printf("   （析构会自己释放；ASan 会顺带检查有没有泄漏）\n");

        // ================= 自检（不用改） =================
        const int expCap[6] = { 0, 1, 2, 4, 4, 8 };
        for (int i = 0; i <= 5; i++) {
            if (gotCap[i] != expCap[i]) {
                ok = false;
                printf("\n*** 扩容轨迹不对：第 %d 次应该是 cap=%d，实际 %d ***\n", i, expCap[i], gotCap[i]);
            }
        }
        if (arr.size() != 5) {
            ok = false; printf("\n*** size 不对：应该是 5，实际 %d ***\n", arr.size());
        }
        if (arr[0] != 10 || arr[4] != 50) {
            ok = false; printf("\n*** 元素不对：arr[0] 应该 10、arr[4] 应该 50 ***\n");
        }
        if (arr[1] != 999) {
            ok = false; printf("\n*** 写不进去：arr[1] 改成 999 之后没生效（operator[] 要返回引用）***\n");
        }
        if (sum(arr) != 1129) {
            ok = false; printf("\n*** sum 不对：应该是 1129，实际 %d ***\n", sum(arr));
        }
    }   // ← 离开这里，arr 析构（TODO 1 的析构函数在这里被调用）

    printf("\n自检：%s\n", ok ? "全部正确 ✅" : "还有问题 ❌");
    return 0;
}
