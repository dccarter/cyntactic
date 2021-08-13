//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <exception>
#include <string>
#include <sstream>

namespace cyntactic {

    class Exception: public std::exception {
    public:
        Exception(std::string msg)
            : mMessage{std::move(msg)}
        {}

        const std::string& message() const { return mMessage; }
        const char* what() const noexcept override { return mMessage.c_str(); }
    protected:
        template <typename ...T>
        static std::string buildMessage(T&&... ps)
        {
            std::stringstream ss;
            (ss << ... << ps);
            return ss.str();
        }
    private:
        std::string mMessage;
    };

    class SyntaxError : public Exception {
    public:
        template <typename ...T>
        SyntaxError(const std::string_view& src, std::size_t line, std::size_t col, T&&... ps)
            : Exception(Exception::buildMessage(src, ":", line, ":", col,
                                                ": error(syntax): ", std::forward<T>(ps)...))
        {}
    };

    class TrieOperationError : public  Exception {
    public:
        typedef enum {
            KeyNotFound,
            KeyAlreadyExists
        } Error;

        template<typename... Args>
        TrieOperationError(Error err, const std::string_view& key, Args&&... args)
            : Exception(Exception::buildMessage(
                    toString(err), "(", key, ")", std::forward<Args>(args)...))
        {}

    private:
        static std::string_view toString(Error err)
        {
            switch (err) {
                case KeyNotFound: return "KeyNotFound";
                case KeyAlreadyExists: return "KeyAlreadyExists";
                default: return "Unknown";
            }
        }
    };
}