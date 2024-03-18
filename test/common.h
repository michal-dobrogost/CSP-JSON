static loadCspJson(const char* path) {
  CjCsp csp;
  int stat = cjCspJsonParse(cspJson, cspJsonLen, &csp);
  if (stat != 0) {
    printf("FAIL: cjCspJsonParse returned %d\n", stat);
    return stat;
  }
}
