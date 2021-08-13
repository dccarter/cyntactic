//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <list>
#include <memory>
#include <string>

namespace cyntactic {

    struct Node {
        typedef enum {
            INVALID,
            NUMBER_TYPE,
            STRING_TYPE,
            BOOL_TYPE
        } Kind;

        using Ptr = std::unique_ptr<Node>;
        Node() = default;
        Node(Kind k) : kind{k} {}

        Kind kind{INVALID};
        std::list<Ptr> Children;
    };

    class AstNumber : Node {
        struct Details {
            short  Size;
            bool   Signed{false};
            bool   Float{false};
        };

    public:
        AstNumber(const std::string_view& tp);

        std::size_t size() const { return mDetails.Size; }
        const std::string& name() const { return mName; }
        bool isFloat() const { return mDetails.Float; }
        bool isSigned() const { return mDetails.Signed; }

    private:
        static const Details& getDetails(const std::string_view& tp);
        std::string mName;
        Details mDetails{};
    };


}
