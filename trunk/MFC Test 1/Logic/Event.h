#pragma once

#include "Common.h"
#include <functional>

namespace Logic
{


   //void simpleEventTest();
   
   
   template<typename ...Args>
   class VargDelegate
   {
   public:
      typedef VargDelegate<Args...>  Base;

      VargDelegate() : Disconnected(false)
      {}
      virtual ~VargDelegate()
      {}

      virtual void Invoke(Args...) PURE;

      void OnDisconnect()
      {
         Disconnected = true;
      }

   protected:
      bool  Disconnected;
   };


   template<typename ...Args>
   class Event;

   template<typename Type, typename ...Args>
   class MethodDelegate : public VargDelegate<Args...>
   {
   private:
      typedef void(Type::*Member)(Args...);
      typedef Event<Args...>  EventType;

   public:
      MethodDelegate(EventType& ev, Type* instance, Member method) 
         : Event(ev), Object(instance), Method(method)
      {
         REQUIRED(instance);
         REQUIRED(method);
      }
      ~MethodDelegate()
      {
         if (!Disconnected)
            Event.UnRegister(this);
      }

      void Invoke(Args... args)
      {
         if (!Disconnected)
            (Object->*Method)(args...);
      }


   private:
      Type*       Object;
      Member      Method;
      EventType&  Event;
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
      ~Event() 
      {
         for (auto& obj : Listeners)
            obj->OnDisconnect();
      }

   public:
      
      void Raise(Args... args)
      {
         for (auto& obj : Listeners)
            obj->Invoke(args...);
      }

   public:

      template <typename Type, typename Member>
      DelegatePtr Register(Type* instance, Member method)
      {
         REQUIRED(instance);
         REQUIRED(method);

         // Create/register delegate
         Delegate* p = new MethodDelegate<Type, Args...>(*this, instance, method);
         Listeners.push_back(p);

         // Return smart ptr
         return DelegatePtr(p);
      }

      void UnRegister(Delegate* p)
      {
         REQUIRED(p);

         Listeners.remove(p);
      }

   private:
      list<Delegate*> Listeners;
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