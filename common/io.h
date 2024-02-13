/**
 * Return 0 on success.
 * contents is malloc'ed and populated by readAll().
 * len is populated by readAll().
 * len does not include null terminator which is appended.
 */
static int readAll(FILE* file, char** contents, size_t* len) {
  if (!file || !contents || !len) {
    return 1;
  }
  *contents = NULL;

  if (fseek(file, 0L, SEEK_END) != 0) {
    return 2;
  }
  *len = ftell(file);
  if (*len < 0) {
    return 3;
  }
  if (fseek(file, 0L, SEEK_SET) != 0) {
    return 4;
  }

  *contents = (char*) malloc(*len + 1);
  if (! (*contents)) {
    return 5;
  }

  if (1 != fread((void*) *contents, *len, 1, file)) {
    return 6;
  }
  (*contents)[*len] = '\0';

  return 0;
}
