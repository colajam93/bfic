#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

int main()
{
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
                    buf += c;
                }
            }
        }
    }
    size_t index{0};
    std::vector<std::uint8_t> v{0};
    auto p = [&index, &v](char c) {
        printf("%c ", c);
        for (auto &&i : v) {
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
        if (false) {
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
            v[index] = std::getchar();
        }
        else if (c == '[') {
            if (v[index] == 0) {
                // todo error handling
                auto t = i + 1;
                do {
                    ++t;
                } while (buf[t] != ']');
                i = t;
            }
        }
        else if (c == ']') {
            if (v[index] != 0) {
                // todo error handling
                auto t = i - 1;
                do {
                    --t;
                } while (buf[t] != '[');
                i = t;
            }
        }
    }
}
