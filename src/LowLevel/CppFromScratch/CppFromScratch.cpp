#include <stdio.h>

struct i_op {
    __declspec(noinline) virtual int f(int x, int y) = 0;
};

struct add_op : i_op {
    __declspec(noinline) virtual int f(int x, int y) {
        return x + y;
    }
};

struct multiplication_op : i_op {
    __declspec(noinline) virtual int f(int x, int y) {
        return x * y;
    }
};

__declspec(noinline) int f(int x, int y) {
    return x + y;
}

__declspec(noinline) int g(i_op & op, int x, int y) {
    return op.f(x, y);
}

int main() {
    volatile int x = 3;
    volatile int y = 4;


    auto z = f(x, y);
    auto op = multiplication_op();
    auto w = g(op, x, y);
    printf("Hello world: %d, %d\n", z, w);
    return 0;
}
