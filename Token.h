#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <map>

#ifndef TOKEN_H
#define TOKEN_H

using namespace std;

enum TokenType {COMMA, PERIOD, Q_MARK, LPAREN, RPAREN, COLON, COLON_DASH, MULTIPLY,
        ADD, SCHEMES, FACTS, RULES, QUERIES, ID, STRING, COMMENT, UNDEFINED, END};

class Token : public exception {
private:
    TokenType t;
    string value;
    int lineNum;

public:
    Token() : t(UNDEFINED), value(nullptr), lineNum(0) {} ;
    Token(TokenType token, string v, int l) : t(token), value(std::move(v)), lineNum(l) {};
    ~Token() override = default;

    string toString()
    {
        ostringstream os;
        map<TokenType, string> tMap;          // Map to print appropriate enum values

        tMap[COMMA] = "COMMA";
        tMap[PERIOD] = "PERIOD";
        tMap[Q_MARK] = "Q_MARK";
        tMap[LPAREN] = "LEFT_PAREN";
        tMap[RPAREN] = "RIGHT_PAREN";
        tMap[COLON] = "COLON";
        tMap[COLON_DASH] = "COLON_DASH";
        tMap[MULTIPLY] = "MULTIPLY";
        tMap[ADD] = "ADD";
        tMap[SCHEMES] = "SCHEMES";
        tMap[FACTS] = "FACTS";
        tMap[RULES] = "RULES";
        tMap[QUERIES] = "QUERIES";
        tMap[ID] = "ID";
        tMap[STRING] = "STRING";
        tMap[COMMENT] = "COMMENT";
        tMap[UNDEFINED] = "UNDEFINED";
        tMap[END] = "EOF";

        os << "(" << tMap[t] << ",\"" << value << "\"," << lineNum << ")";
        return os.str();
    }

    TokenType giveType() { return t; }
    string giveVal() { return value; }

    friend ostream& operator<< (ostream& os, Token& obj)
    {
        os << obj.toString();
        return os;
    }

};

#endif //TOKEN_H
