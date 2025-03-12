#include <iostream>

bool is_big_endian() {
    int num = 1;
    // 强制转换转的是写完的地址, 而不是开始写的地址
    if (*(char*)&num == 1) {
        return false;
    } else {
        return true;
    }
}

int main() {
    int num = 0x12345678;
    char* p = (char*)&num;

    std::cout << "raw data is: " << std::hex << num << std::endl;
    if (is_big_endian()) {
        std::cout << "system is big endian." << std::endl;
        std::cout << "num is: ";
        for (int i = 0; i < sizeof(num); ++i) {
            std::cout << std::hex << (int)*(p + i) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "system is small endian." << std::endl;
        std::cout << "num is: ";
        for (int i = 0; i < sizeof(num); ++i) {
            std::cout << std::hex << (int)*(p + i) << " ";
        }
        std::cout << std::endl;
    }
}