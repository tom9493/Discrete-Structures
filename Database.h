#pragma once

using namespace std;

class Tuple : public vector<string>
{
private:
public:
    Tuple() = default;
    ~Tuple() = default;

    void insertVals(const vector<string>& v) { for (const auto & i : v) { this->push_back(i); } }
    string toString() {
        ostringstream os;
        for (unsigned int i = 0; i < this->size(); i++) { os << this->at(i); }
        return os.str();
    }

    friend ostream& operator<<(ostream& os, Tuple& obj) {
        os << obj.toString();
        return os;
    }
};

class Scheme
{
private:
    string name = "none";
    vector<string> schemeNames;
public:
    Scheme() = default;
    ~Scheme() = default;

    void insertName(string n) { name = std::move(n); }
    void insertSchemeNames(vector<string> nameList) { schemeNames = move(nameList); }
    void insertSchemeName(string s) { schemeNames.push_back(s); }
    string returnSchemeName(int i) { return schemeNames.at(i); }
    vector<string> returnSchemeNames() { return schemeNames; }
    string returnName() { return name; }

    string toString() {
        ostringstream os;
        os << name << "(";
        for (unsigned int i = 0; i < schemeNames.size(); i++) {
            if (i != schemeNames.size() - 1) { os << schemeNames.at(i) << ","; }
            else { os << schemeNames.at(i) << ")"; }
        }
        return os.str();
    }

    friend ostream& operator <<(ostream& os, Scheme& obj) {
        os << obj.toString();
        return os;
    }
};

class Relation
{
private:
    Scheme scheme;
    set<Tuple> tuple;
public:
    Relation() = default;
    ~Relation() = default;

    void insertScheme(Scheme s) { scheme = s; }
    void insertTuple(Tuple& t) { tuple.insert(t); }
    void insertTupleSet(set<Tuple> t) { tuple = t; }
    Scheme getScheme() { return scheme; }
    string returnName() { return scheme.returnName(); }
    set<Tuple> getTupleSet() { return tuple; }
    int getNumTuples() { return tuple.size(); }

    string toString2() {
        ostringstream os;
        for (Tuple i : tuple) { os << i << endl;}
        return os.str();
    }

    string toString() {
        ostringstream os;
        os << scheme << endl;
        for (Tuple i : tuple) { os << i << endl;}
        return os.str();
    }

    friend ostream& operator<<(ostream& os, Relation& obj) {
        os << obj.toString();
        return os;
    }
};

class Database
{
private:
    vector<string> relationNames;
    map<string, Relation> rMap;
public:
    Database() = default;
    ~Database() = default;

    void insertSchemes(vector<Predicate> schemes) {
        Scheme scheme;
        Relation relation;
        int nameCheck = 0;
        for (auto & i : schemes) {
            scheme.insertName(i.getName());
            scheme.insertSchemeNames(i.getParameterNames());
            relation.insertScheme(scheme);
            for (const auto & relationName : relationNames) {
                if (i.getName() == relationName) { nameCheck = 1; }
            }
            if (nameCheck == 0) { relationNames.push_back(i.getName()); }
            rMap[relation.returnName()] = relation;
        }
    }

    void insertTuples(vector<Predicate> facts) {
        for (auto & fact : facts) {
            Tuple tuple;
            tuple.insertVals(fact.getParameterNames());
            rMap[fact.getName()].insertTuple(tuple);
        }
    }

