# Sweet
_A hierarchical test suite for C._

_"Test suite... 'Sweet'... ha. ha."_

## Setup
Download `sweet.h` into your project folder, then simply `#include "sweet.h"` into the compilation unit[1] to test.

[1]: If you're not sure what this means, you can roughly substitute 'file'.

## Usage
### Typical usage:
```
// Test types
Test(expr);          // passes if expr evaluates non-zero
TestEq(a, b);        // - compares a and b byte-by-byte
                     // - no type checking except sizeof a vs b
                     // - only works with addressable values
TestVEq(a, b);       // does a simple value equal (must be a primitive type)
TestOp(a, op, b);    // tests a against b with op. e.g. a >= b

// Grouping
TestGroup("Description of test group")
{ // braces are just for clarity
    // ... tests
    TestGroup("Subgroup of tests")
    {
        // ... more tests
        // etc
    } EndTestGroup;
} EndTestGroup;

// ... other stuff

// Printing
int NumberOfFailedTests = PrintTestResults();
```

### Group fail/pass:
Groups only pass if all their children pass. Skipped children are completely ignored.

### Skipping:
Prepend tests or groups of tests with `Skip` to skip them and any subtests.
This keeps the test around and reminds you that it exists, but doesn't affect the results.

### Unit tests:
Super simple. If your project (the thing to be tested) is `project.c`:

```
// test_project.c
#include 'sweet.h'
#include 'project.c'
int main() {
    // tests on project.c as above
    return PrintTestResults();
}
```

Because `PrintTestResults` returns 0 on all (non-skipped) tests passing and >0 otherwise, it can be used as the return status from `main`.

This means you can determine if your test suite is passing from the command line. You can then use this e.g. in a Git pre-commit hook.

### Within-project testing:
I'm not sure this is currently recommended, but there's no particular reason this shouldn't work, bearing in mind the limitations below.

### Compile-time options
Copy, paste and alter the following `#define`s wherever necessary. The defaults are shown here.
```
// File to print to
#define SWEET_OUTFILE stdout

// Assumes used in 1 compilation unit, change to blank/`extern` if desired
#define SWEET_STATIC static

// Define as blank if `inline` is not supported by your compiler
#define SWEET_INLINE inline


// This is not defined by default. Define as below to remove the ANSI colour escape codes.
#define SWEET_NOCOLOUR
```

### Advanced use:
See `sweet.c` and `sweet2.c`.

Things to look out for:
Calling a function with groups of tests while already in the middle of a group of tests will normally flatten the nenw hierarchy. _Hint: use `NewTestGroup(m)... EndNewTestGroup;` as the base group in called functions._

## Limitations
`sweet.h` currently uses `__COUNTER__` to keep track of the array of tests.
This may not play nicely if your project uses `__COUNTER__` elsewhere...

For the same reason, the tests will be printed in the order that they appear in the compilation unit.

Tests that aren't hit at runtime can't have any information printed about them other than that they were not hit. This includes which file they're in, so they may be guessed to be in the file before if there are no tests hit in between. (If you have any way around this I'd be happy to hear it).
