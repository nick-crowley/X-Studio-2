#pragma once

#include "Common.h"
#include <functional>

namespace Logic
{


   void simpleEventTest();
   
   
   template<typename ...Args>
   class VargDelegate
   {
   public:
      typedef VargDelegate<Args...>  Base;

      virtual ~VargDelegate()
      {}

      virtual void Invoke(Args...) PURE;
      virtual bool Equals(Base* r) PURE;
   };




   template<typename Type, typename ...Args>
   class MethodDelegate : public VargDelegate<Args...>
   {
   private:
      typedef void(Type::*Member)(Args...);

   public:
      MethodDelegate(Type* instance, Member method) : Object(instance), Method(method)
      {
         REQUIRED(instance);
         REQUIRED(method);
      }

      virtual bool Equals(Base* r)
      {
         auto r2 = static_cast<decltype(this)>(r);
         return Object == r2->Object && Method == r2->Method;
      }

      virtual void Invoke(Args... args)
      {
         //if (Object != nullptr && Method != nullptr)
            (Object->*Method)(args...);
      }

   private:
      Type*  Object;
      Member Method;
   };



   template<typename ...Args>
   class Event
   {
   public:
      typedef VargDelegate<Args...> Delegate;

   private:
      typedef shared_ptr<Delegate>  DelegatePtr;
      
   public:
      Event() {}
      ~Event() {}

   public:
      
      void Raise(Args... args)
      {
         for (auto& obj : Listeners)
            obj->Invoke(args...);
      }

      template <typename Type, typename Member>
      void Register(Type* t, Member fn)
      {
         REQUIRED(t);
         REQUIRED(fn);

         Register(DelegatePtr( new MethodDelegate<Type, Args...>(t, fn) ));
      }

      template <typename Type, typename Member>
      void UnRegister(Type* t, Member fn)
      {
         REQUIRED(t);
         REQUIRED(fn);

         UnRegister(DelegatePtr( new MethodDelegate<Type, Args...>(t, fn) ));
      }

   private:
      void Register(DelegatePtr p)
      {
         // Ensure not present
         for (DelegatePtr l : Listeners)
            if (l->Equals(p.get()))    
               return;
         
         Listeners.push_back(p);
      }

      void UnRegister(DelegatePtr p)
      {
         Listeners.remove_if([&p](DelegatePtr& l) { return l->Equals(p.get()); } );
      }

   private:
      list<DelegatePtr> Listeners;
   };

}

using namespace Logic;


   // See: http://codereview.stackexchange.com/questions/36251/c-delegate-implementation-with-member-functions
   //
   //template <typename... Params>
   //class Event
   //{
   //private:
   //   
   //   
   //   
   //public:
   //   typedef function<void(Params...)> Delegate;
   //   typedef typename vector<Delegate>::const_iterator cookie;
   //   

   //   cookie connect(Delegate&& func)
   //   {
   //      Listeners.push_back(std::move(func));
   //      return Listeners.begin() + Listeners.size() - 1;
   //   }

   //   /*template <typename... BindArgs, typename Sfinae = typename std::enable_if<(sizeof...(BindArgs)>1), void>::type >
   //   cookie connect(BindArgs&&... args)
   //   {
   //      return connect(Delegate(std::bind(std::forward<BindArgs>(args)...)));
   //   }*/

   //   void disconnect(cookie which)
   //   {
   //      Listeners.erase(which);
   //   }

   //   template <typename ... Args> 
   //   void operator()(Args...args)
   //   {
   //      for (auto const& fn : Listeners)
   //         fn(args...);
   //   }

   //private:
   //   vector<Delegate> Listeners;
   //};

   //class ExampleEvent : public Event<int>
   //{
   //};