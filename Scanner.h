#pragma once
#include "Token.h"
#include <fstream>
#include <vector>
#include <cctype>

#ifndef SCANNER_H
#define SCANNER_H

class Scanner {
private:
   vector<Token> tList;
    int line;
public:
    Scanner() : line(1) {};
    ~Scanner() = default;

    void Scan(ifstream& in)                            // Scan and tokenize
    {
        int end = 0;
        int lineHold = line;                           // Keeps track of line where string/comments begin
        while(end == 0)
        {
            char c = in.get();
            string s;                                  // Reinitialize string used for token value
            if (c == '\n') { line++; continue; }
            else if (isspace(c)) { continue; }
            else if (c != EOF)
            {
                lineHold = line;
                TokenType a = Tokenize(c, in, s, end, line, lineHold);
                if (a != COMMENT) {
                    Token newToken(a, s, lineHold);
                    tList.push_back(newToken);
                }
            }
            if(in.peek() == -1) { end = 1;}
        }
        Token newToken(END, "", line);
        tList.push_back(newToken);
    }

   static TokenType Tokenize(char c, ifstream& in, string& s, int& end, int& line, int& lineHold)
   {
        s += c;
        if (isalpha(c))                                // If character is a letter
        {
            s = "";
            while (isalpha(in.peek()) || isdigit(in.peek())) {s += c; c = in.get(); }
            s += c;
            if (s == "Schemes") { return SCHEMES; }
            else if (s == "Facts") { return FACTS; }
            else if (s == "Rules") { return RULES; }
            else if (s == "Queries") { return QUERIES; }
            else { return ID; }
        }

        else if (c == '\'')                           // If character begins a string
        {
            lineHold = line;
            while (true)
            {
                c = in.get();
                if (c == '\n') { s += c; line++; }
                else if (c == '\'' && in.peek() != '\'') {s += c; return STRING;}
                else if (c == '\'' && in.peek() == '\'') { s += c; s += in.get(); }
                else if (in.peek() == -1) { if (c != EOF) {s += c;} return UNDEFINED; }
                else {s += c;}
            }
        }

        else if (c == ':')                           // Options for colon or colon_dash
        {
            if (in.peek() == '-') { s += in.get(); return COLON_DASH; }
            else { return COLON; }
        }

        else if (c == '#')                           // If character begins a comment
        {
            s = "";
            lineHold = line;
            if (in.peek() == '|')                    // For multiple line comment
            {
                s += c;
                c = in.get();
                int close = 0;                        // int to exit while loop if multi-line comment is closed
                while (close == 0)
                {
                    if (in.peek() == '\n') { line++; }
                    s += c;
                    c = in.get();
                    if (c == '|')
                    {
                        if (in.peek() == '#') { s += c; s += in.get(); close = 1; }
                        else { continue; }
                    }
                    if (in.peek() == -1) { if (c != EOF) {s += c;} return UNDEFINED;}
                }
                return COMMENT;
            }
            else                                      // For single line comment
            {
                s += c;
                c = in.get();
                while (in.peek() != '\n' /*&& in.peek() != '#'*/)
                {
                    if (in.peek() == -1) { return UNDEFINED; }
                    s += c;
                    c = in.get();
                }
                s += c;
                if (in.peek() == '#') { s += in.get(); }
                return COMMENT;
            }
        }
        else if (isdigit(c)) { return UNDEFINED; }   // If character is digit
        else {                                       // The rest of the single character punctuation options...
            switch(c) {
                case ',': return COMMA;
                case '.': return PERIOD;
                case '?': return Q_MARK;
                case '(': return LPAREN;
                case ')': return RPAREN;
                case '*': return MULTIPLY;
                case '+': return ADD;
                default : return UNDEFINED;
            }
        }
    }

    vector<Token> sendList() { return tList; }

    string toString()                                       // Print function
    {
        ostringstream os;
        for (unsigned int i = 0; i < tList.size(); ++i)
        {
            os << tList.at(i) << endl;
        }
        os << "Total Tokens = " << tList.size() << endl;
        return os.str();
    }

    friend ostream& operator <<(ostream& os, Scanner& obj) // Able to print token vector elements with << operator
    {
        os << obj.toString();
        return os;
    }
};
#endif //UNTITLED_SCANNER_H
