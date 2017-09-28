/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef STATESET_H
#define STATESET_H

#include <boost/unordered_set.hpp>
#include <iostream>
#include "State.h"

extern unsigned int maxplacesbound;
extern std::vector<size_t>* placelistboundindex;

namespace PetriEngine { namespace Structures {

// Big int used for state space statistics
typedef unsigned long long int BigInt;

class StateSet : boost::unordered_set<State*, State::hash, State::equal_to>{
public:
	StateSet(const PetriNet& net, int kbound = 0)
        : boost::unordered_set<State*, State::hash, State::equal_to>
			(8, State::hash(net.numberOfPlaces(), net.numberOfVariables()),
			 State::equal_to(net.numberOfPlaces(),net.numberOfVariables())){
		_discovered = 0;
		_kbound = kbound;
		_maxTokens = 0;
		_places = net.numberOfPlaces();
		_variables = net.numberOfVariables();
                _maxPlaceBound = std::vector<unsigned int>(_places,0);
	}
	StateSet(unsigned int places, unsigned int variables, int kbound = 0)
        : boost::unordered_set<State*, State::hash, State::equal_to>
			(8, State::hash(places, variables),
			 State::equal_to(places, variables)){
		_discovered = 0;
		_kbound = kbound;
		_maxTokens = 0;
		_places = places;
		_variables = variables;
                _maxPlaceBound = std::vector<unsigned int>(_places,0);
        }
	bool add(State* state) {
		_discovered++;
		
		//Check that we can take from the marking
		MarkVal sum = 0;
		for(size_t i = 0; i < _places; i++)
			sum += state->marking()[i];
		
		if(_maxTokens < sum)
			_maxTokens = sum;
		
		//Check that we're within k-bound
		if(_kbound != 0 && sum > _kbound)
			return false;
		
		std::pair<iter, bool> result = this->insert(state);
                // update the max token bound for each place in the net (only for newly discovered markings)
                if (result.second) {
                    
                     if (placelistboundindex == NULL) { // single places in the place bound check
                        for (size_t i = 0; i < _places; i++) {
                            _maxPlaceBound[i] = std::max<unsigned int>(state->marking()[i], _maxPlaceBound[i]);
                        }
                    } else { // multiple places in the place bound check
                        unsigned int sum = 0;
                        for (int i=0; i < (*placelistboundindex).size(); i++) {
                            sum += state->marking()[(*placelistboundindex)[i]];
                        }
                        maxplacesbound = std::max<unsigned int>(sum, maxplacesbound);
                    }
                }
		return result.second;
	}
	bool contains(State* state) {
		_discovered++;
		return this->count(state) > 0;
	}
	BigInt discovered() const {
		return _discovered;
	}
	int maxTokens() const {
	  return _maxTokens;
	}
        
        std::vector<unsigned int> maxPlaceBound() const { 
            return _maxPlaceBound; 
        }
        
private:
    typedef boost::unordered_set<State*, State::hash, State::equal_to>::const_iterator const_iter;
    typedef boost::unordered_set<State*, State::hash, State::equal_to>::iterator iter;
	BigInt _discovered;
	int _kbound;
	int _maxTokens;
        std::vector<unsigned int> _maxPlaceBound;
	unsigned int _places;
	unsigned int _variables;
};

}}


#endif // STATESET_H
