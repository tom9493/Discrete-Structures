#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "DatalogP.h"
#include "Database.h"

int main(int argc, char* argv[]) {

    ifstream in;
    string filename = argv[1];
    in.open(filename);

    Scanner scanner;                  // Create scanner object
    scanner.Scan(in);             // Function scans and tokenizes everything in input file

    string n = "hi";
    string m = "bye";
    string r = "yep";

    Parser parser(n, m, r);
    parser.makeList(scanner.sendList());
    int errorCheck = 0;
    parser.parse(errorCheck);

    DatalogProgram datalog;
    datalog = parser.getDLP();

    if (errorCheck != 0) { cout << "Parse error"; }

    else {
        Database database;
        database.insertSchemes(datalog.getSchemes());
        database.insertTuples(datalog.getFacts());
        database.evalRules(datalog.getRules());
        database.evalQueries(datalog.getQueries());
    }

    in.close();
    return 0;
}
