/*
Copyright (c) 2012-2013, Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory.
Written by Niklas Nielsen and Gregory L. Lee <lee218@llnl.gov>.
LLNL-CODE-645136.
All rights reserved. 

This file is part of Dysect API. For details, see 
https://github.com/lee218llnl. 

Please also read this link the file "LICENSE" included in thie package for
Our Notice and GNU Lesser General Public License. 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (as published by the Free Software 
Foundation) version 2.1 dated February 1999. 

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU 
General Public License for more details. 

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to 

the Free Software Foundation, Inc.
59 Temple Place, Suite 330
Boston, MA 02111-1307 USA 
*/

#include <DysectAPI.h>
#include <libDysectAPI/src/expr-parser.tab.hh>
#include <libDysectAPI/src/lex.yy.h>

using namespace std;
using namespace DysectAPI;
using namespace Dyninst;
using namespace ProcControlAPI;
using namespace Stackwalker;

DysectAPI::DysectErrorCode ExprTree::evaluate(ConditionResult& conditionResult, Process::const_ptr process, THR_ID tid) {
  assert(!"Frontend cannot evaluate expression tree");
  return Error;
}


DysectAPI::DysectErrorCode TargetVar::getValue(ConditionResult& result, Value& c, Process::const_ptr process, THR_ID tid) {
  return Error;
}

DysectAPI::DysectErrorCode ExprNode::getGlobalVars(vector<GlobalState*>& globals) {
  return Error;
}
