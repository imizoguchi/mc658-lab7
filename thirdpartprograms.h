#ifndef THIRDPART_DEFINE
#define THIRDPART_DEFINE

// Update the corresponding commands

//---------------------------------------------------------------------------------
// Name of the pdf reader.
// The command used to open a pdf file is the following:
// <PDF_READER> file.pdf
//
// Update de next macro definition to the pdf viewer installed in your system
//#define PDF_VIEWER "open"  // To open pdf file in MAC OS
#define PDF_VIEWER "evince"  // To open pdf file in Linux/MAC OS
//#define PDF_VIEWER "xpdf"  // xpdf is a program to open pdf files in Linux

#ifndef PDF_VIEWER
#error Eh necessario selecionar o leitor de PDF do seu sistema em 'thirdpartprograms.h'
#endif

//
//---------------------------------------------------------------------------------

#endif

  
