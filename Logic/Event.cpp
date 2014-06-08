#include "stdafx.h"
#include "Event.h"

namespace Logic
{
   namespace Utils
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /*template<typename Type, typename ...Args>
      MethodDelegate::MethodDelegate(Event& ev, Type* instance, Member method) 
            : Event(ev), Object(instance), Method(method)
      {
         REQUIRED(instance);
         REQUIRED(method);
      }

      template<typename Type, typename ...Args>
      MethodDelegate::~MethodDelegate()
      {
         Event.UnRegister(this);
      }*/

   
      //void simpleEventTest()
      //{
      //   MyClass c;
      //   Event<int> ev;

      //   //Event<int>::Handler  easy(&c, &MyClass::method);
      //   /*MethodDelegate<MyClass, Event<int, int>::Signature> test(&c, &MyClass::method);

      //   unique_ptr<MethodDelegate<MyClass,int>> del(new MethodDelegate<MyClass,int>(&c, &MyClass::method));*/

      //   ev.Register<MyClass>(&c, &MyClass::method);
      //   

      //   //ev.Register(new MethodDelegate<MyClass,int>(&c, &MyClass::method));
      //   ev.Raise(42);

      //   ev.UnRegister<MyClass>(&c, &MyClass::method);

      //   ev.Raise(43);

      //   /*Delegate<MyClass,int> rc(&c, &MyClass::method);
      //   rc.Invoke(42);*/
      //}

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   }
}