    void evalRules(vector<Rule> rules) {
        cout << "Rule Evaluation" << endl;
        int check = 0;
        int iterations = 0;
        while (check == 0) {
            // Finds original number of tuples in database. Will exit while loop if it does change after evaluating rules
            int tupleStart = getNumTuples();
            // Evaluate rules one at a time
            for (unsigned int i = 0; i < rules.size(); ++i) {
                cout << rules.at(i) << endl;
                int rStartSize = rMap[rules.at(i).returnHead().getName()].getNumTuples();
                vector<Predicate> rhPredicates = rules.at(i).getList();
                vector<Relation> rhRelations;
                // If the rule has more than 1 predicate on the right side, evaluate all and then join them all
                for (unsigned int j = 0; j < rhPredicates.size(); j++) {
                    Relation r = rMap[rhPredicates.at(j).getName()];
                    // Map that tells which variable appears in the rhPredicate parameter
                    map<string, int> valMap;
                    string schemeName = rhPredicates.at(j).getName();
                    vector<int> positions;
                    vector<Parameter> parameters = rhPredicates.at(j).getParameters();
                    // Each parameter is tested.. constant or variable
                    for (unsigned int k = 0; k < rhPredicates.at(j).numParameters(); k++) {
                        if (parameters.at(k).isConstant()) {
                            // If constant, check relation with same name and select tuples that match
                            r = type1Select(k, parameters.at(k).getName(), r);
                        } else {
                            // If variable, put in valMap or check if it is already there
                            if (valMap.find(parameters.at(k).getName()) == valMap.end()) {
                                // If it isn't, put it in
                                valMap[parameters.at(k).getName()] = k;
                                positions.push_back(k);
                            } else {
                                // If it is there, select tuples where values match in the given columns
                                r = type2Select(valMap[parameters.at(j).getName()], k, r);
                            }
                        }
                    }
                    r = project(positions, r);
                    r = rename(rhPredicates.at(j), r);
                    rhRelations.push_back(r);
                }
                // This relation is the first relation made from the right hand predicates. If there is only one, it
                // will just remain as this. If there are more, they will be joined together into one
                Relation r = rhRelations.at(0);
                // If there was more than one right hand predicate in the rule, natural join all of them
                if (rhPredicates.size() > 1) {
                    for (unsigned int l = 0; l < rhRelations.size() - 1; ++l) {
                        r = naturalJoin(r, rhRelations.at(l + 1));
                    }
                }
                // Now project columns that appear in the head predicate
                Predicate head = rules.at(i).returnHead();
                vector<string> headNames = head.getParameterNames();
                vector<string> schemeNames = r.getScheme().returnSchemeNames();
                vector<int> columns;
                map<string, int> inColumn;
                for (unsigned int j = 0; j < head.numParameters(); j++) {
                    for (unsigned int k = 0; k < schemeNames.size(); k++) {
                        if (headNames.at(j) == schemeNames.at(k) && inColumn.find(schemeNames.at(k)) == inColumn.end()) {
                            inColumn[schemeNames.at(k)] = k;
                            columns.push_back(inColumn[schemeNames.at(k)]);
                        }
                    }
                }
                r = project(columns, r);
                // Rename relation to make it Union compatible with
                r = renameRule(rMap[head.getName()].getScheme(), r);
                set<Tuple> tuples = r.getTupleSet();
                schemeNames = r.getScheme().returnSchemeNames();
                rMap[r.returnName()] = relationUnion(r, rMap[r.returnName()]);
            }
            ++iterations;
            if (getNumTuples() == tupleStart) { check = 1; }
        }
        cout << endl << "Schemes populated after " << iterations << " passes through the Rules." << endl;
    }

    Relation renameRule(const Scheme& s, Relation& r) { r.insertScheme(s); return r; }

    Relation relationUnion(Relation r, Relation& dataBaseRelation) {
        vector<string> schemeNames = r.getScheme().returnSchemeNames();
        set<Tuple> tuples = r.getTupleSet();
        set<Tuple> dbTuples = dataBaseRelation.getTupleSet();
        for (Tuple i : tuples) {
            if (dbTuples.find(i) == dbTuples.end()) {
                dataBaseRelation.insertTuple(i);
                if (!i.empty()) { cout << "  "; }
                for (unsigned int l = 0; l < schemeNames.size(); l++) {
                    cout << schemeNames.at(l) << "=" << i.at(l);
                    if (l != schemeNames.size() - 1) { cout << ", "; }
                }
                cout << endl;
            }
        }
        return dataBaseRelation;
    }

