// ============================================================
//  扩容练习（填空版）—— 把"扩容三步"从类里拆出来,先当普通函数写一遍
//
//  为什么先写这个：类里同时要管 构造函数/析构/拷贝/移动,还要管扩容,
//  一次记太多东西就会像上次那样"写不下去"。这里只有三个普通函数。
//
//  编译运行：双击同目录的 build-grow.cmd
//   （它自动调 VS2022 的 cl.exe，带 ASan 内存检查；等价命令：cl /EHsc /W4 /std:c++17 /fsanitize=address 扩容练习-填空.cpp）
//
//  跑对的样子：
//     size=5 cap=8 期望 size=5 cap=8
//     1 2 3 4 5
//     2 4 6 8 10
//  并且 ASan 不报任何错。
// ============================================================
#include <cstddef>
#include <iostream>

// ── B1：打印前 n 个元素（空格分隔,最后换行）
//         练：const 指针（承诺不改数据）+ 循环边界
void print(const int* a, std::size_t n) {
	for(std::size_t i = 0 ; i < n;++i){
		std::cout << a[i];
		if(i + 1 < n){
			std::cout<<" ";
		}
	}// TODO: 用 for 循环打印 a[0]..a[n-1]
    std::cout << '\n';   // ← 写好后删掉这一行
}

// ── B2：把前 n 个元素全部乘 2
//         练：通过指针改到"原数组",而不是改副本
void doubleAll(int* a, std::size_t n) {
    for(std::size_t i = 0 ; i < n; ++i){
		a[i]*= 2;
	}// TODO
}

// ── B3（最重要）：扩容
//     申请 cap*2 的新内存 → 把前 size 个元素搬过去 → 释放旧的 → 更新 cap → 返回新指针
//     练：扩容三步的顺序;顺序写错 = 读到已释放内存(悬垂)或泄漏
//     注意 cap 是【引用】：调用者那边的容量也要跟着变
int* grow(int* old, std::size_t size, std::size_t& cap) {
    std::size_t newcap = cap * 2;
	int* fresh = new  int[newcap];
	for(std::size_t i = 0; i < size;++i){
		fresh[i] = old[i];
	}
    cap = newcap;
	delete[] old;// TODO: ① new  ② 搬  ③ delete[] 旧的  ④ 更新 cap  ⑤ return
    return fresh;                    // ← 现在这样"能跑",但结果是错的,正好用来对照
}

int main() {
    std::size_t cap = 2;
    std::size_t size = 0;
    int* a = new int[cap];

    for (int v : {1, 2, 3, 4, 5}) {
        if (size == cap) {                 // 装满了 → 扩容
            a = grow(a, size, cap);
        }
        a[size++] = v;
    }

    std::cout << "size=" << size << " cap=" << cap << " 期望 size=5 cap=8\n";

    print(a, size);                        // 期望： 1 2 3 4 5
    doubleAll(a, size);
    print(a, size);                        // 期望： 2 4 6 8 10

    delete[] a;                            // 谁 new[] 谁 delete[]
    return 0;
}
