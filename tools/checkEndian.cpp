

bool is_big_endian() {
    int num = 1;
    // 强转切完以后是留左边的
    // 大端序 0x12345678   低地址-->12 34 56 78<--高地址
    // 小端序 0x12345678   低地址-->78 56 34 12<--高地址
    if (*(char*)&num == 1) {
        return false;
    } else {
        return true;
    }
}