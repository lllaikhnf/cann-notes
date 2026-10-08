# 点评 · A9 类 · 构造 / 析构 / RAII · 第一版

**结果：编译失败，50 个错误 —— 但归到根上只有 8 个。**
其中**第 1 个根因一个人就带出了约 40 条**（这就是"一个错带出一串"的极端案例）。

**先说结论**：这一版的结构和思路**基本是对的**——初始化列表写对了、`sum` 的签名一字不差、两个 `operator[]` 的"两个版本"意识也有了。
卡住你的还是**符号级滑手**，其中 for 里那个逗号，是**第三次**了（A6、A8 各一次）。

---

## 一、根因 ①：析构函数的写法 + 一个多余的 `}`（≈40 条报错都是它）

你写的：

```cpp
    IntArray() :data_(nullptr), size_(0), cap_(0) {}
    ~IntArray()delete[] data_;      // ❌ 少了函数体的花括号
}                                    // ❌ 这个 } 把 class 提前关掉了！
```

正确写法：

```cpp
    IntArray() : data_(nullptr), size_(0), cap_(0) {}
    ~IntArray() { delete[] data_; }   // ✅ 函数体要花括号
```

**那个多余的 `}` 是灾难的开始**：它让 `class IntArray` 在这里就结束了，
于是后面的 `push_back`、`size`、`cap`、`operator[]`、`private:`、`data_/size_/cap_` **全都跑到类外面去了**。
编译器看不懂，就顺着往下报了 40 条：

```
error C2628: "IntArray"后面接"void"是非法的(是否忘记了";"?)     ← push_back 变成"类外面的函数"
error C2065: "size_": 未声明的标识符                            ← 成员变量不认识了
error C2270: "size": 非成员函数上不允许修饰符                    ← const 只能用在成员函数上
error C2059: 语法错误:"private"                                 ← private: 出现在类外面
error C2039: "push_back": 不是 "IntArray" 的成员
```

**所以第 1 条改完，这 40 条会一起消失。** 这就是我一直说的：**只看第一个错误**。

---

## 二、根因 ②：`=` 和 `==`（C++ 最经典的坑）

```cpp
        if (size_ = cap_) {          // ❌ 这是"赋值"，不是"比较"
        if (size_ == cap_) {         // ✅
```

`size_ = cap_` 干了两件事：
1. 把 `cap_` 的值**赋给** `size_`（顺手把 size_ 改坏了）
2. 用赋完的值当条件 —— `cap_` 初始是 0，所以**永远进不了扩容分支**

后果：不扩容 + `size_` 被改坏 → `data_[size_] = v` 往空指针上写 → **程序必崩**（这种错运行时才炸，编译不一定报）。

**两个防御习惯**：
- 编译警告开到 `/W4`，MSVC 会报 `C4706: 条件表达式中的赋值`
- 或者把常量写左边（"尤达写法"）：`if (0 == cap_)` —— 写成 `=` 会直接编译不过

---

## 三、根因 ③④⑤⑥⑦⑧（一条一条来）

| # | 行 | 你写的 | 应该写 | 说明 |
| --- | --- | --- | --- | --- |
| ③ | 32 | `int* fresh = newCap;` | `int* fresh = new int[newCap];` | ❌ `error C2440: 无法从"int"转换为"int*"`。要"申请一块能装 newCap 个 int 的内存"，不能直接把数字当指针 |
| ④ | 33 | `for (int i = 1, i < size_, i++)` | `for (int i = 0; i < size_; i++)` | **两处错**：for 三段用**分号**（写逗号被当成"声明两个 i"→ `C2086: int i 重定义`）；而且要从 `i = 0` 开始搬，写 `1` 会漏掉第 0 个元素 |
| ⑤ | 36 | `size_++` | `size_++;` | 少分号 → `C2143: 缺少";"(在"}"的前面)` |
| ⑥ | 63 | `int operator[](int i) { ... }` | `int operator[](int i) const { ... }` | 少一个 `const`。于是它和上面那个只差返回类型 → `C2556: 重载函数只是在返回类型上不同`（不合法） |
| ⑦ | 81 | `for (int i = 0, i < a.size(), i++)` | `for (int i = 0; i < a.size(); i++)` | **又是同一个逗号**（第 ④ 条的翻版） |
| ⑧ | 82 | `total += a[i];`（在 `const IntArray&` 里） | 同上（修好 ⑥ 就通了） | `C2676: "const IntArray" 不定义该运算符` —— 因为你的 `operator[]` 没带 `const` |

