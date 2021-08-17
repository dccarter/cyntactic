//
// Created by Mpho Mbotho on 2021-08-15.
//

/* TextBox: Abstraction for 2-dimensional text strings, with VT100 linedrawing support */
/* Copyright (c) 2017 Joel Yliluoma - http://iki.fi/bisqwit/ */
/* License: MIT */
/* Requires a C++17 capable compiler and standard library. */

#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace cyntactic {
    
    struct TextBox {
        static constexpr bool ENABLE_VT100 = true;
        static constexpr unsigned char U = 1, D = 2, L = 4, R = 8, NON_LINE = ~(U + D + L + R); // bitmasks

        std::vector<std::string> data;

        /**
         * Place a single character in the given coordinate.
         * Notice that behavior is undefined if the character is in 00-1F range.
         */
        void putchar(char c, std::size_t x, std::size_t y);

        /**
         * Modify a character using a callback
         */
        void modchar(std::size_t x, std::size_t y, std::function<void(char&)> func);

        /**
         * Put a string of characters starting at the given coordinate
         * Note that behavior is undefined if the string contains characters in 00-1F range
         * or if the string includes multibyte characters.
         */
        void putline(const std::string& s, std::size_t x, std::size_t y);

        /**
         * Put a 2D string starting at the given coordinate
         */
        void putbox(std::size_t x, std::size_t y, const TextBox& b);

        /**
         * Delete trailing blank from the bottom and right edges
         */
        void trim();

        /**
         * Calculate the height of the string
         */
        std::size_t height() const { return data.size(); }

        /**
         * Calculate the width of the string
         */
        std::size_t width() const;

        /**
         * Draw a horizontal line
         * If bef=true, the line starts from the left edge of the first character cell, otherwise it starts from its center
         * If aft=true, the line ends in the right edge of the last character cell, otherwise it ends in its center
         */
        void hline(std::size_t x, std::size_t y, std::size_t width, bool bef, bool aft);

        /** Draw a vertical line
         * If bef=true, the line starts from the top edge of the first character cell, otherwise it starts from its center
         * If aft=true, the line ends in the bottom edge of the last character cell, otherwise it ends in its center
         */
        void vline(std::size_t x, std::size_t y, std::size_t height, bool bef, bool aft);

        /**
         * Calculate the earliest X coordinate where the given box could be placed
         * without colliding with existing content in this box. Guaranteed to be <= width().
         */
        std::size_t horizAppendPosition(std::size_t y, const TextBox& b) const;

        /**
         * Calculate the earliest Y coordinate where the given box could be placed
         * without colliding with existing content in this box. Guaranteed to be <= height().
         */
        std::size_t vertAppendPosition(std::size_t x, const TextBox& b) const;

        /**
         * Converts the contents of the box into a std::string with linefeeds and VT100 escapes.
         * If ENABLE_VT100 is false, renders using plain ASCII instead.
        */
        std::string toString() const;

    private:
        std::size_t FindLeftPadding(std::size_t y) const;

        std::size_t FindRightPadding(std::size_t y) const;

        std::size_t FindTopPadding(std::size_t x) const;

        std::size_t FindBottomPadding(std::size_t x) const;
    };

    template <typename T>
    struct TreeGraph {
        using Iterator = typename T::GraphIt::first_type;
        using GraphIt = typename T::GraphIt;
        TreeGraph(const T& node, std::size_t maxWidth)
            : mNode{node},
              mMaxWidth{maxWidth}
        {}

        std::string createAtom() const { return ""; }
        GraphIt countChildren() const;
        bool isOneliner() const { return true; }
        bool isSimple() const { return true; }
        bool separateFirstParam() const { return false; }
        const T& getNode(const Iterator& it) const { return *it; }
        TextBox operator()() const;
    private:
        const T& mNode;
        std::size_t mMaxWidth;
    };

