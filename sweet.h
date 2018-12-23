#ifndef SWEET_H
#include <stdio.h>
/* TODO:
// - option to not print subtests for skipped groups?
// - summary of passes and fails by file... what about by containing function?
// - proper stack?
// - Generic for equal and print
// - better message support
// 		- arbitrary text
// 		- annotation text
// - user definable macro for checking and printing arbitrary types
// - snprintf where possible
// - account for COUNTERs before (during?) test suite (currently 'not hit')
// - add ability to not show child tests, only summary (minimises hit of other counters)
// - collapse skipped groups
// - printf for notes
// - byte comparison for TestEq:
			char buf[2][sizeof(a)];
			for(i = 0; i < sizeof(sort_int_vals); ++i) {
				sprintf(buf[0]+i*3, "%.2x   ", ((unsigned char *)arr)[i]);
				sprintf(buf[1]+i*3, "%.2x   ", ((unsigned char *)sort_int_vals)[i]);
			}
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
#ifndef SWEET_MESSAGE_LENGTH
#define SWEET_MESSAGE_LENGTH 255
#endif/*SWEET_MESSAGE_LENGTH*/

#ifdef  SWEET_NOCOLOUR
#define ANSI_RESET   ""
#define ANSI_RED	 ""
#define ANSI_GREEN   ""
#define ANSI_YELLOW  ""
#define ANSI_BLUE    ""
#define ANSI_MAGENTA ""
#define ANSI_CYAN    ""
#define ANSI_WHITE   ""
#elif defined(SWEET_BRIGHTTEXT)
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED	 "\x1b[91m"
#define ANSI_GREEN   "\x1b[92m"
#define ANSI_YELLOW  "\x1b[93m"
#define ANSI_BLUE    "\x1b[94m"
#define ANSI_MAGENTA "\x1b[95m"
#define ANSI_CYAN    "\x1b[96m"
#define ANSI_WHITE   "\x1b[97m"
#else /*SWEET_NOCOLOUR*/
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED	 "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_WHITE   "\x1b[37m"
#endif/*SWEET_NOCOLOUR*/

#define SWEET_LN(x)  SWEET_CAT(x, __LINE__)

#define SWEET_CAT(a,b)  SWEET_CAT2(a,b)
#define SWEET_CAT2(a,b) SWEET_CAT1(a,b)
#define SWEET_CAT1(a,b) a##b

#define sweet_n_ln SWEET_LN(sweet_n_)

#ifdef SWEET_NO_DECL
#define sweet_decl(x)
#else
#define sweet_decl(x) x
#endif/*SWEET_NO_DECL*/

/* NOTE: i < Parent if in separate function - this just resets to no parent */
#define SWEET_ADDTEST_(i, s, m) \
	Sweet_Tests[i].Parent=(i > GlobalTestSweetParent) ? GlobalTestSweetParent : 0, \
	Sweet_Tests[i].Filename=__FILE__, \
	Sweet_Tests[i].Status=(char)(s), \
	sprintf(Sweet_Tests[i].Message, "%s", m)
#define SWEET_ADDSKIP(i, m)  SWEET_ADDTEST_(i,SWEET_STATUS_Skip,m) 
#define SWEET_ADDNOTE(i, m)  SWEET_ADDTEST_(i,SWEET_STATUS_Note,m) 
#define SWEET_ADDTEST(i,s,m) SWEET_ADDTEST_(i,(s) ? SWEET_STATUS_Pass : SWEET_STATUS_Fail,m) 
#define SWEET_ADDTEXT(i, s, a,a_s, op, b,b_s, f) SWEET_ADDTEST(i, s, ""), \
	sprintf(Sweet_Tests[i].Message, (f&&*f)?a_s" "#op" "b_s"  =>  "f" "#op" "f:a_s" "#op" "b_s, a, b)
#define Test(exp)                   do{SWEET_ADDTEST(__COUNTER__, exp,          #exp      );}while(0)
#define TestEq(a, e)		        do{SWEET_ADDTEST(__COUNTER__, Equal(a,e),   #a" == "#e);}while(0)
#define TestStrEq(a, e)		        do{SWEET_ADDTEXT(__COUNTER__, StrEq(a,e), a,#a,==,e,#e,"%s");}while(0)
#define TestOp(a, op, b, f)         do{SWEET_ADDTEXT(__COUNTER__, (a) op (b), a,#a,op,b,#b,f);}while(0)
#define TestVEq(a, e, f)            do{SWEET_ADDTEXT(__COUNTER__, (a) == (e), a,#a,==,e,#e,f);}while(0)
#define TestVEqEps(a, e, ep, f)     do{SWEET_ADDTEXT(__COUNTER__, ((a)-(ep)) <= (e) && (e) <= ((a)+(ep)), a,#a,~=,e,#e,f);}while(0)
#define TestNote(m)                 do{SWEET_ADDNOTE(__COUNTER__, m            );}while(0)
#define SkipTest(exp)               do{SWEET_ADDSKIP(__COUNTER__, #exp         );}while(0)
#define SkipTestEq(a, e)            do{SWEET_ADDSKIP(__COUNTER__, #a" == "#e   );}while(0)
#define SkipTestStrEq(a, e)         do{SWEET_ADDSKIP(__COUNTER__, #a" == "#e   );}while(0)
#define SkipTestOp(a, op, b, f)     do{SWEET_ADDSKIP(__COUNTER__, #a" "#op" "#b);}while(0)
#define SkipTestVEq(a, e, f)        do{SWEET_ADDSKIP(__COUNTER__, #a" == "#e   );}while(0)
#define SkipTestVEqEps(a, e, ep, f) do{SWEET_ADDSKIP(__COUNTER__, #a" ~= "#e   );}while(0)

#define SweetParentReset() (GlobalTestSweetParentTmp = GlobalTestSweetParent, GlobalTestSweetParent = 0)
#define SweetParentRestore() (GlobalTestSweetParent = GlobalTestSweetParentTmp)

#define TestGroup_(i, m) (SWEET_ADDTEST(i, SWEET_STATUS_Pass, m), GlobalTestSweetParent = (i > GlobalTestSweetParent) ? i : GlobalTestSweetParent)
#define TestGroup(m) for(sweet_decl(int) sweet_n_ln = (TestGroup_(__COUNTER__, m), 0); !sweet_n_ln++; EndTestGroup)        /** REMEMBER: EndTestGroup **/
#define NewTestGroup(m) SweetParentReset(); TestGroup(m)
#define SkipTestGroup_(i, m) SWEET_ADDSKIP(i, m); GlobalTestSweetParent = i
#define SkipTestGroup(m) do{SkipTestGroup_(__COUNTER__, m);        /** REMEMBER: EndTestGroup **/
#define SkipNewTestGroup(m) SweetParentReset(); SkipTestGroup(m)
#define EndTestGroup GlobalTestSweetParent=Sweet_Tests[GlobalTestSweetParent].Parent
#define EndNewTestGroup EndTestGroup; SweetParentRestore()

#define Equal(a, b) (sizeof(a) == sizeof(b) ? Equal_(&(a), &(b), sizeof(a)) : 0)
SWEET_STATIC int
Equal_(void *p1, void *p2, int n)
{
	int i, Result = 1;
	unsigned char *u1 = (unsigned char *)p1, *u2 = (unsigned char *)p2;
	for(i = 0; i < n; ++i) { if(u1[i] != u2[i]) { Result = 0; } }
	return Result;
}
SWEET_STATIC int
StrEq(char *s1, char *s2)
{
	unsigned int i; int Result = 1;
	for(i = 0; Result && s1[i] && s2[i]; ++i, Result = s1[i] == s2[i]);
	return Result;
}

typedef enum test_sweet_status
{
	SWEET_STATUS_Undefined = 0,
	SWEET_STATUS_Fail,
	SWEET_STATUS_Pass,
	SWEET_STATUS_Skip,
	SWEET_STATUS_Note,
} test_sweet_status;

typedef struct test {
	char Status;
	char Message[SWEET_MESSAGE_LENGTH];
	char *Filename;
	unsigned int Parent;
} test;

/* NOTE: leave initial table entry empty */
SWEET_STATIC unsigned int GlobalTestSweetParent = __COUNTER__, GlobalTestSweetParentTmp;

#ifndef SWEET_NUM_TESTS
#define SWEET_NUM_TESTS 256
#endif/*SWEET_NUM_TESTS*/
test Sweet_Tests[SWEET_NUM_TESTS];

#define Sweet_IsGroup(i) (Tests[i+1].Parent == i)

#define Sweet_ConditionalStatusInc(i, Condition, cPass, cFail) { \
	if(Condition) { \
		if	   (Tests[i].Status == SWEET_STATUS_Pass) { ++cPass; } \
		else if(Tests[i].Status == SWEET_STATUS_Fail) { ++cFail; }}}

SWEET_STATIC SWEET_INLINE unsigned int
Sweet_NumParents(unsigned int i)
{
	unsigned int cParents = 0;
	i = Sweet_Tests[i].Parent;
	while(i) { ++cParents; i = Sweet_Tests[i].Parent; }
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

/* returns number of failed atomic tests (doesn't count groups) */
enum { sweetCONTINUE, sweetPAUSE, sweetPAUSE_FAIL };
#define PrintTestResults(loop_pause) PrintTestResults_(Sweet_Tests, __COUNTER__, loop_pause)
SWEET_STATIC int
PrintTestResults_(test *Tests, unsigned int cTests, int LoopPause)
{
	unsigned int i, iTest, iPrevValid, iParent, iChild;
	unsigned int cOGFail = 0, cOGPass = 0, cOIFail = 0, cOIPass = 0, cMissed = 0;
	char TestStatus, *TestColour;
	if( ! Sweet_IsGroup(1)) { fputc('\n', SWEET_OUTFILE); }
	for(i = 1; i < cTests; ++i)
	{ /* update skip/fail status based on others in hierarchy */
		test Test = Tests[i];
		switch(Test.Status)
		{
			case SWEET_STATUS_Undefined:
			case SWEET_STATUS_Note:
			case SWEET_STATUS_Pass: /* do nothing */ break;

			case SWEET_STATUS_Fail:
			{ /* propogate failures up the hierarchy */
				iParent = Tests[i].Parent;
				while(iParent)
				{ Tests[iParent].Status = SWEET_STATUS_Fail; iParent = Tests[iParent].Parent; }
			} break;

			case SWEET_STATUS_Skip:
			{ /* propogate skipping down the hierarchy */
				iChild = i+1;
				while(Tests[iChild].Parent >= i)
				{ Tests[iChild].Status = SWEET_STATUS_Skip; ++iChild; }
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
		if(Test.Filename && (! iPrevValid || !StrEq(Test.Filename, Tests[iPrevValid].Filename)))
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
			case SWEET_STATUS_Note: TestStatus = '>'; TestColour = ANSI_BLUE;  break;
			case SWEET_STATUS_Undefined: TestStatus = '?'; TestColour = ANSI_YELLOW; ++cMissed; break;
			default:                TestStatus = '!'; TestColour = ANSI_MAGENTA "ERROR: UNKNOWN STATUS! ";
		}

		Sweet_Indent(cParents);
		fprintf(SWEET_OUTFILE, "%s[%c] %s" ANSI_RESET "\n", TestColour, TestStatus,
				Test.Status ? Test.Message : "** test code not hit **");

		iParent = Test.Parent;
		if(Tests[iTest+1].Parent < iParent) /* end of group */
		{ /* calculate and append summary of group */
			do /* repeat if dropping multiple levels */
			{
				unsigned int cGPass = 0, cGFail = 0, cIPass = 0, cIFail = 0, iGroup = iTest;
				while(Tests[iGroup].Parent >= iParent) /* go back through all in same group */
				{ /* add individual tests and direct children to respective counters */
					Sweet_ConditionalStatusInc(iGroup, ! Sweet_IsGroup(iGroup),         cIPass, cIFail);
					Sweet_ConditionalStatusInc(iGroup, Tests[iGroup].Parent == iParent, cGPass, cGFail);
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

	switch(LoopPause)
	{
		case sweetPAUSE:
			while(1);
		break;
		case sweetPAUSE_FAIL:
			while(!!cOIFail);
		break;
	}
	return cOIFail;
}

#define SWEET_END_TESTS test Sweet_Tests[__COUNTER__]

#define SWEET_H
#endif/*SWEET_H*/
