#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <stack>
#include <algorithm>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

class ExpressionEvaluator {
private:
    std::string expression;
    size_t pos;
    
    double parseNumber();
    std::string parseIdentifier();
    double parseFactor();
    double parsePower();
    double parseTerm();
    double parseExpression();
    double parseFunction(const std::string& name);
    double factorial(double n);
    double ncr(double n, double r);
    double npr(double n, double r);
    void skipWhitespace();
    
public:
    double evaluate(const std::string& expr, std::string& error);
};

double ExpressionEvaluator::factorial(double n) {
    if (n < 0) return std::nan("");
    if (n != std::floor(n)) return std::nan("");
    
    int int_n = static_cast<int>(n);
    if (int_n > 170) return INFINITY;
    
    double result = 1.0;
    for (int i = 2; i <= int_n; i++) {
        result *= i;
    }
    return result;
}

double ExpressionEvaluator::ncr(double n, double r) {
    if (r > n || r < 0 || n < 0) return std::nan("");
    if (r != std::floor(r) || n != std::floor(n)) return std::nan("");
    
    int int_n = static_cast<int>(n);
    int int_r = static_cast<int>(r);
    
    if (int_r > int_n - int_r) int_r = int_n - int_r;
    
    double result = 1.0;
    for (int i = 0; i < int_r; i++) {
        result *= (int_n - i);
        result /= (i + 1);
    }
    return result;
}

double ExpressionEvaluator::npr(double n, double r) {
    if (r > n || r < 0 || n < 0) return std::nan("");
    if (r != std::floor(r) || n != std::floor(n)) return std::nan("");
    
    int int_n = static_cast<int>(n);
    int int_r = static_cast<int>(r);
    
    double result = 1.0;
    for (int i = 0; i < int_r; i++) {
        result *= (int_n - i);
    }
    return result;
}

void ExpressionEvaluator::skipWhitespace() {
    while (pos < expression.length() && isspace(expression[pos])) {
        pos++;
    }
}

double ExpressionEvaluator::parseNumber() {
    skipWhitespace();
    
    size_t start = pos;
    
    while (pos < expression.length() && 
           (isdigit(expression[pos]) || expression[pos] == '.')) {
        pos++;
    }
    
    if (start == pos) {
        throw std::runtime_error("Expected number at position " + std::to_string(pos));
    }
    
    std::string numStr = expression.substr(start, pos - start);
    return std::stod(numStr);
}

std::string ExpressionEvaluator::parseIdentifier() {
    skipWhitespace();
    
    size_t start = pos;
    
    while (pos < expression.length() && 
           (isalpha(expression[pos]) || expression[pos] == '_')) {
        pos++;
    }
    
    if (start == pos) {
        throw std::runtime_error("Expected identifier at position " + std::to_string(pos));
    }
    
    return expression.substr(start, pos - start);
}

double ExpressionEvaluator::parseFunction(const std::string& name) {
    skipWhitespace();
    
    if (pos >= expression.length() || expression[pos] != '(') {
        throw std::runtime_error("Expected '(' after function at position " + std::to_string(pos));
    }
    pos++;
    
    std::vector<double> args;
    
    skipWhitespace();
    if (expression[pos] != ')') {
        args.push_back(parseExpression());
        
        skipWhitespace();
        while (pos < expression.length() && expression[pos] == ',') {
            pos++;
            skipWhitespace();
            args.push_back(parseExpression());
            skipWhitespace();
        }
    }
    
    skipWhitespace();
    if (pos >= expression.length() || expression[pos] != ')') {
        throw std::runtime_error("Expected ')' after function arguments at position " + std::to_string(pos));
    }
    pos++;
    
    if (name == "abs") {
        if (args.size() != 1) throw std::runtime_error("abs() takes 1 argument");
        return std::abs(args[0]);
    } else if (name == "acos") {
        if (args.size() != 1) throw std::runtime_error("acos() takes 1 argument");
        return std::acos(args[0]);
    } else if (name == "asin") {
        if (args.size() != 1) throw std::runtime_error("asin() takes 1 argument");
        return std::asin(args[0]);
    } else if (name == "atan") {
        if (args.size() != 1) throw std::runtime_error("atan() takes 1 argument");
        return std::atan(args[0]);
    } else if (name == "atan2") {
        if (args.size() != 2) throw std::runtime_error("atan2() takes 2 arguments");
        return std::atan2(args[0], args[1]);
    } else if (name == "ceil") {
        if (args.size() != 1) throw std::runtime_error("ceil() takes 1 argument");
        return std::ceil(args[0]);
    } else if (name == "cos") {
        if (args.size() != 1) throw std::runtime_error("cos() takes 1 argument");
        return std::cos(args[0]);
    } else if (name == "cosh") {
        if (args.size() != 1) throw std::runtime_error("cosh() takes 1 argument");
        return std::cosh(args[0]);
    } else if (name == "exp") {
        if (args.size() != 1) throw std::runtime_error("exp() takes 1 argument");
        return std::exp(args[0]);
    } else if (name == "fac") {
        if (args.size() != 1) throw std::runtime_error("fac() takes 1 argument");
        return factorial(args[0]);
    } else if (name == "floor") {
        if (args.size() != 1) throw std::runtime_error("floor() takes 1 argument");
        return std::floor(args[0]);
    } else if (name == "ln") {
        if (args.size() != 1) throw std::runtime_error("ln() takes 1 argument");
        return std::log(args[0]);
    } else if (name == "log" || name == "log10") {
        if (args.size() != 1) throw std::runtime_error("log() takes 1 argument");
        return std::log10(args[0]);
    } else if (name == "ncr") {
        if (args.size() != 2) throw std::runtime_error("ncr() takes 2 arguments");
        return ncr(args[0], args[1]);
    } else if (name == "npr") {
        if (args.size() != 2) throw std::runtime_error("npr() takes 2 arguments");
        return npr(args[0], args[1]);
    } else if (name == "pow") {
        if (args.size() != 2) throw std::runtime_error("pow() takes 2 arguments");
        return std::pow(args[0], args[1]);
    } else if (name == "sin") {
        if (args.size() != 1) throw std::runtime_error("sin() takes 1 argument");
        return std::sin(args[0]);
    } else if (name == "sinh") {
        if (args.size() != 1) throw std::runtime_error("sinh() takes 1 argument");
        return std::sinh(args[0]);
    } else if (name == "sqrt") {
        if (args.size() != 1) throw std::runtime_error("sqrt() takes 1 argument");
        return std::sqrt(args[0]);
    } else if (name == "tan") {
        if (args.size() != 1) throw std::runtime_error("tan() takes 1 argument");
        return std::tan(args[0]);
    } else if (name == "tanh") {
        if (args.size() != 1) throw std::runtime_error("tanh() takes 1 argument");
        return std::tanh(args[0]);
    } else {
        throw std::runtime_error("Unknown function: " + name);
    }
}

