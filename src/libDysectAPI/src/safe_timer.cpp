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

long SafeTimer::nextTimeout = LONG_MAX;
set<Probe*> SafeTimer::waitingProbes;
map<long, Probe*> SafeTimer::probesTimeoutMap;


bool SafeTimer::startSyncTimer(Probe* probe) {
  if(!probe)
    return false;

  if(syncTimerRunning(probe)) {
    return true;
  }

  Domain* dom = probe->getDomain();
  if(!dom)
    return false;

  long timeout = dom->getWaitTime();

  long expectedTimeout = getTimeStamp() + timeout;

  if(expectedTimeout <= nextTimeout) {
    nextTimeout = expectedTimeout;
  }

  waitingProbes.insert(probe);
  probesTimeoutMap.insert(pair<long, Probe*>(expectedTimeout, probe));

  return true;
}

bool SafeTimer::anySyncReady() {
  long ts = getTimeStamp();

  if(nextTimeout < ts) {
    return true;
  }

  return false;
}

bool SafeTimer::syncTimerRunning(Probe* probe) {
  set<Probe*>::iterator probeIter = waitingProbes.find(probe);
  if(probeIter != waitingProbes.end()) {
    return true;
  }

  return false;
}

std::vector<Probe*> SafeTimer::getAndClearAll() {
  vector<Probe*> ready;

  if(probesTimeoutMap.empty())
    return ready;

  long curTime = getTimeStamp();

  map<long, Probe*>::iterator probeIter = probesTimeoutMap.begin();
  map<long, Probe*>::iterator endIter = probesTimeoutMap.end();

  for(;probeIter != probesTimeoutMap.end(); probeIter++) {
    waitingProbes.erase(probeIter->second);
    ready.push_back(probeIter->second);
  }

  return ready;
}

std::vector<Probe*> SafeTimer::getAndClearSyncReady() {
  vector<Probe*> ready;

  if(probesTimeoutMap.empty())
    return ready;

  long curTime = getTimeStamp();

  map<long, Probe*>::iterator probeIter = probesTimeoutMap.begin();
  map<long, Probe*>::iterator endIter = probesTimeoutMap.end();

  for(;probeIter != probesTimeoutMap.end(); probeIter++) {
    if(probeIter->first > curTime) {
      nextTimeout = probeIter->first;
      endIter = probeIter;
      break;
    }

    waitingProbes.erase(probeIter->second);
    ready.push_back(probeIter->second);
  }

  if(!ready.empty()) {
    probesTimeoutMap.erase(probesTimeoutMap.begin(), endIter);
  }

  if(probesTimeoutMap.empty()) {
    nextTimeout = LONG_MAX;
  }

  return ready;
}

#if 0
long SafeTimer::getTimeStamp() {
  struct timeval tv;
  long elapsedTime = 0;

  gettimeofday(&tv, NULL);

  elapsedTime = tv.tv_sec * 1000.0;
  elapsedTime += tv.tv_usec / 1000.0;

  return elapsedTime;
}
#endif
