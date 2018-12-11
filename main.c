/* 
 * ATTENTION! 
 * main.c.genout 
 * Automatically generated file by GNU/Ars 
 * argv parser generator
 * Do not modify this file by hand. Change
 * ars.c and regenerate the file
 * instead.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ars.c"
#include "dllist.h"

#define SWITCH_CHAR '-'
#define ARGLEN_MAX 255
#define ARGC_MIN 1
#define ARGC_MAX 128
#define DEFAULT_INTERNAL 0
#define DEFAULT_INPUT_ARS "./default.ars"
#define DEFAULT_OUTPUT_C "./genout.main.c"
#define DEFAULT_LOG "./ars.log"
#define DEFAULT_HELP "./default.hlp"

typedef struct {
  char   infile[251];
  int    o;
  char   outfile[251];
  int    l;
  char   logfile[251];
  int    p;
  char   helpfile[251];
  int    v;
  int    h;
  int    i;
  int    B;
  int    r;
  int    W;
  int    w;
  int    E;
  int    g;
} ARG;

typedef struct {
  int internal;
  int infile;
  int outfile;
  int logfile;
  int helpfile;
} PARAMETERS;

int GetArgs(int argc, char **argv, ARG *argp)
{
  int ThisArg;
  int Opt = '\0';

  if(argc <= ARGC_MIN)
  {
    return -1;
  }
  if(argc > ARGC_MAX)
  {
    return -2;
  }
  for(ThisArg = 1; ThisArg < argc; ThisArg++)
  {
    if(argv[ThisArg][0] == SWITCH_CHAR)
    {
      if(strcmp("-o",argv[ThisArg]) == 0)
      {
        argp->o = 1;
        Opt = 'o';
      }
      else if(strcmp("-l",argv[ThisArg]) == 0)
      {
        argp->l = 1;
        Opt = 'l';
      }
      else if(strcmp("-p",argv[ThisArg]) == 0)
      {
        argp->p = 1;
        Opt = 'p';
      }
      else if(strcmp("-v",argv[ThisArg]) == 0)
      {
        argp->v = 1;
      }
      else if(strcmp("-h",argv[ThisArg]) == 0)
      {
        argp->h = 1;
      }
      else if(strcmp("-i",argv[ThisArg]) == 0)
      {
        argp->i = 1;
      }
      else if(strcmp("-B",argv[ThisArg]) == 0)
      {
        argp->B = 1;
      }
      else if(strcmp("-r",argv[ThisArg]) == 0)
      {
        argp->r = 1;
      }
      else if(strcmp("-W",argv[ThisArg]) == 0)
      {
        argp->W = 1;
      }
      else if(strcmp("-w",argv[ThisArg]) == 0)
      {
        argp->w = 1;
      }
      else if(strcmp("-E",argv[ThisArg]) == 0)
      {
        argp->E = 1;
      }
      else if(strcmp("-g",argv[ThisArg]) == 0)
      {
        argp->g = 1;
      }
      else 
      {
        printf("Unknown switch %s\n", argv[ThisArg]);
        return ThisArg;
      }
    }
    else if(Opt != '\0')
    {
      switch(Opt)
      {
        case 'o':
        {
            if(strlen(argv[ThisArg]) > 250)
            {
              return ThisArg;
            }
            strcpy(argp->outfile, argv[ThisArg]);
            break;
        }
        case 'l':
        {
            if(strlen(argv[ThisArg]) > 250)
            {
              return ThisArg;
            }
            strcpy(argp->logfile, argv[ThisArg]);
            break;
        }
        case 'p':
        {
            if(strlen(argv[ThisArg]) > 250)
            {
              return ThisArg;
            }
            strcpy(argp->helpfile, argv[ThisArg]);
            break;
        }
        defalt :
        {
            break;
        }
      }
    }
    else 
    {
        char *EndPtr;
        if(strlen(argv[ThisArg]) > 250)
        {
          return ThisArg;
        }
        strcpy(argp->infile, argv[ThisArg]);
    }
  }
  return 0;
}


int ApplicationMain(char * infile, 
                    int    o, 
                    char * outfile, 
                    int    l, 
                    char * logfile, 
                    int    p, 
                    char * helpfile, 
                    int    v, 
                    int    h, 
                    int    i, 
                    int    B, 
                    int    r, 
                    int    W, 
                    int    w, 
                    int    E, 
                    int    g);

void Help(void);
void Usage(void);
void Version(void);

int main(int argc, char **argv)
{
  int Status;
  int ArgResult;
  ARG ArgList = {0};

  ArgResult = GetArgs(argc, argv, &ArgList);
  if(ArgResult != 0)
  {
    Usage();
    Status = -3;
  }
  else
  {
    /* Calling your program... */
    Status = (ApplicationMain(ArgList.infile,
                              ArgList.o,
                              ArgList.outfile,
                              ArgList.l,
                              ArgList.logfile,
                              ArgList.p,
                              ArgList.helpfile,
                              ArgList.v,
                              ArgList.h,
                              ArgList.i,
                              ArgList.B,
                              ArgList.r,
                              ArgList.W,
                              ArgList.w,
                              ArgList.E,
                              ArgList.g) == 0) ?
     EXIT_SUCCESS :
     EXIT_FAILURE;
  }
  if(Status && -3 != Status) printf("FAULT!\n");
  else printf("ok\n");
  return Status;
}

