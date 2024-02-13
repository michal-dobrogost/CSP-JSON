#ifndef __CJ_TEST_FILES_H__
#define __CJ_TEST_FILES_H__

/*
 * Run a test on the provided cspJson string.
 * On failure: print "FAIL: $message\n" on failure, return non-zero.
 * On success: print nothing, return 0.
 */
typedef int (*testCspJsonT)(const char* cspJson, size_t cspJsonLen);

/**
 * Iterate through all files ending with ".json" in the directory at dirPath,
 * read each file into a string and pass that string to a user provided
 * testCspJsonT function.
 *
 * @return 0 on success.
 */
int testOnFilesInDir(const char* testName, char* dirPath, testCspJsonT fn);

/**
 * Read a file at path into a string and test it with the user provided
 * testCspJsonT function.
 *
 * @return 0 on succes.
 */
int testOnFile(const char* testName, char* path, testCspJsonT fn);

#endif // __CJ_TEST_FILES_H__
