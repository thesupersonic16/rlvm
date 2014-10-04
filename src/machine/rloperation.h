// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006, 2007 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
//
// -----------------------------------------------------------------------

#ifndef SRC_MACHINE_RLOPERATION_H_
#define SRC_MACHINE_RLOPERATION_H_

#include <utility>
#include <string>
#include <type_traits>
#include <vector>

#include "libreallive/bytecode_fwd.h"
#include "libreallive/expression.h"

class MappedRLModule;
class RLModule;

// @defgroup RLOperationGroup RLOperation and its type system
//
// Defines the base classes from which all of the opcode
// implementations derive from. This heirarchy of classes works by
// having one of your operation classes, which handles a specific
// prototype of a specific opcode, derive from one of the subclases
// of RLOperation, specifically RLOp_Void_* and RLOp_Store_*. The
// template parameters of these subclasses refer to the types of the
// parameters, some of which can be composed to represent more complex
// parameters.
//
// Valid type parameters are IntConstant_T, IntReference_T,
// StrConstant_T, StrReference_T, Argc_T< U > (takes another type as a
// parameter). The type parameters change the arguments to the
// implementation function.
//
// Let's say we want to implement an operation with the following
// prototype: <tt>fun (store)doSomething(str, intC+)</tt>. The
// function returns an integer value to the store register, so we want
// to derive the implementation struct from RLOp_Store<>, which will
// automatically place the return value in the store register. Our
// first parameter is a reference to a piece of string memory, so our
// first template argument is StrReference_T. We then take a variable
// number of ints, so we compose IntConstant_T into the template
// Argc_T for our second parameter.
//
// Thus, our sample operation would be implemented with this:
//
// @code
// struct Operation : public RLOp_Store_2<StrReference_T, Argc_T< IntConstant_T
// > > {
//   int operator()(RLMachine& machine, StringReferenceIterator x, vector<int>
// y) {
//     // Do whatever with the input parameters...
//     return 5;
//   }
// };
// @endcode
//
// For information on how to group RLOperations into modules to
// attach to an RLMachine instance, please see @ref ModulesOpcodes
// "Modules and Opcode Definitions".

// Each RLOperation can optionally carry some numeric properties.
enum OperationProperties { PROP_NAME, PROP_FGBG, PROP_OBJSET };

// An RLOperation object implements an individual bytecode
// command. All command bytecodes have a corresponding instance of a
// subclass of RLOperation that defines it.
//
// RLOperations are grouped into RLModule s, which are then added to
// the RLMachine.
class RLOperation {
 public:
  // Default constructor
  RLOperation();

  // Destructor
  virtual ~RLOperation();

  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  RLOperation* SetProperty(int property, int value);
  bool GetProperty(int property, int& value) const;

  // Check made as to whether the instruction pointer should be
  // incremented after the instruction is executed. Override this in
  // the *rare* case where an instruction messes about with the
  // instruction pointer and we don't want to mess with it afterwards.
  // The default implmentation returns true; 99% of instructions want
  // the instruction pointer to be advanced automatically.
  virtual bool AdvanceInstructionPointer();

  // The Dispatch function is implemented on a per type basis and is called by
  // the Module, after checking to make sure that the
  virtual void Dispatch(
      RLMachine& machine,
      const libreallive::ExpressionPiecesVector& parameters) = 0;

  // Parses the parameters in the CommandElement passed in into an
  // output vector that contains parsed ExpressionPieces for each
  virtual void ParseParameters(const std::vector<std::string>& input,
                               libreallive::ExpressionPiecesVector& output) = 0;

  // The public interface used by the RLModule; how a method is Dispatched.
  virtual void DispatchFunction(RLMachine& machine,
                                const libreallive::CommandElement& f);

 private:
  friend class RLModule;
  friend class MappedRLModule;

  typedef std::pair<int, int> Property;
  typedef std::vector<Property> PropertyList;

  // Searches for a property on this object.
  PropertyList::iterator FindProperty(int property) const;

  // Our properties (for the number of properties O(n) is faster than O(log
  // n)...)
  std::unique_ptr<PropertyList> property_list_;

  // The module that owns us (we ask it for properties).
  RLModule* module_;

  // The human readable name for this operation
  std::string name_;
};

// Type definition for a Constant integer value.
//
// This struct is used to define the parameter types of a RLOperation
// subclass, and should not be used directly. It should only be used
// as a template parameter to one of those classes, or of another type
// definition struct.
struct IntConstant_T {
  // The output type of this type struct
  typedef int type;

