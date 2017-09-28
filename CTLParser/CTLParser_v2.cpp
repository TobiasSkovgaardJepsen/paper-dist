/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CTLParser_v2.cpp
 * Author: mossns
 * 
 * Created on April 22, 2016, 10:15 AM
 */

#include "rapidxml-1.13/rapidxml.hpp"
#include "CTLParser_v2.h"
#include "CTLQuery.h"
#include "EvaluateableProposition.h"
#include <algorithm>


using namespace rapidxml;

CTLParser_v2::CTLParser_v2() {
}

CTLParser_v2::CTLParser_v2(const CTLParser_v2& orig) {
}

CTLParser_v2::~CTLParser_v2() {
}

std::string CTLParser_v2::QueryToString(CTLQuery* query){
    if(query->GetQuantifier() == EMPTY && query->GetPath() == pError){
        return query->GetAtom();
    }
    else if (query->GetPath() == pError){
        Quantifier q = query->GetQuantifier();
        if (q == NEG){
            return query->ToString() + "(" + QueryToString(query->GetFirstChild()) + ")";
        }
        else if (q == AND || q == OR){
            return "(" + QueryToString(query->GetFirstChild()) + query->ToString() + QueryToString(query->GetSecondChild()) + ")";
        }
        else assert(false && "Could not print unknown logical query operator");
    }
    else if(query->GetQuantifier() == A || query->GetQuantifier() == E){
        if(query->GetPath() == U){
            return query->ToString() + "(" + QueryToString(query->GetFirstChild()) + ")Reach(" + QueryToString(query->GetSecondChild()) + ")";
        }
        else{
            return query->ToString() + "(" + QueryToString(query->GetFirstChild()) + ")";
        }
    }
    else assert(false && "Could not print unknown query type");
}

CTLQuery* CTLParser_v2::FormatQuery(CTLQuery* query, PetriEngine::PetriNet *net){
    query = FillAtom(query,net);
    query = ConvertAG(query);
    query = ConvertEG(query);
    IdSetting(query, 0);
    query = TemporalSetting(query);
    //assert(false);
    return query;
}

CTLQuery* CTLParser_v2::TemporalSetting(CTLQuery* query) {
    CTLType query_type = query->GetQueryType();
    if(query_type == EVAL){
        assert(!query->IsTemporal);
        return query;
    }
    else if (query_type == LOPERATOR){
        Quantifier quan = query->GetQuantifier();
        if(quan != NEG){
            query->SetFirstChild(TemporalSetting(query->GetFirstChild()));
            query->SetSecondChild(TemporalSetting(query->GetSecondChild()));
            query->IsTemporal = (query->GetFirstChild()->IsTemporal || query->GetSecondChild()->IsTemporal);
        }
        else{
            query->SetFirstChild(TemporalSetting(query->GetFirstChild()));
            query->IsTemporal = (query->GetFirstChild()->IsTemporal);
        }
        return query;
    }
    else if (query_type == PATHQEURY){
        assert(query->IsTemporal);
        if (query->GetPath() == U){
            query->SetFirstChild(TemporalSetting(query->GetFirstChild()));
            query->SetSecondChild(TemporalSetting(query->GetSecondChild()));
        }
        else{
            query->SetFirstChild(TemporalSetting(query->GetFirstChild()));
        }
        return query;
    } else {
        //this should not happen
        assert(false);
    }
}

//returns next available id
int CTLParser_v2::IdSetting(CTLQuery *query, int id)
{
    query->Id = id;
    CTLType query_type = query->GetQueryType();
    if(query_type == EVAL){
        assert(!query->IsTemporal);
        query->Depth = 0;
        return id + 1;
    }
    else if (query_type == LOPERATOR){
        Quantifier quan = query->GetQuantifier();
        if(quan != NEG){
            int afterFirst = IdSetting(query->GetFirstChild(), id + 1);
            int afterSecond = IdSetting(query->GetSecondChild(), afterFirst);
            query->Depth = std::max(query->GetFirstChild()->Depth, query->GetSecondChild()->Depth) + 1;
            return afterSecond;
        }
        else{
            int afterFirst = IdSetting(query->GetFirstChild(), id + 1);
            query->Depth = query->GetFirstChild()->Depth + 1;
            return afterFirst;
        }
    }
    else if (query_type == PATHQEURY){
        assert(query->IsTemporal);
        if (query->GetPath() == U){
            int afterFirst = IdSetting(query->GetFirstChild(), id + 1);
            int afterSecond = IdSetting(query->GetSecondChild(), afterFirst);
            query->Depth = std::max(query->GetFirstChild()->Depth, query->GetSecondChild()->Depth) + 1; 
            return afterSecond;
        }
        else{
            int afterFirst = IdSetting(query->GetFirstChild(), id + 1);
            query->Depth = query->GetFirstChild()->Depth + 1;
            return afterFirst;
        }
    } else {
        //this should not happen
        assert(false);
    }
}


