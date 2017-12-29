#ifndef SWEET_H
/* TODO:
// - option to not print subtests for skipped groups?
// - summary of passes and fails by file
// - define no colour
// - proper stack?
// - Generic for equal and print
// - better message support
*/

#ifndef SWEET_OUTFILE
#define SWEET_OUTFILE stdout
#endif/*SWEET_OUTFILE*/
#ifndef SWEET_STATIC
#define SWEET_STATIC static
#endif/*SWEET_STATIC*/
#ifndef SWEET_INLINE
#define SWEET_INLINE inline
#endif/*SWEET_INLINE*/

#define Equal(a, b) (sizeof(a) == sizeof(b) ? Equal_(&(a), &(b), sizeof(a)) : 0)
SWEET_STATIC int
Equal_(void *p1, void *p2, int n)
{
	int i, Result = 1;
	unsigned char *u1 = (unsigned char *)p1, *u2 = (unsigned char *)p2;
	for(i = 0; i < n; ++i) { if(u1[i] != u2[i]) { Result = 0; } }
	return Result;
}

/* NOTE: i < Parent if in separate file - this just resets to no parent */
#define SWEET_ADDSKIP(i, m) \
	Tests[i].Parent=i > GlobalTestSweetParent ? GlobalTestSweetParent : 0; \
	Tests[i].Filename=__FILE__; \
	Tests[i].Status=SWEET_STATUS_Skip; \
	Tests[i].Message=m
#define SWEET_ADDTEST(i, s, m) \
	Tests[i].Parent=i > GlobalTestSweetParent ? GlobalTestSweetParent : 0; \
	Tests[i].Filename=__FILE__; \
	Tests[i].Status=(s) ? SWEET_STATUS_Pass : SWEET_STATUS_Fail; \
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

#define SweetParentReset() do{GlobalTestSweetParentTmp = GlobalTestSweetParent; GlobalTestSweetParent = 0;}while(0)
#define SweetParentRestore() do{GlobalTestSweetParent = GlobalTestSweetParentTmp;}while(0)

#define TestGroup_(i, m) SWEET_ADDTEST(i, SWEET_STATUS_Pass, m); if(i > GlobalTestSweetParent) GlobalTestSweetParent = i
#define TestGroup(m) do{TestGroup_(__COUNTER__, m);    /** REMEMBER: EndTestGroup **/
#define NewTestGroup(m) SweetParentReset(); TestGroup(m)
#define SkipTestGroup_(i, m) SWEET_ADDSKIP(i, m); GlobalTestSweetParent = i
#define SkipTestGroup(m) do{SkipTestGroup_(__COUNTER__, m);    /** REMEMBER: EndTestGroup **/
#define SkipNewTestGroup(m) SweetParentReset(); SkipTestGroup(m)
#define EndTestGroup GlobalTestSweetParent=Tests[GlobalTestSweetParent].Parent;}while(0)
#define EndNewTestGroup EndTestGroup; SweetParentRestore()
		

#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_WHITE   "\x1b[37m"

typedef enum test_sweet_status
{
	SWEET_STATUS_Undefined = 0,
	SWEET_STATUS_Fail,
	SWEET_STATUS_Pass,
	SWEET_STATUS_Skip
} test_sweet_status;

struct test;
typedef struct test test;
struct test
{
	char *Message;
	char *Filename;
	unsigned int Parent;
	int Status;
};

/* NOTE: leave initial table entry empty */
SWEET_STATIC unsigned int GlobalTestSweetParent = __COUNTER__, GlobalTestSweetParentTmp;

test Tests[];

SWEET_STATIC SWEET_INLINE int
Sweet_IsGroup(unsigned int i)
{
	int Result = Tests[i+1].Parent == i;
	return Result;
}

SWEET_STATIC SWEET_INLINE unsigned int
Sweet_NumParents(unsigned int i)
{
	unsigned int cParents = 0;
	while((i = Tests[i].Parent)) { ++cParents; }
	return cParents;
}

SWEET_STATIC SWEET_INLINE void
Sweet_Indent(unsigned int n)
{
	unsigned int iIndent;
	fputs(ANSI_WHITE, SWEET_OUTFILE);
	for(iIndent = n; iIndent; --iIndent)
	{ fputs("|   ", SWEET_OUTFILE); }
	fputs(ANSI_RESET, SWEET_OUTFILE);
}

SWEET_STATIC SWEET_INLINE void
Sweet_PrintSummary(unsigned int cGPass, unsigned int cGFail, unsigned int cIPass, unsigned int cIFail, unsigned int cMissed)
{
	char *ColourString = cGFail ? ANSI_RED : ANSI_GREEN;
	unsigned int cGTotal = cGPass+cGFail;
	unsigned int cITotal = cIPass+cIFail;
	fprintf(SWEET_OUTFILE, "%sPassed: %u/%u (%5.1f%%)", ColourString,
			cGPass, cGTotal, 100.0*(double)cGPass/(double)cGTotal);
	if(cGPass != cIPass || cGFail != cIFail)
	{ fprintf(SWEET_OUTFILE, "    [ %u/%u (%5.1f%%) ]", cIPass, cITotal, 100.0*(double)cIPass/(double)cITotal); }
	if(cMissed) { fprintf(SWEET_OUTFILE, "    Missed: %u", cMissed); }
	fprintf(SWEET_OUTFILE, ANSI_RESET"\n");
}