double ExpressionEvaluator::parseFactor() {
    skipWhitespace();
    
    if (pos >= expression.length()) {
        throw std::runtime_error("Unexpected end of expression");
    }
    
    if (expression[pos] == '(') {
        pos++;
        double result = parseExpression();
        skipWhitespace();
        if (pos >= expression.length() || expression[pos] != ')') {
            throw std::runtime_error("Expected ')' at position " + std::to_string(pos));
        }
        pos++;
        return result;
    } else if (expression[pos] == '-') {
        pos++;
        return -parseFactor();
    } else if (expression[pos] == '+') {
        pos++;
        return parseFactor();
    } else if (isalpha(expression[pos])) {
        std::string ident = parseIdentifier();
        skipWhitespace();
        
        if (pos < expression.length() && expression[pos] == '(') {
            return parseFunction(ident);
        } else if (ident == "pi") {
            return M_PI;
        } else if (ident == "e") {
            return M_E;
        } else {
            throw std::runtime_error("Unknown identifier: " + ident + " at position " + std::to_string(pos - ident.length()));
        }
    } else {
        return parseNumber();
    }
}

double ExpressionEvaluator::parsePower() {
    double left = parseFactor();
    skipWhitespace();
    
    while (pos < expression.length() && expression[pos] == '^') {
        pos++;
        double right = parsePower();
        left = std::pow(left, right);
        skipWhitespace();
    }
    
    return left;
}

double ExpressionEvaluator::parseTerm() {
    double left = parsePower();
    skipWhitespace();
    
    while (pos < expression.length() && 
           (expression[pos] == '*' || expression[pos] == '/' || expression[pos] == '%')) {
        char op = expression[pos];
        pos++;
        double right = parsePower();
        
        if (op == '*') {
            left *= right;
        } else if (op == '/') {
            if (right == 0) {
                left = (left >= 0) ? INFINITY : -INFINITY;
            } else {
                left /= right;
            }
        } else if (op == '%') {
            left = std::fmod(left, right);
        }
        
        skipWhitespace();
    }
    
    return left;
}

double ExpressionEvaluator::parseExpression() {
    double left = parseTerm();
    skipWhitespace();
    
    while (pos < expression.length() && 
           (expression[pos] == '+' || expression[pos] == '-')) {
        char op = expression[pos];
        pos++;
        double right = parseTerm();
        
        if (op == '+') {
            left += right;
        } else {
            left -= right;
        }
        
        skipWhitespace();
    }
    
    return left;
}

double ExpressionEvaluator::evaluate(const std::string& expr, std::string& error) {
    expression = expr;
    pos = 0;
    
    try {
        skipWhitespace();
        if (pos >= expression.length()) {
            error = "Empty expression";
            return 0.0;
        }
        
        double result = parseExpression();
        skipWhitespace();
        
        if (pos < expression.length()) {
            error = "Unexpected character at position " + std::to_string(pos) + ": '" + expression[pos] + "'\n";
            error += expr + "\n";
            error += std::string(pos, ' ') + "^";
            return 0.0;
        }
        
        return result;
        
    } catch (const std::runtime_error& e) {
        error = e.what();
        error += "\n" + expr + "\n";
        error += std::string(pos, ' ') + "^";
        return 0.0;
    }
}