CTLQuery* CTLParser_v2::FillAtom(CTLQuery* query, PetriEngine::PetriNet *net) {
    CTLType query_type = query->GetQueryType();
    if(query_type == EVAL){
        EvaluateableProposition *proposition = new EvaluateableProposition(query->GetAtom(), net);
        query->SetProposition(proposition);
        return query;
    }
    else if (query_type == LOPERATOR){
        Quantifier quan = query->GetQuantifier();
        if(quan != NEG){
            FillAtom(query->GetFirstChild(),net);
            FillAtom(query->GetSecondChild(),net);
        }
        else{
            FillAtom(query->GetFirstChild(),net);
        }
        return query;
    }
    else if (query_type == PATHQEURY){
        if (query->GetPath() == U){
            FillAtom(query->GetFirstChild(),net);
            FillAtom(query->GetSecondChild(),net);
        }
        else{
            FillAtom(query->GetFirstChild(),net);
        }
        return query;
    }
    else assert(false && "Could not traverse unknown query type");
}

CTLQuery* CTLParser_v2::ConvertAG(CTLQuery* query) {
    CTLType query_type = query->GetQueryType();
    if(query_type == EVAL){
        return query;
    }
    else if (query_type == LOPERATOR){
        Quantifier quan = query->GetQuantifier();
        if(quan != NEG){
            query->SetFirstChild(ConvertAG(query->GetFirstChild()));
            query->SetSecondChild(ConvertAG(query->GetSecondChild()));
        }
        else{
            query->SetFirstChild(ConvertAG(query->GetFirstChild()));
        }
        return query;
    }
    else if (query_type == PATHQEURY){
        if (query->GetPath() == U){
            query->SetFirstChild(ConvertAG(query->GetFirstChild()));
            query->SetSecondChild(ConvertAG(query->GetSecondChild()));
        }
        else if (query->GetQuantifier() == A && query->GetPath() == G){
            
            CTLQuery *neg_two = new CTLQuery(NEG, pError, false, "");
            neg_two->SetFirstChild(query->GetFirstChild());
            CTLQuery *ef_q = new CTLQuery(E, F, false, "");
            ef_q->SetFirstChild(neg_two);
            CTLQuery *neg_one = new CTLQuery(NEG, pError, false, "");
            neg_one->SetFirstChild(ef_q);
            query = neg_one;
            
            query->SetFirstChild(ConvertAG(query->GetFirstChild()));
        }
        else{
            query->SetFirstChild(ConvertAG(query->GetFirstChild()));
        }
        return query;
    }
    else assert(false && "Could not traverse unknown query type");
}

CTLQuery* CTLParser_v2::ConvertEG(CTLQuery* query) {
    CTLType query_type = query->GetQueryType();
    if(query_type == EVAL){
        return query;
    }
    else if (query_type == LOPERATOR){
        Quantifier quan = query->GetQuantifier();
        if(quan != NEG){
            query->SetFirstChild(ConvertEG(query->GetFirstChild()));
            query->SetSecondChild(ConvertEG(query->GetSecondChild()));
        }
        else{
            query->SetFirstChild(ConvertEG(query->GetFirstChild()));
        }
        return query;
    }
    else if (query_type == PATHQEURY){
        if (query->GetPath() == U){
            query->SetFirstChild(ConvertEG(query->GetFirstChild()));
            query->SetSecondChild(ConvertEG(query->GetSecondChild()));
        }
        else if (query->GetQuantifier() == E && query->GetPath() == G){
            
            CTLQuery *neg_two = new CTLQuery(NEG, pError, false, "");
            neg_two->SetFirstChild(query->GetFirstChild());
            CTLQuery *ef_q = new CTLQuery(A, F, false, "");
            ef_q->SetFirstChild(neg_two);
            CTLQuery *neg_one = new CTLQuery(NEG, pError, false, "");
            neg_one->SetFirstChild(ef_q);
            query = neg_one;
            
            query->SetFirstChild(ConvertEG(query->GetFirstChild()));
        }
        else{
            query->SetFirstChild(ConvertEG(query->GetFirstChild()));
        }
        return query;
    }
    else assert(false && "Could not traverse unknown query type");
}


