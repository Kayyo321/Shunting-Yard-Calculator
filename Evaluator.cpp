/**
 * @Algorithm
 * Shunting Yard algorithm created by Edsger W. Dijkstra.
 * @License
 * This calculator uses material from the Wikipedia article
 * Shunting-yard algorithm (https://en.wikipedia.org/wiki/Shunting-yard_algorithm),
 * which is released under the
 * Creative Commons Attribution-Share-Alike License 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
 */

#include <iostream> // cout && getline
#include <string>   // string
#include <vector>   // vector
#include <deque>    // deque
#include <cmath>    // pow

enum class tokenType {
    nil,
    lpa,
    rpa,
    add,
    sub,
    mul,
    div,
    mod,
    exp,
    i32,
    f32
};

class token {
public:
    [[nodiscard]] std::string toString() const {
        return "(" + strData + ", " + std::to_string(intData)
                + ", " + std::to_string(fltData) + ", [" + std::to_string(precedence)
                + " : " + tokenTypeStrings[(int)type] + "])";
    }

    std::string strData {};
    long intData {};
    float fltData {};
    size_t precedence {};
    tokenType type {tokenType::nil};
    bool rAssociative {false};
    bool unary {false};

private:
    const std::vector<const char *> tokenTypeStrings {
            "nil",
            "lpa",
            "rpa",
            "add",
            "sub",
            "mul",
            "div",
            "mod",
            "exp",
            "i32",
            "f32"
    };
};

class lexana {
public:
    /* For debugging lexer output. */
    [[maybe_unused]] [[nodiscard]] std::string toString() const {
        std::string s {"[\n"};
        for (const token &t: formatTokens) {
            s += '\t' + t.toString() + '\n';
        }
        s += ']';
        return s;
    }

    [[nodiscard]] std::vector<token> &getTokens() {
        return formatTokens;
    }

    void lex(const std::string &_data) {
        data = _data;

        for (size_t i {0}; i < data.size(); ++i) {
        _again:
            char c {data[i]};
            token t {};

            switch (c) {
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    ++i;
                    goto _again;

                case '(':
                    t.type = tokenType::lpa;
                    t.strData = '(';
                    t.precedence = 9;
                    t.rAssociative = false;
                    break;

                case ')':
                    t.type = tokenType::rpa;
                    t.strData = ')';
                    t.precedence = 0;
                    t.rAssociative = false;
                    break;

                case '+':
                    t.type = tokenType::add;
                    t.strData = '+';
                    t.precedence = 2;
                    t.rAssociative = false;
                    break;

                case '-':
                    if (formatTokens.empty() || (formatTokens.back().type == tokenType::add
                                                 || formatTokens.back().type == tokenType::sub
                                                 || formatTokens.back().type == tokenType::div
                                                 || formatTokens.back().type == tokenType::mul
                                                 || formatTokens.back().type == tokenType::mod
                                                 || formatTokens.back().type == tokenType::exp)
                        || formatTokens.back().type == tokenType::lpa) {
                        t.unary = true;
                        t.precedence = 5;
                        t.rAssociative = false;
                        t.strData = 'm';
                        t.precedence = 5;
                        t.type = tokenType::sub;
                    }
                    else {
                        t.type = tokenType::sub;
                        t.strData = '-';
                        t.precedence = 2;
                        t.rAssociative = false;
                    }
                    break;

                case '/':
                    t.type = tokenType::div;
                    t.strData = '/';
                    t.precedence = 3;
                    t.rAssociative = false;
                    break;

                case '*':
                case 'x':
                    t.type = tokenType::mul;
                    t.strData = '*';
                    t.precedence = 3;
                    t.rAssociative = false;
                    break;

                case '%':
                    t.type = tokenType::mod;
                    t.strData = '%';
                    t.precedence = 6;
                    t.rAssociative = false;
                    break;

                case '^':
                    t.type = tokenType::exp;
                    t.strData = '^';
                    t.precedence = 4;
                    t.rAssociative = true;
                    break;

                case '0':
                case '9':
                case '8':
                case '7':
                case '6':
                case '5':
                case '4':
                case '3':
                case '2':
                case '1': {
                    std::string floatStr {};
                    t.type = tokenType::i32;

                    while (std::isdigit(c) || c == '_' || c == '.') {
                        if (c == '_') {
                            c = data[++i];
                            continue;
                        }
                        else if (c == '.') {
                            if (t.type == tokenType::f32) {
                                std::cerr << "Redefinition of float: " << t.toString() << '\n';
                                exit(1);
                            }
                            else {
                                t.type = tokenType::f32;
                                floatStr += '.';
                            }
                        }
                        else {
                            floatStr += c;
                        }

                        c = data[++i];
                    }

                    if (t.type == tokenType::f32) {
                        t.fltData = std::stof(floatStr);
                    }
                    else {
                        t.intData = std::stoi(floatStr);
                    }

                    --i; // Let for loop skip last char.

                    break;
                }

                case '.':
                    if (isdigit(data[i+1])) {
                        t.type = tokenType::f32;
                        c = data[++i];
                        std::string floatStr {'.'};

                        while (isdigit(c) || c == '_' || c == '.') {
                            if (c == '_') {
                                c = data[++i];
                                continue;
                            }
                            else if (c == '.') {
                                if (t.type == tokenType::f32) {
                                    std::cerr << "Redefinition of float: " << t.toString() << '\n';
                                    exit(1);
                                }
                                else {
                                    t.type = tokenType::f32;
                                    floatStr += '.';
                                }
                            }
                            else {
                                floatStr += c;
                            }

                            c = data[++i];
                        }

                        if (t.type == tokenType::f32) {
                            t.fltData = std::stof(floatStr);
                        }
                        else {
                            t.intData = std::stoi(floatStr);
                        }

                        --i; // Let for loop skip last char.
                    }
                    else {
                        std::cerr << "Unexpected: '.'\n";
                        exit(1);
                    }
                    break;

                default:
                    std::cerr << "Unexpected: " << c << '\n';
                    exit(1);
            }

            formatTokens.push_back(t);
        }
    }

private:
    std::string data {};
    std::vector<token> formatTokens {};
};

