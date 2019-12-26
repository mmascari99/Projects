 -- Author: Michael Mascari, mmascari2017@my.fit.edu
 -- Course: CSE 4250, Fall 2019
 -- Project: Project #3, Expression Evaluation
 -- This program compiles with gnat version :
 -- GNAT 7.4.0
 -- GNAT GPL 2017 (20170515-63)

with Ada.Containers.Doubly_Linked_Lists;
use Ada.Containers;

generic
   type Element_T is private;
package Stack is
   procedure Push(X: Element_T);
   function Pop return Element_T;
   Stack_Empty : exception;
end Stack;
