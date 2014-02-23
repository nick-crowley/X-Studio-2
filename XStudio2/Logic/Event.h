#pragma once

#include "Common.h"
#include <functional>

namespace Logic
{
   /// <summary>Empty function declspec</summary>
   #define handler       

   /// <summary>Forward declaration</summary>
   template<typename ...Args>
   class Event;
   
   /// <summary>Abstract base for delegates, only constrained by the type of the method arguments and not its containing class</summary>
   /// <typeparam name="Args">Member function argument types</typeparam>
   template<typename ...Args>
   class VargDelegate
   {
      friend class Event<Args...>;

      // ------------------------ TYPES --------------------------
   
      // --------------------- CONSTRUCTION ----------------------

   public:
      VargDelegate() : Connected(true)
      {}
      virtual ~VargDelegate()
      {}

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------	
   public:
      virtual void Invoke(Args...) const PURE;

      // ----------------------- MUTATORS ------------------------
   private:
      /// <summary>Called by an event to indicate it has fallen out of scope, and all delegates are now invalid</summary>
      void OnDisconnect()
      {
         Connected = false;
      }

      // -------------------- REPRESENTATION ---------------------

   protected:
      bool  Connected;
   };


   /// <summary>Delegate for invoking class instance methods</summary>
   /// <typeparam name="Type">Type of class containing the member function to invoke</typeparam>
   /// <typeparam name="Args">Member function argument types</typeparam>
   template<typename Type, typename ...Args>
   class MethodDelegate : public VargDelegate<Args...>
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Class method function pointer</summary>
      typedef void(Type::*Member)(Args...);

      /// <summary>Type of event to handle</summary>
      typedef Event<Args...>  EventType;

      /// <summary>Only the event can create this class</summary>
      friend class EventType;

      // --------------------- CONSTRUCTION ----------------------
   private:
      /// <summary>Creates a delegate for an event handler</summary>
      /// <param name="ev">The event that invokes the delegate</param>
      /// <param name="instance">The instance that will be invoked</param>
      /// <param name="method">Pointer to the member function to invoke</param>
      MethodDelegate(EventType& ev, Type* instance, Member method) : Event(ev), Object(instance), Method(method)
      {
         REQUIRED(instance);
         REQUIRED(method);
      }

   public:
      /// <summary>Detach from the event upon destruction, unless event has already fallen from scope</summary>
      ~MethodDelegate()
      {
         if (Connected)
            Event.UnRegister(this);
      }

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Invoke the delegate</summary>
      /// <param name="args">Arguments</param>
      void Invoke(Args... args) const
      {
         if (Connected)
            (Object->*Method)(args...);
      }

      // -------------------- REPRESENTATION ---------------------
   private:
      Type*       Object;
      Member      Method;
      EventType&  Event;
   };


   /// <summary>An event able to invoke class member functions with any signature. 
   /// Automatically handles disconnection if either party falls out of scope</summary>
   /// <typeparam name="Args">Event handler argument types</typeparam>
   template<typename ...Args>
   class Event
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Event delegate with matching signature (but no class type)</summary>
      typedef VargDelegate<Args...>  DelegateBase;

   public:
      /// <summary>Delegate smart pointer</summary>
      typedef shared_ptr<DelegateBase>  DelegatePtr;

      /// <summary>Friendly name for a delegate</summary>
      typedef DelegatePtr  Handler;
      
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Disconnects any remaining listeners</summary>
      ~Event()
      {
         for (auto& obj : Listeners)
            obj->OnDisconnect();
      }

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      /// <summary>Raises the event</summary>
      /// <param name="args">Arguments</param>
      void Raise(Args... args) const
      {
         for (auto& obj : Listeners)
            obj->Invoke(args...);
      }

      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Registers a class method to handle this event</summary>
      /// <typeparam name="Type">Type of class that will handle the event</typeparam>
      /// <typeparam name="Member">Type of member function that will handle the event</typeparam>
      /// <param name="instance">The instance that will handle the event</param>
      /// <param name="method">Pointer to the member function that will handle event</param>
      /// <returns>Delegate smart pointer</returns>
      template <typename Type, typename Member>
      DelegatePtr Register(Type* instance, Member method)
      {
         REQUIRED(instance);
         REQUIRED(method);

         // Create/register delegate
         DelegateBase* p = new MethodDelegate<Type, Args...>(*this, instance, method);
         Listeners.push_back(p);

         // Return smart ptr
         return DelegatePtr(p);
      }

      /// <summary>Unregisters a delegate</summary>
      /// <param name="p">The delegate</param>
      void UnRegister(DelegateBase* p)
      {
         REQUIRED(p);

         // Remove/detatch delegate
         Listeners.remove(p);
         p->OnDisconnect();
      }

      // -------------------- REPRESENTATION ---------------------
   private:
      list<DelegateBase*> Listeners;
   };

   /// <summary>Event with no arguments</summary>
   typedef Event<>                   SimpleEvent;

   /// <summary>Handler for event with no arguments</summary>
   typedef SimpleEvent::DelegatePtr  EventHandler;
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