static void
TestsInIncludedFile()
{
	int a = 1, b = 2;

	TestGroup("Full test suite")
	{
		Test(a == b);
		Test(a != b);
		TestOp(a, <=, b);
	} EndTestGroup();
}
