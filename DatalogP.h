#pragma once

#include <set>
#include <iterator>

class Parameter
{
private:
    string name;
    string expression = "";
    bool constant = false;
public:
    explicit Parameter(string& n) : name(n) {} ;
    ~Parameter() = default;

    bool isConstant() { return constant; }
    void setConstant() { constant = true; }
    void buildExp(string& s) { expression += s; }
    void clearExp() { expression = ""; }
    bool isEmpty() { return expression.empty(); }
    string getName() { return name; }

    string toString() {
        if (!expression.empty()) { return expression; }
        else { return name; }
    }

    friend ostream& operator<<(ostream& os, Parameter& obj) { os << obj.toString(); return os; }
};

class Predicate
{
private:
    string name;
    vector<Parameter> parameter;
public:
    explicit Predicate(string& n) : name(n) {};
    ~Predicate() = default;

    void push_back(Parameter& obj) { parameter.push_back(obj); }

    string getName() { return name; }
    int numParameters() { return parameter.size(); }
    vector<Parameter> getParameters() { return parameter; }

    vector<string> getParameterNames() {
        vector<string> names;
        for (auto & i : parameter) {
            names.push_back(i.getName());
        }
        return names;
    }

    string toString() {
        ostringstream os;
        os << name << "(";
        for (unsigned int i = 0; i < parameter.size(); i++) {
            os << parameter.at(i);
            if (i != parameter.size() - 1) { os << ","; } else { os << ")"; }
        }
        return os.str();
    }

    friend ostream& operator<<(ostream& os, Predicate& obj) { os << obj.toString(); return os; }
};

class Rule
{
private:
    Predicate head;
    vector<Predicate> list;
public:
    explicit Rule(string& n) : head(n) {};
    ~Rule() = default;

    void pushHead(Predicate& obj) { head = obj; }
    void pushPred(Predicate& obj) { list.push_back(obj); }
    vector<Predicate> getList() { return list; }
    void clearList() { vector<Predicate> emptyList; list = emptyList; }
    Predicate returnHead() { return head; }

    string toString() {
        ostringstream os;
        os << head << " :- ";
        for (unsigned int i = 0; i < list.size(); i++) {
            os << list.at(i);
            if (i != list.size() - 1) { os << ","; } else { os << "."; }
        }
        return os.str();
    }

    friend ostream& operator<<(ostream& os, Rule& obj) { os << obj.toString(); return os; }
};

class DatalogProgram
{
private:
    vector<Predicate> schemes;
    vector<Predicate> facts;
    vector<Rule> rules;
    vector<Predicate> queries;
    set<string> domain;
public:
    DatalogProgram() = default;
    ~DatalogProgram() = default;

    vector<Predicate> getSchemes() { return schemes; }
    vector<Predicate> getFacts() { return facts; }
    vector<Rule> getRules() { return rules; }
    vector<Predicate> getQueries() { return queries; }

    void pushScheme(Predicate& obj) { schemes.push_back(obj); }
    void pushFact(Predicate& obj) { facts.push_back(obj); }
    void pushRule(Rule& obj) { rules.push_back(obj); }
    void pushQuery(Predicate& obj) { queries.push_back(obj); }

    void makeDomain() {
        vector<string> stringVec;
        for (auto & fact : facts) {
            stringVec = fact.getParameterNames();
            for (const auto & j : stringVec) {
                domain.insert(j);
            }
        }
    }

    string toString() {
        ostringstream os;

        os << "Schemes(" << schemes.size() << "):" << endl;
        for (auto & scheme : schemes) { os << "  " << scheme << endl; }

        os << "Facts(" << facts.size() << "):" << endl;
        for (auto & fact : facts) { os << "  " << fact << "." << endl; }

        os << "Rules(" << rules.size() << "):" << endl;
        for (auto & rule : rules) { os << "  " << rule << endl; }

        os << "Queries(" << queries.size() << "):" << endl;
        for (auto & query : queries) { os << "  " << query << "?" << endl; }

        os << "Domain(" << domain.size() << "):" << endl;
        auto itr = domain.begin();
        unsigned int k = 0;
        for (; itr != domain.end(); itr++) {
            os << "  " << *itr;
            k++;
            if (k < domain.size()) { os << endl; }
        }
        return os.str();
    }

    friend ostream& operator<<(ostream& os, DatalogProgram& obj) { os << obj.toString(); return os; }
};


