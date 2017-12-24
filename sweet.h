#ifndef SWEET_H

#define Equal(a, b) (sizeof(a) <= sizeof(double) ? ((a) == (b)) : \
					sizeof(a) != sizeof(b) ? 0 : \
					Equal_(&(a), &(b), sizeof(a)))
static int
Equal_(void *p1, void *p2, int n)
{
	int Result = 1;
	char *u1 = (char *)p1, *u2 = (char *)p2;
	for(int i = 0; i < n; ++i) { if(u1[i] != u2[i]) { Result = 0; } }
	return Result;
}

#ifndef PRINTFN
#define PRINTFN(...) printf(__VA_ARGS__)
#endif// PRINTFN

#define SWEET_ADDSKIP(i, m) \
	Tests[i].Parent=GlobalTestSweetParent; \
	Tests[i].Level=Tests[GlobalTestSweetParent].Level+1; \
	Tests[i].Status=SWEET_STATUS_Skip; \
	Tests[i].Message=m
#define SWEET_ADDTEST(i, s, m) \
	Tests[i].Parent=GlobalTestSweetParent; \
	Tests[i].Level=Tests[GlobalTestSweetParent].Level+1; \
	Tests[i].Status=!!(s); \
	Tests[i].Message=m
 /* 0-1 = fail/pass - skip dealt with elsewhere */
#define Test(exp)            do{SWEET_ADDTEST(__COUNTER__, exp,        #exp         );}while(0)
#define TestOp(a, op, b)     do{SWEET_ADDTEST(__COUNTER__, a op b,     #a" "#op" "#b);}while(0)
#define TestEq(a, e)		 do{SWEET_ADDTEST(__COUNTER__, Equal(a,e), #a" == "#e   );}while(0)
#define TestVEq(a, e)        do{SWEET_ADDTEST(__COUNTER__, (a) == (e), #a" == "#e   );}while(0)
#define SkipTest(exp)        do{SWEET_ADDSKIP(__COUNTER__,             #exp         );}while(0)
#define SkipTestOp(a, op, b) do{SWEET_ADDSKIP(__COUNTER__,             #a" "#op" "#b);}while(0)
#define SkipTestEq(a, e)     do{SWEET_ADDSKIP(__COUNTER__,             #a" == "#e   );}while(0)
#define SkipTestVEq(a, e)    do{SWEET_ADDSKIP(__COUNTER__,             #a" == "#e   );}while(0)

#define TestGroup_(i, m) SWEET_ADDTEST(i, 1, m); GlobalTestSweetParent = i
#define TestGroup(m) do{TestGroup_(__COUNTER__, m);}while(0);
#define SkipTestGroup_(i, m) SWEET_ADDSKIP(i, m); GlobalTestSweetParent = i
#define SkipTestGroup(m) do{SkipTestGroup_(__COUNTER__, m);}while(0);
#define EndTestGroup() do{GlobalTestSweetParent=Tests[GlobalTestSweetParent].Parent;}while(0)

#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_RESET   "\x1b[0m"

typedef enum test_sweet_status
{
	SWEET_STATUS_Fail = 0,
	SWEET_STATUS_Pass,
	SWEET_STATUS_Skip,

	SWEET_STATUS_Count,
} test_sweet_status;

struct test;
typedef struct test test;
struct test
{
	char *Message;
	unsigned int Parent;
	unsigned int Level;
	int Status;
};

static unsigned int GlobalTestSweetParent = __COUNTER__;

test Tests[];

static inline int
IsGroupOfTests(unsigned int i)
{
	int Result = Tests[i+1].Parent == i;
	return Result;
}

// returns number of failed atomic tests (doesn't count groups)
#define PrintTestResults() PrintTestResults_(Tests, __COUNTER__)
static int
PrintTestResults_(test *Tests, unsigned int cTests)
{
	Tests[0].Message = "Overall Tests";
	Tests[0].Status = SWEET_STATUS_Pass;

	unsigned int cFail = 0, cPass = 0;
	puts("\n");
	for(unsigned int i = 1; i < cTests; ++i)
	{ // update skip/fail status based on others in hierarchy
		test Test = Tests[i];
		switch(Test.Status)
		{
			case SWEET_STATUS_Fail:
			{ // propogate failures up the hierarchy
				unsigned int iParent = i;
				++cFail; // only individual tests (not groups) fail at this point
				while(iParent)
				{
					/* printf("fail: %u\n", iParent); */
					iParent = Tests[iParent].Parent;
					Tests[iParent].Status = SWEET_STATUS_Fail;
				}
			} break;

			case SWEET_STATUS_Pass: if(!IsGroupOfTests(i)) { ++cPass; } break;

			case SWEET_STATUS_Skip:
			{ // propogate skipping down the hierarchy
				unsigned int iChild = i;
				printf("skip: %u\n", iChild);
				while(Tests[++iChild].Parent >= i)
				{ Tests[iChild].Status = SWEET_STATUS_Skip; }
			} break;

			default: printf(ANSI_MAGENTA "ERROR: UNKNOWN STATUS at %u" ANSI_RESET, i);
		}
	}

	unsigned int PrevLevel = 0;
	unsigned int cL1Fail = 0, cL1Pass = 0;
	for(unsigned int i = 1; i < cTests; ++i)
	{ // print out status
		test Test = Tests[i];
		int cIndents = 0;
		int iParent = i;
		while(iParent)// && cIndents < sizeof(Tests)/sizeof(*Tests))
		{ // Count number of parents
			++cIndents;
			iParent = Tests[iParent].Parent;
			/* if( ! Tests[i].Success) {Tests[iParent].Success = 0;} */
		}
		if(Test.Status == SWEET_STATUS_Pass)
		{
			if(Test.Parent == 0) { ++cL1Pass; }
		}
		if(Test.Parent == 0 && Test.Status == SWEET_STATUS_Fail) { ++cL1Fail; }

		char TestStatus, *TestColour;
		switch(Test.Status)
		{
			case SWEET_STATUS_Fail:
				TestStatus = 'X'; TestColour = ANSI_RED;    break;
			case SWEET_STATUS_Pass:
				TestStatus = '/'; TestColour = ANSI_GREEN;  break;
			case SWEET_STATUS_Skip:
				TestStatus = '-'; TestColour = ANSI_YELLOW; break;
			default:
				TestStatus = '?'; TestColour = ANSI_MAGENTA "ERROR: UNKNOWN STATUS! ";
		}

		PRINTFN("%*.d%s[%c] %s"ANSI_RESET"\n", 4*(cIndents-1),0, TestColour, TestStatus, Test.Message);
		PrevLevel = Test.Level;
	}
	puts("\n");
	return cFail;
}

#define SWEET_END_TESTS test Tests[__COUNTER__ - 1]

#define SWEET_H
#endif//SWEET_H
