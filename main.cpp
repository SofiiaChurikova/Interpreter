#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <cctype>

using namespace std;

class Tokenization {
public:
    static queue<string> Tokenize(const string& input) {
        queue<string> tokens;
        string token;

        for (size_t i = 0; i < input.length(); ++i) {
            char ch = input[i];
            if (isdigit(ch) || ch == '.' || (ch == '-' && (i == 0 || !isdigit(input[i - 1])))) {
                token += ch;
            } else if (ch == '(' || ch == ')' || string("+-*/").find(ch) != string::npos) {
                if (!token.empty()) {
                    tokens.push(token);
                    token.clear();
                }
                tokens.push(string(1, ch));
            } else if (ch == ' ') {
                if (!token.empty()) {
                    tokens.push(token);
                    token.clear();
                }
            }
        }

        if (!token.empty()) {
            tokens.push(token);
        }

        return tokens;
    }
};

class ShuntingYard {
private:
    static bool IsOperator(const string& token) {
        return token == "+" || token == "-" || token == "*" || token == "/";
    }

    static int Priority(const string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        return 0;
    }
public:
    static queue<string> TransformToRPN(queue<string>& tokens) {
        stack<string> operStack;
        queue<string> outputQueue;

        while (!tokens.empty()) {
            string token = tokens.front();
            tokens.pop();

            if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
                outputQueue.push(token);
            } else if (IsOperator(token)) {
                while (!operStack.empty() && Priority(operStack.top()) >= Priority(token)) {
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
            }
        }

        while (!operStack.empty()) {
            outputQueue.push(operStack.top());
            operStack.pop();
        }

        return outputQueue;
    }
};

int main() {
    string input;

    while (true) {
        cout << "Enter the mathematical expression (or '0' to exit): ";
        getline(cin, input);

        if (input[0] == '0') {
            break;
        } else {
            queue<string> tokens = Tokenization::Tokenize(input);
            queue<string> output = ShuntingYard::TransformToRPN(tokens);

            while (!output.empty()) {
                cout << output.front() << " ";
                output.pop();
            }
            cout << endl;
        }
    }

    return 0;
}
