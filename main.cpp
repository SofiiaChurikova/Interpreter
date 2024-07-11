#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <map>
#include <csetjmp>

using namespace std;

map<string, double> variables;
jmp_buf jumpBuffer;

void signalHandler(int sign) {
    cerr << "Error: wrong entry " << endl;
    longjmp(jumpBuffer, 1);
}

class Tokenization {
public:
    static queue<string> Tokenize(const string &input) {
        queue<string> tokens;
        string token;

        for (size_t i = 0; i < input.length(); ++i) {
            char ch = input[i];
            if (isdigit(ch) || ch == '.' || (ch == '-' && (i == 0 || !isdigit(input[i - 1])))) {
                token += ch;
            } else if (ch == '(' || ch == ')' || string("+-*/,=").find(ch) != string::npos) {
                if (!token.empty()) {
                    tokens.push(token);
                    token.clear();
                }
                tokens.push(string(1, ch));
            } else if (isalpha(ch)) {
                size_t funcEnd = i;
                while (funcEnd < input.length() && (isalpha(input[funcEnd]) || isdigit(input[funcEnd]))) {
                    ++funcEnd;
                }
                string funcName = input.substr(i, funcEnd - i);
                tokens.push(funcName);
                i = funcEnd - 1;
            } else if (ch == ' ') {
                if (!token.empty()) {
                    tokens.push(token);
                    token.clear();
                }
            } else {
                throw invalid_argument("Invalid character in input: " + string(1, ch));
            }
        }

        if (!token.empty()) {
            tokens.push(token);
        }

        return tokens;
    }
};

class ShuntingYard {
public:
    static bool IsOperator(const string &token) {
        return token == "+" || token == "-" || token == "*" || token == "/";
    }

    static bool IsFunction(const string &token) {
        return token == "pow" || token == "abs" || token == "max" || token == "min";
    }

    static int Priority(const string &op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        return 0;
    }

    static queue<string> TransformToRPN(queue<string> &tokens) {
        stack<string> operStack;
        queue<string> outputQueue;

        while (!tokens.empty()) {
            string token = tokens.front();
            tokens.pop();

            if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1])) || variables.
                count(token)) {
                outputQueue.push(token);
            } else if (IsFunction(token)) {
                operStack.push(token);
            } else if (IsOperator(token)) {
                while (!operStack.empty() && (IsFunction(operStack.top()) || Priority(operStack.top()) >=
                                              Priority(token))) {
                    outputQueue.push(operStack.top());
                    operStack.pop();
                }
                operStack.push(token);
            } else if (token == "(") {
                operStack.push(token);
            } else if (token == ")") {
                while (operStack.top() != "(") {
                    outputQueue.push(operStack.top());
                    operStack.pop();
                }
                operStack.pop();
                if (!operStack.empty() && IsFunction(operStack.top())) {
                    outputQueue.push(operStack.top());
                    operStack.pop();
                }
            } else if (token == ",") {
                while (!operStack.empty() && operStack.top() != "(") {
                    outputQueue.push(operStack.top());
                    operStack.pop();
                }
            } else {
                throw invalid_argument("Invalid token in input: " + token);
            }
        }

        while (!operStack.empty()) {
            outputQueue.push(operStack.top());
            operStack.pop();
        }

        return outputQueue;
    }
};

class Calculation {
private:
    static double ApplyOperator(const string &op, double a, double b) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") {
            if (b == 0) {
                throw invalid_argument("Division by zero");
            }
            return a / b;
        } else if (op == "pow") {
            return pow(a, b);
        } else if (op == "max") {
            return max(a, b);
        } else if (op == "min") {
            return min(a, b);
        }
        throw invalid_argument("Invalid operator or function");
    }

public:
    static double CalculateResult(queue<string> &rpnQueue, string &var) {
        stack<double> stack;

        while (!rpnQueue.empty()) {
            string token = rpnQueue.front();
            rpnQueue.pop();

            if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
                stack.push(stod(token));
            } else if (variables.count(token)) {
                stack.push(variables[token]);
            } else if (ShuntingYard::IsOperator(token)) {
                double operand2 = stack.top();
                stack.pop();
                double operand1 = stack.top();
                stack.pop();
                double result = ApplyOperator(token, operand1, operand2);
                stack.push(result);
            } else if (ShuntingYard::IsFunction(token)) {
                if (token == "abs") {
                    double operand = stack.top();
                    stack.pop();
                    stack.push(abs(operand));
                } else {
                    double operand2 = stack.top();
                    stack.pop();
                    double operand1 = stack.top();
                    stack.pop();
                    double result = ApplyOperator(token, operand1, operand2);
                    stack.push(result);
                }
            } else {
                throw invalid_argument("Invalid token in RPN queue: " + token);
            }
        }

        double finalResult = stack.top();
        if (!var.empty()) {
            variables[var] = finalResult;
        }
        return finalResult;
    }
};

class Variable {
private:
    static string Replace(string str, const string &from, const string &to) {
        size_t start = 0;
        while ((start = str.find(from, start)) != string::npos) {
            str.replace(start, from.length(), to);
            start += to.length();
        }
        return str;
    }

public:
    static void SaveVar(const string &input) {
        size_t pos = input.find('=');
        if (pos == string::npos) {
            throw invalid_argument("Invalid variable assignment.");
        }

        string varName = input.substr(0, pos);
        varName.erase(remove_if(varName.begin(), varName.end(), ::isspace), varName.end());
        string varValue = input.substr(pos + 1);

        queue<string> tokensVar = Tokenization::Tokenize(varValue);
        queue<string> outputVar = ShuntingYard::TransformToRPN(tokensVar);
        double resultVar = Calculation::CalculateResult(outputVar, varName);

        variables[varName] = resultVar;
    }

    static string ReplaceVar(const string &input) {
        string result = input;
        queue<string> tokens = Tokenization::Tokenize(input);
        while (!tokens.empty()) {
            string token = tokens.front();
            tokens.pop();

            if (variables.find(token) != variables.end()) {
                result = Replace(result, token, to_string(variables[token]));
            }
        }

        return result;
    }

    static void ParseVar(string &input, string &assignVar) {
        size_t pos = input.find('=');
        if (pos != string::npos) {
            assignVar = input.substr(0, pos);
            assignVar.erase(remove_if(assignVar.begin(), assignVar.end(), ::isspace), assignVar.end());
            input = input.substr(pos + 1);
        }
    }
};

int main() {
    signal(SIGSEGV, signalHandler);

    string input;

    while (true) {
        getline(cin, input);

        if (input == "0") {
            break;
        } else {
            if (setjmp(jumpBuffer) == 0) {
                try {
                    if (input.find("var ") == 0) {
                        Variable::SaveVar(input.substr(4));
                    } else {
                        string var;
                        Variable::ParseVar(input, var);
                        string processedInput = Variable::ReplaceVar(input);
                        queue<string> tokens = Tokenization::Tokenize(processedInput);
                        queue<string> output = ShuntingYard::TransformToRPN(tokens);
                        double result = Calculation::CalculateResult(output, var);
                        cout << "Result: " << result << endl;
                    }
                } catch (const invalid_argument &e) {
                    cerr << "Error: " << e.what() << endl;
                } catch (const exception &e) {
                    cerr << "Exception: " << e.what() << endl;
                } catch (...) {
                    cerr << "Unknown error occurred" << endl;
                }
            }
        }
    }

    return 0;
}