/* An utility function that can be used to create a tree graph rendering from a structure.
 *
 * Parameters:
 *   e:                The element that will be rendered.
 *                     Possibly some user-defined type that represents a node in a tree structure.
 *   maxwidth:         The maximum width of the resulting box in characters.
 *   create_atom:      A functor of type std::string(const ParamType&).
 *                     It renders the given element into an 1D string.
 *                     Note that the string must not contain multibyte characters,
 *                     because size() will be used to determine its width in columns.
 *   count_children:   A functor of type std::pair<ForwardIterator,ForwardIterator>(const ParamType&).
 *                     It returns a pair of iterators representing the range of children
 *                     for the given element.
 *                     create_tree_graph will call itself recursively for each element in this range.
 *   oneliner_test:    A functor of type bool(const ParamType&).
 *                     If the result is true, enables simplified horizontal topology.
 *   simple_test:      A functor of type bool(const ParamType&).
 *                     If the result is true, enables very simplified horizontal topology.
 *   separate1st_test: A functor of type bool(const ParamType&).
 *                     If the result is true, create_tree_graph() will always render
 *                     the first child alone on a separate line, but the rest of them
 *                     may get rendered horizontally.
 *
 * Topology types:
 *
 *        Vertical:
 *
 *              element
 *              ├─child1
 *              ├─child2
 *              └─child3
 *
 *        Horizontal:
 *
 *              element
 *              └─┬─────────┬─────────┐
 *                child1    child2    child3
 *
 *        Simplified horizontal:
 *
 *              element──┬───────┬───────┐
 *                       child1  child2  child3
 *
 *        Very simplified horizontal:
 *
 *              element──child1
 *
 * The vertical and horizontal topologies are automatically chosen
 * depending on the situation compared to the maxwidth parameter,
 * and according to the constraint given by separate1st_test().
 *
 * Simplified topology will be used if oneliner_test() returns true,
 *                                     separate1st_test() returns false,
 *                                     all children fit on one line,
 *                                     and very simplified topology is not used.
 *
 * Very simplified topology will be used if oneliner_test() returns true,
 *                                          separate1st_test() returns false,
 *                                          simple_test() returns true,
 *                                          there is only 1 child,
 *                                          and it fits on one line.
 */

    template <typename T>
    TextBox TreeGraph<T>::operator()() const
    {
        TextBox result;
        auto atom = createAtom();
        result.putline(atom, 0, 0);

        if (auto param_range = countChildren(); param_range.first != param_range.second) {
            std::vector<TextBox> boxes;
            boxes.reserve(std::distance(param_range.first, param_range.second));
            for (auto i = param_range.first; i != param_range.second; ++i) {
                auto maxWidth = (mMaxWidth >= (16 + 2)) ? mMaxWidth - 2 : 16;
                boxes.emplace_back(TreeGraph(getNode(i), maxWidth)());
            }

            constexpr std::size_t margin = 4, firstx = 2;

            std::size_t sum_width = 0;
            for (const auto& b: boxes) sum_width += b.width() + margin;

            bool oneliner = false;
            if (isOneliner() && !separateFirstParam()) {
                std::size_t totalwidth = 0;
                for (auto i = boxes.begin();;) {
                    const auto& cur = *i;
                    if (++i == boxes.end()) {
                        totalwidth += cur.width();
                        break;
                    }
                    //const auto& next = *i;
                    totalwidth += cur.width()/*cur.horiz_append_position(0, next)*/ + margin;
                }
                oneliner = (atom.size() + margin + totalwidth) < mMaxWidth;
            }
            bool simple = oneliner && boxes.size() == 1 && isSimple(); // ret, addrof, etc.

            std::size_t y = simple ? 0 : 1;

            for (auto i = boxes.begin(); i != boxes.end(); ++i) {
                auto next = ++std::vector<TextBox>::iterator(i);
                const TextBox& cur = *i;
                unsigned width = cur.width();

                std::size_t usemargin = (simple || oneliner) ? (margin / 2) : margin;
                std::size_t x = result.horizAppendPosition(y, cur) + usemargin;
                if (x == usemargin) x = oneliner ? atom.size() + usemargin : firstx;
                if (!oneliner && (x + width > mMaxWidth || (separateFirstParam() && i == ++boxes.begin()))) {
                    // Start a new line if this item won't fit in the end of the current line
                    x = firstx;
                    simple = false;
                    oneliner = false;
                }

                // At the beginning of line, judge whether to add room for horizontal placement
                bool horizontal = x > firstx;
                if (!oneliner && !horizontal && next != boxes.end() && !(separateFirstParam() && i == boxes.begin())) {
                    std::size_t nextwidth = next->width();
                    std::size_t combined_width = cur.horizAppendPosition(0, *next) + margin + nextwidth;
                    if (combined_width <= mMaxWidth) {
                        // Enact horizontal placement by giving 1 row of room for the connector
                        horizontal = true;
                        TextBox combined = cur;
                        combined.putbox(cur.horizAppendPosition(0, *next) + margin, 0, *next);
                        y = std::max(result.vertAppendPosition(x, combined), std::size_t(1));
                        if (!oneliner) ++y;
                    }
                }
                if (!horizontal)
                    y = std::max(result.vertAppendPosition(x, cur), std::size_t(1));
                if (horizontal && !simple && !oneliner)
                    for (;;) {
                        // Check if there is room for a horizontal connector. If not, increase y
                        TextBox conn;
                        conn.putline(std::string(1 + (x - 0), '-'), 0, 0);
                        if (result.horizAppendPosition(y - 1, conn) > x) ++y; else break;
                        y = std::max(result.vertAppendPosition(x, cur), y);
                    }

                if (simple) {
                    if (x > atom.size())
                        result.hline(atom.size(), 0, 1 + x - atom.size(), false, false);
                } else if (oneliner) {
                    unsigned cx = x, cy = y - 1;
                    if (x > atom.size())
                        result.hline(atom.size(), 0, 1 + x - atom.size(), false, false);
                    result.vline(cx, cy, 1, false, true);
                } else if (horizontal) {
                    unsigned cx = x, cy = y - 1;
                    result.vline(0, 1, 1 + (cy - 1), true, false);
                    result.hline(0, cy, 1 + (cx - 0), false, false);
                    result.vline(cx, cy, 1, false, true);
                } else {
                    unsigned cx = x - 1, cy = y;
                    result.vline(0, 1, 1 + (cy - 1), true, false);
                    result.hline(0, cy, 1 + (cx - 0), false, true);
                }

                result.putbox(x, y, cur);
            }
        }
        result.trim();
        return result;
    }
}