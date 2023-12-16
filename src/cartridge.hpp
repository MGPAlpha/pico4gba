#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

struct AbstractPicoData {
    const char* data;
    int size;
};

char hexDigitToData(char digit);

struct CartSection {
    const char* name;
    const char* start;
    const char* end;
    unsigned int length;

    CartSection(const char* name, const char* start, const char* end);
    CartSection();
    CartSection findSection(const char* name) const;

    AbstractPicoData processAsLua();
    AbstractPicoData processAsGfx();
    AbstractPicoData processAsLabel();
};

class CartReader {
    public:
        CartReader(const CartSection s);
        int readLine(char* buff, int maxLen);
        const char* getCurr();
        const char* getNext();
    private:
        const char* curr;
        const char* next;
        CartSection s;
};

class ProcessedCartridge {
    public:
        ProcessedCartridge(const CartSection s);
        const AbstractPicoData getLua() const;
        const AbstractPicoData getGfx() const;
        const AbstractPicoData getLabel() const;
    private:
        AbstractPicoData lua;
        AbstractPicoData gfx;
        AbstractPicoData label;
};

#endif