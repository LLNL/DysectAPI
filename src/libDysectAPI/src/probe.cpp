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

vector<ProbeRequest*> Probe::requestQueue;

void Probe::linkComponents() {
  assert(dom != 0);

  dom->owner = this;

  event->owner = this;

  if(cond) {
    cond->owner = this;
  }

  if(!actions.empty()) {
    vector<Act*>::iterator actIter = actions.begin();
    for(; actIter != actions.end(); actIter++) {
      Act* action = *actIter;

      action->owner = this;
    }
  }
}

Probe::Probe( Event* event, 
              Cond* cond, 
              Domain* dom, 
              DysectAPI::Act* act, 
              lifeSpan life) : event(event), 
                               cond(cond), 
                               dom(dom), 
                               life(life),
                               timerId(0),
                               procSetInitialized(false),
                               awaitingNotifications(0),
                               awaitingActions(0),
                               parent(0) {

  if(dom == 0) {
    dom = Domain::inherit();
  }
  
  assert(dom != 0);
  assert(event != 0);

  if(act)
    actions.push_back(act);

  linkComponents();
}

Probe::Probe( Event* event, 
              Cond* cond, 
              Domain* dom, 
              vector<Act*> acts, 
              lifeSpan life) : event(event),
                               cond(cond),
                               dom(dom),
                               life(life),
                               parent(0),
                               timerId(0),
                               procSetInitialized(false),
                               awaitingActions(0),
                               awaitingNotifications(0) {
  if(dom == 0) {
    dom = Domain::inherit();
  }

  assert(dom != 0);
  assert(event != 0);

  actions = acts; 

  linkComponents();
}

Probe::Probe( Event* event, 
              Domain* dom, 
              DysectAPI::Act *act,
              lifeSpan life) : event(event), 
                               dom(dom), 
                               life(life),
                               parent(0), 
                               timerId(0),
                               procSetInitialized(false),
                               awaitingNotifications(0),
                               awaitingActions(0),
                               cond(0){
  if(dom == 0) {
    dom = Domain::inherit();
  }

  assert(dom != 0);
  assert(event != 0);

  if(act)
    actions.push_back(act);

  linkComponents();
}


Probe::Probe( Event* event, 
              Domain* dom, 
              vector<DysectAPI::Act*> acts,
              lifeSpan life) : event(event), 
                               dom(dom), 
                               life(life),
                               parent(0), 
                               timerId(0),
                               procSetInitialized(false),
                               awaitingNotifications(0),
                               awaitingActions(0),
                               cond(0){
  if(dom == 0) {
    dom = Domain::inherit();
  }

  assert(dom != 0);
  assert(event != 0);

  actions = acts; 

  linkComponents();
}

Probe::Probe( Event* event, 
              DysectAPI::Act *act,
              lifeSpan life) : event(event), 
                               life(life),
                               parent(0), 
                               cond(0), 
                               timerId(0),
                               procSetInitialized(false),
                               awaitingActions(0),
                               awaitingNotifications(0) {
  dom = Domain::inherit();
 
  if(act)
    actions.push_back(act);

  linkComponents();
}



Probe* Probe::link(Probe* probe) {
  linked.push_back(probe);

  probe->parent = this;

  return this;
}

Probe* Probe::getParent() {
  return parent;
}

Domain* Probe::getDomain() {
  return dom;
}

DysectAPI::DysectErrorCode Probe::prepareStream(treeCallBehavior callBehavior) {
  if(!dom) {
    return DomainNotFound;
  }

  if(dom->isInherited()) {
    Inherit *curDom = dynamic_cast<Inherit*>(dom);
    Domain *newDom = 0;
    if(curDom->copyDomainFromParent(newDom) != OK) {
      return Err::warn(Error, "Domain could not inherit parent domain info");
    }

    //delete(curDom);
    dom = newDom;
  }

  Domain::addToMap(dom);

  if(dom->prepareStream() != OK) {
    return StreamError;
  }

  if(callBehavior == recursive) {
    for(int i = 0; i < linked.size(); i++) {
      if(linked[i]->prepareStream(callBehavior) != OK)
        return StreamError;
    }
  }

  return OK;
}

DysectAPI::DysectErrorCode Probe::prepareEvent(treeCallBehavior callBehavior) {
  assert(event != 0);

  if(!event->prepare()) {
    return Error;
  }

  if(callBehavior == recursive) {
    for(int i = 0; i < linked.size(); i++) {
      if(linked[i]->prepareEvent(callBehavior) != OK)
        return Error;
    }
  }

  return OK;
}

DysectAPI::DysectErrorCode Probe::prepareCondition(treeCallBehavior callBehavior) {
  Err::verbose(true, "Prepare condition");
  if(cond != 0) {
    if(!cond->prepare()) {
      return Error;
    }
  } else {
    Err::verbose(true, "Preparing condition");
  }

  if(callBehavior == recursive) {
    for(int i = 0; i < linked.size(); i++) {
      if(linked[i]->prepareCondition(callBehavior) != OK)
        return Error;
    }
  }

  return OK;
}

DysectAPI::DysectErrorCode Probe::prepareAction(treeCallBehavior callBehavior) {
  if(!actions.empty()) {
    vector<Act*>::iterator actIter = actions.begin();
    for(; actIter != actions.end(); actIter++) {
      Act* action = *actIter;

      if(!action->prepare()) {
        return Error;
      }
    }
  }

  if(callBehavior == recursive) {
    for(int i = 0; i < linked.size(); i++) {
      if(linked[i]->prepareAction(callBehavior) != OK)
        return Error;
    }
  }

  return OK;
}

lifeSpan Probe::getLifeSpan() {
  return life;
}

int Probe::getId() {
  if(!dom) {
    return 0;
  }

  return dom->getId();
}

AggregateFunction* Probe::getAggregate(int id) {
  if(aggregates.empty())
    return 0;

  map<int, AggregateFunction*>::iterator aggIter = aggregates.find(id);
  if(aggIter == aggregates.end())
    return 0;

  AggregateFunction* aggFunc = aggIter->second;

  return aggFunc;
}
