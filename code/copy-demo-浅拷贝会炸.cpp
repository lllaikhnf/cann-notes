// A10 附：浅拷贝真的会炸 —— 这个文件是"故意炸给你看"的
// 编译（一定要带 ASan，不然可能"看起来没事"）：
//   cl /nologo /EHsc /W4 /utf-8 /std:c++17 /Zi /fsanitize=address /Fe:boom.exe copy-demo-浅拷贝会炸.cpp /link /DEBUG
#include <cstdio>

class Shallow {
public:
    int* data_;
    int  size_;

    Shallow(int n, int fill) : data_(new int[n]), size_(n) {
        for (int i = 0; i < n; i++) data_[i] = fill;
    }

    // 析构里正常释放 —— 问题不在它，在于"默认拷贝构造"让两个对象指向同一块内存
    ~Shallow() { delete[] data_; }
};

int main(void)
{
    Shallow a(3, 7);
    Shallow b = a;                       // ← 编译器默认的拷贝构造：b.data_ 和 a.data_ 是同一个地址
    printf("a.data_ = %p\n", (void*)a.data_);
    printf("b.data_ = %p\n", (void*)b.data_);
    printf("两个地址相同 → 作用域结束时，a 和 b 会各自 delete[] 同一块内存\n");
    printf("（下面就是 double free，ASan 会拦下来并报错）\n");
    return 0;
}
