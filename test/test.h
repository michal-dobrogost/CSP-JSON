#ifndef __CJ_TEST_TEST_H__
#define __CJ_TEST_TEST_H__

#define TRACE(x)
//#define TRACE(x) printf(x "\n");

#define TEST(call) \
  { \
    printf(#call "... "); \
    fflush(stdout); \
    call; \
    printf("OK\n"); \
  }

#define EXPECT_RETURN(call, value) \
  { \
    TRACE("  EXPECT_RETURN(" #call ", " #value ")"); \
    int stat = call; \
    if (stat != value) { \
      printf("\n  " #call " returned %d expected %d\n", stat, value); \
      exit(1); \
    } \
  }

#define EXPECT_EQ(code, value) \
  { \
    TRACE("  EXPECT_EQ(" #code", " #value ")"); \
    int x = code; \
    if (x!= value) { \
      printf("\n  %s is %d expected %d\n", #code, x, value); \
      exit(1); \
    } \
  }

#define EXPECT_STR_EQ(code, value) \
  { \
    TRACE("  EXPECT_STR_EQ(" #code", " #value ")"); \
    const char* x = code; \
    if (strcmp(x, value) != 0) { \
      printf("\n  %s is \"%s\" expected \"%s\"\n", #code, x, value); \
      exit(1); \
    } \
  }

#define EXPECT_PTR_EQ(code, value) \
  { \
    TRACE("  EXPECT_PTR_EQ(" #code", " #value ")"); \
    const void* x = (void*) code; \
    if (x != ((void*) value)) { \
      printf("\n  %s is %p expected %p\n", #code, x, ((void*) value)); \
      exit(1); \
    } \
  }

#define EXPECT_PTR_NEQ(code, value) \
  { \
    TRACE("  EXPECT_PTR_NEQ(" #code", " #value ")"); \
    const void* x = (void*) code; \
    if (x == ((void*) value)) { \
      printf("\n  %s is %p expected %p\n", #code, x, ((void*) value)); \
      exit(1); \
    } \
  }

#endif // __CJ_TEST_TEST_H__