CTLQuery * CTLParser_v2::ParseXMLQuery(std::vector<char> buffer, int query_number) {
    #ifdef DEBUG
    std::cout << "Creating doc\n" << std::flush;
    #endif
    xml_document<> doc;
    xml_node<> * root_node;
    
    #ifdef DEBUG
    std::cout << "Size of Path enum: " << sizeof(Path)*8 <<"\n";
    #endif
    doc.parse<0>(&buffer[0]);
    

#ifdef DEBUG
    std::cout << "First node id: " << doc.first_node()->name() << std::endl;
#endif

    root_node = doc.first_node();

#ifdef Analysis
    std::cout << "\nAnalysis:: Queries:" << std::endl;
#endif
    int i = 1;
    for (xml_node<> * property_node = root_node->first_node("property"); property_node; property_node = property_node->next_sibling()) {
        if(i == query_number){
            xml_node<> * id_node = property_node->first_node("id");
            xml_node<> * formula_node = id_node->next_sibling("description")->next_sibling("formula");
            CTLQuery * query = xmlToCTLquery(formula_node->first_node());
            return query;
        }
        i++;
    }
    assert(false && "Query number did not match a property in the provided .xml file.");
}

QueryMeta* CTLParser_v2::GetQueryMetaData(std::vector<char> buffer) {
    xml_document<> doc;
    xml_node<> * root_node;
    doc.parse<0>(&buffer[0]);
    root_node = doc.first_node();
    QueryMeta *meta_d = new QueryMeta();
    xml_node<> * first_property_node = root_node->first_node("property");
    xml_node<> * id_node = first_property_node->first_node("id");
    std::string model_name(id_node->value());
    int i = 0;
    for (xml_node<> * property_node = root_node->first_node("property"); property_node; property_node = property_node->next_sibling()) {    
        i++;
    }
    meta_d->numberof_queries = i;
    std::size_t pos = model_name.find_last_of("-0") - 1;
    meta_d->model_name = model_name.substr(0, pos);
    return meta_d;
}


CTLQuery* CTLParser_v2::xmlToCTLquery(xml_node<> * root) {
    char *root_name = root->name();
    char firstLetter = root_name[0];
    CTLQuery *query;
    if (firstLetter == 'a') {
        //Construct all-paths
        query = new CTLQuery(A, getPathOperator(root), false, "");
        assert(query->GetQuantifier() == A && "Failed setting quantifier");
    }
    else if (firstLetter == 'e' ) {
        //Construct exists-path
        query = new CTLQuery(E, getPathOperator(root), false, "");
        assert(query->GetQuantifier() == E && "Failed setting path operator");
    }
    else if (firstLetter == 'n' ) {
        //Construct negation
        query = new CTLQuery(NEG, pError, false, "");
        assert(query->GetQuantifier() == NEG && "Failed setting negation operator");
    }
    else if (firstLetter == 'c' ) {
        //Construct conjunction
        query = new CTLQuery(AND, pError, false, "");
        assert(query->GetQuantifier() == AND && "Failed setting and operator");
    }
    else if (firstLetter == 'd' ) {
        //Construct disjunction
        query = new CTLQuery(OR, pError, false, "");
        assert(query->GetQuantifier() == OR && "Failed setting or operator");
    }
    else if (firstLetter == 'i' ) {
        //Construct Atom
        std::string atom_str = "";
        if (root_name[1] == 's' ){
            //Fireability Query File
            atom_str = root->name();
            atom_str = atom_str + "(";
            root = root->first_node();
            atom_str = atom_str + root->value();
            for (xml_node<> * transition_node = root->next_sibling(); transition_node; transition_node = transition_node->next_sibling()) {
                atom_str = atom_str + ", " + transition_node->value();
            }
            atom_str = atom_str + ")";
        }
        else if (root_name[1] == 'n') {
            //Cardinality Query File
            std::string loperator = root->name();
            xml_node<> * par_node = root->first_node();
            std::string first = parsePar(par_node);
            par_node = par_node->next_sibling();
            std::string second = parsePar(par_node);
            
            loperator = loperator_sym(loperator);
            
            atom_str = first + loperator + second;
            
        }
        else assert(false && "Incorrectly format of .xml file provided");
        query = new CTLQuery(EMPTY, pError, true, atom_str);
        query->Depth = 0;
        return query;
    }
    else if (firstLetter == 't' ){
        std::string atom_str = "integer-constant(0) le integer-constant(0)";
        query = new CTLQuery(EMPTY, pError, true, atom_str);
        query->Depth = 0;
        return query;
    }
    else if (firstLetter == 'f' ){
        std::string atom_str = "integer-constant(2) le integer-constant(0)";
        query = new CTLQuery(EMPTY, pError, true, atom_str);
        query->Depth = 0;
        return query;
    }
    else assert(false && "Failed parsing .xml file provided. Incorrect format.");
    
    if (query->GetPath() == U) {
        xml_node<> * child_node = root->first_node()->first_node();
        query->SetFirstChild(xmlToCTLquery(child_node->first_node()));
        child_node = child_node->next_sibling();
        query->SetSecondChild(xmlToCTLquery(child_node->first_node()));
        
        query->Depth = (max_depth(query->GetFirstChild()->Depth, query->GetSecondChild()->Depth)) + 1;
    }
    else if (query->GetQuantifier() == AND || query->GetQuantifier() == OR) {
        xml_node<> * child_node = root->first_node();
        query->SetFirstChild(xmlToCTLquery(child_node));
        child_node = child_node->next_sibling();
        query->SetSecondChild(xmlToCTLquery(child_node));
        
        query->Depth = (max_depth(query->GetFirstChild()->Depth, query->GetSecondChild()->Depth)) + 1;
    }
    else if (query->GetQuantifier() == NEG) {
        query->SetFirstChild(xmlToCTLquery(root->first_node()));
        query->Depth = query->GetFirstChild()->Depth + 1;
    }
    else if (query->GetPath() == pError){
        assert(false && "Failed setting Path operator");
    }
    else if (query->GetQueryType() == PATHQEURY) {
        query->SetFirstChild(xmlToCTLquery(root->first_node()->first_node()));
        query->Depth = query->GetFirstChild()->Depth + 1;
    }
    else{
        assert(false && "Attemting to give atom children - ERROR");
    }
    
    return query;
}

