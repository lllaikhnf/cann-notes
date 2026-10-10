// A12 练习：运算符重载 —— 让自定义类型用起来像内置类型
// 编译运行：双击 build-op.cmd
// 目标：给 Vec2 补上 4 个运算符，让它能像 double 一样写：a + b、std::cout << a、v[0] = 9
//       （A10/A11 你已经把"对象怎么出生、怎么复制、怎么搬走"搞定了，这次解决"看起来好不好用"）
#include <cstdio>
#include <cmath>
#include <iostream>
#include <sstream>
#include <ostream>
#ifdef _WIN32
#include <windows.h>
#endif

class Vec2 {
public:
    Vec2() : x_(0.0), y_(0.0) {}
    Vec2(double x, double y) : x_(x), y_(y) {}

    double x() const { return x_; }
    double y() const { return y_; }

    // ---------- 以下是给你的例子，不用改 ----------

    // 例子 A：成员函数形式的复合赋值 operator-=（照着它写 TODO 1）
    Vec2& operator-=(const Vec2& rhs) {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        return *this;          // 复合赋值【一律】返回 *this 的引用
    }

    // 例子 B：成员函数形式的 operator*（产生新对象、不改自己 → 末尾 const）
    Vec2 operator*(double k) const {
        return Vec2(x_ * k, y_ * k);
    }
    // ---------- 给你的例子结束 ----------


    // ============================================================
    // TODO 1：复合赋值  +=   （成员函数，返回 Vec2&）
    //   要点：和例子 A 一模一样，只把减号换成加号
    //   想一想：为什么返回 Vec2& 而不是 Vec2？（README 第 2 节）
    // ============================================================
    Vec2& operator+=(const Vec2& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        return *this;
    }


    // ============================================================
    // TODO 4：下标  operator[]  —— 【两个版本都要写】
    //   版本一（非 const）：返回 double&，这样才能当左值：v[0] = 9.0;
    //   版本二（const）   ：返回 const double&，const Vec2 只能调这个
    //   规则：i == 0 返回 x_，否则返回 y_（就两个分量，不用做越界检查）
    //   ★ 两个版本的区别只有"返回的引用带不带 const"，这是 C++ 的标准套路
    // ============================================================
    double& operator[](int i) {
        if (i != 0) {
            return y_;
        }
        return x_;           
    }
    const double& operator[](int i) const {

        return (i == 0) ? x_ : y_;             // ← 同上
    }

private:
    double x_, y_;
};


// ============================================================
// TODO 2：二元加法  operator+  —— 【自由函数】形式
//   为什么不用成员函数？
//     ① 两个操作数地位对等：a + b 和 b + a 都该能用
//     ② 成员函数要求"左操作数必须是你这个类"（将来若是 3.0 + v 就没法写）
//   惯用写法（三行以内）：先拷贝一份 a → 再 += b → 最后返回副本
//   ★ 别返回引用！+ 的语义是"给出一个新对象"，返回引用会连到临时量上（C4172）
// ============================================================
Vec2 operator+(const Vec2& a, const Vec2& b) {
    Vec2 r = a; r += b;
    return r;             // ← 占位实现
}


// ============================================================
// TODO 3：输出  operator<<  —— 【必须】是自由函数
//   因为左操作数是 std::ostream，不是你写的类 —— 成员函数做不到这件事
//   要求输出格式：(x, y)          例如 Vec2(3,4) → (3, 4)
//   提示：os << '(' << v.x() << ", " << v.y() << ')'; 然后 return os;
//   ★ 返回类型必须是 std::ostream&，否则 std::cout << a << b 连写会断
// ============================================================
std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    os << '(' << v.x() << "," << " " << v.y() << ')';
    return os;
}


// ============================================================
// 例子 C：相等比较 operator==（自由函数，也是给你照抄的样板）
// ============================================================
bool operator==(const Vec2& a, const Vec2& b) {
    return std::fabs(a.x() - b.x()) < 1e-9 && std::fabs(a.y() - b.y()) < 1e-9;
}


int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    Vec2 a(3.0, 4.0), b(1.0, 2.0);

    printf("=== 1) 复合赋值 += 与二元 + ===\n");
    Vec2 t(1.0, 1.0);
    t += a;
    std::cout << "   t(1,1) += a(3,4) → t = " << t << "      ← 期望 (4, 5)\n";
    Vec2 s = a + b;
    std::cout << "   a + b = " << s << "                  ← 期望 (4, 6)\n";

    printf("\n=== 2) 链式输出（operator<< 返回 ostream& 才能连写）===\n");
    std::cout << "   a = " << a << " , b = " << b << "\n";

    printf("\n=== 3) 下标：非 const 版能当左值 ===\n");
    Vec2 v(1.0, 2.0);
    v[0] = 9.0;                    // 只有返回 double& 才写得出来
    std::cout << "   v(1,2) 之后 v[0]=9 → v = " << v << "   ← 期望 (9, 2)\n";
    const Vec2 cv(5.0, 6.0);
    std::cout << "   const 对象 cv[1] = " << cv[1] << "          ← 期望 6\n";

    printf("\n=== 4) 例子：operator* 与 operator== ===\n");
    std::cout << "   a * 2 = " << (a * 2.0) << "               ← 期望 (6, 8)\n";
    std::cout << "   a == Vec2(3,4) ? " << (a == Vec2(3.0, 4.0) ? "是" : "否")
              << "          ← 期望 是\n";

    // ---- 自检 ----
    std::ostringstream oss;
    oss << a;
    bool fmt_ok = (oss.str() == "(3, 4)");

    bool ok = (std::fabs(t[0] - 4.0) < 1e-9) && (std::fabs(t[1] - 5.0) < 1e-9)
           && (std::fabs(s[0] - 4.0) < 1e-9) && (std::fabs(s[1] - 6.0) < 1e-9)
           && (std::fabs(v[0] - 9.0) < 1e-9) && (std::fabs(v[1] - 2.0) < 1e-9)
           && (std::fabs(cv[1] - 6.0) < 1e-9)
           && fmt_ok;

    printf("\n自检：%s\n", ok ? "全部正确 ✅" : "还有问题 ❌（对照上面每行的期望值）");
    if (!fmt_ok) {
        printf("（operator<< 的输出要正好是 \"(3, 4)\"：圆括号 + 逗号 + 一个空格）\n");
    }
    printf("（ASan 会顺手检查内存问题 —— 进程正常退出就说明没有越界 / 泄漏）\n");
    return ok ? 0 : 1;
}
