/* 
 * File:   Reducer.h
 * Author: srba
 *
 * Created on 15 February 2014, 10:50
 */

#ifndef REDUCER_H
#define	REDUCER_H

#include "PetriNet.h"
#include "PQL/Contexts.h"
#include <PetriParse/PNMLParser.h>

namespace PetriEngine {

    class Reducer {
    public:
        Reducer(PetriNet *net);
        ~Reducer();
        void CreateInhibitorPlacesAndTransitions(PetriNet* net, PNMLParser::InhibitorArcList inhibarcs, MarkVal* placeInInhib, MarkVal* transitionsInInhib);
        void Print(PetriNet* net, MarkVal* m0, MarkVal* placeInQuery, MarkVal* placeInInhib, MarkVal* transitionsInInhib); // prints the net, just for debugging
        void Reduce(PetriNet* net, MarkVal* m0, MarkVal* placeInQuery, MarkVal* placeInInhib, MarkVal* transitionsInInhib, int enablereduction);
        const std::vector<unsigned int> NonreducedTrace(PetriNet* net, const std::vector<unsigned int>& trace); // returns trace in the original net before reduction

        int RemovedTransitions() const {
            return _removedTransitions;
        }

        int RemovedPlaces() const {
            return _removedPlaces;
        }

        int RuleA() const {
            return _ruleA;
        }

        int RuleB() const {
            return _ruleB;
        }

        int RuleC() const {
            return _ruleC;
        }

        int RuleD() const {
            return _ruleD;
        }

    private:
        int _removedTransitions;
        int _removedPlaces;
        int _ruleA, _ruleB, _ruleC, _ruleD;

        typedef std::list< std::pair<int, int> > unfoldTransitionsType;
        unfoldTransitionsType* unfoldTransitions;
        unfoldTransitionsType* unfoldTransitionsInit;

        void expandTrace(unsigned int t, std::vector<unsigned int>& trace);

        int _nplaces, _ntransitions;
        int* _inArc;
        int* _outArc;

        // The reduction methods return true if they reduced something and reductions should continue with other rules
        bool ReducebyRuleA(PetriNet* net, MarkVal* m0, MarkVal* placeInQuery, MarkVal* placeInInhib, MarkVal* transitionInInhib);
        bool ReducebyRuleB(PetriNet* net, MarkVal* m0, MarkVal* placeInQuery, MarkVal* placeInInhib, MarkVal* transitionInInhib);
        bool ReducebyRuleC(PetriNet* net, MarkVal* m0, MarkVal* placeInQuery, MarkVal* placeInInhib, MarkVal* transitionInInhib);
        bool ReducebyRuleD(PetriNet* net, MarkVal* m0, MarkVal* placeInQuery, MarkVal* placeInInhib, MarkVal* transitionInInhib);
    };

    class QueryPlaceAnalysisContext : public PQL::AnalysisContext {
        PetriEngine::MarkVal* _placeInQuery;
    public:

        QueryPlaceAnalysisContext(const PetriNet& net, MarkVal* placeInQuery) : PQL::AnalysisContext(net) {
            _placeInQuery = placeInQuery;
        };

        ResolutionResult resolve(std::string identifier) const {
            ResolutionResult result;
            result.offset = -1;
            result.success = false;
            for (size_t i = 0; i < _places.size(); i++) {
                if (_places[i] == identifier) {
                    result.offset = i;
                    result.isPlace = true;
                    result.success = true;
                    _placeInQuery[i]++;
                    return result;
                }
            }
            for (size_t i = 0; i < _variables.size(); i++) {
                if (_variables[i] == identifier) {
                    result.offset = i;
                    result.isPlace = false;
                    result.success = true;
                    return result;
                }
            }
            return result;
        }

    };


}


#endif	/* REDUCER_H */

