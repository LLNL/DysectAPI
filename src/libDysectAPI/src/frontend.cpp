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
#include <DysectAPI/Frontend.h>
#include <signal.h>

using namespace std;
using namespace DysectAPI;
using namespace MRN;

bool Frontend::running = true;

int Frontend::selectTimeout = 1;
int Frontend::numEvents = 0;
bool Frontend::breakOnEnter = true;
bool Frontend::breakOnTimeout = true;

class STAT_FrontEnd* Frontend::statFE = 0;

DysectAPI::DysectErrorCode Frontend::listen() {
  int ret;
  int idle = 0;

  // Install handler for (ctrl-c) abort
  // signal(SIGINT, Frontend::interrupt);
  //

  printf("Waiting for events (! denotes captured event)\n");
  printf("Hit <enter> to stop session\n");
  fflush(stdout);

  {
  do {
    // select() overwrites fd_set with ready fd's
    // Copy fd_set structure
    fd_set fdRead = Domain::getFdSet();

    if(breakOnEnter)
      FD_SET(0, &fdRead); //STDIN

    struct timeval timeout;
    timeout.tv_sec =  Frontend::selectTimeout;
    timeout.tv_usec = 0;

    ret = select(Domain::getMaxFd() + 1, &fdRead, NULL, NULL, &timeout);
    
    if(ret < 0) {
      //return Err::warn(DysectAPI::Error, "select() failed to listen on file descriptor set.");
      return DysectAPI::OK;
    }

    if(FD_ISSET(0, &fdRead) && breakOnEnter) {
      Err::info(true, "Stopping session - enter key was hit");
      break;
    }

    // Look for owners
    vector<Domain*> doms = Domain::getFdsFromSet(fdRead);

    if(doms.size() == 0) {
      if(Frontend::breakOnTimeout && (--Frontend::numEvents < 0)) {
        Err::info(true, "Stopping session - increase numEvents for longer sessions");
        break;
      }

    } else {
      printf("\n");
      fflush(stdout);
    }

    for(int i = 0; i < doms.size(); i++) {
      Domain* dom = doms[i];
      
      PacketPtr packet;
      int tag;

      if(!dom->getStream()) {
        return Err::warn(Error, "Stream not available for domain %x", dom->getId());
      }

      do {
        ret = dom->getStream()->recv(&tag, packet, false);
        if(ret == -1) {
          return Err::warn(Error, "Receive error");

        } else if(ret == 0) {
          break;
        }

        int count;
        char *payload;
        int len;

        if(packet->unpack("%d %auc", &count, &payload, &len) == -1) {
          return Err::warn(Error, "Unpack error");
        }

        if(Domain::isProbeEnabledTag(tag)) {
          Domain* dom = 0;

          if(!Domain::getDomainFromTag(dom, tag)) {
            Err::warn(false, "Could not get domain from tag %x", tag);
          } else {
            //Err::info(true, "[%d] Probe %x enabled (payload size %d)", count, dom->getId(), len);
            //Err::info(true, "[%d] Probe %x enabled", count, dom->getId());
          }

          Probe* probe = dom->owner;
          if(!probe) {
            Err::warn(false, "Probe object not found for %x", dom->getId());
          } else {
            probe->handleActions(count, payload, len);
          }

          // Empty bodied probe
          // Check wether backends are waiting for releasing processes
          if(dom->isBlocking()) {
            dom->sendContinue();
          }
        }
        
      } while(1);

      dom->getStream()->clear_DataNotificationFd();
    }
  } while(running);
  }
}

DysectAPI::DysectErrorCode Frontend::broadcastStreamInits() {
  vector<Probe*>& roots = ProbeTree::getRoots();
  long numRoots = roots.size();
  
  for(int i = 0; i < numRoots; i++) {
    Probe* probe = roots[i];
    probe->broadcastStreamInit(recursive);
  }

  return OK;

}

DysectAPI::DysectErrorCode Frontend::createStreams(struct DysectFEContext_t* context) {
  if(!context) {
    return Err::warn(Error, "Context not set");
  }

  statFE = context->statFE;
  
  vector<Probe*>& roots = ProbeTree::getRoots();
  long numRoots = roots.size();

  // Populate domain with MRNet network and debug process tables
  Domain::setFEContext(context);
  
  // Prepare streams
  for(int i = 0; i < numRoots; i++) {
    Probe* probe = roots[i];
    probe->prepareStream(recursive);
  }

  // Create streams
  for(int i = 0; i < numRoots; i++) {
    Probe* probe = roots[i];
    probe->createStream(recursive);
  }

  if(Domain::createFdSet() != OK) {
    return Error;
  }

  
  for(int i = 0; i < numRoots; i++) {
    Probe* probe = roots[i];

    if(probe->prepareAction(recursive) != OK) {
      Err::warn(Error, "Error occured while preparing actions");
    }
  }

  return OK;
}

void Frontend::stop() {
  Frontend::running = false;
}

void Frontend::interrupt(int sig) {
  Frontend::running = false;
}

STAT_FrontEnd* Frontend::getStatFE() {
  assert(statFE != 0);

  return statFE;
}

void Frontend::setStopCondition(bool breakOnEnter, bool breakOnTimeout, int timeout) {
  Err::verbose(true, "Break on enter key: %s", breakOnEnter ? "yes" : "no");
  Err::verbose(true, "Break on timeout: %s", breakOnTimeout ? "yes" : "no");

  Frontend::breakOnEnter = breakOnEnter;
  Frontend::breakOnTimeout = breakOnTimeout;
  Frontend::numEvents = timeout;
}
