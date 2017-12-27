#include <stdio.h>
#include "../macros/macro.h"
#include "sweet.h"

typedef struct t
{
	int val[3];
	struct t* ptr;
} t;

int
main(int argc, char **argv)
{
	int x = 1;
	int y = 1;
	/* printf("Global parent: %u\n", GlobalTestSweetParent); */
	TestGroup("Single Test");
	TestOp(x, >=, y);
	EndTestGroup();
	TestGroup("Full test suite")
	{
		TestGroup("Skip group")
		{
			Test(x == y);
			TestOp(x, !=, y);
			SkipTestVEq(x, y);
			EndTestGroup();
		}
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
				/* printf("Global parent: %u\n", GlobalTestSweetParent); */
				int Offset = OffsetOf(t, ptr);
				int isize = 3*sizeof(int);
				TestEq(Offset, isize);
				EndTestGroup();
				/* printf("Global parent: %u\n", GlobalTestSweetParent); */
			}
			EndTestGroup();
		}
	} EndTestGroup();

	return PrintTestResults();
}

SWEET_END_TESTS;