SWEET_STATIC SWEET_INLINE void
Sweet_ConditionalStatusInc(unsigned int i, int Condition, unsigned int *cPass, unsigned int *cFail)
{
	if(Condition)
	{
		if	   (Tests[i].Status == SWEET_STATUS_Pass) { ++*cPass; }
		else if(Tests[i].Status == SWEET_STATUS_Fail) { ++*cFail; }
	}
}

/* returns number of failed atomic tests (doesn't count groups) */
#define PrintTestResults() PrintTestResults_(Tests, __COUNTER__)
SWEET_STATIC int
PrintTestResults_(test *Tests, unsigned int cTests)
{
	unsigned int i, iTest, iPrevValid, iParent;
	unsigned int cOGFail = 0, cOGPass = 0, cOIFail = 0, cOIPass = 0, cMissed = 0;
	char TestStatus, *TestColour;
	if( ! Sweet_IsGroup(1)) { fputc('\n', SWEET_OUTFILE); }
	for(i = 1; i < cTests; ++i)
	{ /* update skip/fail status based on others in hierarchy */
		test Test = Tests[i];
		switch(Test.Status)
		{
			case SWEET_STATUS_Undefined:
			case SWEET_STATUS_Pass: /* do nothing */ break;

			case SWEET_STATUS_Fail:
			{ /* propogate failures up the hierarchy */
				unsigned int iParent = i;
				while((iParent = Tests[iParent].Parent))
				{ Tests[iParent].Status = SWEET_STATUS_Fail; }
			} break;

			case SWEET_STATUS_Skip:
			{ /* propogate skipping down the hierarchy */
				unsigned int iChild = i;
				while(Tests[++iChild].Parent >= i)
				{ Tests[iChild].Status = SWEET_STATUS_Skip; }
			} break;

			default: fprintf(SWEET_OUTFILE, ANSI_MAGENTA "ERROR: UNKNOWN STATUS at %u" ANSI_RESET, i);
		}
	}

	for(iTest = 1; iTest < cTests; ++iTest)
	{ /* print out status and calculate/print group summary */
		test Test = Tests[iTest];
		int cParents = Sweet_NumParents(iTest);
		int IsGroup = Sweet_IsGroup(iTest);
		if(IsGroup) { Sweet_Indent(cParents); fputc('\n', SWEET_OUTFILE); }

		iPrevValid = iTest;
		while(iPrevValid && ! Tests[--iPrevValid].Filename);
		if(Test.Filename && (! iPrevValid || Test.Filename != Tests[iPrevValid].Filename))
		{ /* print underlined filename */
			fprintf(SWEET_OUTFILE, "%s\n", Test.Filename);
			for(i = 0; Test.Filename[i]; i++) { fputc('=', SWEET_OUTFILE); }
			fprintf(SWEET_OUTFILE, "\n\n");
		}

		switch(Test.Status)
		{ /* set status character and colour */
			case SWEET_STATUS_Fail: if(!Test.Parent) ++cOGFail; if(!IsGroup) ++cOIFail;
									TestStatus = 'X'; TestColour = ANSI_RED;     break;
			case SWEET_STATUS_Pass: if(!Test.Parent) ++cOGPass; if(!IsGroup) ++cOIPass;
									TestStatus = '/'; TestColour = ANSI_GREEN;   break;
			case SWEET_STATUS_Skip: TestStatus = '-'; TestColour = ANSI_YELLOW;  break;
			case SWEET_STATUS_Undefined: TestStatus = '?'; TestColour = ANSI_YELLOW; ++cMissed; break;
			default:                TestStatus = '!'; TestColour = ANSI_MAGENTA "ERROR: UNKNOWN STATUS! ";
		}

		Sweet_Indent(cParents);
		fprintf(SWEET_OUTFILE, "%s[%c] %s"ANSI_RESET"\n", TestColour, TestStatus,
				Test.Message ? Test.Message : "** test code not hit **");

		iParent = Test.Parent;
		if(Tests[iTest+1].Parent < iParent) /* end of group */
		{ /* calculate annd append summary of group */
			do /* repeat if dropping multiple levels */
			{
				unsigned int cGPass = 0, cGFail = 0, cIPass = 0, cIFail = 0, iGroup = iTest;
				while(Tests[iGroup].Parent >= iParent) /* go back through all in same group */
				{ /* add individual tests and direct children to respective counters */
					Sweet_ConditionalStatusInc(iGroup, ! Sweet_IsGroup(iGroup),         &cIPass, &cIFail);
					Sweet_ConditionalStatusInc(iGroup, Tests[iGroup].Parent == iParent, &cGPass, &cGFail);
					--iGroup;
				}
				if(cGPass || cGFail) /* ignore skipped groups */
				{ /* print group summary */
					unsigned int cIndents = Sweet_NumParents(iParent);
					Sweet_Indent(cIndents);
					Sweet_PrintSummary(cGPass, cGFail, cIPass, cIFail, 0);
					Sweet_Indent(cIndents);
					fputc('\n', SWEET_OUTFILE);
				}
				iParent = Tests[iParent].Parent;
			}
			while(Tests[iTest+1].Parent < iParent);
		}
	}

	fputs("\n========\nOverall:\n========\n", SWEET_OUTFILE);
	Sweet_PrintSummary(cOGPass, cOGFail, cOIPass, cOIFail, cMissed);
	fputc('\n', SWEET_OUTFILE);
	return cOIFail;
}

#define SWEET_END_TESTS test Tests[__COUNTER__]

#define SWEET_H
#endif/*SWEET_H*/
