#include "sized-data.h"
#include "steg-client.h"

#include "errors.h"
#include "file-io.h"

#include <check.h>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COURSE "cs551"
#define IMG_DIR COURSE "/projects/prj3/aux/images"

static const char *TEST_FILES[] = {
  "rose.ppm",
  "logo.ppm",                           //(width*height*3)%8 == 0
  "pexels-photo-86243.ppm",             //(width*heigth*3)%8 == 2
  "garden-rose-red-pink-56866.ppm",     //(width*heigth*3)%8 == 4
  "dahlia-red-blossom-bloom-60597.ppm", //(width*heigth*3)%8 == 6
};

enum {
  N_FILES = sizeof(TEST_FILES)/sizeof(TEST_FILES[0])
};


typedef struct {
  const char *id;
  size_t nBytes;
  const char *bytes;
} TestData;

typedef struct {
  const char *serverDir;
  const char *stegMod;
  TestData testData[N_FILES];
} TestInfo;

static TestInfo testInfo;

/*************************** Utility Functions *************************/

/** Return 1 iff str starts with start. */
static int
str_starts_with(const char *str, const char *start)
{
  const char *p = strstr(str, start);
  return p != NULL && p == str;
}

/** Return pointer to first byte b in *bytes for which ctype(b) returns
 *  non-zero.  Return NULL of a NUL-char is encountered.
 */
static char *
find_ctype(const char *bytes, int (*ctype)(int))
{
  for (const char *p = bytes; *p; p++) {
    if (ctype(*p)) return (char *)p;
  }
  return NULL;
}

/** Read file $HOME/IMG_DIR/basename returning its content via *testData.
 */
static void
read_img_data(const char *basename, TestData *testData)
{
  const char *home = getenv("HOME");
  if (!home) fatal("cannot get HOME env var");
  char path[strlen(home) + 1 + strlen(IMG_DIR) + 1 + strlen(basename) + 1];
  sprintf(path, "%s/%s/%s", home, IMG_DIR, basename);
  testData->id = basename;
  testData->bytes = read_file(path, &testData->nBytes);
}

static void
setup_test_info(const char *serverDir, const char *stegMod)
{
  testInfo.serverDir = serverDir;
  testInfo.stegMod = stegMod;
  for (int i = 0; i < N_FILES; i++) {
    read_img_data(TEST_FILES[i], &testInfo.testData[i]);
  }
}

/************************ Normal Test Case *****************************/

#define MSG "She sells sea shells on the sea shore"

START_TEST(hide_unhide)
{
  const TestData *testDataP = &testInfo.testData[_i];
  SizedData *imgData;
  const char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                               testDataP->nBytes, testDataP->bytes,
                               MSG, &imgData);
  ck_assert_msg(!err, "%s == NULL", err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     imgData->size, imgData->data, &msgData);
  ck_assert_ptr_eq(err, NULL);
  ck_assert_str_eq(msgData->data, MSG);
  free(msgData);
  free(imgData);
}
END_TEST

static TCase *
normal_tcase()
{
  TCase *tcase = tcase_create("normal");
  tcase_add_loop_test(tcase, hide_unhide, 0, N_FILES);
  return tcase;
}

/********************* Bad Steg Magic Test Case ************************/

START_TEST(bad_steg_magic)
{
  const TestData *dataP = &testInfo.testData[_i];
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, dataP->bytes,
                         MSG, &imgData);
  ck_assert_ptr_eq(err, NULL);
  char badBytes[dataP->nBytes];
  memcpy(badBytes, dataP->bytes, dataP->nBytes);
  char *p = find_ctype(badBytes, isspace);
  //point p to first byte of max-colors
  for (int i = 0; i < 3; i++) {
    p = find_ctype(p, isdigit);
    p = find_ctype(p, isspace);
  }
  p++; //point past single whitespace after max # of colors
  *p = *p ^ 1; //toggle LSB of first STG_MAGIC byte
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     imgData->size, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_BAD_MAGIC: ") == 1,
                "STEG_BAD_MAGIC != %s", err);
  free(err);
  free(imgData);
}
END_TEST

START_TEST(unhide_non_stegd)
{
  const TestData *dataP = &testInfo.testData[_i];
  SizedData *msgData;
  char *err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                           dataP->nBytes, dataP->bytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_BAD_MAGIC: ") == 1,
                "STEG_BAD_MAGIC != %s", err);
  free(err);
}
END_TEST

START_TEST(hide_stegd)
{
  const TestData *dataP = &testInfo.testData[_i];
  SizedData *imgData1;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, dataP->bytes,
                         MSG, &imgData1);
  ck_assert_ptr_eq(err, NULL);
  SizedData *imgData2;
  err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         imgData1->size, imgData1->data,
                         MSG, &imgData2);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_BAD_MAGIC: ") == 1,
                "STEG_BAD_MAGIC != %s", err);
  free(err);
  free(imgData1);
}
END_TEST

static TCase *
bad_steg_magic_tcase()
{
  TCase *tcase = tcase_create("bad_steg_magic");
  tcase_add_loop_test(tcase, bad_steg_magic, 0, N_FILES);
  tcase_add_loop_test(tcase, unhide_non_stegd, 0, N_FILES);
  tcase_add_loop_test(tcase, hide_stegd, 0, N_FILES);
  return tcase;
}

