// Copyright (c) 2012-2013, Aptarism SA.
//
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the University of California, Berkeley nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#ifndef MEDIAGRAPH_BINARY_SERIALIZER_H
#define MEDIAGRAPH_BINARY_SERIALIZER_H

#include <string>
#include <stdint.h>

#include "type_visitor.h"

namespace media_graph {

//! Serialize known types to binary.
class BinarySerializer : public TypeConstVisitor {
  public:
    virtual bool process(const int &value);
    virtual bool process(const int64_t &value);
    virtual bool process(const bool &value);

    virtual bool process(const float &value);
    virtual bool process(const double &value);

    virtual bool process(const std::string &value);

    const std::string &value() const { return serialized_value_; }

  private:
    std::string serialized_value_;
};

//! De-serialize known types from binary.
class BinaryDeSerializer : public TypeVisitor {
  public:
    BinaryDeSerializer(const std::string &serialized_value)
        : serialized_value_(serialized_value) { }

    virtual bool process(int *value);
    virtual bool process(int64_t *value);
    virtual bool process(bool *value);

    virtual bool process(float *value);
    virtual bool process(double *value);

    virtual bool process(std::string *value);

    const std::string &remaining() const { return serialized_value_; }

  private:
    std::string serialized_value_;
};

}  // namespace media_graph

#endif  // MEDIAGRAPH_BINARY_SERIALIZER_H
