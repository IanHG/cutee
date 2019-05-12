#ifndef CUTEE_FORMATER_HPP_INCLUDED
#define CUTEE_FORMATER_HPP_INCLUDED

namespace cutee
{

struct formater
{
   virtual const char* bold_on() const = 0;
   
   virtual const char* bold_off() const = 0;
   
   virtual const char* default_color() const = 0;
   
   virtual const char* warning_color() const = 0;
   
   virtual const char* file_color() const = 0;

   virtual const char* type_color() const = 0;
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
