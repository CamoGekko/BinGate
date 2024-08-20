#include <iostream>
#include <vector>
#include <string>


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

bool eval(std::vector<std::string> tks) {
    if (tks[0] == "AND") {
        return tks[1] == tks[2];
    }

    if (tks[0] == "OR") {
        return tks[1] == "1" || tks[2] == "1";
    }

    if (tks[0] == "XOR") {
        return (tks[1] == "1" && tks[2] == "0") || (tks[1] == "0" && tks[2] == "1");
    }

    if (tks[0] == "NOT") {
        return tks[1] != tks[2];
    }

    if (tks[0] == "NAND") {
        return !(tks[1] == "1" && tks[2] == "1");
    }

    if (tks[0] == "NOR") {
        return !(tks[1] == "1" || tks[2] == "1");
    }

    if (tks[0] == "XNOR") {
        return (tks[1] == "1" && tks[2] == "1") || (tks[1] == "0" && tks[2] == "0");
    }

    return NULL; // Default case
}

std::string lexise(std::vector<std::string> vec){
    std::string code;
    std::vector<std::string> vecEval;
    for(int i = 0; i < vec.size(); i++){

        if(vec[i] == "VAR"){
            code += std::format("bool {} = {};", vec[i + 1], vec[i + 2]);
            i += 2;
        }

        if(vec[i] == "("){
            vecEval = vec[i] + " " + vec[i + 1] + " " + vec[i + 2];
            bool eval = eval(vecEval);
            if(eval != NULL){
                vec[i + 2] = "";
                vec[i + 1] = "";
                vec[i] = eval;
            }
            
        }

        if(vec[i] == "AND"){
            code += std::format("if({} == {}){return 1;} else {return 0;};", vec[i + 1], vec[i + 2]);
            i += 2;
        }

        if(vec[i] == "OR"){
            code += std::format("if({} || {}){return 1;} else {return 0;};", vec[i + 1], vec[i + 2]);
            i += 2;
        }        

        if(vec[i] == "XOR"){
            code += std::format("if(({} && !{}) || (!{} && {})){return 1;} else {return 0;};", vec[i + 1], vec[i + 2], vec[i + 1], vec[i + 2]);
            i += 2;
        }  

        if(vec[i] == "NOT"){
            code += std::format("if({} != {}){return 1;} else {return 0;};", vec[i + 1], vec[i + 2]);
            i += 2;
        }   
        
        if(vec[i] == "NAND"){
            code += std::format("if(!({} && {})) {{ return 1; }} else {{ return 0; }};", vec[i + 1], vec[i + 2]);
            i += 2;
        }    

        if (vec[i] == "XNOR") {
            code += std::format("if(({} && {}) || (!{} && !{})) {{ return 1; }} else {{ return 0; }};", vec[1], vec[2], vec[1], vec[2]);
            i += 2;
        }   

        if (vec[i] == "XNOR") {
            code += std::format("if(({} && {}) || (!{} && !{})) {{ return 1; }} else {{ return 0; }};", vec[1], vec[2], vec[1], vec[2]);
            i += 2;
        }                   
    }

    return code;
}

int main(int argc, char** argv) {
    std::string input = "XOR 1 0";
    char delimiter = ' '; // The delimiter for tokenization

    // Tokenize the input string
    std::vector<std::string> tokens = tokenize(input, delimiter);
    lexise(tokens);

    // Print the tokens
    for (const std::string& token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
