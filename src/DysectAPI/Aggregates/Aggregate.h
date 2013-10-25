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

#ifndef __DYSECTAPI_AGGREGATE_h
#define __DYSECTAPI_AGGREGATE_h

enum agg_type {
  minAgg = 1,
  maxAgg = 2,
  // String aggregates
  funcLocAgg = 3,
  fileLocAgg = 4,
  paramNamesAgg = 5,
  tracesAgg = 6,
  staticStrAgg = 7,

  // Folds to aggregates
  descAgg = 8
};

#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "DysectAPI/Aggregates/Data.h"
#include "DysectAPI/Aggregates/AggregateFunction.h"
#include "DysectAPI/Aggregates/CmpAgg.h"
//#include "DysectAPI/Aggregates/Min.h"
#include "DysectAPI/Aggregates/StrAgg.h"
#include "DysectAPI/Aggregates/Location.h"
#include "DysectAPI/Aggregates/DescVar.h"
#include "DysectAPI/Aggregates/Packet.h"

namespace DysectAPI {
  class Agg {
  public:
    static bool aggregateIdFromName(std::string name, int& id);
  };
}

#endif