/************************ Bad Format Test Case *************************/

START_TEST(bad_ppm_magic)
{
  const TestData *dataP = &testInfo.testData[_i];
  char badBytes[dataP->nBytes];
  memcpy(badBytes, dataP->bytes, dataP->nBytes);
  badBytes[0] = 'x';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, badBytes, MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     dataP->nBytes, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);

  free(err);
}
END_TEST

START_TEST(bad_width)
{
  const TestData *dataP = &testInfo.testData[_i];
  char badBytes[dataP->nBytes];
  memcpy(badBytes, dataP->bytes, dataP->nBytes);
  char *p = badBytes;
  for (int i = 0; i < 1; i++) {
    p = find_ctype(p, isspace);
    p = find_ctype(p, isdigit);
  }
  *p = 'x';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, badBytes, MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     dataP->nBytes, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
}
END_TEST

START_TEST(bad_height)
{
  const TestData *dataP = &testInfo.testData[_i];
  char badBytes[dataP->nBytes];
  memcpy(badBytes, dataP->bytes, dataP->nBytes);
  char *p = badBytes;
  for (int i = 0; i < 2; i++) {
    p = find_ctype(p, isspace);
    p = find_ctype(p, isdigit);
  }
  *p = 'x';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, badBytes, MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     dataP->nBytes, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                   "STEG_FORMAT != %s", err);
  free(err);
}
END_TEST

START_TEST(bad_colors)
{
  const TestData *dataP = &testInfo.testData[_i];
  char badBytes[dataP->nBytes];
  memcpy(badBytes, dataP->bytes, dataP->nBytes);
  char *p = badBytes;
  for (int i = 0; i < 3; i++) {
    p = find_ctype(p, isspace);
    p = find_ctype(p, isdigit);
  }
  *p = 'x';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, badBytes, MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") ==  1,
                "STEG_FORMAT != %s", err);
  free(err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     dataP->nBytes, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
}
END_TEST

START_TEST(incorrect_colors)
{
  const TestData *dataP = &testInfo.testData[_i];
  char badBytes[dataP->nBytes];
  memcpy(badBytes, dataP->bytes, dataP->nBytes);
  char *p = find_ctype(badBytes, isspace);
  for (int i = 0; i < 3; i++) { p = find_ctype(p, isdigit); }
  *p = '1';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, badBytes, MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     dataP->nBytes, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
}
END_TEST

START_TEST(bad_length)
{
  const TestData *dataP = &testInfo.testData[_i];
  char badBytes[dataP->nBytes - 1];
  memcpy(badBytes, dataP->bytes, dataP->nBytes - 1);
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes - 1, badBytes, MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     dataP->nBytes - 1, badBytes, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_FORMAT: ") == 1,
                "STEG_FORMAT != %s", err);
  free(err);
}
END_TEST

static TCase *
bad_format_tcase()
{
  TCase *tcase = tcase_create("bad_format");
  tcase_add_loop_test(tcase, bad_ppm_magic, 0, N_FILES);
  tcase_add_loop_test(tcase, bad_width, 0, N_FILES);
  tcase_add_loop_test(tcase, bad_height, 0, N_FILES);
  tcase_add_loop_test(tcase, bad_colors, 0, N_FILES);
  tcase_add_loop_test(tcase, incorrect_colors, 0, N_FILES);
  tcase_add_loop_test(tcase, bad_length, 0, N_FILES);
  return tcase;
}


/********************* Message Length Test Case ************************/

enum {
  STG_MAGIC_LEN = 6
};

START_TEST(max_message_length)
{
  const TestData *dataP = &testInfo.testData[_i];
  char *p = find_ctype(dataP->bytes, isspace);
  //point p to first byte of max-colors
  for (int i = 0; i < 3; i++) {
    p = find_ctype(p, isdigit);
    p = find_ctype(p, isspace);
  }
  p++; //point past single whitespace after max # of colors
  const size_t nHeaderBytes = p - dataP->bytes;
  const size_t nPixelBytes = dataP->nBytes - nHeaderBytes;
  const size_t maxMsgLen = nPixelBytes/8 - STG_MAGIC_LEN - 1;
  char msg[maxMsgLen + 1];
  memset(msg, 'x', maxMsgLen);  msg[maxMsgLen] = '\0';
  SizedData *imgData;
  const char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                               dataP->nBytes, dataP->bytes,
                               msg, &imgData);
  ck_assert_ptr_eq(err, NULL);
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     imgData->size, imgData->data, &msgData);
  ck_assert_ptr_eq(err, NULL);
  ck_assert_str_eq(msgData->data, msg);
  free(msgData);
  free(imgData);
}
END_TEST