    Relation naturalJoin(Relation rOne, Relation rTwo) {
        Relation newRelation;
        Scheme newScheme;
        Scheme rOneScheme = rOne.getScheme();
        Scheme rTwoScheme = rTwo.getScheme();
        set<Tuple> tuples1 = rOne.getTupleSet();
        set<Tuple> tuples2 = rTwo.getTupleSet();
        vector<int> positionList;
        // Check for matching schemes in the relation
        /*The first for loop represents the scheme names (parameters) in the first relation and the second for loop
          represents the schemes names in the second relation. It shuffles through all of them to find matches. The
          vector represents which positions in the first relation match which positions in the second. The for loop
          after will use the even numbers as the first relation tuples and even as second relation tuples*/
        for (unsigned int i = 0; i < rOneScheme.returnSchemeNames().size(); i++) {
            for (unsigned int j = 0; j < rTwoScheme.returnSchemeNames().size(); j++) {
                if (rOneScheme.returnSchemeNames().at(i) == rTwoScheme.returnSchemeNames().at(j)) {
                    positionList.push_back(i); positionList.push_back(j);
                }
            }
        }
        newScheme = combineSchemes(positionList, rOneScheme, rTwoScheme);
        newRelation.insertScheme(newScheme);
        /* After the positions have been found, shuffle through similarly with all the tuples in the first relation, and
          if each of the matching scheme positions also match with the tuple values in each relation, combine those
          tuples. How do you combine tuples??? */
        for (Tuple i : tuples1) {
            for (Tuple j : tuples2) {
                if (!positionList.empty()) {
                    bool same = true;
                    for (unsigned int k = 0; k < positionList.size(); k = k + 2) {
                        /* If the tuple value in rOne at position k does not match the tuple value in rTwo at position k+1,
                         the tuples cannot be combined */
                        if (i.at(positionList.at(k)) != j.at(positionList.at(k + 1))) { same = false; }
                    }
                    if (same) {
                        Tuple newTuple = combineTuples(positionList, i, j);
                        newRelation.insertTuple(newTuple);
                    }
                }
                /* If positionList is empty, there are no matches in the scheme, so everything can be combined*/
                else {
                    Tuple newTuple = combineTuples(positionList, i, j);
                    newRelation.insertTuple(newTuple);
                }
            }
        }
        return newRelation;
    }

    Scheme combineSchemes(const vector<int>& positionList, Scheme scheme1, Scheme scheme2) {
        Scheme s;
        vector<string> scheme1Names = scheme1.returnSchemeNames();
        vector<string> scheme2Names = scheme2.returnSchemeNames();
        // Insert all scheme parameters from the first scheme
        for (unsigned int k = 0; k < scheme1Names.size(); ++k) { s.insertSchemeName(scheme1Names.at(k)); }
        /*If positionList is not empty, only insert schemes not already included from first relation scheme. Else,
          insert all schemes names from scheme 2 into the new scheme*/
        for (unsigned int k = 0; k < scheme2Names.size(); ++k) {
            if (!positionList.empty()) {
                for (unsigned int l = 0; l < positionList.size(); l = l + 2) {
                    if (k != positionList.at(l + 1)) { s.insertSchemeName(scheme2Names.at(k)); }
                }
            }
            else { s.insertSchemeName(scheme2Names.at(k)); }
        }
        return s;
    }

    Tuple combineTuples(const vector<int>& positionList, Tuple i, Tuple j) {
        Tuple t;
        // Push back all values of first tuple
        for (unsigned int k = 0; k < i.size(); k++) { t.push_back(i.at(k)); }
        /* Push back only the values that have not been included. These are the values that are not included as the
         values in the odd positions in positionList. If positionList is empty, push back all values.*/
        if (!positionList.empty()) {
            for (unsigned int k = 0; k < j.size(); k++) {
                for (unsigned int l = 0; l < positionList.size(); l = l + 2) {
                    if (k != positionList.at(l + 1)) { t.push_back(j.at(k)); }
                }
            }
        }
        else { for (unsigned int k = 0; k < j.size(); k++) { t.push_back(j.at(k)); } }
        return t;
    }

    int getNumTuples() {
        int numTuples = 0;
        for (unsigned int i = 0; i < relationNames.size(); i++) {
            numTuples = numTuples + rMap[relationNames.at(i)].getTupleSet().size();
        }
        return numTuples;
    }

