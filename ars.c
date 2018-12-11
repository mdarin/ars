/*  ars.c - code generator
 *
 *  ARS - argv parser generator, based on GENARGS.
 *  
 *  Version: 0.2.6 
 *
 *  Copyright (C) 2013  Michael Darin
 *  Copyright (C) 2000  Richard Heathfield
 *                      Eton Computer Systems Ltd
 *                      Macmillan Computer Publishing
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program; if not, write
 *  to the Free Software Foundation, Inc., 675 Mass Ave,
 *  Cambridge, MA 02139, USA.
 *
 *  Richard Heathfield may be contacted by email at:
 *     binary@eton.powernet.co.uk
 *
 */
/* This program generates the GetArgs() function.
 * It takes as input (from stdin) a file with the
 * following grammar:
 *
 *  допилено мной
 * <------------------------
 * синтаксис командной строки 
 * arg one of:
 *   switch
 *   val
 *   optval
 * ну и соответственно порядок переключателей и опций свободный 
 * регулярные же рабираются жёстко из тех аргуметов что не 
 * совпали ни с одним правилом  
 * ------------------------>
 *  infile:
 *    specifier
 *    infile specifier
 *
 *  specifier: one of
 *    switchspecifier
 *    doublespecifier
 *    longspecifier
 *    stringspecifier
 *
 *  switchspecifier:
 *    -identifier
 *
 *  doublespecifier:
 *    identifier D
 *
 *  longspecifier:
 *    identifier L
 *
 *  stringspecifier:
 *    identifier S length
 *  <------------------------
 *  Добавлено в грамматику в версии 2
 *  только разбор параметра как строки
 *  остальное на далее..
 *  optionwithvalue: one of
 *    -identifier=doublespecifier
 *    -identifier=longspecifier
 *    -identifier=stringspecifier  
 *  ------------------------>
 *  identifier:
 *    letter
 *    identifiertail letter
 *
 *  identifiertail:
 *    letterorunderscore identifiertail
 *    digit identifiertail
 *    letterorunderscore
 *    digit
 *
 *  letterorunderscore:
 *    letter
 *    _
 *
 *  letter: one of
 *    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
 *    a b c d e f g h i j k l m n o p q r s t u v w x y z
 *
 *  digit: one of
 *    0 1 2 3 4 5 6 7 8 9
 *
 *  length:
 *    1 to 255
 *
 *
 * Example input:
 * -r infile S 20 outfile S 20 lbound D ubound D maxrec L
 * 
 * Extended example for v.2
 *
 * -r infile S 31 outfile S 31 lbound D ubound D maxrec L -u=uname S 31 -p=passwd S 31
 *
 * Output: a program fragment on stdout.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dllist.h"


#define VERSION "0.2.6"
#define INDENT_2  "  "
#define INDENT_4  "    "
#define MAX_IDENT_LEN 32
#define MAX_STR_LEN   255
#define LINES_PER_SCREEN 23
#define BOOL   'B'
#define DOUBLE 'D'
#define LONG   'L'
#define STRING 'S'
#define OPTARG 'O'
/* Arguments of type 'B' are switches,
 * which are either On or Off, so they
 * don't need a data type or a length.
 */

