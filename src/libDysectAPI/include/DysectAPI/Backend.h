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

#ifndef __BACKEND_H
#define __BACKEND_H

namespace DysectAPI {
	class Backend {
    static enum BackendState {
      start,
      bindingStreams,
      ready,
      running,
      shutdown
    } state;

    static bool streamBindAckSent;

    static MRN::Stream* controlStream;
    static std::set<tag_t> missingBindings; //!< Set of tags needed to be bound by incoming front-end packets
    static Dyninst::Stackwalker::WalkerSet *walkerSet;

    static Dyninst::ProcControlAPI::ProcessSet::ptr enqueuedDetach;

    static DysectErrorCode bindStream(int tag, MRN::Stream* stream);
    static DysectErrorCode ackBindings();

    static DysectErrorCode enableProbeRoots(); //!< Initiates initial probe enablement

    static Dyninst::ProcControlAPI::Process::cb_ret_t handleEvent(Dyninst::ProcControlAPI::Process::const_ptr process,
                                 Dyninst::ProcControlAPI::Thread::const_ptr thread,
                                 Event* dysectEvent);

  public:
    static DysectErrorCode pauseApplication();
    static DysectErrorCode resumeApplication();

    static DysectErrorCode relayPacket(MRN::PacketPtr* packet, int tag, MRN::Stream* stream); //!< Incoming packages with DysectAPI signature in tag

    static DysectErrorCode prepareProbes(struct DysectBEContext_t* context);

    static DysectErrorCode registerEventHandlers();

    static Dyninst::Stackwalker::WalkerSet* getWalkerset();
    static Dyninst::ProcControlAPI::ProcessSet::ptr getProcSet();

    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleBreakpoint(Dyninst::ProcControlAPI::Event::const_ptr ev); //!< Called upon breakpoint encountering
    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleCrash(Dyninst::ProcControlAPI::Event::const_ptr ev);
    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleSignal(Dyninst::ProcControlAPI::Event::const_ptr ev); //!< Called upon signal raised
    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleProcessExit(ProcControlAPI::Event::const_ptr ev);
    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleGenericEvent(Dyninst::ProcControlAPI::Event::const_ptr ev);

    static DysectErrorCode handleTimerEvents();
    static DysectErrorCode handleQueuedOperations();

    static DysectErrorCode enqueueDetach(Dyninst::ProcControlAPI::Process::const_ptr process);
    static DysectErrorCode detachEnqueued();

    /**
     Event handler for:
      - Post-fork 
      - Pre-exit
      - Post-exit
      - crash
     */
    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleProcessEvent(Dyninst::ProcControlAPI::Event::const_ptr ev);

    /**
     Event handler for:
      - Thread-create
      - Pre-thread destroy
      - Post-thread destroy
     */
    static Dyninst::ProcControlAPI::Process::cb_ret_t  handleThreadEvent(Dyninst::ProcControlAPI::Event::const_ptr ev);
  };
}

#endif
