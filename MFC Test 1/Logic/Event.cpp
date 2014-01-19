#include "stdafx.h"
#include "Event.h"

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   //void something(int a)
   //{
   //}

   class MyClass
   {
   public:
      void method(int a)
      {
         Console << L"Method called with " << a << ENDL;
      }

   };

   //void anything()
   //{
   //   //Event<void (int, int), int, int> test;

   //   function<void (int, bool)> boy;
   //   boy(4, true);

   //   
   //   ExampleEvent test;
   //   MyClass c;

   //   test.connect(ExampleEvent::Delegate(something));

   //   test.connect( ExampleEvent::Delegate( std::bind(&MyClass::method, &c, 5) ) );
   //   test.connect( std::bind(&MyClass::method, &c, std::placeholders::_1) );

   //   test.connect( ExampleEvent::Delegate([](int i) {}) );
   //}


   void simpleEventTest()
   {
      MyClass c;
      Event<int> ev;

      //Event<int>::Handler  easy(&c, &MyClass::method);
      /*MethodDelegate<MyClass, Event<int, int>::Signature> test(&c, &MyClass::method);

      unique_ptr<MethodDelegate<MyClass,int>> del(new MethodDelegate<MyClass,int>(&c, &MyClass::method));*/

      ev.Register<MyClass>(&c, &MyClass::method);
      

      //ev.Register(new MethodDelegate<MyClass,int>(&c, &MyClass::method));
      ev.Raise(42);

      ev.UnRegister<MyClass>(&c, &MyClass::method);

      ev.Raise(43);

      /*Delegate<MyClass,int> rc(&c, &MyClass::method);
      rc.Invoke(42);*/
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------
}