    void evalQueries(vector<Predicate> queries) {
        cout << endl << "Query Evaluation" << endl;
        // Evaluate one query at a time
        for (unsigned int i = 0; i < queries.size(); i++) {
            Relation r = rMap[queries.at(i).getName()];
            // Map that tells which variable appears in the query parameter
            map<string, int> valMap;
            string schemeName = queries.at(i).getName();
            cout << queries.at(i) << "? ";
            vector<int> positions;
            vector<Parameter> parameters = queries.at(i).getParameters();
            // Each parameter is tested.. constant or variable
            for (unsigned int j = 0; j < queries.at(i).numParameters(); j++) {
                if (parameters.at(j).isConstant()) {
                    // If constant, check relation with same name and select tuples that match
                    r = type1Select(j, parameters.at(j).getName(), r);
                }
                else {
                    // If variable, put in valMap or check if it is already there
                    if (valMap.find(parameters.at(j).getName()) == valMap.end()) {
                        // If it isn't, put it in
                        valMap[parameters.at(j).getName()] = j;
                        positions.push_back(j);
                    }
                    else {
                        // If it is, select tuples where values match in the given columns
                        r = type2Select(valMap[parameters.at(j).getName()], j, r);
                    }
                }
            }
            r = project(positions, r);
            r = rename(queries.at(i), r);
            // Output of queries here
            if (r.getNumTuples() == 0) { cout << "No"; if (i < queries.size() - 1)  { cout << endl; } }
            else {
                cout << "Yes(" << r.getNumTuples() << ")";
                set<Tuple> tuples = r.getTupleSet();
                vector<string> schemeNames = r.getScheme().returnSchemeNames();
                if (!schemeNames.empty() || i != queries.size() - 1) { cout << endl;}
                unsigned int tupleCount = 0;
                for (Tuple k : tuples) {
                    if (!k.empty()) { cout << "  "; }
                    for (unsigned int l = 0; l < schemeNames.size(); l++) {
                        cout << schemeNames.at(l) << "=" << k.at(l);
                        if (l != schemeNames.size() - 1) { cout << ", ";}
                    }
                    ++tupleCount;
                    if (!k.empty() && (tupleCount != tuples.size() || i != queries.size() -1)) { cout << endl; }
                }
            }
        }
    }

    Relation type1Select(int& position, const string& val, Relation r) {
        Relation newRelation;
        newRelation.insertScheme(r.getScheme());
        set<Tuple> tuples = r.getTupleSet();
        for (Tuple i : tuples) { if (i.at(position) == val) { newRelation.insertTuple(i); } }
        return newRelation;
    }

    Relation type2Select(int one, int two, Relation r) {
        Relation newRelation;
        newRelation.insertScheme(r.getScheme());
        set<Tuple> tuples = r.getTupleSet();
        for (Tuple i : tuples) { if (i.at(one) == i.at(two)) { newRelation.insertTuple(i); } }
        return newRelation;
    }

    // Project changes relation r into relation with different columns. Gets rid of duplicate columns
    Relation project(vector<int> columns, Relation r) {
        Relation newRelation;
        Scheme scheme;
        // Inserts name of scheme (the identity before the parantheses)
        scheme.insertName(r.getScheme().returnName());
        set<Tuple> tuples = r.getTupleSet();
        for (unsigned int i = 0; i < columns.size(); i++) {
            // Inserts a parameter (scheme name) into the scheme for however many columns there are
            scheme.insertSchemeName(r.getScheme().returnSchemeName(columns.at(i)));
        }
        for (Tuple i : tuples) {
            Tuple newTuple;
            // This for loop inserts tuples from original relation r, but excludes column numbers not included
            // in the column vector. Column vector made previously by find duplicate scheme names
            for (int column : columns) { newTuple.push_back(i.at(column)); }
            newRelation.insertScheme(scheme);
            newRelation.insertTuple(newTuple);
        }
        return newRelation;
    }

    // Rename changes name of each parameter in r scheme to match given predicate scheme
    Relation rename(Predicate pred, Relation r) {
        Relation newRelation;
        Scheme scheme;
        scheme.insertName(r.getScheme().returnName());
        vector<Parameter> pParas = pred.getParameters();
        // Predicate map that matches predicate name with parameter
        map<string, string> pMap;
        for (unsigned int i = 0; i < pParas.size(); i++) {
            // If parameter is not constant and is not in the map
            if (!pParas.at(i).isConstant() && pMap.find(pParas.at(i).getName()) == pMap.end()) {
                scheme.insertSchemeName(pParas.at(i).getName());
                pMap[pParas.at(i).getName()] = "here";
            }
        }
        newRelation.insertScheme(scheme);
        if (r.getNumTuples() != 0) { newRelation.insertTupleSet(r.getTupleSet()); }
        return newRelation;
    }

    string toString() {
        ostringstream os;
        for (const auto & relationName : relationNames) { cout << rMap[relationName] << endl; }
        return os.str();
    }

    friend ostream& operator<<(ostream& os, Database& obj) {
        os << obj.toString();
        return os;
    }
};
