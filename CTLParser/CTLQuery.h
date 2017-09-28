/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CTLQuery.h
 * Author: Søren Moss Nielsen
 *
 * Created on April 22, 2016, 7:57 AM
 */

#ifndef CTLQUERY_H
#define CTLQUERY_H
#include "CTLParser.h"
#include "EvaluateableProposition.h"

enum CTLType {PATHQEURY = 1, LOPERATOR = 2, EVAL = 3, TYPE_ERROR = -1};

class CTLQuery {
    
public:
    CTLQuery(Quantifier q, Path p, bool isAtom, std::string atom_str);
    virtual ~CTLQuery();
    
    int Id;
    int Depth;
    
    CTLType GetQueryType();
    CTLQuery* GetFirstChild();
    CTLQuery* GetSecondChild();
    void SetFirstChild(CTLQuery *q);
    void SetSecondChild(CTLQuery *q);
    std::string ToString();
    
    Quantifier GetQuantifier();
    Path GetPath();
    std::string GetAtom();
    
    EvaluateableProposition* GetProposition();
    void SetProposition(EvaluateableProposition *p);
    
    bool IsTemporal;
    
    
private:
    std::string CreateEvaluateableProposition(std::string a);
    
    bool _hasQuantifier;
    bool _hasPath;
    bool _hasAtom;
    Quantifier _q;
    Path _path;
    std::string _a;
    
    CTLQuery* _firstchild;
    CTLQuery* _secondchild;
    
    EvaluateableProposition* _proposition;
};

#endif /* CTLQUERY_H */

