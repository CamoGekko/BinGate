#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Global flags
bool translate_only = false;
std::string filename;

// Function to tokenize the string based on a given delimiter
std::vector<std::string> tokenize(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char ch : str) {
        if (ch == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token); // Add token to the list
                token.clear();           // Clear token for the next one
            }
        } else {
            token += ch; // Build the token
        }
    }
    // Add the last token (if any) after the loop
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to evaluate logical operations
std::string eval(const std::vector<std::string>& tks) {
    if (tks.empty()) return "";

    auto evaluateExpression = [](const std::vector<std::string>& expr) -> std::string {
        if (expr.size() < 3) {
            std::cerr << "Error: Invalid expression." << std::endl;
            return "";
        }

        if (expr[0] == "AND") {
            return "(" + expr[1] + " && " + expr[2] + ")";
        }

        if (expr[0] == "OR") {
            return "(" + expr[1] + " || " + expr[2] + ")";
        }

        if (expr[0] == "XOR") {
            return "((" + expr[1] + " && !" + expr[2] + ") || (!" + expr[1] + " && " + expr[2] + "))";
        }

        if (expr[0] == "NOT") {
            if (expr.size() != 2) {
                std::cerr << "Error: NOT requires exactly one operand." << std::endl;
                return "";
            }
            return "(!" + expr[1] + ")";
        }

        if (expr[0] == "NAND") {
            return "!(" + expr[1] + " && " + expr[2] + ")";
        }

        if (expr[0] == "NOR") {
            return "!(" + expr[1] + " || " + expr[2] + ")";
        }

        if (expr[0] == "XNOR") {
            return "(" + expr[1] + " == " + expr[2] + ")";
        }

        std::cerr << "Error: Unrecognized logical operation: " << expr[0] << std::endl;
        return "";
    };

    std::vector<std::string> expr;
    std::string result;

    for (size_t i = 0; i < tks.size(); ++i) {
        if (tks[i] == "(") {
            int paren_count = 1;
            size_t j = i + 1;
            std::vector<std::string> nested_expr;
            while (j < tks.size() && paren_count > 0) {
                if (tks[j] == "(") paren_count++;
                if (tks[j] == ")") paren_count--;
                if (paren_count > 0) nested_expr.push_back(tks[j]);
                j++;
            }
            if (paren_count == 0) {
                result += "(" + eval(nested_expr) + ")";
                i = j - 1;
            } else {
                std::cerr << "Error: Mismatched parentheses." << std::endl;
                return "";
            }
        } else {
            expr.push_back(tks[i]);
            if (expr.size() > 1 && (expr[expr.size() - 2] == "AND" || expr[expr.size() - 2] == "OR" ||
                expr[expr.size() - 2] == "XOR" || expr[expr.size() - 2] == "NAND" ||
                expr[expr.size() - 2] == "NOR" || expr[expr.size() - 2] == "XNOR")) {
                
                result += evaluateExpression(expr) + " ";
                expr.clear();
            }
        }
    }

    if (!expr.empty()) {
        result += evaluateExpression(expr) + " ";
    }

    return result;
}

std::string lexise(const std::vector<std::string>& vec) {
    std::string code;
    std::vector<std::string> expr;
    code += "#include <iostream>\nint main(){\n";

    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == "VAR") {
            if (i + 2 < vec.size()) {
                if (vec[i + 2] == "AND" || vec[i + 2] == "OR" || vec[i + 2] == "XOR" ||
                    vec[i + 2] == "NOT" || vec[i + 2] == "NAND" || vec[i + 2] == "NOR" || vec[i + 2] == "XNOR") {
                    
                    int paren_count = 0;
                    for (size_t j = i + 2; j < vec.size(); ++j) {
                        if (vec[j] == "(") paren_count++;
                        if (vec[j] == ")") paren_count--;
                        expr.push_back(vec[j]);
                        if (paren_count == 0) break;
                    }

                    code += "bool " + vec[i + 1] + " = " + eval(expr) + ";\n";
                    i += expr.size() + 1;
                    expr.clear();
                } else {
                    code += "bool " + vec[i + 1] + " = " + vec[i + 2] + ";\n";
                    i += 2;
                }
            } else {
                std::cerr << "Error: Missing value for VAR.\n";
            }
        } else if (vec[i] == "OUT") {
            if (i + 1 < vec.size()) {
                code += "std::cout << " + vec[i + 1] + " << std::endl;\n";
                i++;
            } else {
                std::cerr << "Error: Missing output variable.\n";
            }
        } else if (vec[i] == "(") {
            int paren_count = 1;
            i++;
            while (i < vec.size() && paren_count > 0) {
                if (vec[i] == "(") paren_count++;
                if (vec[i] == ")") paren_count--;
                if (paren_count > 0) expr.push_back(vec[i]);
                i++;
            }
            if (paren_count == 0) {
                code += eval(expr) + ";\n";
                expr.clear();
            } else {
                std::cerr << "Error: Mismatched parentheses.\n";
            }
        }
    }

    code += "\nreturn 0;\n}";

    return code;
}

int processArgs(int argc, char** argv) {
    for (int i = 2; i < argc; ++i) {  // Start at 2 to skip the program name and script filename(argv[1])
        std::string arg = argv[i];

        if (arg == "--help") {
            std::cout << "Help: \n[-t] to only translate into C++ code\n[-o] file name to compile to\n";
        } else if (arg == "-t") {
            translate_only = true;
        } else if (arg == "-o") {
            if (i + 1 < argc) {  // Ensure the next argument exists
                filename = argv[i + 1];
                i++; // Skip the next argument since it's the filename
            } else {
                std::cerr << "Error: --file option requires a filename.\n";
            }
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Please provide the script file as an argument." << std::endl;
        return 1;
    }

    // Process arguments
    processArgs(argc, argv);

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open the file: " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    std::string fullScript;

    // Read the file line by line
    while (std::getline(file, line)) {
        fullScript += line + " "; // Add a space between lines for correct tokenization
    }

    file.close();

    // Tokenize the entire script
    std::vector<std::string> tokens = tokenize(fullScript, ' ');
    std::string generatedCode = lexise(tokens);

    // Print generated code if -t option is used
    if (translate_only) {
        std::cout << "Generated Code:\n" << generatedCode << std::endl;
    } else {
        // Save to file if -o option is used
        if (!filename.empty()) {
            std::ofstream outputFile(filename);
            if (outputFile) {
                outputFile << generatedCode;
                outputFile.close();
                std::cout << "Code has been written to " << filename << std::endl;
            } else {
                std::cerr << "Error: Could not write to file: " << filename << std::endl;
                return 1;
            }
        }
    }

    return 0;
}