void Help(void)
{
  char buffer[8];

  printf("ars version %s - GNU Ars - argv parser generator\n", VERSION);
  printf("Authors:  \n");
  printf("  Michael Darin\n");  
  printf("  Richard Heathfield\n");
  printf("  Eton Computer Systems Ltd\n");
  printf("  Macmillan Computer Publishing\n\n");
  printf("  Copyright (C) 2000-2013 Free Software Foundation, Inc.\n");
  printf("Usage: ars [options] infile\n");
  printf("General options:\n");
  printf("  -v          display current version, increase verbosity\n");
  printf("  -h          display this help\n");
  printf("  -i	      inmplement main function into output file\n");
  printf("  -o outfile  set output filename\n");
  printf("  -l logfile  set log filename\n");
  printf("  -p hlpfile  set plain text help filename\n");
  printf("  -B          set ars internal library and include path\n");
  printf("  -r          run compiled source\n");
  printf("  -W          set or reset (with 'no-' prefix) 'warning' (see man page)\n");
  printf("  -w          disable all warnings\n");
  printf("Preprocessor options: [NOT IMPLEMENTE YET!]\n");
  printf("  -E          preprocess only\n");
  printf("Linker options: [NOT IMPLEMENTE YET!]\n");
  printf("  ...\n");
  fprintf(stderr, "Press ENTER to continue...\n ");
  fgets(buffer, sizeof buffer, stdin);
  printf("Debugger options: [NOT IMPLEMENTE YET!]\n");
  printf("  -g          generate runtime debug info\n");
  printf("  -b N        show N callers in stack traces\n");
  printf("Misc options:[NOT IMPLEMENTE YET!]\n");
  printf("\n");
}


void Usage(void)
{
   printf("ars version %s - GNU Ars - argv parser generator\n", VERSION);
   printf("Usage: ars [options] infile\n\n");
   return;
}

void Version(void)
{
  printf("ars (GNU Ars v.%s) - argv parser generator\n\n", VERSION); 
  printf("Authors:\n");
  printf("  Michael Darin\n");  
  printf("  Richard Heathfield\n");
  printf("  Eton Computer Systems Ltd\n");
  printf("  Macmillan Computer Publishing\n\n");
  printf("Copyright (C) 2000-2013 Free Software Foundation, Inc.\n");
  return;
}

/* Write, or call, your application here.
 * ApplicationMain must return int. 0 indicatesn * success. Any other value indicates failure.
 */

