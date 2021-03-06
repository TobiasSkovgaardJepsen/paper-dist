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
#include "RandomDFS.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include "../Structures/LimitedStateAllocator.h"

#include <list>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace PetriEngine::PQL;
using namespace PetriEngine::Structures;

namespace PetriEngine{ namespace Reachability {

ReachabilityResult RandomDFS::reachable(const PetriNet &net,
										const MarkVal *m0,
										const VarVal *v0,
										PQL::Condition *query){
	StateSet states(net, _kbound);
	LimitedStateAllocator<> allocator(net, _memorylimit);
	std::list<State*> stack;
	srand(time(0));	// Chosen by fair dice roll

	State* s0 = allocator.createState();
	if(!s0)
		return ReachabilityResult(ReachabilityResult::Unknown,
								   "Memory bound exceeded");
	memcpy(s0->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());

	stack.push_back(s0);
	states.add(s0);
	State* ns = allocator.createState();
	if(!ns)
		return ReachabilityResult(ReachabilityResult::Unknown,
								   "Memory bound exceeded");

	int countdown = rand() % 800000;
	unsigned int max = 0;
	int count = 0;
	BigInt exploredStates = 1;
	BigInt expandedStates = 0;
	std::vector<BigInt> enabledTransitionsCount (net.numberOfTransitions());
	
	if(query->evaluate(*s0, &net)){
		return ReachabilityResult(ReachabilityResult::Satisfied, "Query was satisfied",
								  expandedStates, exploredStates, states.discovered(), enabledTransitionsCount, states.maxTokens(), states.maxPlaceBound(), s0->pathLength(), s0->trace());
	}
	
	State* s = s0;
	while(!stack.empty()){
		// Progress reporting and abort checking
		if(count++ & 1<<17){
			if(stack.size() > max)
				max = stack.size();
			count = 0;
			// Report progress
			reportProgress((double)(max - stack.size())/(double)max);
			// Check abort
			if(abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										"Search was aborted.");
		}

		s = stack.back();
		stack.pop_back();

		//Hack for when query is null and we're look to print a random state
		if(!query && countdown-- <= 0){
			//TODO: Remove this hack or copy the code for random state generation, if we need that feature...
			printf("%s == %i ", net.placeNames()[0].c_str(), s->marking()[0]);
			for(int p = 1; p < net.numberOfPlaces(); p++)
				printf(" and %s == %i ", net.placeNames()[p].c_str(), s->marking()[p]);
			for(int x = 0; x < net.numberOfVariables(); x++)
				printf(" and %s == %i ", net.variableNames()[x].c_str(), s->valuation()[x]);
			return ReachabilityResult();
		}

		State* succ[net.numberOfTransitions()];
		memset(succ, 0, net.numberOfTransitions()*sizeof(State*));
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns, 1)){
				enabledTransitionsCount[t]++;
				if(states.add(ns)){
					exploredStates++;
					ns->setParent(s);
					ns->setTransition(t);
					if(query && query->evaluate(*ns, &net))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												"A state satisfying the query was found", expandedStates, exploredStates,
												states.discovered(), enabledTransitionsCount, states.maxTokens(), states.maxPlaceBound(), ns->pathLength(), ns->trace());
					succ[t] = ns;
					ns = allocator.createState();
					if(!ns)
						return ReachabilityResult(ReachabilityResult::Unknown,
												   "Memory bound exceeded",
												   expandedStates, exploredStates, states.discovered(), enabledTransitionsCount, states.maxTokens(), states.maxPlaceBound());
				}
			}
		}
		// Randomly sorts states into the stack
		expandedStates++;
		int random;
		int t;
		do {
			random = rand() % net.numberOfTransitions();
			t = random;
			do{
				if(succ[t]){
					stack.push_back(succ[t]);
					succ[t] = NULL;
					t++;
					break;
				}
				t = (t+1) % net.numberOfTransitions();
			} while(t != random);
		} while(t != random);
	}

	//Hack for when query is null and we're look to print a random state
	if(!query){
		printf("%s == %i ", net.placeNames()[0].c_str(), s->marking()[0]);
			for(int p = 1; p < net.numberOfPlaces(); p++)
				printf(" and %s == %i ", net.placeNames()[p].c_str(), s->marking()[p]);
			for(int x = 0; x < net.numberOfVariables(); x++)
				printf(" and %s == %i ", net.variableNames()[x].c_str(), s->valuation()[x]);
	}


	return ReachabilityResult(ReachabilityResult::NotSatisfied,
						"No state satisfying the query exists.", expandedStates, exploredStates,
						states.discovered(), enabledTransitionsCount, states.maxTokens(), states.maxPlaceBound());
}

}} // Namespaces
