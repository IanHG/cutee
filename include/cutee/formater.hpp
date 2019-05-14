#ifndef CUTEE_FORMATER_HPP_INCLUDED
#define CUTEE_FORMATER_HPP_INCLUDED

#include <map>
#include <string>
#include <functional>
#include <regex>

namespace cutee
{

namespace detail
{

// Remove whitespace from string in-place
static inline void remove_whitespace(std::string& s)
{
   s.erase
      (  std::remove_if
         (  s.begin()
         ,  s.end()
         ,  [](char &c) 
            {
			      return std::isspace<char>(c, std::locale::classic());
			   }
         )
      ,  s.end()
      );
}

// Remove whitespace from string copy
static inline std::string remove_whitespace_copy(std::string s)
{
   remove_whitespace(s);
   return s;
}

} /* namespace detail */

/**
 * Formater symbol replacer
 **/
struct symbol_replacer
{
   using key_t      = std::string;
   using function_t = std::function<const char*()>;
   using map_t      = std::map<key_t, function_t>;

   map_t       _map;
   bool        _remove_whitespace{true};
   std::regex  _regex            {"\\[/(.*?)\\]"};

   std::string& replace_in_string(std::string& str) const
   {
      try 
      {
         std::smatch match;
         typename map_t::const_iterator iter;
         
         while (std::regex_search(str, match, _regex))
         {
            auto key = _remove_whitespace ? detail::remove_whitespace_copy(match.str(1)) : match.str(1);
            if(!key.empty() && (iter = _map.find(key)) != _map.end())
            {
               str.replace(match.prefix().second, match.suffix().first, (iter->second)());
            }
            else
            {
               str.erase(match.prefix().second, match.suffix().first);
            }
         }
      } 
      catch (std::regex_error& e) 
      {
         // Syntax error in the regular expression
         std::cerr << " symbol_replacer : regex error !" << std::endl;
         throw;
      } 

      return str;
   }

   std::string replace_in_string(std::string&& str) const
   {
      return replace_in_string(str);
   }
   
   std::string replace_in_string_copy(std::string str) const
   {
      return replace_in_string(str);
   }
};


/**
 * Formater
 **/
struct formater
   :  public symbol_replacer
{
   formater()
      :  symbol_replacer
         {  symbol_replacer::map_t
            {  {"bold_on" ,      [this](){ return this->bold_on() ; } }
            ,  {"bold_off",      [this](){ return this->bold_off(); } }
            ,  {"default_color", [this](){ return this->default_color(); } }
            ,  {"warning_color", [this](){ return this->warning_color(); } }
            ,  {"file_color",    [this](){ return this->file_color(); } }
            ,  {"type_color",    [this](){ return this->type_color(); } }
            ,  {"name_color",    [this](){ return this->name_color(); } }
            }
         }
   {
   }

   virtual const char* bold_on() const = 0;
   
   virtual const char* bold_off() const = 0;
   
   virtual const char* default_color() const = 0;
   
   virtual const char* warning_color() const = 0;
   
   virtual const char* file_color() const = 0;

   virtual const char* type_color() const = 0;
   
   virtual const char* name_color() const = 0;
};

template<class T>
struct formater_middleware
   :  public formater
{
   const T& self() const
   {
      return static_cast<const T&>(*this);
   }

   const char* bold_on() const
   {
      return self()._bold_on;
   }
   
   const char* bold_off() const
   {
      return self()._bold_off;
   }
   
   const char* default_color() const
   {
      return self()._default_color;
   }
   
   const char* warning_color() const
   {
      return self()._warning_color;
   }
   
   const char* file_color() const
   {
      return self()._file_color;
   }

   const char* type_color() const
   {
      return self()._type_color;
   }
   
   const char* name_color() const
   {
      return self()._name_color;
   }
};

struct fancy_formater
   :  public formater_middleware<fancy_formater>
{
   const char _bold_on [8] = "\e[1m\0";
   const char _bold_off[8] = "\e[0m\0";
   
   const char _default_color[8] = "\e[39m\0";
   const char _warning_color[8] = "\e[31m\0";
   const char _type_color   [8] = "\e[32m\0";
   const char _file_color   [8] = "\e[33m\0";
   const char _name_color   [8] = "\e[34m\0";
};

struct raw_formater
   :  public formater_middleware<raw_formater>
{
   const char _bold_on [2] = "\0";
   const char _bold_off[2] = "\0";
   
   const char _default_color[2] = "\0";
   const char _warning_color[2] = "\0";
   const char _type_color   [2] = "\0";
   const char _file_color   [2] = "\0";
   const char _name_color   [2] = "\0";
};

struct format
{
   enum value : int { fancy, raw };

   static std::unique_ptr<formater> create(const format& form)
   {
      switch(form._value)
      {
         case fancy:
            return std::unique_ptr<formater>{ new fancy_formater{} };
         case raw:
            return std::unique_ptr<formater>{ new raw_formater{} };
      }

      return std::unique_ptr<formater>{nullptr};
   }

   constexpr format(value v) : _value(v) 
   { 
   }

   value _value;
};

} /* namespace cutee */

#endif /* CUTEE_FORMATER_HPP_INCLUDED */
