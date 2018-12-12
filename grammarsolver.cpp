/*
 * File: grammarsolver.cpp
 * --------------------------
 * Name:Haseeb Khan
 * Section leader: Chriss Gregg
 * This file contains grammar generating code for CS106B.
 *
 * EXTRA FEATURE: Robust grammar solver (in buildBNF function)
 */

#include "grammarsolver.h"
#include "map.h"
#include "random.h"
#include "vector.h"
#include "strlib.h"
#include <string>

using namespace std;

// Type defines
typedef Vector<string> Rule;
typedef Vector<Rule> Rules;
typedef Map<string, Rules> BNF;

// build bnf map from input file
void buildBNF(istream& input, BNF& bnf)
{
    string line;
    while (getline(input, line))
    {
        // split non-terminal and rules
        // v[0] - non-terrminal
        // v[1] - rules
        Vector<string> v = stringSplit(line, "::=");
        if (v.size() == 2)
        {
            v[0] = trim(v[0]);
            v[1] = trim(v[1]);
            // split rules string to separate rules
            Vector<string> rulesStr = stringSplit(v[1], "|");
            Vector<Rule> rules;
            for (auto &ruleStr: rulesStr)
            {
                ruleStr = trim(ruleStr);
                // split rule to separate symbols
                Rule rule =  stringSplit(ruleStr, " ");
                for (auto &symbol: rule)
                    symbol = trim(symbol);

                rules.add(rule);
            }

            if (bnf.containsKey(v[0])) // check duplicates
            {
                string error = string("buildBNF() error: duplicate symbol ") + v[0] + ".";
                throw error;
            }

            bnf.add(v[0], rules); // add rules to bnf map
        }
    }
}

// recursive function to generate random grammar
string generate(const BNF& bnf, string symbol)
{    
    // split symbol string to separate symbols
    Vector<string> symbols = stringSplit(symbol, " ");

    // if symbol is terminal
    if (symbols.size() == 1 && !bnf.containsKey(symbol))
        return symbol;

    string grammar;
    for (const auto &s: symbols)
    {
        // get rules for current symbol
        Rules rules = bnf.get(s);
        // choose random rule
        Rule randomRule = rules[randomInteger(0, rules.size() - 1)];

        // recursively generate grammar string
        for (auto &r: randomRule)
        {            
            string s = generate(bnf, r);
            s = trim(s);
            grammar += s + " ";
        }
    }

    return trim(grammar);
}

/**
 * Generates grammar for a given symbol a certain number of times given
 * a BNF input file.
 *
 * This will be called by grammarmain.cpp.
 *
 * @param input - Input stream of BNF file.
 * @param symbol - Symbol to generate
 * @param times - Number of times grammar is generated
 * @return Vector of strings of size times with random generations of symbol
 */
Vector<string> grammarGenerate(istream& input, string symbol, int times)
{
    if (symbol.empty())
        throw string("grammarGenerate() error: no symbol.");

    if (times <= 0)
        return {};

    BNF bnf;

    try
    {
        buildBNF(input, bnf); // throws exceptions
    }
    catch(string &e) // to rethrow an exception to the calling function
    {
        throw e;
    }

    Vector<string> v(times);
    for (int i = 0; i < times; ++i)
    {        
        // generate grammar 'times' times
        v[i] =  generate(bnf, symbol);
    }

    return v;
}