  // Convert the incoming parameter objects into the resulting type
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position);

  // Parse the raw parameter string and put the results in ExpressionPiece
  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output);

  enum { is_complex = false };
};

// Type definition for a constant string value.
//
// This struct is used to define the parameter types of a RLOperation
// subclass, and should not be used directly. It should only be used
// as a template parameter to one of those classes, or of another type
// definition struct.
struct StrConstant_T {
  // The output type of this type struct
  typedef std::string type;

  // Convert the incoming parameter objects into the resulting type
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position);

  // Parse the raw parameter string and put the results in ExpressionPiece
  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output);

  enum { is_complex = false };
};

struct empty_struct {};

// Defines a null type for the Special parameter.
struct Empty_T {
  typedef empty_struct type;

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position) {
    return empty_struct();
  }

  // Parse the raw parameter string and put the results in ExpressionPiece
  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {}

  enum { is_complex = false };
};

// Implements a special case operation. This should be used with
// things that don't follow the usually function syntax in the
// bytecode, such as weird gotos, et cetera.
//
// RLOp_SpecialCase gives you complete control of the Dispatch,
// performing no type checking, no parameter conversion, and no
// implicit instruction pointer advancement.
//
// @warning This is almost certainly not what you want. This is only
// used to define handlers for CommandElements that aren't
// FunctionElements. Meaning the Gotos and Select. Also, you get to do
// weird tricks with the
class RLOp_SpecialCase : public RLOperation {
 public:
  // Empty function defined simply to obey the interface
  virtual void Dispatch(
      RLMachine& machine,
      const libreallive::ExpressionPiecesVector& parameters) override;

  virtual void DispatchFunction(RLMachine& machine,
                                const libreallive::CommandElement& f) override;

  // Default implementation that simply parses everything as data;
  // doesn't work in the case of complex expressions.
  virtual void ParseParameters(
      const std::vector<std::string>& input,
      libreallive::ExpressionPiecesVector& output) override;

  // Method that is overridden by all subclasses to implement the
  // function of this opcode
  virtual void operator()(RLMachine&, const libreallive::CommandElement&) = 0;
};

// Base class for all the normal operations; This is the third
// revision of this part of the type system. It was revised into two
// monolithic classes, RLOp_Void<> and RLOp_Store<>, but that got a
// bit wacky with what was a big set of if/else if statements
// Dispatching to every possilbe operator(). It also suffered the
// weakeness that each derived struct had a vtable with 26*4 entries
// for all the possible number of operations, most of which have a
// default implementation that just throws an exception; it's possible
// to have a mismatch between the stated function signature and the
// overloaded method.
//
// This implementation doesn't suffer that. Parameter mismatch between
// the type definition and the operator()() implementation is a
// compile time error now. (While porting from the old system to the
// new, I discovered and fixed several of these errors.) The vtable
// isn't hueg liek xbox anymore.
template <typename A = Empty_T,
          typename B = Empty_T,
          typename C = Empty_T,
          typename D = Empty_T,
          typename E = Empty_T,
          typename F = Empty_T,
          typename G = Empty_T,
          typename H = Empty_T,
          typename I = Empty_T,
          typename J = Empty_T,
          typename K = Empty_T,
          typename L = Empty_T,
          typename M = Empty_T,
          typename N = Empty_T,
          typename O = Empty_T,
          typename P = Empty_T,
          typename Q = Empty_T,
          typename R = Empty_T,
          typename S = Empty_T,
          typename T = Empty_T,
          typename U = Empty_T,
          typename V = Empty_T,
          typename W = Empty_T,
          typename X = Empty_T,
          typename Y = Empty_T,
          typename Z = Empty_T>
struct RLOp_NormalOperation : public RLOperation {
 public:
  virtual void ParseParameters(
      const std::vector<std::string>& input,
      libreallive::ExpressionPiecesVector& output) override;
};

