#pragma once

/*
MIT License

Copyright (c) 2024 Aiekick

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define _USE_MATH_DEFINES
#include <array>
#include <math.h>
#include <string>
#include <limits>
#include <memory>
#include <chrono>
#include <cstring>
#include <sstream>
#include <iostream>
#include <functional>
#include <unordered_map>

namespace ez {

// Class that encapsulates a string
class String {
private:
    char* m_Data;     // Pointer to the character string
    size_t m_Length;  // Length of the string

public:
    // Default constructor: initializes members to nullptr and length to 0
    String()
        : m_Data(nullptr), m_Length(0) {
    }

    // Constructor from a std::string
    String(const ::std::string& str) {
        if (!str.empty()) {
            m_Length = str.size();
            m_Data   = new char[m_Length + 1];
            ::std::memcpy(m_Data, str.data(), m_Length + 1);
        } else {
            m_Data   = nullptr;
            m_Length = 0;
        }
    }

    // Constructor from a C-style string (const char*)
    String(const char* str) {
        if (str) {
            m_Length = ::std::strlen(str);
            m_Data   = new char[m_Length + 1];
            ::std::memcpy(m_Data, str, m_Length + 1);
        } else {
            m_Data   = nullptr;
            m_Length = 0;
        }
    }

    // Constructor from a single character
    String(char ch) {
        m_Length  = 1;
        m_Data    = new char[2];
        m_Data[0] = ch;
        m_Data[1] = '\0';
    }

    // Copy constructor
    String(const String& other) {
        if (other.m_Data) {
            m_Length = other.m_Length;
            m_Data   = new char[m_Length + 1];
            ::std::memcpy(m_Data, other.m_Data, m_Length + 1);
        } else {
            m_Data   = nullptr;
            m_Length = 0;
        }
    }

    // Move constructor
    String(String&& other) noexcept : m_Data(other.m_Data), m_Length(other.m_Length) {
        other.m_Data   = nullptr;
        other.m_Length = 0;
    }

    // Copy assignment operator
    String& operator=(const String& other) {
        if (this != &other) {  // Avoid self-assignment
            delete[] m_Data;   // Free existing memory
            if (other.m_Data) {
                m_Length = other.m_Length;
                m_Data   = new char[m_Length + 1];
                ::std::memcpy(m_Data, other.m_Data, m_Length + 1);
            } else {
                m_Data   = nullptr;
                m_Length = 0;
            }
        }
        return *this;
    }

    // Move assignment operator
    String& operator=(String&& other) noexcept {
        if (this != &other) {  // Avoid self-assignment
            delete[] m_Data;   // Free existing memory
            m_Data         = other.m_Data;
            m_Length       = other.m_Length;
            other.m_Data   = nullptr;  // Reset moved object
            other.m_Length = 0;
        }
        return *this;
    }

    // Destructor: releases allocated memory
    ~String() {
        delete[] m_Data;
    }

    // Static method to create a String from a double
    static String fromDouble(double value) {
        char buffer[32];
        ::std::snprintf(buffer, sizeof(buffer), "%.16g", value);  // Convert double to string
        return String(buffer);
    }

    // Method to create a std::string from a String
    ::std::string to_string() const {
        if (m_Data == nullptr) {
            return {};
        }
        return std::string(m_Data, m_Length);
    }

    // Equality operator
    bool operator==(const String& other) const {
        if (m_Length != other.m_Length) {
            return false;  // If lengths are different, strings are different
        }
        return ::std::memcmp(m_Data, other.m_Data, m_Length) == 0;  // Compare data
    }

    // Inequality operator
    bool operator!=(const String& other) const {
        return !(*this == other);
    }

    // Equality operator with a C-style string
    bool operator==(const char* str) const {
        return ::std::strcmp(m_Data, str) == 0;
    }

    // Inequality operator with a C-style string
    bool operator!=(const char* str) const {
        return ::std::strcmp(m_Data, str) != 0;
    }

    // Returns the encapsulated C-style string
    const char* c_str() const {
        return m_Data ? m_Data : "";
    }

    // Returns the length of the string
    size_t length() const {
        return m_Length;
    }

    // Concatenation operator with another String
    String operator+(const String& other) const {
        size_t newLength = m_Length + other.m_Length;                         // Calculate new length
        char* newData    = new char[newLength + 1];                           // Allocate memory
        ::std::memcpy(newData, m_Data, m_Length);                             // Copy first string
        ::std::memcpy(newData + m_Length, other.m_Data, other.m_Length + 1);  // Copy second string
        return String(newData);                                               // Return new String
    }

    // Concatenation operator with a C-style string
    String operator+(const char* str) const {
        size_t strLength = ::std::strlen(str);                  // Calculate length of str
        size_t newLength = m_Length + strLength;                // Calculate new length
        char* newData    = new char[newLength + 1];             // Allocate memory
        ::std::memcpy(newData, m_Data, m_Length);               // Copy first string
        ::std::memcpy(newData + m_Length, str, strLength + 1);  // Copy str
        return String(newData);                                 // Return new String
    }

    // Stream output operator overload for displaying the string
    friend ::std::ostream& operator<<(::std::ostream& os, const String& str) {
        os << str.c_str();  // Write the string to the stream
        return os;
    }
};

}  // namespace ez

namespace std {

// Specialization of std::hash to use ez::String as a key in containers
template <>
struct hash<ez::String> {
    size_t operator()(const ez::String& s) const noexcept {
        size_t hash      = 0;
        const char* data = s.c_str();
        for (size_t i = 0; i < s.length(); ++i) {
            hash = hash * 31 + static_cast<size_t>(data[i]);  // Simple hash calculation
        }
        return hash;
    }
};

}  // namespace std

namespace ez {

// Overload of the + operator to allow concatenation of const char* and ez::String
static inline String operator+(const char* lhs, const String& rhs) {
    size_t lhsLength = ::std::strlen(lhs);                              // Calculate length of lhs
    size_t newLength = lhsLength + rhs.length();                        // Calculate new length
    char* newData    = new char[newLength + 1];                         // Allocate memory
    ::std::memcpy(newData, lhs, lhsLength);                             // Copy lhs
    ::std::memcpy(newData + lhsLength, rhs.c_str(), rhs.length() + 1);  // Copy rhs
    return String(newData);                                             // Return new String
}

// Definition of aliases for functions with different numbers of arguments
typedef ::std::function<double(double)> UnaryFunctor;
typedef ::std::function<double(double, double)> BinaryFunctor;
typedef ::std::function<double(double, double, double)> TernaryFunctor;
typedef ::std::unordered_map<String, double> VarContainer;
typedef ::std::unordered_map<String, double> ConstantContainer;

// Structure representing a function with its number of arguments and behavior
struct Function {
    int precedence = 0;
    UnaryFunctor unaryFunctor;
    BinaryFunctor binaryFunctor;
    TernaryFunctor ternaryFunctor;
    size_t argCount = 0U;
};

// Container to store available functions
typedef ::std::unordered_map<String, Function> FunctionContainer;

// Definition of possible token types
enum class TokenType {
    NUMBER = 0,
    LPAREN,
    RPAREN,
    VARIABLE,
    OPERATOR,
    FUNCTION,
    SEPARATOR,
    Count
};

// Structure representing a token (lexical unit)
struct Token {
    TokenType type;
    String value;
};

// Definition of possible error codes
enum class ErrorCode {
    NONE = 0,
    PARSE_ERROR,
    EVALUATION_NAN,
    EVALUATION_INF,
    DIVISION_BY_ZERO,
    UNKNOWN_NODE_TYPE,
    EMPTY_PARENTHESIS,
    VARIABLE_NOT_FOUND,
    FUNCTION_NOT_FOUND,
    OPERATOR_NOT_FOUND,
    UNMATCHED_PARENTHESIS,
    FUNCTION_WRONG_ARGUMENTS_COUNT,
    Count
};


// Definition of possible node types in a syntax tree
enum class NodeType {
    NUMBER = 0,
    VARIABLE,
    OPERATOR,
    FUNCTION,
    Count
};

// Structure representing a node in the syntax tree
struct Node {
    NodeType type = NodeType::NUMBER;
    String name;
    double value = 0.0;
    ::std::array< ::std::shared_ptr<Node>, 3> childs{};  // Array of pointers to child nodes
    size_t childCount = 0;
};

// Class to manage exceptions specific to expression evaluation
class ExprException : public ::std::exception {
public:
    ExprException(ErrorCode code, const String& message)
        : m_Code(code), m_Message(message) {
    }

    ErrorCode getCode() const {
        return m_Code;
    }

    const char* what() const noexcept override {
        return m_Message.c_str();  // Return the error message
    }

private:
    ErrorCode m_Code;
    String m_Message;
};

// Main class for evaluating mathematical expressions
class Expr {
private:
    String m_Expr;                                            // Expression to evaluate
    Node m_RootExpr;                                          // Root of the syntax tree
    VarContainer m_ParsedVariables;                           // Container for variables found during parsing
    VarContainer m_DefinedVariables;                          // Container for variables defined after parsing
    ConstantContainer m_Constant;                             // Container for constants
    FunctionContainer m_Functions;                            // Container for functions
    double m_EvalResult = 0.0;                                // Evaluation result
    bool m_Verbose      = false;                              // Verbose mode
    ::std::stringstream m_printExpr;                          // Stream to print the expression and result
    ::std::chrono::duration<double, ::std::milli> m_Elapsed;  // Evaluation time

public:
    Expr() {
        // Initialization of common constants
        addConstant("pi", M_PI);     // PI
        addConstant("e", M_E);       // e

        // Initialization of common unary functions
        addFunction("-", 1, [](double a) { return -a; });

        addFunction("abs", 1, [](double a) { return ::std::abs(a); });
        addFunction("floor", 1, [](double a) { return ::std::floor(a); });
        addFunction("ceil", 1, [](double a) { return ::std::ceil(a); });
        addFunction("round", 1, [](double a) { return ::std::round(a); });

        addFunction("fract", 1, [](double a) { return a - ::std::floor(a); });
        addFunction("sign", 1, [this](double a) { return m_Sign(a); });

        addFunction("sin", 1, [](double a) { return ::std::sin(a); });
        addFunction("cos", 1, [](double a) { return ::std::cos(a); });
        addFunction("tan", 1, [](double a) { return ::std::tan(a); });

        addFunction("asin", 1, [](double a) { return ::std::asin(a); });
        addFunction("acos", 1, [](double a) { return ::std::acos(a); });
        addFunction("atan", 1, [](double a) { return ::std::atan(a); });

        addFunction("sinh", 1, [](double a) { return ::std::sinh(a); });
        addFunction("cosh", 1, [](double a) { return ::std::cosh(a); });
        addFunction("tanh", 1, [](double a) { return ::std::tanh(a); });

        addFunction("asinh", 1, [](double a) { return ::std::asinh(a); });
        addFunction("acosh", 1, [](double a) { return ::std::acosh(a); });
        addFunction("atanh", 1, [](double a) { return ::std::atanh(a); });

        addFunction("ln", 1, [](double a) { return ::std::log(a); });
        addFunction("log", 1, [](double a) { return ::std::log(a); });
        addFunction("log1p", 1, [](double a) { return ::std::log1p(a); });
        addFunction("logb", 1, [](double a) { return ::std::logb(a); });
        addFunction("log2", 1, [](double a) { return ::std::log2(a); });
        addFunction("log10", 1, [](double a) { return ::std::log10(a); });

        addFunction("sqrt", 1, [](double a) { return ::std::sqrt(a); });
        addFunction("exp", 1, [](double a) { return ::std::exp(a); });

        // Initialization of common binary functions
        addFunction("mod", 1, [](double a, double b) { return ::std::fmod(a, b); });
        addFunction("atan2", 1, [](double a, double b) { return ::std::atan2(a, b); });
        addFunction("min", 1, [this](double a, double b) { return m_Min(a, b); });
        addFunction("max", 1, [this](double a, double b) { return m_Max(a, b); });
        addFunction("step", 1, [this](double a, double b) { return m_Step(a, b); });
        addFunction("hypot", 1, [](double a, double b) { return ::std::hypot(a, b); });
        addFunction("smoothabs", 1, [this](double a, double b) { return m_SmoothAbs(a, b); });

        // Initialization of common ternary functions
        addFunction("clamp", 1, [this](double a, double b, double c) { return m_Clamp(a, b, c); });
        addFunction("saturate", 1, [this](double a, double b, double c) { return m_Clamp(a, b, c); });
        addFunction("lerp", 1, [this](double a, double b, double c) { return m_Mix(a, b, c); });
        addFunction("mix", 1, [this](double a, double b, double c) { return m_Mix(a, b, c); });
        addFunction("smoothstep", 1, [this](double a, double b, double c) { return m_SmoothStep(a, b, c); });
    }

    // Method to parse an expression
    Expr& parse(const String& vExpr) {
        m_Expr      = vExpr;                       // Store the expression
        m_ParsedVariables.clear();                 // clearing discoverd vairable during parsing
        auto tokens = m_tokenize(m_Expr.c_str());  // Tokenize the expression
        size_t pos  = 0;
        m_RootExpr  = m_parseExpression(tokens, pos, 0);  // Parse the expression to create the syntax tree

        // Check for remaining tokens after parsing
        if (pos < tokens.size()) {
            // If the next token is an unmatched closing parenthesis
            if (tokens[pos].value == ")") {
                throw ExprException(ErrorCode::UNMATCHED_PARENTHESIS, "Unmatched parenthesis found at the end of the expression.");
            }
            // If other tokens remain, which is also abnormal
            throw ExprException(ErrorCode::PARSE_ERROR, "Unexpected token found after complete parsing.");
        }

        return *this;
    }

    // Method to set the value of a variable
    Expr& set(const String& vName, const double vValue, const bool vIfNotExist = false) {
        if (vIfNotExist) {
            if (m_DefinedVariables.find(vName) != m_DefinedVariables.end()) {  // If exists
                return *this;
            }
        }
        m_DefinedVariables[vName] = vValue;
        return *this;
    }

    // Method to evaluate the expression
    Expr& eval() {
        auto startTime = ::std::chrono::steady_clock::now();  // Start time
        m_EvalResult   = m_evalNode(m_RootExpr);              // Evaluate the root of the syntax tree
        auto endTime   = ::std::chrono::steady_clock::now();  // End time
        m_Elapsed      = endTime - startTime;                 // Calculate elapsed time
        return *this;
    }

    // Method to add a constant
    Expr& addConstant(const String& vName, const double vValue) {
        m_Constant[vName] = vValue;  // Add the constant
        return *this;
    }

    // Method to add a unary function
    Expr& addFunction(const String& vName, int precedence, const UnaryFunctor& functor) {
        m_Functions[vName] = {precedence, functor, nullptr, nullptr, 1};  // Add the function with its precedence
        return *this;
    }

    // Method to add a binary function
    Expr& addFunction(const String& vName, int precedence, const BinaryFunctor& functor) {
        m_Functions[vName] = {precedence, nullptr, functor, nullptr, 2};
        return *this;
    }

    // Method to add a ternary function
    Expr& addFunction(const String& vName, int precedence, const TernaryFunctor& functor) {
        m_Functions[vName] = {precedence, nullptr, nullptr, functor, 3};
        return *this;
    }

    // Method to enable or disable verbose mode
    Expr& setVerbose(bool vVerbose) {
        m_Verbose = vVerbose;
        return *this;
    }

    // Returns the evaluation result
    double getResult() {
        return m_EvalResult;
    }

    // Prints the expression and its result
    Expr& print() {
        m_printExpr = {};
        m_printExpr << "Expr \"" << m_Expr << "\"";
        for (const auto& it : m_DefinedVariables) {
            m_printExpr << ", " << it.first << " = " << it.second;
        }
        m_printExpr << " => " << m_EvalResult;
        ::std::cout << m_printExpr.str() << " (" << getEvalTime() << " ms)" << ::std::endl;
        return *this;
    }

    // Checks if the evaluation result is close to a given value
    bool check(const double vValue) {
        return (::std::abs(m_EvalResult - vValue) < 0.001);
    }

    // Returns the evaluation time in milliseconds
    double getEvalTime() {
        return m_Elapsed.count();
    }

    // test if a variable found during parsing exist
    bool isParsedVariableExist(const String& vName) {
        return (m_ParsedVariables.find(vName) != m_ParsedVariables.end());
    }

    // Returns variables as a constant reference
    const VarContainer& getParsedVars() {
        return m_ParsedVariables;
    }

    // Returns variables as a modifiable reference
    VarContainer& getParsedVarsRef() {
        return m_ParsedVariables;
    }

    // test if a defined variable exist
    bool isDefinedVariableExist(const String& vName) {
        return (m_DefinedVariables.find(vName) != m_ParsedVariables.end());
    }

    // Returns defined variables as a constant reference
    const VarContainer& getDefinedVars() {
        return m_DefinedVariables;
    }

    // Returns defined variables as a modifiable reference
    VarContainer& getDefinedVarsRef() {
        return m_DefinedVariables;
    }

protected:
    // Tokenize the expression
    ::std::vector<Token> m_tokenize(const char* expr) {
        ::std::vector<Token> tokens;
        ::std::istringstream stream(expr);
        char ch;
        while (stream >> ch) {
            if (::std::isdigit(ch) || ch == '.') {
                stream.putback(ch);
                double number;
                stream >> number;
                tokens.push_back({TokenType::NUMBER, String::fromDouble(number)});
            } else if (::std::isalpha(ch)) {
                String identifier(ch);
                while (stream.get(ch) && ::std::isalnum(ch)) {
                    identifier = identifier + String(ch);
                }
                stream.putback(ch);
                tokens.push_back({TokenType::VARIABLE, identifier});  // Traitement générique comme identifiant
            } else if (ch == '(') {
                tokens.push_back({TokenType::LPAREN, String("(")});
            } else if (ch == ')') {
                tokens.push_back({TokenType::RPAREN, String(")")});
            } else if (ch == ',') {
                tokens.push_back({TokenType::SEPARATOR, String(",")});
            } else {
                String op(ch);
                while (stream.get(ch) && !::std::isalnum(ch) && ch != ' ' && ch != '(' && ch != ')' && ch != ',') {
                    op = op + String(ch);
                }
                stream.putback(ch);
                tokens.push_back({TokenType::OPERATOR, op});
            }
        }
        if (m_Verbose) {
            m_log("Tokens: ");
            for (const auto& token : tokens) {
                m_log(token.value + " ");
            }
            m_log("\n");
        }
        return tokens;
    }


    // Logs messages if verbose mode is enabled
    void m_log(const String& message) {
        if (m_Verbose) {
            ::std::cout << message.c_str();
        }
    }

    // Evaluate a node in the syntax tree
    double m_evalNode(const Node& node) {
        double result = 0.0;

        switch (node.type) {
            case NodeType::NUMBER: {
                result = node.value;
                break;
            }
            case NodeType::VARIABLE: {
                auto it = m_DefinedVariables.find(node.name);
                if (it == m_DefinedVariables.end()) {
                    throw ExprException(ErrorCode::VARIABLE_NOT_FOUND, "Variable not found: " + node.name);
                }
                result = it->second;
                break;
            }
            case NodeType::FUNCTION: {
                auto it = m_Functions.find(node.name);
                if (it == m_Functions.end()) {
                    // Special case handling for the "!" operator (factorial)
                    if (node.name == "!") {
                        double operand = m_evalNode(*node.childs[0]);
                        result         = m_Factorial(operand);  // Call the factorial function
                    } else {
                        throw ExprException(ErrorCode::FUNCTION_NOT_FOUND, "Function not found: " + node.name);
                    }
                } else {
                    if (it->second.argCount == 1) {
                        double operand = m_evalNode(*node.childs[0]);
                        result         = it->second.unaryFunctor(operand);
                    } else if (it->second.argCount == 2) {
                        double left  = m_evalNode(*node.childs[0]);
                        double right = m_evalNode(*node.childs[1]);
                        result       = it->second.binaryFunctor(left, right);
                    } else if (it->second.argCount == 3) {
                        double first  = m_evalNode(*node.childs[0]);
                        double second = m_evalNode(*node.childs[1]);
                        double third  = m_evalNode(*node.childs[2]);
                        result        = it->second.ternaryFunctor(first, second, third);
                    }
                }
                break;
            }
            case NodeType::OPERATOR: {
                double left  = m_evalNode(*node.childs[0]);
                double right = m_evalNode(*node.childs[1]);
                if (node.name == "+") {
                    result = left + right;
                } else if (node.name == "-") {
                    result = left - right;
                } else if (node.name == "*") {
                    result = left * right;
                } else if (node.name == "/") {
                    if (right == 0.0) {
                        throw ExprException(ErrorCode::DIVISION_BY_ZERO, "Division by zero");
                    }
                    result = left / right;
                } else if (node.name == "^") {
                    result = ::std::pow(left, right);
                } else if (node.name == "%") {
                    if (right == 0.0) {
                        throw ExprException(ErrorCode::DIVISION_BY_ZERO, "Division by zero");
                    }
                    result = ::std::fmod(left, right);
                }
                break;
            }
            default:
                throw ExprException(ErrorCode::UNKNOWN_NODE_TYPE, "Unknown node type");
        }

        // Vérification du résultat pour NaN ou Inf
        if (::std::isnan(result)) {
            throw ExprException(ErrorCode::EVALUATION_NAN, "Result is NaN");
        } else if (::std::isinf(result)) {
            throw ExprException(ErrorCode::EVALUATION_INF, "Result is Inf");
        }

        if (m_Verbose) {
            m_log("Evaluating Node: " + String::fromDouble(result) + "\n");
        }

        return result;
    }

    // Parse an expression to create a syntax tree
    Node m_parseExpression(::std::vector<Token>& tokens, size_t& pos, int precedence) {
        if (pos >= tokens.size()) {
            throw ExprException(ErrorCode::PARSE_ERROR, "Unexpected end of expression");
        }
        Node node = m_parseFactor(tokens, pos);
        while (pos < tokens.size()) {
            String op        = tokens[pos].value;
            int opPrecedence = 0;
            if (tokens[pos].type == TokenType::OPERATOR) {
                if (op == "+" || op == "-" || op == "*" || op == "/" || op == "^" || op == "%" || op == "!") {
                    opPrecedence = (op == "+" || op == "-") ? 1 : (op == "*" || op == "/" || op == "%") ? 2
                                                                                                        : 3;
                } else {
                    throw ExprException(ErrorCode::OPERATOR_NOT_FOUND, "Operator not found: " + op);
                }
            }
            if (opPrecedence <= precedence) {
                break;
            }
            ++pos;
            if (pos >= tokens.size()) {
                throw ExprException(ErrorCode::PARSE_ERROR, "Incomplete expression after operator: " + op);
            }
            Node rightNode = m_parseExpression(tokens, pos, opPrecedence);
            Node opNode;
            opNode.type       = NodeType::OPERATOR;
            opNode.name       = op;
            opNode.childs[0]  = ::std::make_shared<Node>(node);
            opNode.childCount = 2;
            opNode.childs[1]  = ::std::make_shared<Node>(rightNode);
            node              = opNode;
        }
        return node;
    }

    // Parse a factor in an expression (number, variable, parenthesis, function)
    Node m_parseFactor(::std::vector<Token>& tokens, size_t& pos) {
        Node node;
        if (pos >= tokens.size()) {
            throw ExprException(ErrorCode::PARSE_ERROR, "Unexpected end of expression");
        }

        if (tokens[pos].type == TokenType::NUMBER) {
            node.type  = NodeType::NUMBER;
            node.value = ::std::stod(tokens[pos].value.c_str());
            ++pos;

            // Gestion de l'opérateur postfixé "!" pour les nombres
            if (pos < tokens.size() && tokens[pos].type == TokenType::OPERATOR && tokens[pos].value == "!") {
                Node opNode;
                opNode.type       = NodeType::FUNCTION;
                opNode.name       = "!";
                opNode.childs[0]  = ::std::make_shared<Node>(node);
                opNode.childCount = 1;
                node              = opNode;  // Remplacer le nœud par l'opérateur postfixé
                ++pos;
            }
        } else if (tokens[pos].type == TokenType::VARIABLE) {
            String identifier = tokens[pos].value;
            ++pos;

            if (pos < tokens.size() && tokens[pos].type == TokenType::LPAREN) {
                auto it = m_Functions.find(identifier);
                if (it == m_Functions.end()) {
                    throw ExprException(ErrorCode::FUNCTION_NOT_FOUND, "Function not found: " + identifier);
                }

                ++pos;  // Passer la parenthèse ouvrante

                if (pos < tokens.size() && tokens[pos].type == TokenType::RPAREN) {
                    throw ExprException(ErrorCode::FUNCTION_WRONG_ARGUMENTS_COUNT, "Function called with incorrect number of arguments: " + identifier);
                }

                node.type       = NodeType::FUNCTION;
                node.name       = identifier;
                node.childs[0]  = ::std::make_shared<Node>(m_parseExpression(tokens, pos, 0));
                node.childCount = 1;

                while (pos < tokens.size() && tokens[pos].type == TokenType::SEPARATOR) {
                    ++pos;
                    if (node.childCount >= 3) {
                        throw ExprException(ErrorCode::PARSE_ERROR, "Too many arguments provided for function: " + identifier);
                    }
                    node.childs[node.childCount++] = ::std::make_shared<Node>(m_parseExpression(tokens, pos, 0));
                }

                if (pos >= tokens.size() || tokens[pos].type != TokenType::RPAREN) {
                    throw ExprException(ErrorCode::UNMATCHED_PARENTHESIS, "Unmatched parenthesis");
                }
                ++pos;

                if (node.childCount != it->second.argCount) {
                    throw ExprException(ErrorCode::FUNCTION_WRONG_ARGUMENTS_COUNT, "Incorrect number of arguments for function: " + identifier);
                }

            } else {
                // Gestion des variables (non-fonctions et non-constantes)
                auto it = m_Constant.find(identifier);
                if (it != m_Constant.end()) {
                    node.type  = NodeType::NUMBER;
                    node.value = it->second;
                } else {
                    node.type                     = NodeType::VARIABLE;
                    node.name                     = identifier;
                    m_ParsedVariables[identifier] = 0.0;  // Ajout de la variable trouvée avec une valeur par défaut de 0.0
                }

                // Gestion de l'opérateur postfixé "!" pour les variables
                if (pos < tokens.size() && tokens[pos].type == TokenType::OPERATOR && tokens[pos].value == "!") {
                    Node opNode;
                    opNode.type       = NodeType::FUNCTION;
                    opNode.name       = "!";
                    opNode.childs[0]  = ::std::make_shared<Node>(node);
                    opNode.childCount = 1;
                    node              = opNode;  // Remplacer le nœud par l'opérateur postfixé
                    ++pos;
                }
            }
        } else if (tokens[pos].type == TokenType::LPAREN) {
            ++pos;
            if (pos < tokens.size() && tokens[pos].type == TokenType::RPAREN) {
                throw ExprException(ErrorCode::EMPTY_PARENTHESIS, "Empty parenthesis found");
            }
            node = m_parseExpression(tokens, pos, 0);
            if (pos >= tokens.size() || tokens[pos].type != TokenType::RPAREN) {
                throw ExprException(ErrorCode::UNMATCHED_PARENTHESIS, "Unmatched parenthesis");
            }
            ++pos;
        } else if (tokens[pos].type == TokenType::OPERATOR) {
            String op = tokens[pos].value;
            if (op == "-") {
                node.type = NodeType::FUNCTION;
                node.name = tokens[pos].value;
                ++pos;
                node.childs[0]  = ::std::make_shared<Node>(m_parseFactor(tokens, pos));
                node.childCount = 1;
            } else if (op == "!") {
                throw ExprException(ErrorCode::PARSE_ERROR, "Factorial operator '!' cannot be used alone or in prefix position.");
            } else {
                throw ExprException(ErrorCode::PARSE_ERROR, "Unexpected operator: " + tokens[pos].value);
            }
        } else {
            throw ExprException(ErrorCode::PARSE_ERROR, "Unexpected token: " + tokens[pos].value);
        }

        return node;
    }

    /////////////////////////////////////////
    //// BUILTINS FUNCTIONS /////////////////
    /////////////////////////////////////////

    // Calculate a factorial
    double m_Factorial(double vValue) {
        if (vValue < 0 || ::std::floor(vValue) != vValue) {
            throw ExprException(ErrorCode::PARSE_ERROR, "Factorial is not defined for negative or non-integer values.");
        }
        double result    = 1;
        const auto count = static_cast<int>(vValue);
        for (int i = 1; i <= count; ++i) {
            result *= i;
        }
        return result;
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/min.xhtml
    double m_Min(double vX, double vY) {
        if (vX < vY) {
            return vX;
        }
        return vY;
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/max.xhtml
    double m_Max(double vX, double vY) {
        if (vX > vY) {
            return vX;
        }
        return vY;
    }

    // https://www.shadertoy.com/???? : sqrt(v*v+k) with k >= 0
    double m_SmoothAbs(double vV, double vK) {
        return ::std::sqrtf(vV * vV + ::std::abs(vK));
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/clamp.xhtml
    // Clamp (glsl), Saturate (hlsl)
    double m_Clamp(double vX, double vMinVal, double vMaxVal) {
        return m_Min(m_Max(vX, vMinVal), vMaxVal);
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/smoothstep.xhtml
    double m_SmoothStep(double vEdge0, double vEdge1, double vX) {
        double t = m_Clamp((vX - vEdge0) / (vEdge1 - vEdge0), 0.0, 1.0);
        return t * t * (3.0 - 2.0 * t);
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/mix.xhtml
    // Mix (glsl), lerp (hlsl)
    double m_Mix(double vX, double vY, double vA) {
        return vX * (1.0 - vA) + vY * vA;
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/step.xhtml
    double m_Step(double vEdge, double vX) {
        return vX < vEdge ? 0.0 : 1.0;
    }

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/sign.xhtml
    double m_Sign(double vX) {
        if (vX < 0.0) {
            return -1.0;
        } else if (vX > 0.0) {
            return 1.0;
        }
        return 0.0;
    }
};

}  // namespace ez
