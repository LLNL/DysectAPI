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
using namespace SymtabAPI;
using namespace ProcControlAPI;
using namespace Stackwalker;

using namespace Dyninst;
using namespace Stackwalker;

bool Time::enable() {
  assert(owner != 0);
 
  if(codeLocations.empty()) {
    return true;
  }

  Domain* dom = owner->getDomain();

  ProcessSet::ptr procset;
  
  if(!dom->getAttached(procset)) {
    return Err::warn(false, "Could not get procset from domain");
  }

  if(!procset) {
    return Err::warn(false, "Process set not present");
  }

  return enable(procset);
}

bool Time::enable(ProcessSet::ptr lprocset) {
  if(!lprocset) {
    return Err::warn(false, "Process set not present");
  }
  
  if(procset && !procset->empty()) {
    procset = procset->set_union(lprocset);
  } else {
    procset = lprocset;
  }

  return true;
}

bool Time::disable() {
  if(procset && !procset->empty()) {
    procset.clear();
  }

  return true;
}

bool Time::disable(ProcessSet::ptr lprocset) {

  if(procset && !procset->empty()) {
    procset = procset->set_difference(lprocset);
  }

  return true;
}

bool Time::isEnabled(Dyninst::ProcControlAPI::Process::const_ptr process) {
  bool result = false;

  if(procset) {
    ProcessSet::iterator procIter = procset->find(process);
    result = (procIter != procset->end());
  }

  return result;

  return true;
}