START_TEST(too_big_message)
{
  const TestData *dataP = &testInfo.testData[_i];
  char *p = find_ctype(dataP->bytes, isspace);
  //point p to first byte of max-colors
  for (int i = 0; i < 3; i++) {
    p = find_ctype(p, isdigit);
    p = find_ctype(p, isspace);
  }
  p++; //point past single whitespace after max # of colors
  const size_t nHeaderBytes = p - dataP->bytes;
  const size_t nPixelBytes = dataP->nBytes - nHeaderBytes;
  const size_t maxMsgLen = nPixelBytes/8 - STG_MAGIC_LEN - 1;
  char msg[maxMsgLen + 2];
  memset(msg, 'x', maxMsgLen + 1);  msg[maxMsgLen + 1] = '\0';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, dataP->bytes,
                         msg, &imgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_TOO_BIG: ") == 1,
                "STEG_TOO_BIG != %s", err);

  free(err);
}
END_TEST

static TCase *
message_length_tcase()
{
  TCase *tcase = tcase_create("message length");
  tcase_add_loop_test(tcase, max_message_length, 0, N_FILES);
  tcase_add_loop_test(tcase, too_big_message, 0, N_FILES);
  return tcase;
}

/*********************** Bad Message Test Case *************************/

START_TEST(bad_message)
{
  const TestData *dataP = &testInfo.testData[_i];
  char *p = find_ctype(dataP->bytes, isspace);
  //point p to first byte of max-colors
  for (int i = 0; i < 3; i++) {
    p = find_ctype(p, isdigit);
    p = find_ctype(p, isspace);
  }
  p++; //point past single whitespace after max # of colors
  const size_t nHeaderBytes = p - dataP->bytes;
  const size_t nPixelBytes = dataP->nBytes - nHeaderBytes;
  const size_t maxMsgLen = nPixelBytes/8 - STG_MAGIC_LEN - 1;
  char msg[maxMsgLen + 1];
  memset(msg, 'x', maxMsgLen);  msg[maxMsgLen] = '\0';
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, testInfo.stegMod,
                         dataP->nBytes, dataP->bytes, msg, &imgData);
  ck_assert_ptr_eq(err, NULL);
  char badStegImg[imgData->size];
  memcpy(badStegImg, imgData->data, imgData->size);
  char *lastByteP = &badStegImg[nHeaderBytes + nPixelBytes/8*8 - 1];
  *lastByteP |= 1;
  SizedData *msgData;
  err = stegc_unhide(testInfo.serverDir, testInfo.stegMod,
                     imgData->size, badStegImg, &msgData);
  ck_assert_ptr_ne(err, NULL);
  ck_assert_msg(str_starts_with(err, "STEG_BAD_MSG: ") == 1,
                "STEG_TOO_BIG != %s", err);
  free(err);
  free(imgData);
}
END_TEST

static TCase *
bad_message_tcase()
{
  TCase *tcase = tcase_create("bad message");
  tcase_add_loop_test(tcase, bad_message, 0, N_FILES);
  return tcase;
}

/*************************** Module Test Case **************************/

START_TEST(bad_module_path)
{
  const TestData *testDataP = &testInfo.testData[_i];
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, "non-existing.mod",
                               testDataP->nBytes, testDataP->bytes,
                               MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  free(err);
}
END_TEST

START_TEST(empty_module)
{
  const TestData *testDataP = &testInfo.testData[_i];
  SizedData *imgData;
  char *err = stegc_hide(testInfo.serverDir, "steg-err.mod",
                               testDataP->nBytes, testDataP->bytes,
                               MSG, &imgData);
  ck_assert_ptr_ne(err, NULL);
  free(err);
}
END_TEST

static TCase *
bad_module_tcase()
{
  TCase *tcase = tcase_create("bad module");
  tcase_add_loop_test(tcase, bad_module_path, 0, N_FILES);
  tcase_add_loop_test(tcase, empty_module, 0, N_FILES);
  return tcase;
}

/***************** Test Suite and main() Test Runner *******************/

static Suite *
steg_suite(void)
{
  Suite *suite = suite_create("steg");
  suite_add_tcase(suite, normal_tcase());
  suite_add_tcase(suite, bad_steg_magic_tcase());
  suite_add_tcase(suite, bad_format_tcase());
  suite_add_tcase(suite, message_length_tcase());
  suite_add_tcase(suite, bad_message_tcase());
  suite_add_tcase(suite, bad_module_tcase());
  return suite;
}

int
main(int argc, const char *argv[])
{
  if (argc == 4 ? strcmp(argv[1], "-v") != 0 : argc != 3) {
    fatal("usage: %s [-v] SERVER_DIR STEG_MODULE", argv[0]);
  }
  bool isVerbose = (argc == 4);
  const char *serverDir = (isVerbose) ? argv[2] : argv[1];
  const char *stegMod = (isVerbose) ? argv[3] : argv[2];
  setup_test_info(serverDir, stegMod);
  Suite *suite = steg_suite();
  SRunner *runner = srunner_create(suite);
  srunner_run_all(runner, (isVerbose) ? CK_VERBOSE : CK_NORMAL);
  int nFailed = srunner_ntests_failed(runner);
  srunner_free(runner);
  for (int i = 0; i < (int)(sizeof(TEST_FILES)/sizeof(TEST_FILES[0])); i++) {
    free((void *)testInfo.testData[i].bytes);
  }
  return (nFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