/*
  надо ввсести ещё состоя над имеющимися
  автомат должен быть с состоянии
  GetArg() ->
    | GetVariable()
    | GetType()
    | GetLen()
    | Stop().
  GetOptArg() ->
    | GetVariable()
    | GetType()
    | GetLen()
    | Stop().
  GetSwithc() ->
    | GetVariable()
    | GetType()
    | GetLen()
    | Stop().
   
  надо усовершенствоват автомат реализующий разбор
*/
#define NON_SWITCH_TYPES "DLS"
#define GET_VARIABLE   'V'
#define GET_TYPE       'T'
#define GET_LEN        'L'
#define STOP           'S'
#define SWITCH_CHAR    '-'
typedef struct ARGUMENT
{
  char Name[MAX_IDENT_LEN];
  int Type;
  size_t Len; /* for strings */
  int Opt;
  char Value[MAX_IDENT_LEN];
} ARGUMENT;
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int ParseInput(DLLIST **ArgList,
               FILE *InStream,
               FILE *LogStream)
{
  int Status = EXIT_SUCCESS;
  ARGUMENT Arg = {0};
  char InBuff[256];
  char *VarName = NULL;
  char *VarType = NULL;
  char *VarLen = NULL;
  char *EndPtr = NULL;
  char *VarVal = NULL;
  const char Delims[] = " ,\t\n";
  char *Data = NULL;
  char *NextToken = NULL;
  int HaveData = 0;
  int State = GET_VARIABLE;
  while(EXIT_SUCCESS == Status &&
        State != STOP)
  {
    fprintf(LogStream,
            "Status report: Data? %s\n",
            HaveData ? "Yes" : "No");
    if(!HaveData)
    {
      Data = fgets(InBuff, sizeof InBuff, InStream);
      if(Data != NULL)
      {
        fprintf(LogStream,
                "Status report: Got data %s\n",
                InBuff);
        HaveData = 1;
        NextToken = InBuff;
      }
      else
      {
        if(State != GET_VARIABLE)
        {
          fprintf(LogStream,
                  "Syntax error - unexpected "
                  "end of stream.\n");
          Status = EXIT_FAILURE;
        }
        State = STOP;
      }
    }
    fprintf(LogStream,
            "Status report: Current State: %c\n",
            State);
      
    switch(State)
    {
      case STOP:
        break;
      case GET_VARIABLE:
        VarName = strtok(NextToken, Delims);
        NextToken = NULL;
        if(VarName == NULL)
        {
          fprintf(LogStream,
                  "Status report: Couldn't find "
                  "variable name. Looking again...\n");
          HaveData = 0;
        }
        else
        {
          fprintf(LogStream,
                  "Status report: Found variable name "
                  "[%s].\n",
                  VarName);
          if(VarName[0] == SWITCH_CHAR)
          {
            if(strlen(VarName + 1) > MAX_IDENT_LEN)
            {
              fprintf(LogStream,
                      "Constraint violation -"
                      " Identifier name too long.\n");
              Status = EXIT_FAILURE;
              State = STOP;
            }
            else
            {
              if('=' == VarName[2])
              {
               // вообще тут надо новое состояние сделать
               // разбор опции это уже рекурсивное правило 
               // получить опцию и имя переменной содержащей значение опции
               // printf(" :: parsed: %s\n", VarName);
                char opt[MAX_STR_LEN] = {0};
                strncpy(opt, VarName + 1, 1);
               // printf("   :: option: %s\n"\
                       "   :: char opt: %c\n", 
               //         opt, Arg.Opt);            
                
                char val[MAX_STR_LEN] = {0};
                strcpy(val, VarName + 3);
               // printf("   :: value: %s\n", val);
                Arg.Opt = (int)opt[0];
                strcpy(Arg.Name, val);
                strcpy(Arg.Value, val);
                // перевести указатель на следующий символо после имени пременной содержащей значеие опции
                State = GET_TYPE;        
              }
              else //*/// get_switch();
              {
                strcpy(Arg.Name, VarName + 1);
                Arg.Type = BOOL;
                Arg.Len = 0;
                if(DL_SUCCESS != DLAppend(ArgList,
                                        0,
                                        &Arg,
                                        sizeof Arg))
                {
                  /* A */
                  fprintf(LogStream,
                        "Program error A - Couldn't"
                        " add to list (memory alloc"
                        "ation failure?).\n");
                  Status = EXIT_FAILURE;
                  State = STOP;
                }
              }
            }
          }
          else
          {
            if(strlen(VarName) > MAX_IDENT_LEN)
            {
              fprintf(LogStream,
                      "Constraint violation - "
                      "Identifier name too long.\n");
              Status = EXIT_FAILURE;
              State = STOP;
            }
            else
            {
              strcpy(Arg.Name, VarName);
              State = GET_TYPE;
            }
          }
        }
        break;
      case GET_TYPE:
        VarType = strtok(NextToken, Delims);
        NextToken = NULL;
        if(VarType == NULL)
        {
          fprintf(LogStream,
                  "Status report: Couldn't find variable"
                  " type. Looking again...\n");
          HaveData = 0;
        }
        else
        {
          fprintf(LogStream,
                  "Status report: Found variable"
                  " type [%s].\n",
                  VarType);
          if(VarType[1] != '\0' ||
             strchr(NON_SWITCH_TYPES, VarType[0]) == NULL)
          {
            // если это опция со значением то разобрать её
            if(0 == strcmp("SO", VarType))
            {
              Arg.Type = OPTARG;
              Arg.Len = 0;
              State = GET_LEN; 
            } 
            else // иначе это ошибка
            { 
              fprintf(LogStream,
                    "Syntax error - Unknown type %s.\n",
                    VarType);
              State = STOP;
              Status = EXIT_FAILURE;
            }
          }
          else
          {
            fprintf(LogStream,
                    "Status report: saving var [%s]\n",
                    VarName);
            Arg.Type = VarType[0];
            Arg.Len = 0;
            if(VarType[0] == STRING)
            {
              State = GET_LEN;
            }
            else
            {
              if(DL_SUCCESS != DLAppend(ArgList,
                                        0,
                                        &Arg,
                                        sizeof Arg))
              {
                /* B */
                fprintf(LogStream,
                        "Program error B - Couldn't add"
                        " to list (memory allocation fa"
                        "ilure?).\n");
                Status = EXIT_FAILURE;
                State = STOP;
              }
              else
              {
                State = GET_VARIABLE;
              }
            }
          }
        }
        break;
      case GET_LEN:
        VarLen = strtok(NextToken, Delims);
        NextToken = NULL;
        if(VarLen == NULL)
        {
          fprintf(LogStream,
                  "Status report: Couldn't find string "
                  "length. Looking again...\n");
          HaveData = 0;
        }
        else
        {
          fprintf(LogStream,
                  "Status report: Found string"
                  " length %s.\n",
                  VarLen);
          Arg.Len = (size_t)strtoul(VarLen, &EndPtr, 10);
          if(EndPtr == VarLen ||
             Arg.Len == 0     ||
             Arg.Len > MAX_STR_LEN)
          {
            fprintf(LogStream,
                    "Constraint violation - "
                    "illegal string length.\n");
            State = STOP;
            Status = EXIT_FAILURE;
          }
          else
          {
            if(DL_SUCCESS != DLAppend(ArgList,
                                      0,
                                      &Arg,
                                      sizeof Arg))
            {
              /* C */
              fprintf(LogStream,
                      "Program error C - Couldn't"
                      " add to list (memory alloc"
                      "ation failure?).\n");
              Status = EXIT_FAILURE;
              State = STOP;
            }
            else
            {
              State = GET_VARIABLE;
            }
          }
        }
        break;
      default:
        /* D */
        fprintf(LogStream,
                "Program error D - Unknown state %c.\n",
                State);
        fflush(LogStream);
        assert(0); /* This program is broken */
        State = STOP;
        State = EXIT_FAILURE;
        break;
    }
  }
  
  return Status;
}
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int WalkArgs(int Tag, void *p, void *Parms)
{
  ARGUMENT *Arg = p;
  FILE *fp = Parms;
  switch(Arg->Type)
  {
    case OPTARG: 
      fprintf(fp,
              "-%c, %s, %c, %u\n",
              Arg->Opt,
              Arg->Value,
              Arg->Type,
              (unsigned)Arg->Len
             ); 
    case BOOL:
      fprintf(fp,
              "-%s\n",
              Arg->Name);
      break;
    case STRING:
      fprintf(fp,
              "%s, %c, %u\n",
              Arg->Name,
              Arg->Type,
              (unsigned)Arg->Len);
      break;
    default:
      fprintf(fp,
              "%s, %c\n",
              Arg->Name,
              Arg->Type);
      break;
  }
  return 0;
}
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int WriteHeaders(FILE *OutStream, char *OutFile)
{
  fprintf(OutStream,
          "/* \n"
          " * ATTENTION! \n");
  fprintf(OutStream, " * %s \n", OutFile);
  fprintf(OutStream,
          " * Automatically generated file by\n");
  fprintf(OutStream,
          " * GNU Ars - argv parser generator\n");
  fprintf(OutStream,
          " * Do not modify this file by hand. Change\n");
  fprintf(OutStream,
          " * ars.c and regenerate the file\n");
  fprintf(OutStream,
          " * instead.\n * <ars ver: %s/>\n", VERSION);
  fprintf(OutStream,
          " */\n");
  fprintf(OutStream, "#include <stdio.h>\n");
  fprintf(OutStream, "#include <stdlib.h>\n");
  fprintf(OutStream, "#include <string.h>\n\n");
  fprintf(OutStream,
          "#define SWITCH_CHAR '%c'\n",
          SWITCH_CHAR);
  return EXIT_SUCCESS;
}
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int WriteTypedef(FILE *OutStream, DLLIST *ArgList)
{
  ARGUMENT *Arg;
  fprintf(OutStream, "#define ARGLEN_MAX 255\n"\
                     "#define ARGC_MIN 1\n"\
                     "#define ARGC_MAX 128\n\n"\
                     "typedef struct {\n");
  while(ArgList)
  {
    Arg = DLGetData(ArgList, NULL, NULL);
    fprintf(OutStream, "  ");
    switch(Arg->Type)
    {
      case OPTARG:
        fprintf(OutStream, "int    %c;\n", Arg->Opt);
        fprintf(OutStream, "  ");
        fprintf(OutStream, "char   %s[%u];\n",
               Arg->Value,
               (unsigned)(Arg->Len + 1));
        break;
      case BOOL:
        fprintf(OutStream, "int    %s;\n", Arg->Name);
        break;
      case LONG:
        fprintf(OutStream, "long   %s;\n", Arg->Name);
        break;
      case DOUBLE:
        fprintf(OutStream, "double %s;\n", Arg->Name);
        break;
      case STRING:
        fprintf(OutStream, "char   %s[%u];\n",
               Arg->Name,
               (unsigned)(Arg->Len + 1));
        break;
    }
    ArgList = DLGetNext(ArgList);
  }
  fprintf(OutStream, "} ARG;\n\n");
  return EXIT_SUCCESS;
}
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int CountBools(DLLIST *ArgList)
{
  int Count = 0;
  ARGUMENT *Arg;
  while(ArgList)
  {
    Arg = DLGetData(ArgList, NULL, NULL);
    if(BOOL == Arg->Type)
    {
      ++Count;
    }
    ArgList = DLGetNext(ArgList);
  }
  return Count;
}
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int WriteFunction(FILE *OutStream, DLLIST *ArgList)
{
  DLLIST *Start = NULL;
  ARGUMENT *Arg = NULL;
  DLLIST *Iterator = NULL; 
  int CompArgs = 0;
  int OptArgs = 0;
  int ThisArg = 0;
  int ThisCompArg = 0;
  char *Indent = INDENT_2;
  char opt[8] = {'-','#','\0'};
  OptArgs = CountBools(ArgList);
  CompArgs = DLCount(ArgList) - OptArgs;
  Start = DLGetFirst(ArgList);
  fprintf(OutStream,
          "int GetArgs(int argc,"
          " char **argv, ARG *argp)\n");
  fprintf(OutStream, "{\n");
  fprintf(OutStream, "  int ThisArg;\n");
  fprintf(OutStream, "  int CompArg;\n");
  fprintf(OutStream, "  int Opt = \'\\0\';\n");
  fprintf(OutStream, "\n");
  //fprintf(OutStream, "  if(argc <= %d)\n", CompArgs);
  fprintf(OutStream, "  if(argc <= ARGC_MIN)\n"); 
  fprintf(OutStream, "  {\n");
  fprintf(OutStream, "    return -1;\n");
  fprintf(OutStream, "  }\n");
  //fprintf(OutStream, "  if(argc > %d)\n", CompArgs + OptArgs + 1);
  fprintf(OutStream, "  if(argc > ARGC_MAX)\n");
  fprintf(OutStream, "  {\n");
  fprintf(OutStream, "    return -2;\n");
  fprintf(OutStream, "  }\n");
  fprintf(OutStream, "  for(ThisArg = 1, CompArg = 0;"
         " ThisArg < argc; ThisArg++)\n");
  fprintf(OutStream, "  {\n");
  

  /* Generate switch arguments parser code */
  if(OptArgs > 0)
  {
    fprintf(OutStream, "    if(argv[ThisArg][0] == "
                       "SWITCH_CHAR)\n");
    fprintf(OutStream, "    {\n      ");    
    /*
       Тут надо условие видимо 
       Если количество свичей или опициональных аргументов >1,
       то выводить после каждого if ключевое слово else
    */
    for(Iterator = Start; NULL != Iterator; Iterator = DLGetNext(Iterator))
    {
      Arg = DLGetData(Iterator, NULL, NULL);
      /* Generate option name parser code */
      if(Arg->Type == OPTARG)
      {
        // Convert Opt to string
        opt[1] = Arg->Opt; 
        fprintf(OutStream, "if(strcmp(\"%s\",argv[ThisArg]) == 0)\n",
                opt);
        fprintf(OutStream, "      {\n");
        fprintf(OutStream,
                "        argp->%c = 1;\n        Opt = \'%c\';\n",
                (char)Arg->Opt, (char)Arg->Opt);
        fprintf(OutStream, "      }\n");
        fprintf(OutStream, "      else ");
      }
      /* Generate switch parser code */     
      if(Arg->Type == BOOL)
      {
        fprintf(OutStream,
                "if(strcmp(\"%c%s\",argv[ThisArg]) == 0)\n",
                SWITCH_CHAR,
                Arg->Name);
        fprintf(OutStream, "      {\n");
        fprintf(OutStream,
                "        argp->%s = 1;\n",
                Arg->Name);
        
        
        fprintf(OutStream, "      }\n");
        fprintf(OutStream, "      else ");
      }
      
    } 
    fprintf(OutStream, "\n");
    fprintf(OutStream, "      {\n");
    fprintf(OutStream,
           "        printf(\"Unknown switch "\
           "%%s\\n\", argv[ThisArg]);\n");
    fprintf(OutStream, "        return ThisArg;\n");
    fprintf(OutStream, "      }\n");
    ArgList = Start;
    fprintf(OutStream, "    }\n");
    fprintf(OutStream, "    else if(Opt != \'\\0\')\n");
    fprintf(OutStream, "    {\n");
    Indent = INDENT_4;
  }

  /* Generate optional arguments parser code */
  
  fprintf(OutStream, "%s  switch(Opt)\n", Indent);
  fprintf(OutStream, "%s  {\n", Indent);
  
  for(Iterator = Start; NULL != Iterator; Iterator = DLGetNext(Iterator)) 
  {  
    Arg = DLGetData(Iterator, NULL, NULL); 
    if(BOOL != Arg->Type)
    {    
      switch(Arg->Type)
      {
        case LONG:   
        case DOUBLE: 
        case STRING:    
          break;
        case OPTARG: 
           fprintf(OutStream, 
              "%s    case \'%c\':\n", 
              Indent,
              Arg->Opt);
          fprintf(OutStream,
              "%s    {\n",
              Indent);
          fprintf(OutStream,
                  "%s        if(strlen(argv"\
                  "[ThisArg]) > %d)\n",
                  Indent,
                  (unsigned)Arg->Len);
          fprintf(OutStream,
                  "%s        {\n",
                  Indent);
          fprintf(OutStream,
                  "%s          return ThisArg;\n",
                  Indent);
          fprintf(OutStream,
                  "%s        }\n",
                  Indent);
          fprintf(OutStream,
                  "%s        strcpy(argp->%s, "\
                  "argv[ThisArg]);\n",
                  Indent,
                  Arg->Value);
          fprintf(OutStream,
              "%s        break;\n",
              Indent);
          fprintf(OutStream,
              "%s    }\n",
              Indent);
          break;   
        default:
          /* Unsupported type, already validated. */
          assert(0);
          break;
      }     
      ++ThisCompArg;
    }
  }

  /* Generate arguments parser code */

  fprintf(OutStream,  "%s    defalt :\n", Indent);
  fprintf(OutStream, "%s    {\n", Indent);
  
  // <-----------------

    // здесь можно выводить обработку опциональных занчений
    // по умолчанию если ни один обработчик не подошёл

  // ----------------->
  
  fprintf(OutStream,
              "%s        break;\n",
              Indent);
  fprintf(OutStream,
              "%s    }\n",
              Indent);
 // */
  fprintf(OutStream, "%s  }\n", Indent);
  fprintf(OutStream, "\n");
  fprintf(OutStream, "%s  ++CompArg;\n", Indent);
  
  if(OptArgs > 0)
  {
    fprintf(OutStream, "    }\n");
  }
  // < --------------------------
   
    
  fprintf(OutStream, "    else \n");
  fprintf(OutStream, "    {\n");
 
  fprintf(OutStream,
                "%s    char *EndPtr;\n",
                Indent);
  
  for(Iterator = Start; NULL != Iterator; Iterator = DLGetNext(Iterator)) 
  { 
    Arg = DLGetData(Iterator, NULL, NULL);
    switch(Arg->Type)
    {
      case BOOL:
      case OPTARG:
        break;
      case LONG:
        fprintf(OutStream,
                "%s    argp->%s = "
                "strtol(argv[ThisArg]"
                ", &EndPtr, 10);\n",
                Indent,
                Arg->Name);
        fprintf(OutStream,
                "%s    if(EndPtr == "
                "argv[ThisArg])\n",
                Indent);
        fprintf(OutStream,
                "%s    {\n",
                Indent);
        fprintf(OutStream,
                "%s     return ThisArg;\n",
                Indent);
        fprintf(OutStream,
                "%s    }\n",
                Indent); 
        break;
      case DOUBLE:
        fprintf(OutStream,
                "%s    argp->%s = strtod"
                "(argv[ThisArg], &EndPtr);\n",
                Indent,
                Arg->Name);
        fprintf(OutStream,
                "%s    if(EndPtr "
                "== argv[ThisArg])\n",
               Indent);
        fprintf(OutStream,
                "%s    {\n",
                Indent);
        fprintf(OutStream,
                "%s      return ThisArg;\n",
                Indent);
        fprintf(OutStream,
                "%s    }\n",
                Indent); 
        break;
      case STRING:
        fprintf(OutStream,
                "%s    if(strlen(argv"\
                "[ThisArg]) > %d)\n",
                Indent,
                (unsigned)Arg->Len);
        fprintf(OutStream,
                "%s    {\n",
                Indent);
        fprintf(OutStream,
                "%s      return ThisArg;\n",
                Indent);
        fprintf(OutStream,
                "%s    }\n",
                Indent);
        fprintf(OutStream,
                "%s    strcpy(argp->%s, "\
                "argv[ThisArg]);\n",
                Indent,
                Arg->Name);
        break;  
     default:
        /* Unsupported type, already validated. */
        assert(0);
        break;  
    } 
  }
  
  fprintf(OutStream, "    }\n");
  // --------------------------->
  fprintf(OutStream, "  }\n");
  fprintf(OutStream, "  return 0;\n");
  fprintf(OutStream, "}\n\n");
  return EXIT_SUCCESS;
}
/* *
 * -------------------------------------------------------
 * Function: 
 * Input:
 * Output:
 * Description:
 */
