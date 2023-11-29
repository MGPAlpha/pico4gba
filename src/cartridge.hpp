#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

struct CartSection {
    const char* name;
    const char* start;
    const char* end;
    unsigned int length;

    CartSection(const char* name, const char* start, const char* end);
    CartSection();
    CartSection findSection(const char* name) const;
};

class CartReader {
    public:
        CartReader(const CartSection s);
        int readLine(char* buff, int maxLen);
    private:
        const char* curr;
        const char* next;
        CartSection s;
};

#endif