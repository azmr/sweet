#include <stdio.h>
#include "sweet.h"
#include "sweet2.c"

#define OffsetOf(type, el) (unsigned long long)(&((type *)0)->el)
typedef struct t
{
	int val[3];
	struct t* ptr;
} t;

static void
SameFileTests()
{
	NewTestGroup("Same file function tests");
	{
		TestOp(sizeof(char*), >=, sizeof(int));
	} EndNewTestGroup;
}

static void
SameFileUnhitTests()
{
	if(0)
	{
		TestGroup("Same file function unhit tests");
		{
			TestOp(sizeof(char*), >=, sizeof(int));
		} EndTestGroup;
	}
}

int
main(int argc, char **argv)
{
	t T1 = {{0}};
	t T2 = {{0}};
	t T3 = {{0}};
	T1.val[1] = 8;
	T2.val[1] = 8;
	T3.val[1] = 7;
	TestEq(T1, T2);
	TestEq(T1, T3);
	int x = 1;
	int y = 1;
	TestGroup("Single Test");
	{
		TestOp(x, >=, y);
		SameFileUnhitTests();
		SameFileTests();
	} EndTestGroup;
	TestGroup("Full test suite")
	{
		SkipTestGroup("Skip group")
		{
			Test(x == y);
			TestOp(x, !=, y);
			SkipTestVEq(x, y);
		} EndTestGroup;
		TestGroup("Group of tests...")
		{
			SkipTest(x == y);
			SkipTestEq(x, y);
			SkipTestOp(x, ==, y);
			SkipTestVEq(OffsetOf(t, ptr), 4*sizeof(int));
			SkipTestEq(x, y);

			Test(x == y);
			TestEq(x, y);
			TestOp(x, >=, y);
			TestVEq(OffsetOf(t, ptr), 4*sizeof(int));
			TestGroup("Var tests")
			{
				int Offset = OffsetOf(t, ptr);
				int isize = 3*sizeof(int);
				TestEq(Offset, isize);
			} EndTestGroup;
		}
	} EndTestGroup;
	TestOp(x, <=, y);
	TestsInIncludedFile();


	return PrintTestResults();
}

SWEET_END_TESTS;