### ⚠️ 关于 ④ 和 ⑦：这是**第三次**了

`for` 的三段之间用**分号** `;`，不是逗号 `,`：

```cpp
for (int i = 0; i < size_; i++)     // ✅ 分号分号
for (int i = 0, i < size_, i++)     // ❌ 编译器当成"声明两个变量"
```

回顾一下：**A6** 你在参数/实参里把 `,` 写成 `;`；**A8** 你在 `})` 后面漏 `;`；**这次**你在 for 里把 `;` 写成 `,`。
三次都是"分隔符号用错"。**这不是智商问题，是手感问题**，靠一个动作解决：

> **写完 for 那一行，手指停一下，数一遍：两个分号，都在吗？**
> 顺带看一眼：是 `=` 还是 `==`？（赋值/比较）
> 再顺带一眼：`new` 后面有没有类型和方括号？（`new int[n]`）

这三个动作加起来 5 秒，能挡掉你目前 90% 的编译错误。

---

## 四、做对的地方（这些是真对）

| 位置 | 你写的 | 评价 |
| --- | --- | --- |
| 构造函数 | `IntArray() :data_(nullptr), size_(0), cap_(0) {}` | ✅ **初始化列表完全正确**，三个成员一个不差 |
| `size()` / `cap()` | `int size() const { return size_; }` / `int cap() const` | ✅ 对了，`const` 也没忘 |
| `operator[]` 带引用 | `int& operator[](int i) { return data_[i]; }` | ✅ 返回引用，所以能写进去 |
| 两个版本 | 你确实写了**两个** `operator[](int i)` | ✅ **意识对了**——知道要写两个版本，只是第二个忘了加 `const` 关键字 |
| `sum` 签名 | `int sum(const IntArray& a)` | ✅ **一字不差**，正是我强调的 `const&` |
| 扩容三步顺序 | 申请 → 搬 → `delete[]` → 接手 | ✅ 顺序对 |

**知识点你是懂的。** 这一版的问题是"手"，不是"脑子"。

---

## 五、改完应该长这样

```cpp
class IntArray {
public:
    IntArray() : data_(nullptr), size_(0), cap_(0) {}
    ~IntArray() { delete[] data_; }                  // ① 花括号；删掉多余的 }

    void push_back(int v) {
        if (size_ == cap_) {                          // ② == 不是 =
            int newCap = (cap_ == 0) ? 1 : cap_ * 2;
            int* fresh = new int[newCap];             // ③ 类型 + 方括号
            for (int i = 0; i < size_; i++) {         // ④ 分号；从 0 开始
                fresh[i] = data_[i];
            }
            delete[] data_;
            data_ = fresh;
            cap_ = newCap;
        }
        data_[size_] = v;
        size_++;                                      // ⑤ 分号
    }

    int size() const { return size_; }
    int cap()  const { return cap_; }
    int& operator[](int i)       { return data_[i]; }
    int  operator[](int i) const { return data_[i]; }  // ⑥ 加 const

private:
    int* data_;
    int  size_;
    int  cap_;
};

int sum(const IntArray& a) {
    int total = 0;
    for (int i = 0; i < a.size(); i++) {              // ⑦ 分号
        total += a[i];
    }
    return total;
}
```

改完双击 `build-class.cmd`，应该看到：
- 编译 **0 error 0 warning**（/W4 + ASan）
- 扩容轨迹 **1 → 2 → 4 → 4 → 8**
- `sum(arr)=1129`
- 末行 `自检：全部正确 ✅`

---

## 六、自测 3 问（改完答复我）

1. `if (size_ = cap_)` 里，`size_` 最后变成了几？为什么这个错编译**不一定**报？
2. `for` 三段之间为什么必须是分号？写成逗号编译器把它理解成了什么？
3. 为什么 `int operator[](int i)` 必须带 `const` 才能被 `sum(const IntArray&)` 调用？

---

## 七、一句话总结

> **结构对、签名对、初始化列表对；错在三个符号（`;` `;` `==`）和一个 `const`。**
> 你已经是"知道要写什么"的阶段了，接下来练的是"落笔不出错"——靠的是写完那 5 秒自检，不是更用力。

---

—— 本机 · 2026-10-08
