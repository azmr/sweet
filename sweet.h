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

	// Tests[i].Level=Tests[GlobalTestSweetParent].Level+1; 
#define SWEET_ADDSKIP(i, m) \
	Tests[i].Parent=GlobalTestSweetParent; \
	Tests[i].Status=SWEET_STATUS_Skip; \
	Tests[i].Message=m
#define SWEET_ADDTEST(i, s, m) \
	Tests[i].Parent=GlobalTestSweetParent; \
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

static inline unsigned int
SweetNumParents(unsigned int i)
{
	unsigned int cParents = 0;
	while(i)
	{
		++cParents;
		i = Tests[i].Parent;
	}
	return cParents;
}
static void
SweetGroup(unsigned int i)
{
}

// returns number of failed atomic tests (doesn't count groups)
#define PrintTestResults() PrintTestResults_(Tests, __COUNTER__)
static int
PrintTestResults_(test *Tests, unsigned int cTests)
{
	Tests[0].Message = "Overall Tests";
	Tests[0].Status = SWEET_STATUS_Pass;

	unsigned int cFail = 0, cPass = 0;
	if( ! IsGroupOfTests(1)) { puts(""); }
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
				/* printf("skip: %u\n", iChild); */
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
		if(IsGroupOfTests(i)/*&& Tests[i].Parent != i-1*/) { puts(""); }
		test Test = Tests[i];
		int cIndents = SweetNumParents(i);
		if(Test.Parent == 0 && Test.Status == SWEET_STATUS_Pass) { ++cL1Pass; }
		if(Test.Parent == 0 && Test.Status == SWEET_STATUS_Fail) { ++cL1Fail; }

		char TestStatus, *TestColour;
		switch(Test.Status)
		{ // set status character and colour
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

		unsigned int iParent = Test.Parent;
		int IsFinalTest = i == cTests-1; 
		if(IsFinalTest || Tests[i+1].Parent < iParent) // end of group
		{ // append summary of group
			do // repeat if dropping multiple levels
			{
				unsigned int cGPass = 0, cGFail = 0, iGroup = i;
				while(Tests[iGroup].Parent >= iParent) // includes subtests
				{ // go back through all of group (and their subtests)
					if(Tests[iGroup].Parent == iParent) // does not include subtests
					{ // add the status
						if     (Tests[iGroup].Status == SWEET_STATUS_Pass) ++cGPass;
						else if(Tests[iGroup].Status == SWEET_STATUS_Fail) ++cGFail;
					}
					--iGroup;
				}
				printf("%*.d%sPassed: %u/%u"ANSI_RESET"\n\n", 4*(SweetNumParents(iParent)-1),0,
						cGFail ? ANSI_RED : ANSI_GREEN, cGPass, cGPass + cGFail);
				if(IsFinalTest) { break; }
				iParent = Tests[iParent].Parent;
			}
			while(Tests[i+1].Parent < iParent);
		}

	}
	printf("%sIndividual tests: %u/%u"ANSI_RESET"\n\n", cL1Fail ? ANSI_RED : ANSI_GREEN, cPass, cPass+cFail);
	return cFail;
}

#define SWEET_END_TESTS test Tests[__COUNTER__ - 1]

#define SWEET_H
#endif//SWEET_H
