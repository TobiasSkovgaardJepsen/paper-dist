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
#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a DFS manner, using a hash table */
class DepthFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	DepthFirstReachabilitySearch(int kbound = 0, int memorylimit = 0)
		: ReachabilitySearchStrategy(){
		_kbound = kbound;
		_memorylimit = memorylimit;
	}


	/** Perform reachability check using DFS with hash table */
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* initialMarking,
								 const VarVal* initialAssignment,
								 PQL::Condition* query);
private:
	/** A step in the reachability search */
	struct Step{
		Step(Structures::State* s, unsigned int t){
			state = s;
			this->t = t;
		}
		Structures::State* state;
		unsigned int t;
	};
	int _kbound;
	int _memorylimit;
};
} // Reachability
} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
