 -- Author: Michael Mascari, mmascari2017@my.fit.edu
 -- Course: CSE 4250, Fall 2019
 -- Project: Project #3, Expression Evaluation
 -- This program compiles with gnat version :
 -- GNAT 7.4.0
 -- GNAT GPL 2017 (20170515-63)

with Ada.Containers.Doubly_Linked_Lists;
use Ada.Containers;

package body Stack is

   package Float_Container is new Doubly_Linked_Lists(Element_T);
   use Float_Container;
   The_Stack: List;
   procedure Push(X: Element_T) is
   begin
      Append(The_Stack, X);
   end Push;
   function Pop return Element_T is
      Result: Element_T;
   begin
      if Is_Empty(The_Stack) then
         raise Stack_Empty;
      end if;
      Result := Last_Element(The_Stack);
      Delete_Last(The_Stack);
      return Result;
   end Pop;
end Stack;
