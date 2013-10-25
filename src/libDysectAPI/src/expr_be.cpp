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
using namespace SymtabAPI;

DysectAPI::DysectErrorCode ExprTree::evaluate(ConditionResult& conditionResult, Process::const_ptr process, THR_ID tid) {
  assert(root != 0);

  Value constant;
  DysectAPI::DysectErrorCode code;

  vector<GlobalState*> globals;
  code = root->getGlobalVars(globals);

  if(!globals.empty()) {
    // Global values must retrieved before deeming result of expression tree
    assert(false); // Not supported at the moment
  }


  Err::verbose(true, "Starting evaluation");
  code = root->evaluate(conditionResult, constant, process, tid);
  if(code != OK) {
    return code;
  }

  return OK;
}

DysectAPI::DysectErrorCode TargetVar::getValue(ConditionResult& result, Value& c, Process::const_ptr process, Dyninst::THR_ID tid) {

  ProcDebug *processState = NULL;
  vector<THR_ID> threads;
  int ret;

  Walker* proc = (Walker*)process->getData();
  if(!proc) {
    return Err::warn(Error, "Walker not found for process: %d", process->getPid());
  }

  DataLocation* dataLoc;


  if(!DataLocation::findVariable(process, proc, name, dataLoc)) {
    // Variable not found
    return Err::warn(Error, "Variable '%s' not found", name.c_str());
  }

  Value nc;
  DysectAPI::DysectErrorCode code = dataLoc->getValue(nc);

  Err::verbose(true, "Read long value: %ld", nc.getValue<long>());
  Err::verbose(true, "Read int value: %d", nc.getValue<int>());

  if(code != OK)
    return code;

  c.copy(nc);
  result = Resolved;

  return DysectAPI::OK;
}

DysectAPI::DysectErrorCode ExprNode::getGlobalVars(vector<GlobalState*>& globals) {
  return OK;
}
