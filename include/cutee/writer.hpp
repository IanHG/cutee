#pragma once
#ifndef CUTEE_WRITER_HPP_INCLUDED
#define CUTEE_WRITER_HPP_INCLUDED

#include <iostream>
#include <memory>
#include <string>
#include <tuple>

#include "formater.hpp"

namespace cutee
{

struct writer
{
   virtual ~writer() = default;
   virtual void write(const std::string& msg) const = 0;
};

struct formated_writer
   :  public writer
{
   using formater_ptr_t = typename format::formater_ptr_t;
   
   std::ostream&  _os         = std::cout;
   formater_ptr_t _formater   = formater_ptr_t{nullptr};

   formated_writer(std::ostream& os, const format& form)
      :  _os(os)
      ,  _formater(format::create(form))
   {
   }

   formated_writer(std::tuple<std::ostream&, const format&>&& t)
      :  formated_writer(std::get<0>(t), std::get<1>(t))
   {
   }

   void write(const std::string& msg) const
   {
      _os << this->_formater->replace_in_string_copy(msg);
   }
};

struct writer_collection
   :  public writer
{
   std::vector<std::unique_ptr<writer> > _writers;

   template<class... Ts>
   writer_collection(Ts&&... ts)
   {
      initialize(std::forward<Ts>(ts)...);
   }

   template<class T, class... Ts>
   void initialize(T&& t, Ts&&... ts)
   {
      _writers.emplace_back(std::unique_ptr<writer>{new formated_writer{std::get<0>(t), std::get<1>(t)}});
      initialize(std::forward<Ts>(ts)...);
   }
   
   void initialize()
   {
   }

   void write(const std::string& msg) const
   {
      for(const auto& w : _writers)
      {
         w->write(msg);
      }
   }
};

} /* namespace cutee */

#endif /* CUTEE_WRITER_HPP_INCLUDED */
