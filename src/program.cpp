//
// Created by Mpho Mbotho on 2021-08-16.
//

#include <program.hpp>

namespace cyntactic {

    std::string Program::toString(bool compressed) const
    {
        return "Program";
    }

    void Program::dump(std::ostream& os) const
    {
        TreeGraph<Node> graph(*this, 132-2);
        TextBox dump{};
        dump.putbox(2, 0, graph());
        os << dump.toString();
    }

}