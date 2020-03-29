#pragma once
#include <utility>

#include "Scanner.h"
#include "DatalogP.h"

class Parser {
private:
    Token token;
    Predicate pred;
    Parameter para;
    Rule rul;
    DatalogProgram dataLP;
    vector<Token> tList;
    int i = 0;
    int inExp = 0;
    int inRule = 0;
public:
    Parser(string& n, string& m, string& r) : token(UNDEFINED, "", 0), pred(n), para(m), rul(r) {};
    ~Parser() = default;

    void makeList(vector<Token> list) { tList = std::move(list); }

    DatalogProgram getDLP() { return dataLP; }

    void parse(int& errorCheck) {
        try {
            token = getToken();
            S();
        } catch (Token &t) { if (t.giveType() != END) { cout << "Failure!" <<
        endl << t; errorCheck = 1; return; } }
        //cout << "Success!" << endl;
    }

    void S()
    {
        while (token.giveType() != END) {
            match(SCHEMES);
            match(COLON);
            schemeList();
            match(FACTS);
            match(COLON);
            if (token.giveType() != RULES) {
                factList();
                dataLP.makeDomain();
            }
            match(RULES);
            match(COLON);
            if (token.giveType() != QUERIES) { ruleList(); }
            match(QUERIES);
            match(COLON);
            query();
            queryList();
        }
    }

    void schemeList() {
        scheme();
        if (token.giveType() == ID) { schemeList(); }
    }

    void scheme() {
        string name = token.giveVal();          // ID at start of predicate
        pred = Predicate(name);             // assigned to predicate object
        match(ID);
        match(LPAREN);
        string exp = token.giveVal(); paraToPred(exp, token.giveType());
        match(ID);
        if (token.giveType() != RPAREN) { idList(); }
        match(RPAREN);
        dataLP.pushScheme(pred);
    }

    void idList() {
        match(COMMA);
        string exp = token.giveVal(); paraToPred(exp, token.giveType());
        match(ID);
        if (token.giveType() == COMMA) { idList(); }
    }

    void factList() {
        fact();
        if (token.giveType() == ID) { factList(); }
    }

    void fact() {
        string name = token.giveVal();
        pred = Predicate(name);
        match(ID);
        match(LPAREN);
        string exp = token.giveVal(); paraToPred(exp, token.giveType());
        match(STRING);
        if (token.giveType() != RPAREN) { stringList(); }
        match(RPAREN);
        match(PERIOD);
        dataLP.pushFact(pred);
    }

    void stringList() {
        match(COMMA);
        string exp = token.giveVal(); paraToPred(exp, token.giveType());
        match(STRING);
        if (token.giveType() == COMMA) { stringList(); }
    }

    void ruleList() {
        rul.clearList();
        rule();
        if (token.giveType() == ID) { ruleList(); }
    }

    void rule() {
        inRule = 1;
        para.clearExp();
        headPredicate();
        match(COLON_DASH);
        predicate();
        if (token.giveType() == COMMA) { predicateList(); }
        match(PERIOD);
        dataLP.pushRule(rul);
        inRule = 0;
    }

    void headPredicate() {
        string name = token.giveVal();
        pred = Predicate(name);
        match(ID);
        match(LPAREN);
        string exp = token.giveVal(); paraToPred(exp, token.giveType());
        match(ID);
        if (token.giveType() != RPAREN) { idList(); }
        match(RPAREN);
        rul.pushHead(pred);
    }

    void predicate() {
        string name = token.giveVal();
        pred = Predicate(name);
        match(ID);
        match(LPAREN);
        parameter();
        if (token.giveType() != RPAREN) { parameterList(); }
        if (!para.isEmpty()) { pred.push_back(para); para.clearExp(); }
        match(RPAREN);
        if (inRule == 1) { rul.pushPred(pred); }
        else { dataLP.pushQuery(pred); }
    }

    void predicateList() {
        match(COMMA);
        predicate();
        if (token.giveType() == COMMA) { predicateList(); }
    }

    void parameter() {
        if (token.giveType() == ID && inExp == 0) {
            string exp = token.giveVal(); paraToPred(exp, token.giveType());
            match(ID);
        }
        else if (token.giveType() == STRING && inExp == 0) {
            string exp = token.giveVal(); paraToPred(exp, token.giveType());
            match(STRING);
        }
        else if (token.giveType() == ID && inExp == 1) {
            string exp = token.giveVal(); para.buildExp(exp);
            match(ID);
        }
        else if (token.giveType() == STRING && inExp == 1) {
            string exp = token.giveVal(); para.buildExp(exp);
            match(STRING);
        }
        else { expression(); }

    }

    void parameterList() {
        if (!para.isEmpty()) { pred.push_back(para); para.clearExp(); inExp = 0;}
        match(COMMA);
        parameter();
        if (token.giveType() == COMMA) { parameterList(); }
    }

    void expression() {
        inExp = 1;
        string val = token.giveVal(); para.buildExp(val);
        match(LPAREN);
        parameter();
        tOperator();
        parameter();
        val = token.giveVal(); para.buildExp(val);
        match(RPAREN);
    }

    void tOperator() {
        if (token.giveType() == ADD) {
            string val = token.giveVal(); para.buildExp(val);
            match(ADD);
        }
        else if (token.giveType() == MULTIPLY) {
            string val = token.giveVal(); para.buildExp(val);
            match(MULTIPLY);
        }
        else { throw token; }
    }

    void query() {
        para.clearExp();
        predicate();
        match(Q_MARK);
    }

    void queryList() {
        query();
        if (token.giveType() == ID) { queryList(); }
    }

    void paraToPred(string& val, TokenType t) {
        para = Parameter(val);
        if (t == STRING) (para.setConstant());
        pred.push_back(para);
    }

    Token getToken() { return tList.at(i++); }

    void match(TokenType t) {
        if (token.giveType() == t) { token = getToken(); }
        else { throw token; }
    }

    string toString() {
        ostringstream os;
        for (auto & j : tList) { os << j << endl; }
        return os.str();
    }
};
