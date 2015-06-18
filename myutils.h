#ifndef MYUTILS_DEFINE
#define MYUTILS_DEFINE
#include <string>
#include <iostream>  
#include <time.h>
using namespace std;

//================================================================================================================
// SOME CONSTANTS
//
// maximum number of characters used by a command used by the system routine.
#define MAXCOMMANDSIZE 1000

//================================================================================================================
//    ROUTINES TO DEAL WITH PDF
// set default pdf reader used in some programs
// For example, if it is possible to open a pdf file
// with the command "open file.pdf"
// then, you can use the following command to define
// the "open" as a pdf reader
// set_pdfreader("open");
void set_pdfreader(string programname);

// To see a pdf file. It uses the pdf reader defined by set_pdfreader.
int view_pdffile(string filename);


//================================================================================================================
//    ROUTINES FOR TYPE CONVERSION

// convert a double value to string
std::string DoubleToString(double val);

// convert a string to double
double StringToDouble(string s);

// convert a string to int
int StringToInt(string s);

// convert a int value to string
std::string IntToString(int val);
inline void Pause(void) {cout<<"Pause";std::cin.get();cout<<"\n";}

// colors must match the ColorName in myutils.cpp
typedef enum Color {NOCOLOR,WHITE,BLACK,RED,GREEN,BLUE,YELLOW,MAGENTA,CYAN,GRAY,ORANGE} Color;

// Given a color code, return its name
std::string ColorName(int cor);

//================================================================================================================
//    ROUTINES FOR TIME MANIPULATION

long time70(void);  /* retorna o tempo em segundos desde 1/jan/1970*/
void printtime(long t); /* imprime o tempo em dias, horas, minutos, seg.*/

void sprinttime(char *s,long t); /* imprime o tempo na string s */
void shortprinttime(long t); /* imprime o tempo na string s */

// MACROS TO READ AN INSTANCE

#define SPECIFICATION_ERROR(msg)                                     \
  do {                                                              \
    cerr << "ERROR (" << __FILE__ << ":" <<                         \
    __LINE__ << "): At line " << lineno <<                  \
    ", input does not match specification.\n";                 \
    cerr << "\t" << msg << "\n";                                    \
    exit(EXIT_FAILURE);                                             \
  } while(0)

#define DISPLAY_ERROR(msg)                                           \
  do {                                                               \
    cerr << "ERROR: " << msg << endl;                                \
    exit(EXIT_FAILURE);                                              \
  } while(0);

#define GET_INPUT(regexp, fmt, ...)                              \
  do {                                                            \
    string line;                                                  \
    lineno++;                                                     \
    if (!getline(cin, line)) {                                    \
      SPECIFICATION_ERROR("Line does not exist.");                \
    }                                                             \
    /* if ( regex_match(line.begin(), line.end(), regex(regexp) )) { */ \
    if ( true ) {                                                 \
      if (sscanf(line.c_str(), fmt, __VA_ARGS__) < 0) {           \
        SPECIFICATION_ERROR("Format string was not matched.");    \
      }                                                           \
    } else {                                                      \
      SPECIFICATION_ERROR("Regexp did not match the line.");      \
    }                                                             \
  } while(0)

#define GET_INPUT_SERIES(regexp, ar)                                   \
    do {                                                                 \
      ar.clear();                                                        \
      string line;                                                       \
      lineno++;                                                          \
      if (!getline(cin, line)) {                                         \
        SPECIFICATION_ERROR("Line does not exist.");                     \
      }                                                                  \
      ostringstream sout;                                                \
      sout <<  "\\d{1,8}( " << regexp << ")*";                           \
      /* if (regex_match(line.begin(), line.end(), regex(sout.str()))) { */\
      if (true) {                                                        \
        istringstream sin(line);                                         \
        int n;                                                           \
        if (!(sin >> n)) {                                               \
          SPECIFICATION_ERROR("Could not retrieve size of series.");     \
        }                                                                \
        __typeof(ar[0]) aux;                                             \
        for(int i=0; i<n; i++) {                                         \
          if (sin >> aux) {                                              \
            ar.push_back(aux);                                           \
          } else {                                                       \
            SPECIFICATION_ERROR("Could not retrieve element of series.");\
          }                                                              \
        }                                                                \
        if (sin >> aux) {                                                \
          SPECIFICATION_ERROR("Too many elements");                      \
        }                                                                \
      } else {                                                           \
        SPECIFICATION_ERROR("Regexp did not match the line.");           \
      }                                                                  \
    } while(0)

#define INTEGER_REG "\\d{1,8}"
#define DOUBLE_REG "\\d*\\.?\\d+([eE][-+]?\\d+)?"

#endif
