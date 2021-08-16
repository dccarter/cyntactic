//
// Created by Mpho Mbotho on 2021-08-13.
//

#include "node.hpp"
#include "trie.hpp"

namespace cyntactic {

    template <>
    std::pair<Node::iterator , Node::iterator> TreeGraph<Node>::countChildren() const {
        return std::make_pair(mNode.Children.begin(), mNode.Children.end());
    }

    template <>
    bool TreeGraph<Node>::isOneliner() const { return !mNode.Children.empty(); }

    template <>
    const Node& TreeGraph<Node>::getNode(const Iterator& it) const {
        return *(*it);
    }

    template <>
    std::string TreeGraph<Node>::createAtom() const { return mNode.toString(); }
}