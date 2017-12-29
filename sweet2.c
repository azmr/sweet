static void
TestsInIncludedFile()
{
	int a = 1, b = 2;

	NewTestGroup("Included test suite")
	{
		Test(a == b);
		Test(a != b);
		TestOp(a, <=, b);
	} EndNewTestGroup;
}
