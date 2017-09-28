/* VerifyPN - TAPAAL Petri Net Engine
 * Copyright (C) 2014 Jiri Srba <srba.jiri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUERYXMLPARSER_H
#define	QUERYXMLPARSER_H

#include "xmlsp/xmlsp_dom_vector.h"
#include "PNMLParser.h"

#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <sstream> 

using namespace std;

extern std::vector<std::string>* placelistbound;

class QueryXMLParser {
public:    
	QueryXMLParser(const PNMLParser::TransitionEnablednessMap &transitionEnabledness);
 //      ~QueryXMLParser();
    
        struct QueryItem {
            string id; // query name
            string queryText; // only EF queries will be here
            bool negateResult; // true if the final result should be negated
            bool isPlaceBound; // true if the query is a place-bound one (returns integer)
            string placeNameForBound;
            enum { 
                PARSING_OK,
                UNSUPPORTED_QUERY,
            } parsingResult;
        };

        typedef vector<QueryItem> Queries;
        typedef Queries::iterator QueriesIterator;
        Queries queries;

	bool parse(const string& xml, int query);
        void printQueries();
        void printQueries(int i);
        
private:
        bool parsePropertySet(XMLSP::DOMElement* element, int query);
        bool parseProperty(XMLSP::DOMElement* element);
        bool parseTags(XMLSP::DOMElement* element);
        bool parseFormula(XMLSP::DOMElement* element, string &queryText, bool &negateResult, bool &isPlaceBound, string &placeNameForBound);
        bool parseBooleanFormula(XMLSP::DOMElement* element, string &queryText);
        bool parseIntegerExpression(XMLSP::DOMElement* element, string &queryText);
        string parsePlace(XMLSP::DOMElement* element);
        PNMLParser::TransitionEnablednessMap _transitionEnabledness;
};

#endif	/* QUERYXMLPARSER_H */

