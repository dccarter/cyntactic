//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <string>
#include <string_view>

#include <node.hpp>

namespace cyntactic::ast {

    class NumberType : public Node {
        struct Details {
            short  Size;
            bool   Signed{false};
            bool   Float{false};
        };

    public:
        NumberType(const std::string_view& tp);

        std::size_t size() const { return mDetails.Size; }
        const std::string_view& name() const { return mName; }
        bool isFloat() const { return mDetails.Float; }
        bool isSigned() const { return mDetails.Signed; }
        std::string toString(bool compressed = true) const override;
    private:
        static const Details& getDetails(const std::string_view& tp, std::string_view& name);
        Details mDetails{};
        std::string_view mName{};
    };
}