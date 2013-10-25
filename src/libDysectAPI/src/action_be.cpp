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
using namespace MRN;

bool Stat::prepare() {
  return true;
}

bool Stat::collect(Dyninst::ProcControlAPI::Process::const_ptr process,
                   Dyninst::ProcControlAPI::Thread::const_ptr thread) {

  return true;
}

bool Stat::finishFE(int count) {
  assert(!"Finish Front-end should not be run on backend-end!");
  return false;
}

bool Stat::finishBE(struct packet*& p, int& len) {
  Err::info(true, "Send merged stack trace!");

  return true;
}

bool StackTrace::collect(Dyninst::ProcControlAPI::Process::const_ptr process,
                   Dyninst::ProcControlAPI::Thread::const_ptr thread) {

  if(traces) {
    traces->collect((void*)&process, (void*)&thread);
  }

  return true;
}

bool StackTrace::finishFE(int count) {
  assert(!"Finish Front-end should not be run on backend-end!");
  return false;
}

bool StackTrace::finishBE(struct packet*& p, int& len) {
  vector<AggregateFunction*> aggregates;

  if(traces) {
    aggregates.push_back(traces);

    if(!AggregateFunction::getPacket(aggregates, len, p)) {
      return Err::warn(false, "Packet could not be constructed from aggregates!");
    }

    traces->clear();
  }

  return true;
}

bool Trace::collect(Process::const_ptr process,
                    Thread::const_ptr thread) {

  if(aggregates.empty())
    return true;

  vector<AggregateFunction*>::iterator aggIter = aggregates.begin();
  for(;aggIter != aggregates.end(); aggIter++) {
    AggregateFunction* aggregate = *aggIter;
    if(aggregate) {
      Err::verbose(true, "Collect data for trace message");
      aggregate->collect((void*)&process, (void*)&thread); // Ugly, but aggregation headers cannot know about Dyninst
    }
  }

  return true;
}

bool Trace::finishBE(struct packet*& p, int& len) {
  if(aggregates.empty())
    return true;

  
  vector<AggregateFunction*>::iterator aggIter;

  // If we have synthetic aggregate, we need to get actual aggregates
  vector<AggregateFunction*> realAggregates;
  aggIter = aggregates.begin();
  for(;aggIter != aggregates.end(); aggIter++) {
    AggregateFunction* aggregate = *aggIter;
    
    if(aggregate) {
      if(aggregate->isSynthetic()) {
        aggregate->getRealAggregates(realAggregates);
      } else {
        realAggregates.push_back(aggregate);
      }
    }
  }

  if(!AggregateFunction::getPacket(realAggregates, len, p)) {
    return Err::warn(false, "Packet could not be constructed from aggregates!");
  }

  aggIter = realAggregates.begin();
  for(;aggIter != realAggregates.end(); aggIter++) {
    AggregateFunction* aggregate = *aggIter;
    if(aggregate) {
      aggregate->clear();
    }
  }

  return true;
}

bool Trace::finishFE(int count) {
  assert(!"Finish Front-end should not be run on backend-end!");
  return false;
}

bool Detach::prepare() {
  return true;
}

bool Detach::collect(Process::const_ptr process,
                     Thread::const_ptr thread) {
  return true;
}

bool Detach::finishBE(struct packet*& p, int& len) {
  return true;
}

bool Detach::finishFE(int count) {
  assert(!"Finish Front-end should not be run on backend-end!");
  return false;
}
