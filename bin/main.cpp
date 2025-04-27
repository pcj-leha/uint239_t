#include <lib/number.h>
#include <iostream>

int main() {
    uint239_t value;
    value = FromInt(15, 0) / FromString("0", 5);
    std::cout << value << std::endl;

    return 0;
}

