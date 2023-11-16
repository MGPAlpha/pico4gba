int main() {

    (*(unsigned short *)0x4000000) = 3 | (1<<10);

    *((unsigned short *)0x6000000) = 0b0111111111111111;

}