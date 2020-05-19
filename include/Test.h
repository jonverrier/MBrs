
/////////////////////////////////////////
// Test.h
// Contains helper functions for testing
/////////////////////////////////////////

// Helper function that tests copy contruction, assignment
// a1 and a2 must be different instances
template<class AType>	
void 
testConstructionAndCopy (const AType& a1, const AType& a2)
{
   AType copy1 (a1);
   AType copy2 (a2);

   // Check equality operators
   Assert::IsTrue(a1 == a1);     // self test
   Assert::IsTrue (! (a1 != a1)); // self test
   Assert::IsTrue(a1 == copy1);
   Assert::IsTrue(a2 == copy2);
   Assert::IsTrue (a1 != a2);
   Assert::IsTrue (!(a1 == a2));

   // Check assignment, including self assignment
   copy1 = a1;
   copy2 = a2;
   Assert::IsTrue(a1 == copy1);
   Assert::IsTrue(a2 == copy2);
   copy1 = copy1;
   copy2 = copy2;
   Assert::IsTrue(a1 == copy1);
   Assert::IsTrue(a2 == copy2);
};

// Helper function that tests copy contruction, assignment, stream in/out
// a1 and a2 must be different
template<class AType>	
void 
testStandardBehavior (const AType& a1, const AType& a2)
{
   AType copy1 (a1);
   AType copy2 (a2);

   testConstructionAndCopy (copy1, copy2);
};

// Helper function that tests relational operators
// a1 and a2 must be the same, a3 must be greater
template<class AType>	void
testRelationalOperators(const AType& a1, const AType& a2, const AType& a3)
{
   Assert::IsTrue(a1 == a2);
   Assert::IsTrue(a3 != a1);
   Assert::IsTrue(a3 != a2);

   Assert::IsTrue(a1 < a3);
   Assert::IsTrue(a1 <= a3);
   Assert::IsTrue(a1 <= a2);

   Assert::IsTrue(a3 > a1);
   Assert::IsTrue(a3 >= a1);
   Assert::IsTrue(a2 >= a1);

   // Check self comparison
   Assert::IsTrue(!(a2 < a2));
   Assert::IsTrue(!(a2 > a2));
   Assert::IsTrue(a2 == a2);
};

