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

using namespace std;
using namespace DysectAPI;
using namespace Dyninst;
using namespace ProcControlAPI;

DysectAPI::DysectErrorCode Probe::createStream(treeCallBehavior callBehavior) {
  if(!dom) {
    return DomainNotFound;
  }

  if(dom->createStream() != OK)
    return StreamError;

  if(callBehavior == recursive) {
    for(int i = 0; i < linked.size(); i++) {
      if(linked[i]->createStream(callBehavior) != OK) 
        return StreamError;
    }
  }

  return OK;
} 

DysectAPI::DysectErrorCode Probe::broadcastStreamInit(treeCallBehavior callBehavior) {
  if(!dom) {
    return Err::warn(DomainNotFound, "domain not found");
  }

  if(dom->broadcastStreamInit() != OK) {
    return Err::warn(StreamError, "stream error");
  }

  if(callBehavior == recursive) {
    for(int i = 0; i < linked.size(); i++) {
      if(linked[i]->broadcastStreamInit(callBehavior) != OK) {
        
        return Err::warn(StreamError, "failed to broadcast inits");
      }
    }
  }

  return OK;
}

DysectErrorCode Probe::notifyTriggered() {
  return Error;
}

DysectErrorCode Probe::evaluateConditions(ConditionResult& result, 
                                    Process::const_ptr process, 
                                    Thread::const_ptr thread) {
  return Error;
}

bool Probe::wasTriggered(Process::const_ptr process, Thread::const_ptr thread) {
  return false;
}

bool Probe::enable() {
  return false;
}

DysectErrorCode Probe::handleActions(int count, char *payload, int len) {
  if(actions.empty()) {
    return OK;
  }

  aggregates.clear();

  if((payload != 0) && (len > 1)) {

    AggregateFunction::getAggregates(aggregates, (struct packet*)payload);
  }

  vector<Act*>::iterator actIter = actions.begin();
  for(;actIter != actions.end(); actIter++) {
    Act* act = *actIter;
    if(act) {
      act->finishFE(count);
    }
  }

  return OK;
}
