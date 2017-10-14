#include "ArgParse.hpp"
#include "String.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    using namespace vrlk::ArgParse;
    using namespace vrlk::String;

    constexpr auto VISUALIZE_STATE = "print";
    auto parser = ArgumentParser();
    parser.addArgument(VISUALIZE_STATE)
        .type(ArgumentType::Optional)
        .shortName("p")
        .save();
    auto args = parser.parseArgs(argc, argv);
    if (args.second) {
        std::cerr << args.second.message() << std::endl;
        return 1;
    }

    bool visualizeState =
        args.first.find(VISUALIZE_STATE) != std::end(args.first);

    // TODO: add lexer and parser
    std::string buf;
    {
        const std::string commands = "><+-.,[]";
        while (true) {
            auto c = std::getchar();
            if (c == EOF) {
                break;
            }
            else {
                auto it =
                    std::find(std::begin(commands), std::end(commands), c);
                if (it != std::end(commands)) {
                    buf += static_cast<char>(c);
                }
            }
        }
    }
    size_t index{0};
    std::vector<std::uint8_t> v{0};
    auto p = [&index, &v](char c) {
        printf("%c ", c);
        for (auto&& i : v) {
            printf("|%02d", i);
        }
        printf("|\n  ");
        for (size_t i = 0; i < index; ++i) {
            printf("   ");
        }
        printf(" ^^\n");
    };
    for (size_t i = 0; i < buf.size(); ++i) {
        auto c = buf[i];
        if (visualizeState) {
            p(c);
        }
        if (c == '>') {
            ++index;
            if (v.size() <= index) {
                v.emplace_back();
            }
        }
        else if (c == '<') {
            if (index == 0) {
            }
            --index;
        }
        else if (c == '+') {
            ++v[index];
        }
        else if (c == '-') {
            --v[index];
        }
        else if (c == '.') {
            std::putchar(v[index]);
        }
        else if (c == ',') {
            v[index] = static_cast<unsigned char>(std::getchar());
        }
        else if (c == '[') {
            if (v[index] == 0) {
                // TODO: error handling
                auto t = i + 1;
                do {
                    ++t;
                } while (buf[t] != ']');
                i = t;
            }
        }
        else if (c == ']') {
            if (v[index] != 0) {
                // TODO: error handling
                auto t = i - 1;
                do {
                    --t;
                } while (buf[t] != '[');
                i = t;
            }
        }
    }
}
