 -- Author: Michael Mascari, mmascari2017@my.fit.edu
 -- Course: CSE 4250, Fall 2019
 -- Project: Project #3, Expression Evaluation
 -- This program compiles with gnat version :
 -- GNAT 7.4.0
 -- GNAT GPL 2017 (20170515-63)

with Ada.Containers.Doubly_Linked_Lists;use Ada.Containers;
with Ada.Text_IO;use Ada.Text_IO;
with Ada.Integer_Text_IO;use Ada.Integer_Text_IO;
with Stack;

procedure eval is
   package New_Stack is new Stack(Element_T=>Character);
   use New_Stack;

   --  forgive me for their names, keywords clashed with ideal names for functions
   function Addition return Integer;
   function Min return Integer;
   function Andd return Integer;
   function Orr return Integer;

   Flag: Integer;
   A: Character;
   procedure Read_Stream is
      Length: Natural;
      Line: String(1..10000);
   begin
      Flag := 0; --  Flag defaults to Tropical
      Get_Line(Line, Length); --  read up to 100 characters at a time
      for i in Line(1..Length)'Range loop --  loops over then length of the input line
         if Line(i) = 'B' or Line(i) = 'b' then --  Checks for uppercase and lowercase
            Flag := 1;
            Push(Line(i));
         elsif Line (i) = '*' then --  * operator means convergance of the string
            if Flag = 0 then
               Flag := 2;
               Put("0");
            else
               Flag := 3;
               Put("1");
            end if;
         elsif Line(i) /= ' ' then --  Pushes all input that is not spaces
            Push(Line(i));
         end if;
      end loop;
      if Length = 0 then --  EOF (blank input) means termination of program
         Flag := 4;
      end if;
   end Read_Stream;

   function Addition return Integer is --  tropical function
      a,b: Integer;
      z: Character;
   begin
      z := Pop;
      if z = '.' then
         a := Addition; --  recursion!
      elsif z = '+' then
         a := Min; --  not recursion!
      else
         a := Integer'Value ((1 => z)); --  turns the pop into an integer
      end if;

      z := Pop;
      if z = '.' then
         b := Addition;
      elsif z = '+' then
         b := Min;
      else
         b := Integer'Value ((1 => z)); --  prepares the second integer
      end if;

   --  deals with the idea that 0 is infinity and 1 is 0.
      if a = 0 then
         return 0;
      end if;
      if b = 0 then
         return 0;
      end if;
      if a = 1 then
         a := 0;
      end if;
      if b = 1 then
         b := 0;
      end if;

   --  returns the addition
      a := a + b;
      if a = 0 then
         a := 1;
      end if;
      return a;
   end Addition;

   function Min return Integer is --  tropical function
      a,b: Integer;
      z: Character;
   begin
      z := Pop;
      if z = '.' then
         a := Addition; --  not recursion!
      elsif z = '+' then
         a := Min; --  recursion!
      else
         a := Integer'Value ((1 => z)); --  pop into an integer
      end if;

      z := Pop;
      if z = '.' then
         b := Addition;
      elsif z = '+' then
         b := Min;
      else
         b := Integer'Value ((1 => z)); --  pop into an integer
      end if;

   --  deals with the idea that 0 is infinity and 1 is 0.
      if b = 0 then
         return a;
      elsif b < a then
         a := b;
      end if;
      return a;
   end Min;

   --  had to name it Orr because or is a keyword
   function Orr return Integer is --  boolean function
      a,b: Integer;
      z: Character;
   begin
      z := Pop;
      if z = '.' then
         a := Andd; --  not recursion!
      elsif z = '+' then
         a := Orr; --  recursion!
      else
         a := Integer'Value ((1 => z));
      end if;

      z := Pop;
      if z = '.' then
         b := Andd; --  not recursion!
      elsif z = '+' then
         b := Orr; --  recursion!
      else
         b := Integer'Value ((1 => z));
      end if;

   --  if either bool is larger than 0, or function returns 1
      if a > 0 then
         return 1;
      elsif b > 0 then
         return 1;
      else
         return 0;
      end if;

   end Orr;

   --  had to name it Andd because and is a keyword
   function Andd return Integer is
      a,b: Integer;
      z: Character;
   begin
      z := Pop;
      if z = '.' then
         a := Andd; --  recursion!
      elsif z = '+' then
         a := Orr; --  not recursion!
      else
         a := Integer'Value ((1 => z)); --  pop into an integer so they can be AND
      end if;

      z := Pop;
      if z = '.' then
         b := Andd;
      elsif z = '+' then
         b := Orr;
      else
         b := Integer'Value ((1 => z)); --  pop into an integer so they can be AND
      end if;

   --  if both values are positive, return 1
      if a > 0 then
         if b > 0 then
            return 1;
         else
            return 0;
         end if;
      else
         return 0;
      end if;
   end Andd;

   --  procedure to decide which tropical operation to use
   procedure Tropical is
      I, Z: Integer;
   begin
      A:= Pop;
      while A /= 'Z' and A /= 'z' loop --  checks upper and lowercase
         if A = '+' then
            I := Min;
            Put(I);
         elsif A = '.' then
            I := Addition;
            Put(I);
         else
            Z := Integer'Value ((1 => A));
            Put(Z); --  if input is "Z 7" Output is "7"
         end if;

         if A /= 'Z' and A /= 'z' then --  checks upper and lower case
            A:= Pop;
         end if;

      end loop;
   end Tropical;

   --  procedure to decide which boolean operation to use
   --  Please forgive the procedure name. Boolean is keyword
   procedure Boo is
      I,Z: Integer;
   begin

      A:= Pop;
      while A /= 'B'  and A /= 'b' loop --  checks upper and lower case
         if A = '+' then
            I := Orr;
            Put(I); --  final answer from recursion
         elsif A = '.' then
            I := Andd;
            Put(I); --  final answer from recursion
         else
            Z := Integer'Value ((1 => A));
            if Z > 0 then --  if input is "B 7" output is "1"
               Z := 1;
            end if;
            Put(Z);
         end if;

         if A /= 'B' and A /= 'b' then --  checks upper and lower case
            A:= Pop;
         end if;
      end loop;
   end Boo;

begin
   loop
      Read_Stream;

--  flag is set in Read_Stream procedure
--  Flag is 0 for tropical, 1 for boolean, 2 or 3 for * operator, and 4 for EOF
--  Flag 2 and Flag 3 need to empty the stack before the next line is read in
      if Flag = 0 then
         Tropical;
      elsif Flag = 1 then
         Boo;
      elsif Flag = 2 or Flag = 3 then
         A := Pop;
         if Flag = 2 then
            while A /= 'Z' and A /= 'z' loop
               A := Pop;
            end loop;
         elsif Flag = 3 then
            while A /= 'B' and A /= 'b' loop
               A := Pop;
            end loop;
         end if;
      elsif Flag = 4 then
         return;
      end if;

      Put_Line("");
   end loop;
end eval;
