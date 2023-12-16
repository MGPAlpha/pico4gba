#include "cartridge.hpp"
#include <cstdio>
#include <cstring>
#include "print.h"

char hexDigitToData(char digit) {
    if (digit >= 48 && digit < 58) {
        return digit - 48;
    } else if (digit >= 65 && digit < 71) {
        return digit - 65+10;
    } else if (digit >= 97 && digit < 103) {
        return digit - 97+10;
    } else {
        return 0;
    }
}

CartSection::CartSection(const char* name, const char* start, const char* end) {
    this->name = name;
    this->start = start;
    this->end = end;
    this->length = end-start;
}

CartSection::CartSection() : CartSection(nullptr, nullptr, nullptr) {}

CartSection CartSection::findSection(const char* name) const {
    CartReader reader = CartReader(*this);
    
    char sectionHeader[20];
    snprintf(sectionHeader, 19, "__%s__", name);
    int headerLen = strlen(sectionHeader);

    char buffer[256];
    while (reader.readLine(buffer, 255)) {
        if (!strcmp(sectionHeader, buffer)) {
            goto foundSection;
        }
    }
    return CartSection();
    foundSection:
    const char* start = reader.getNext(); 
    while (reader.readLine(buffer, 255)) {
        int linelen = strlen(buffer);
        if (linelen > 4 &&
                buffer[0] == '_' &&
                buffer[1] == '_' &&
                buffer[linelen-2] == '_' &&
                buffer[linelen-1] == '_'
        ) {
            break;
        }
    }
    const char* end = reader.getCurr();

    return CartSection(name, start, end);
    
}


AbstractPicoData CartSection::processAsLua() {
    int length = this->end - this->start;
    char* data = new char[length+1];
    strncpy(data, this->start, length);
    return AbstractPicoData{data, length};
}
AbstractPicoData CartSection::processAsGfx() {
    return AbstractPicoData{};
}
AbstractPicoData CartSection::processAsLabel() {
    int labelSize = 128*64;
    char* data = new char[labelSize];
    CartReader r = CartReader(*this);
    char buff[256];
    int i = 0;
    outer: while (r.readLine(buff, 255)) {
        char* curr = buff;
        while (*curr) {
            char pixelPair;
            char leftVal = hexDigitToData(curr[1]);
            char rightVal = hexDigitToData(curr[0]);
            pixelPair = leftVal | (rightVal << 4);
            data[i] = pixelPair;
            i++;
            curr += 2;
            if (i >= labelSize) goto doneReading;
        }
    }
    doneReading:
    return AbstractPicoData{data, labelSize};
}


const char* CartReader::getCurr() {
    return this->curr;
}
const char* CartReader::getNext() {
    return this->next;
}

CartReader::CartReader(const CartSection s) {
    this->s = s;
    this->next = s.start;
    this->curr = nullptr;
}

int CartReader::readLine(char* buff, int maxLen) {
    if (!next) return 0;
    const char* source = next;
    char* dest = buff;

    for (; source < s.end && dest - buff < maxLen && *source != '\n'; source++, dest++) {
        *dest = *source;
    }

    *dest = 0;


    curr = next;
    if (source < s.end - 1) next = source + 1;
    else next = nullptr;

    return 1;
}


ProcessedCartridge::ProcessedCartridge(const CartSection s) {
    lua = s.findSection("lua").processAsLua();
    gfx = s.findSection("gfx").processAsGfx();
    label = s.findSection("label").processAsLabel();
}

const AbstractPicoData ProcessedCartridge::getLua() const {
    return lua;
}
const AbstractPicoData ProcessedCartridge::getGfx() const {
    return gfx;
}
const AbstractPicoData ProcessedCartridge::getLabel() const {
    return label;
}