std::deque<token> shuntingYard(std::vector<token> &tokens) {
    std::deque<token> queue;
    std::vector<token> stack;

    for (const auto &t: tokens) {
        switch(t.type) {
            case tokenType::i32:
            case tokenType::f32:
                queue.push_back(t);
                break;

            case tokenType::add:
            case tokenType::sub:
            case tokenType::mul:
            case tokenType::div:
            case tokenType::mod:
            case tokenType::exp: {
                const auto o1 {t};

                while(!stack.empty()) {
                    const auto o2 = stack.back();

                    if((! o1.rAssociative && o1.precedence <= o2.precedence)
                       || (o1.rAssociative && o1.precedence <  o2.precedence)) {
                        stack.pop_back();
                        queue.push_back(o2);
                        continue;
                    }

                    break;
                }

                stack.push_back(o1);
                break;
            }

            case tokenType::lpa:
                stack.push_back(t);
                break;

            case tokenType::rpa: {
                bool match {false};

                while (!stack.empty() && stack.back().type != tokenType::lpa) {
                    queue.push_back(stack.back());
                    stack.pop_back();
                    match = true;
                }

                if (!match && stack.empty()) {
                    std::cerr << "Right parenthesis error: " << t.toString() << '\n';
                    return {};
                }

                if (!stack.empty()) stack.pop_back();
                break;
            }

            default:
                std::cerr << "Error: " << t.toString();
                return {};
        }
    }

    while(!stack.empty()) {
        if(stack.back().type == tokenType::lpa) {
            std::cerr << "Mismatched parentheses error\n";
            return {};
        }

        queue.push_back(std::move(stack.back()));
        stack.pop_back();
    }

    return queue;
}

float compute(std::deque<token> &formatted) {
    std::vector<float> stack {};

    while (!formatted.empty()) {
        const token &t {formatted.front()};
        formatted.pop_front();
        switch (t.type) {
            case tokenType::nil: break;

            case tokenType::i32:
                stack.push_back((float)t.intData);
                break;

            case tokenType::f32:
                stack.push_back(t.fltData);
                break;

            case tokenType::add:
            case tokenType::sub:
            case tokenType::mul:
            case tokenType::div:
            case tokenType::mod:
            case tokenType::exp:
                if (t.unary) {
                    float &rhs {stack.back()};
                    rhs *= -1;
                }
                else {
                    const float rhs {stack.back()};
                    stack.pop_back();
                    const float lhs {stack.back()};
                    stack.pop_back();

                    switch (t.type) {
                        case tokenType::exp:
                            stack.push_back(std::pow(lhs, rhs));
                            break;

                        case tokenType::mul:
                            stack.push_back(lhs * rhs);
                            break;

                        case tokenType::div:
                            stack.push_back(lhs / rhs);
                            break;

                        case tokenType::add:
                            stack.push_back(lhs + rhs);
                            break;

                        case tokenType::sub:
                            stack.push_back(lhs - rhs);
                            break;

                        default: break;
                    }
                }
                break;

            default: break;
        }
    }

    return stack.back();
}

int main() {
    std::string exprStr {};
    lexana *lexer {new lexana {}};

    while (true) {
        std::cout << "Enter an mathematical expression ('exit' to stop): ";
        std::getline(std::cin, exprStr);
        std::cout << '\n';

        if (exprStr == "exit") {
            break;
        }

        lexer->lex(exprStr);

        std::deque<token> formatted {shuntingYard(lexer->getTokens())};

        const float expr {compute(formatted)};

        std::cout << "That evaluates out to:\n" << expr << "\n\n";

        lexer->getTokens().clear();
    }

    delete lexer;

    return 0;
}
