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

#ifndef __DYSECTAPI_CMP_AGG_H
#define __DYSECTAPI_CMP_AGG_H

namespace DysectAPI {
  class CmpAgg : public AggregateFunction {
  protected:
    va_list args_;
    std::string fmt_;
    std::vector<DataRef*> params_;
  
    Value curVal;

    virtual bool compare(Value& newVal, Value& oldVal) = 0;

  public:
    CmpAgg(agg_type ltype, int id, int count, std::string fmt, void* payload);
    CmpAgg(agg_type ltype, std::string fmt);
    
    Value& getVal() { return curVal; }
    
    bool aggregate(AggregateFunction* agg);
    
    bool collect(void* process, void* thread);
    
    bool print();
    bool getStr(std::string& str);

    bool clear();
    
    int getSize();
    
    int writeSubpacket(char *p);

    bool isSynthetic();
    bool getRealAggregates(std::vector<AggregateFunction*>& aggregates);
    bool fetchAggregates(Probe* probe);
  };

  class Min : public CmpAgg {
  protected:
    bool compare(Value& lhs, Value& rhs);

  public:
    Min(int id, int count, std::string fmt, void* payload);
    Min(std::string fmt, ...);
  };

  class Max : public CmpAgg {
  protected:
    bool compare(Value& lhs, Value& rhs);

  public:
    Max(int id, int count, std::string fmt, void* payload);
    Max(std::string fmt, ...);
  };
}

#endif
