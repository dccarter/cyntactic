//
// Created by Mpho Mbotho on 2021-08-15.
//

/* Textbox: Abstraction for 2-dimensional text strings, with VT100 linedrawing support */
/* Copyright (c) 2017 Joel Yliluoma - http://iki.fi/bisqwit/ */
/* License: MIT */
/* Requires a C++17 capable compiler and standard library. */

#include "textbox.hpp"

namespace cyntactic {

    void TextBox::putchar(char c, std::size_t x, std::size_t y)
    {
        if (y >= data.size()) data.resize(y + 1);
        if (data[y].size() == x) data[y] += c;
        else {
            if (data[y].size() < x) data[y].resize(x + 1, ' ');
            data[y][x] = c;
        }
    }

    void TextBox::modchar(std::size_t x, std::size_t y, std::function<void(char&)> func)
    {
        if (y >= data.size()) data.resize(y + 1);
        if (data[y].size() <= x) data[y].resize(x + 1, ' ');
        func(data[y][x]);
    }

    void TextBox::putline(const std::string& s, std::size_t x, std::size_t y)
    {
        if (y >= data.size()) { data.resize(y + 1); }
        if (x > data[y].size()) { data[y].append(x - data[y].size(), ' '); }
        std::size_t begin = 0;
        for (; x < data[y].size() && begin < s.size(); ++begin, ++x) {
            unsigned char c = s[begin];
            if (c == ' ' || !c) continue;
            char& tgt = data[y][x];
            if (tgt == ' ' || !tgt || (c & NON_LINE)) tgt = c;
            else {
                if (tgt & NON_LINE) tgt = 0;
                tgt |= c;
            }
        }
        if (s.size() > begin) { data[y].append(s, begin, s.size() - begin); }
    }

    void TextBox::putbox(std::size_t x, std::size_t y, const TextBox& b)
    {
        for (std::size_t p = 0; p < b.data.size(); ++p) putline(b.data[p], x, y + p);
    }

    void TextBox::trim()
    {
        for (auto& s: data) {
            std::size_t end = s.size();
            while (end > 0 && (s[end - 1] == ' ' || s[end - 1] == '\0')) { --end; }
            s.erase(end);
        }
        while (!data.empty() && data.back().empty()) data.pop_back();
    }

    std::size_t TextBox::width() const
    {
        std::size_t result = 0;
        for (const auto& s: data) result = std::max(result, s.size());
        return result;
    }

    void TextBox::hline(std::size_t x, std::size_t y, std::size_t width, bool bef, bool aft)
    {
        for (std::size_t p = 0; p < width; ++p)
            modchar(x + p, y, [&](char& c) {
                if (c & NON_LINE) c = 0;
                if (p > 0 || bef) c |= L;
                if (aft || (p + 1) < width) c |= R;
            });
    }

    void TextBox::vline(std::size_t x, std::size_t y, std::size_t height, bool bef, bool aft)
    {
        for (std::size_t p = 0; p < height; ++p)
            modchar(x, y + p, [&](char& c) {
                if (c & NON_LINE) c = 0;
                if (p > 0 || bef) c |= U;
                if (aft || (p + 1) < height) c |= D;
            });
    }

    std::size_t TextBox::horizAppendPosition(std::size_t y, const TextBox& b) const
    {
        // Find leftmost position where box b can be appended into *this without overlap
        std::size_t mywidth = width()/*, myheight = height()*/, theirheight = b.height();

        std::size_t reduce = mywidth;
        for (std::size_t p = 0; p < theirheight; ++p) {
            std::size_t theirpadding = b.FindLeftPadding(p);
            std::size_t mypadding = FindRightPadding(y + p);
            reduce = std::min(reduce, mypadding + theirpadding);
        }
        return mywidth - reduce;
    }

    std::size_t TextBox::vertAppendPosition(std::size_t x, const TextBox& b) const {
        // Find topmost position where box b can be appended into *this without overlap
        std::size_t /*mywidth = width(), */myheight = height(), theirwidth = b.width();

        std::size_t reduce = myheight;
        for (std::size_t p = 0; p < theirwidth; ++p) {
            std::size_t theirpadding = b.FindTopPadding(p);
            std::size_t mypadding = FindBottomPadding(x + p);
            reduce = std::min(reduce, mypadding + theirpadding);
        }
        return myheight - reduce;
    }

    std::string TextBox::toString() const {
        std::string result;
        bool drawing = false, quo = false, space = true, unstr = false;
        std::string cur_attr;
        constexpr const char* const linedraw = ENABLE_VT100 ? "xxxqjkuqmltqvwn" : "|||-'.+-`,+-+++";
        auto attr = [&](const char* s) {
            if constexpr(ENABLE_VT100) {
                if (cur_attr != s) {
                    result += "\33[";
                    result += s;
                    result += 'm';
                    cur_attr = s;
                }
            }
        };
        auto append = [&](bool v, char c) {
            if constexpr(ENABLE_VT100) {
                const char* a = nullptr;
                bool num = false;
                if (v && !drawing) {
                    a = "0;32";
                    result += "\33)0\16";
                    drawing = v;
                }
                else if (!v && drawing) {
                    a = "";
                    result += "\33)B\17";
                    drawing = v;
                }
                if (!v && c == '"') {
                    quo = !quo;
                    if (quo) a = "1;34";
                }
                if (!v && !quo && ((c >= '0' && c <= '9') || c == '-')) {
                    a = space ? "1;38;5;165" : "0;38;5;246";
                    num = true;
                }
                if (!v && !quo && ((c >= 'a' && c <= 'z') || c == '_')) { a = "1;37"; }
                if (!v && !quo && c >= 'A' && c <= 'Z') { a = "0;38;5;246"; }
                if (!v && !quo && c == '`') {
                    unstr = true;
                    c = ' ';
                }
                if (c == '\n') unstr = false;
                if (unstr) a = nullptr;
                if (a) attr(a);
                if (!num) space = (c == ' ');
            }
            result += c;
        };
        for (std::size_t h = height(), y = 0; y < h; ++y) {
            const std::string& s = data[y];
            for (std::size_t x = 0; x < s.size(); ++x) {
                unsigned char c = s[x];
                if (c > 0 && c < 16) append(true, linedraw[c - 1]);
                else append(false, c);
            }
            attr("");
            append(false, '\n');
        }
        return result;
    }

    std::size_t TextBox::FindLeftPadding(std::size_t y) const {
        std::size_t max = width(), result = 0;
        if (y >= data.size()) return max;
        const std::string& line = data[y];
        while (result < line.size() && (line[result] == ' ' || line[result] == '\0')) { ++result; }
        return result;
    }

    std::size_t TextBox::FindRightPadding(std::size_t y) const {
        std::size_t max = width(), position = max, result = 0;
        if (y >= data.size()) return max;
        const std::string& line = data[y];
        while (position-- > 0 &&
               (position >= line.size() || line[position] == ' ' || line[position] == '\0')) { ++result; }
        return result;
    }

    std::size_t TextBox::FindTopPadding(std::size_t x) const {
        std::size_t result = 0, max = data.size();
        while (result < max &&
               (x >= data[result].size() || data[result][x] == ' ' || data[result][x] == '\0')) { ++result; }
        return result;
    }

    std::size_t TextBox::FindBottomPadding(std::size_t x) const {
        std::size_t result = 0, max = data.size(), position = max;
        while (position-- > 0 &&
               (x >= data[position].size() || data[position][x] == ' ' || data[position][x] == '\0')) { ++result; }
        return result;
    }

}