namespace internal {

// Machinery for doing recursion in ParseParameters.
struct _sentinel_type {};

template <typename T>
void ParseEachParameter(
    unsigned int& position,
    const std::vector<std::string>& input,
    libreallive::ExpressionPiecesVector& output,
    typename std::enable_if<std::is_same<T, _sentinel_type>::value,
    int>::type* dummy = nullptr) {
  // The recursive base case does nothing.
}

template <typename T, typename... Args>
void ParseEachParameter(
    unsigned int& position,
    const std::vector<std::string>& input,
    libreallive::ExpressionPiecesVector& output,
    typename std::enable_if<!std::is_same<T, _sentinel_type>::value,
    int>::type* dummy = nullptr) {
  T::ParseParameters(position, input, output);
  ParseEachParameter<Args...>(position, input, output);
}

// This should really be in the stdlib.
template<int...>
struct index_tuple{};

template<int I, typename IndexTuple, typename... Types>
struct make_indexes_impl;

template<int I, int... Indexes, typename T, typename ... Types>
struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...> {
  typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>,
                                     Types...>::type type;
};

template<int I, int... Indexes>
struct make_indexes_impl<I, index_tuple<Indexes...> > {
    typedef index_tuple<Indexes...> type;
};

template<typename ... Types>
struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...>
{};

}  // namespace internal

template <typename... Args>
class RLOpcode : public RLOperation {
 public:
  virtual void ParseParameters(
      const std::vector<std::string>& input,
      libreallive::ExpressionPiecesVector& output) override {
    unsigned int position = 0;
    internal::ParseEachParameter<Args..., internal::_sentinel_type>(
        position, input, output);
  }

  virtual void Dispatch(
      RLMachine& machine,
      const libreallive::ExpressionPiecesVector& parameters) {
    // The following does not work in gcc 4.8.2, but it's supposed to!
    // Parameter unpacking inside an initializer-clause is supposed to always
    // be evaluated in the order it appears.
    //
    // http://stackoverflow.com/questions/12048221/c11-variadic-template-function-parameter-pack-expansion-execution-order
    unsigned int position = 0;
    std::tuple<typename Args::type...> tuple =
        std::tuple<typename Args::type...>{
      Args::getData(machine, parameters, position)...
    };
    DispatchImpl(machine, tuple,
                 typename internal::make_indexes<Args...>::type());
  }

  virtual void operator()(RLMachine&, typename Args::type...) = 0;

 private:
  template<int... Indexes>
  void DispatchImpl(RLMachine& machine,
                    const std::tuple<typename Args::type...>& args,
                    internal::index_tuple<Indexes...>) {
    operator()(machine, std::get<Indexes>(args)...);
  }
};


// Partial specialization for RLOp_Normal::check_types for when
// everything is empty (aka an operation that takes no parameters)
template <>
inline void RLOp_NormalOperation<
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T,
    Empty_T>::ParseParameters(const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {}

template <typename A,
          typename B,
          typename C,
          typename D,
          typename E,
          typename F,
          typename G,
          typename H,
          typename I,
          typename J,
          typename K,
          typename L,
          typename M,
          typename N,
          typename O,
          typename P,
          typename Q,
          typename R,
          typename S,
          typename T,
          typename U,
          typename V,
          typename W,
          typename X,
          typename Y,
          typename Z>
void RLOp_NormalOperation<
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z>::ParseParameters(const std::vector<std::string>& input,
                        libreallive::ExpressionPiecesVector& output) {
  unsigned int position = 0;
  A::ParseParameters(position, input, output);
  B::ParseParameters(position, input, output);
  C::ParseParameters(position, input, output);
  D::ParseParameters(position, input, output);
  E::ParseParameters(position, input, output);
  F::ParseParameters(position, input, output);
  G::ParseParameters(position, input, output);
  H::ParseParameters(position, input, output);
  I::ParseParameters(position, input, output);
  J::ParseParameters(position, input, output);
  K::ParseParameters(position, input, output);
  L::ParseParameters(position, input, output);
  M::ParseParameters(position, input, output);
  N::ParseParameters(position, input, output);
  O::ParseParameters(position, input, output);
  P::ParseParameters(position, input, output);
  Q::ParseParameters(position, input, output);
  R::ParseParameters(position, input, output);
  S::ParseParameters(position, input, output);
  T::ParseParameters(position, input, output);
  U::ParseParameters(position, input, output);
  V::ParseParameters(position, input, output);
  W::ParseParameters(position, input, output);
  X::ParseParameters(position, input, output);
  Y::ParseParameters(position, input, output);
  Z::ParseParameters(position, input, output);
}
struct RLOp_Void_Void : public RLOp_NormalOperation<> {
  virtual void Dispatch(
      RLMachine& machine,
      const libreallive::ExpressionPiecesVector& parameters) override;

  virtual void operator()(RLMachine&) = 0;
};

#endif  // SRC_MACHINE_RLOPERATION_H_