/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int ApplicationMain(char * infile, 
                    int    o, 
                    char * outfile, 
                    int    l, 
                    char * logfile, 
                    int    p, 
                    char * helpfile, 
                    int    v, 
                    int    h, 
                    int    i, 
                    int    B, 
                    int    r, 
                    int    W, 
                    int    w, 
                    int    E, 
                    int    g)
{
  int Status;
  DLLIST *ArgList = NULL;
  FILE *fpIn, *fpOut, *fpLog, *fpHelp; 
  int internal = DEFAULT_INTERNAL;
  char * InFile = DEFAULT_INPUT_ARS;
  char * OutFile = DEFAULT_OUTPUT_C; 
  char * LogFile = DEFAULT_LOG; 
  char * HelpFile = DEFAULT_HELP;  

  /* Check all switches and optional arguments and to do anything... */
  if(o) {
  	    printf("outfile: %s\n", outfile);
            if(NULL != outfile) OutFile = outfile;
        }
  if(l) {
  	    printf("logfile: %s\n", logfile);
            if(NULL != logfile) LogFile = logfile;
        }
  if(p) {
  	    printf("helpfile: %s\n", helpfile);
            if(NULL != helpfile) HelpFile = helpfile;
        }
  if(v) {
  	    Version();
            return -3;
        }
  if(h) {
            Help(); 
            return -3; 
        }
  if(i) {
  	    internal = 1;
        }
  if(B) {
  	    printf("B: %d\n", B);
        }
  if(r) {
  	    printf("r: %d\n", r);
        }
  if(W) {
  	    printf("W: %d\n", W);
        }
  if(w) {
  	    printf("w: %d\n", w);
        }
  if(E) {
  	    printf("E: %d\n", E);
        }
  if(g) {
  	    printf("g: %d\n", g);
        }
  fprintf(stderr, "infile: %s\n", infile);
  // -------------------------------------
  printf("\n\n");                              
  //if(NULL != infile && '\0' != *infile) 
  InFile = infile;
  //fprintf(stderr, "in file: %s\n", InFile);
  //fprintf(stderr, "out file: %s\n", OutFile);
  //fprintf(stderr, "log file: %s\n", LogFile);
  //fprintf(stderr, "hel pfile: %s\n", HelpFile);
  //fprintf(stderr, "internal: %d\n", internal);	  
  fpLog = fopen(LogFile, "w");
  if(fpLog == NULL)
  {
    fprintf(stderr,
            "Can't open file %s for logging.\n",
            LogFile);
    fprintf(stderr, "Using stderr.\n");
    fpLog = stderr;
  }
  fpIn = fopen(InFile, "r");
  if(fpIn == NULL)
  {
    fprintf(stderr,
            "Can't open file %s for reading.\n",
            InFile);
    fprintf(stderr, "Using stdin.\n");
    fpIn = stdin;
  }
  fpOut = fopen(OutFile, "w");
  if(fpOut == NULL)
  {
    fprintf(fpLog,
            "Can't open file %s for writing.\n",
            OutFile);
    fprintf(fpLog, "Using stdout.\n");
    fpOut = stdout;
  }
  fpHelp = fopen(HelpFile, "r");
  if(fpHelp == NULL)
  {
    fprintf(stderr,
            "Can't open file %s for reading.\n",
            HelpFile);
    fprintf(stderr, "Using stdin.\n");
    fpHelp = stdin;
  }
  Status = ParseInput(&ArgList, fpIn, fpLog);
  if(EXIT_SUCCESS == Status)
  {
    DLWalk(ArgList, WalkArgs, fpLog);
    Status = WriteHeaders(fpOut, OutFile);
  }
  if(EXIT_SUCCESS == Status)
  {
    Status = WriteTypedef(fpOut, ArgList);
  }
  if(EXIT_SUCCESS == Status)
  {
    Status = WriteFunction(fpOut, ArgList);
  }
  if(EXIT_SUCCESS == Status)
  {
    Status = WriteMain(fpOut, fpHelp, ArgList, internal);
  }
  if(fpLog != stderr)
  {
    fclose(fpLog);
  }
  if(fpIn != stdin)
  {
    fclose(fpIn);
  }
  if(fpOut != stdout)
  {
    fclose(fpOut);
  }
  if(fpHelp != stdin)
  {
    fclose(fpHelp);
  }
  DLDestroy(&ArgList);
  return Status;
}

