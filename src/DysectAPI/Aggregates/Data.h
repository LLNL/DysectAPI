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

#ifndef __DYSECTAPI_DATA_h
#define __DYSECTAPI_DATA_h

namespace DysectAPI {
  class Value {
    friend class DataLocation;
    friend class LocalVariableLocation;
    friend class GlobalVariableLocation;

    void *buf;
    size_t len;

    int compare(Value& c);

  public:
    // XXX: Use stdint types (int8_t, uint64_t, ...)
    enum content_t {
      noType,
      boolType,
      intType,
      longType,
      floatType,
      doubleType
    };

    enum cmp_t {
      gt = 4,
      lt = 2,
      eq = 1
    };

    enum content_t getType();
    void setType(content_t type) { content = type; }

    Value();
    Value(bool bval);
    Value(float fval);
    Value(double dval);
    Value(int ival);
    Value(long lval);
    Value(std::string fmt, void *lbuf);

    template<typename T> void populate(T val) {
      if((len == 0) && (buf == 0)) {
        buf = new T();
        len = sizeof(T);

        *(T*)buf = val;
      }
    }
    
    template<typename T> T getValue() {
      T val = 0;
      
      switch(content) {
        case noType:
          fprintf(stderr, "Can not get value for unknown data type\n", content);
        break;
        case boolType: {
          bool ret = *(bool*)buf;
          val = (T)ret;
        }
        break;
        case intType: {
          int ret = *(int*)buf;
          val = (T)ret;
        }
        break;
        case longType: {
          long ret = *(long*)buf;
          val = (T)ret;
        }
        break;
        default:
          fprintf(stderr, "Data type not yet supported: %d\n", content);
        break;
      }

      return val;
    }

    bool isEqual(Value& c);
    bool isNotEqual(Value& c);
    bool isLessThan(Value& c);
    bool isLessThanEqual(Value& c);
    bool isGreaterThan(Value& c);
    bool isGreaterThanEqual(Value& c);

    void copy(const Value& rhs);

    int getLen() { return len; }
    void* getBuf() { return buf; }

    bool getStr(std::string& str);

    Value& operator=(Value& rhs);
    bool operator<(Value &rhs);
    bool operator>(Value &rhs);

  private:
    enum content_t content;
  };
}
#endif