int WriteMain(FILE *OutStream,
              FILE *HelpStream,
              DLLIST *ArgList,
              int InternalApp)
{
  DLLIST *Arg;
  DLLIST *Start = DLGetFirst(ArgList);
  DLLIST *Iterator = NULL;
  ARGUMENT *Data;
  char *MainText[] =
  {
    "",
    "%", /* write prototype for ApplicationMain() */
    "",
    "void Help(void);",
    "",
    "int main(int argc, char **argv)",
    "{",
    "  int Status;",
    "  int ArgResult;",
    "  ARG ArgList = {0};",
    "",
    "  ArgResult = GetArgs(argc, argv, &ArgList);",
    "  if(ArgResult != 0)",
    "  {",
    "    Help();",
    "    Status = EXIT_FAILURE;",
    "  }",
    "  else",
    "  {",
    "    /* Calling your program... */",
    "^", /* Write function call for ApplicationMain() */
    "  }",
    "",
    "  if(Status) printf(\"FAULT!\\n\");",
    "  else printf(\"ok\\n\");",
    "",
    "  return Status;",
    "}",
    "",
    "void Help(void)",
    "{",
    "!",
    "}",
    "",
    "&", /* Write function body for ApplicationMain() */
    NULL
  };
  int i;
  int j;
  char buffer[MAX_STR_LEN];
  char *p;
  char opt[8] = {0};
  for(i = 0; MainText[i] != NULL; i++)
  {
    switch(MainText[i][0])
    {
      case '!':
        /* Generate the Help() function */
        fprintf(OutStream, "  char buffer[8];\n\n");
        j = 0;
        while(fgets(buffer, sizeof buffer, HelpStream))
        {
          p = strchr(buffer, '\n');
          if(p != NULL)
          {
            *p = '\0';
          }
          fprintf(OutStream,
                  "  printf(\"%s\\n\");\n",
                  buffer);
          ++j;
          if(j == LINES_PER_SCREEN)
          {
            fprintf(OutStream,
                    "  fprintf(stderr, \"Press ENTER to continue...\\n \");\n");
            fprintf(OutStream,
                    "  fgets(buffer, sizeof buffer, "\
                    "stdin);\n");
            j = 0;
          }
        }
        break;
      case '%':
      case '&':
        if(MainText[i][0] == '&')
        {
          if(!InternalApp)
          {
            break;
          }
          fprintf(OutStream,
                  "/* Write, or call, your"
                  " application here.\n");
          fprintf(OutStream,
                  " * ApplicationMain must"
                  " return int. 0 indicatesn");
          fprintf(OutStream,
                  " * success. Any other value "
                  "indicates failure.\n");
          fprintf(OutStream,
                  " */\n\n");
        }
        /* Generate the prototype */
        fprintf(OutStream, "int ApplicationMain(");
        j = 0;
        for(Arg = ArgList;
            Arg != NULL;
            Arg = DLGetNext(Arg))
        {
          Data = DLGetData(Arg, NULL, NULL);
          switch(Data->Type)
          {
            case BOOL:
              fprintf(OutStream,
                      "%sint    ",
                      j == 0 ?
                           "" :
                           ", \n        "\
                           "            ");
              break;
            case LONG:
              fprintf(OutStream,
                      "%slong   ",
                      j == 0 ?
                           "" :
                           ", \n        "\
                           "            ");
              break;
            case DOUBLE:
              fprintf(OutStream,
                      "%sdouble ",
                      j == 0 ?
                           "" :
                           ", \n        "\
                           "            ");
              break;
            case STRING:
              fprintf(OutStream,
                      "%schar * ",
                      j == 0  ?
                           "" :
                           ", \n        "\
                           "            ");
              break;
            // <-----------------------------
            case OPTARG:
	     fprintf(OutStream,
                      "%sint    ",
                      j == 0 ?
                           "" :
                           ", \n        "\
                           "            ");	
              // не очевидно, но работает :) 
              fprintf(OutStream, "%c", Data->Opt);
              fprintf(OutStream,
                      "%schar * ",
                      j == 0  ?
                           "" :
                           ", \n        "\
                           "            ");
              break;
             // ----------------------------- >
            default:
              fprintf(stderr,
                      "program error in fun"\
                      "ction WriteMain()\n");
              assert(0);
              break;
          }
          ++j;
          fprintf(OutStream, "%s", Data->Name);
        }
        fprintf(OutStream,
                ")%s\n",
                MainText[i][0] == '%' ? ";" : "");
        if(MainText[i][0] == '&')
        {
          fprintf(OutStream, "{\n");
          /* Generate options checker into body */
          fprintf(OutStream, "/* Check all switches and optional "
                             "arguments and to do anything... */\n");
          
          for(Iterator = Start; NULL != Iterator; Iterator = DLGetNext(Iterator)) 
          { 
            Data = DLGetData(Iterator, NULL, NULL);
            switch(Data->Type) {
            case BOOL:
              fprintf(OutStream,
			"  if(%s) {\n", Data->Name);
              fprintf(OutStream,
			"  	    printf(\"%s: %%d\\n\", %s);\n",
				Data->Name, Data->Name); 
              fprintf(OutStream,
			"        }\n");
              break;
            case LONG:

              break;
            case DOUBLE:
              
              break;
            case STRING:
              fprintf(OutStream,
                    "printf(\"%s: %%s\\n\", %s);\n",
                    Data->Name, Data->Name);
              break;
            case OPTARG:
              opt[0] = Data->Opt;
              opt[1] = '\0';
              fprintf(OutStream,
			"  if(%s) {\n", opt);
                   
	      fprintf(OutStream,
			"  	    printf(\"%s: %%s\\n\", %s);\n",
                        Data->Value, Data->Value);
	      fprintf(OutStream,
			"        }\n");  	
              break;
            default:
              fprintf(stderr,
                      "program error in fun"\
                      "ction WriteMain()\n");
              assert(0);
	      break;
            }
          }
          fprintf(OutStream,"  /*TODO  Boldly! Insert some code over here! :)*/\n");
          fprintf(OutStream,"  return 0;\n}\n\n");
        }
        break;
      case '^':
        /* Generate the call */
        fprintf(OutStream,
                "    Status = (ApplicationMain(");
        j = 0;
        for(Arg = ArgList;
            Arg != NULL;
            Arg = DLGetNext(Arg))
        {
          Data = DLGetData(Arg, NULL, NULL);
          switch(Data->Type) {
          // <-----------------------------
          case OPTARG:
            opt[0] = Data->Opt;
            opt[1] = '\0';
            fprintf(OutStream, "%sArgList.%s",
                    j == 0 ?
                    "" :
                    ",\n                              ",
                    opt);
             ++j;
             fprintf(OutStream, "%sArgList.%s",
                    j == 0 ?
                    "" :
                    ",\n                              ",
                    Data->Value);
             ++j;
            break;
          // ----------------------------- >
          default:
            fprintf(OutStream, "%sArgList.%s",
                    j == 0 ?
                    "" :
                    ",\n                              ",
                    Data->Name);
             ++j;
            break;
          //--------------
          }
        }
        fprintf(OutStream,
                ") == 0) ?\n     EXIT_SUCCESS "
                ":\n     EXIT_FAILURE;\n");
        break;
      default:
        fprintf(OutStream, "%s\n", MainText[i]);
        break;
    }
  }
  return EXIT_SUCCESS;
}