Path CTLParser_v2::getPathOperator(xml_node<> * quantifyer_node){
    char path_firstLetter = quantifyer_node->first_node()->name()[0];
    if(path_firstLetter == 'f')
        return F;
    else if (path_firstLetter == 'g')
        return G;
    else if (path_firstLetter == 'n')
        return X;
    else if (path_firstLetter == 'u')
        return U;
    else assert(false && "Failed parsing path operator. Incorrect format.");
}

std::string CTLParser_v2::parsePar(xml_node<> * parameter){
    std::string parameter_str = parameter->name();
    parameter_str = parameter_str + "(";
    
    if (parameter->name()[0] == 't'){
        xml_node<> * place_node = parameter->first_node();
        parameter_str = parameter_str + place_node->value();
        for(place_node = place_node->next_sibling(); place_node; place_node = place_node->next_sibling()){
            parameter_str = parameter_str + ", " + place_node->value();
        }
        parameter_str = parameter_str + ")";
    }
    
    else if(parameter->name()[0] == 'i'){
        parameter_str = parameter_str + parameter->value() + ")";
    }
    else assert(false && "Failed parsing cardinality parameter. Incorrect format.");
    return parameter_str;
}

/*
 for (xml_node<> * transition_node = root->next_sibling(); transition_node; transition_node = transition_node->next_sibling()) {
                atom_str = atom_str + ", " + transition_node->value();
            }
            atom_str = atom_str + ")";
 */

int CTLParser_v2::max_depth(int a, int b){
    if(a < b) return b; return a;
}

std::string CTLParser_v2::loperator_sym(std::string loperator){
    if(loperator.compare("integer-le") == 0){
        return " le ";
    }
    else if(loperator.compare("integer-ge")){
        return " ge ";
    }
    else if(loperator.compare("integer-eq")){
        return " eq ";
    }
    else return " " + loperator + " ";
}

CTLQuery * CTLParser_v2::CopyQuery(CTLQuery *source){
    if(source->GetQueryType() == EVAL){
        return new CTLQuery(EMPTY, pError, true, source->GetAtom());
    }
    else if(source->GetQueryType() == LOPERATOR){
        CTLQuery *dest = new CTLQuery(source->GetQuantifier(), pError, false, "");
        if(source->GetQuantifier() != NEG){
            dest->SetFirstChild(CopyQuery(source->GetFirstChild()));
            dest->SetSecondChild(CopyQuery(source->GetSecondChild()));
        }
        else {
            dest->SetFirstChild(CopyQuery(source->GetFirstChild()));
        }
        return dest;
    }
    else if(source->GetQueryType() == PATHQEURY){
        CTLQuery *dest = new CTLQuery(source->GetQuantifier(), source->GetPath(), false, "");
        if(source->GetPath() == U){
            dest->SetFirstChild(CopyQuery(source->GetFirstChild()));
            dest->SetSecondChild(CopyQuery(source->GetSecondChild()));
        }
        else{
            dest->SetFirstChild(CopyQuery(source->GetFirstChild()));
        }
        return dest;
    }
    else assert(false && "ERROR::: Copying query failed");
}
