# ars

Cmd line parser generator for console WinApps
Генератор парсеров командной стороки для консольных приложений Windows

### Original description


This program generates the GetArgs() function.
It takes as input (from stdin) a file with the
  following grammar:
 
  /* 
	 * impruved by me cmd line syntax may by
	 */
  
	arg one of:
    switch
    val
    optval
  
	/*
	 * and recpectevely order of switches and options is free now
	 * regural option stay parsed in hard order 
	 * if there are no rule has been found 
	 */

  infile:
    specifier
    infile specifier

  specifier: one of
    switchspecifier
    doublespecifier
    longspecifier
    stringspecifier

  switchspecifier:
    -identifier

  doublespecifier:
    identifier D

  longspecifier:
    identifier L

  stringspecifier:
    identifier S length
  
	/*
	 * There is implemented only string parameter parsing.
	 * Others types of parameters is like futures :)
	 */

  optionwithvalue: one of
    -identifier=doublespecifier
    -identifier=longspecifier
    -identifier=stringspecifier 
  
  identifier:
    letter
    identifiertail letter

  identifiertail:
    letterorunderscore identifiertail
    digit identifiertail
    letterorunderscore
    digit

  letterorunderscore:
    letter
    _

  letter: one of
    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    a b c d e f g h i j k l m n o p q r s t u v w x y z

  digit: one of
    0 1 2 3 4 5 6 7 8 9

  length:
    1 to 255


 Example input:
 -r infile S 20 outfile S 20 lbound D ubound D maxrec L
 
 Extended example for v.2

 -r infile S 31 outfile S 31 lbound D ubound D maxrec L -u=uname S 31 -p=passwd S 31

  Output: a program fragment on stdout.

 
## Usage example

clone the repository...

<code>$ cd to/clonned/ars/directory</code>

<code>$ make</code>

<code>$ ./ars default.ars -i -o generated_main.c -p default.hlp</code>

You should see something like this one output: 
``
outfile: generated_main.c
helpfile: default.hlp
infile: default.ars


ok
```
And in the directory should appear *generated_main.c*

Compile
 
<code>$ gcc -c generated_main.c</code>
<code>$ gcc generated_main.o -o generated</code>

...and run!
<code>$ ./generated</code>
```
Boldly! Insert some help info about your application here :)
or create your own plain text help file and give it to ars 
in optional argument like this one

$ars ... -p yourhelpfilename ...

Good luke!

FAULT!
```
Try input file and halp command -h

<code>$ ./generated -h</code>
```
infile: 
h: 1
ok
```
<code>$ ./generated default.ars</code>
```
infile: default.ars
ok
```

WARNING! Please, do not name your target file as 'main.c' 
force your mind to work heartly to create more another beautiful name. 
Otherwise your target output will overwrite the existing one 'main.c' :)

