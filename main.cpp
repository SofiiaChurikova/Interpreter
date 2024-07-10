#include <iostream>
#include <queue>
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

int main() {
    string input;

    while (true) {
        cout << "Enter the mathematical expression (or '0' to exit): ";
        getline(cin, input);

        if (input[0] == '0') {
            break;
        } else {
            queue<string> tokens = Tokenization::Tokenize(input);

            while (!tokens.empty()) {
                cout << tokens.front() << " ";
                tokens.pop();
            }
            cout << endl;
        }
    }

    return 0;
}
