/* Output from p2c, the Pascal-to-C translator */
/* From input file "pint.p" */
/* P2c options:
     Language  BERK
     StructFiles 1 */

/* P2c produced macro calls "BUFEOF(stdin)",
     which were edited to "P_eof(stdin)" */

/* Translation and editing done by I.J.A. van Geel
     e-mail: I.J.A.vanGeel@twi.tudelft.nl */

/* Date: august 22 1996 */


/*Assembler and interpreter of Pascal code*/
/*K. Jensen, N. Wirth, Ch. Jacobi, ETH May 76*/


#include "p2c.h"
#include <math.h>


/* Note for the implementation.
   ===========================
This interpreter is written for the case where all the fundamental types
take one storage unit.
In an actual implementation, the handling of the sp pointer has to take
into account the fact that the types may have lengths different from one:
in push and pop operations the sp has to be increased and decreased not
by 1, but by a number depending on the type concerned.
However, where the number of units of storage has been computed by the
compiler, the value must not be corrected, since the lengths of the types
involved have already been taken into account.
                                                                 */


#define codemax         8650
#define pcmax           17500
#define maxstk          13650   /* size of variable store */
#define overi           13655   /* size of integer constant table = 5 */
#define overr           13660   /* size of real constant table = 5 */
#define overs           13730   /* size of set constant table = 70 */
#define overb           13820
#define overm           18000
#define maxstr          18001
#define largeint        26144
#define begincode       3
#define inputadr        5
#define outputadr       6
#define prdadr          7
#define prradr          8
#define duminst         62


typedef char bit4;

typedef char bit6;

typedef short bit20;

typedef enum {
  undef, int_, reel, bool, sett, adr, mark_, car
} datatype;
typedef short address;

typedef Char beta[25];   /*error message*/
typedef long settype[3];

typedef Char alfa_[10];


typedef struct _REC_code {
  /* the program */
  unsigned op1 : 7, p1 : 4;
  signed int q1 : 16;
  unsigned op2 : 7, p2 : 4;
  short q2;
} _REC_code;

typedef union _REC_store {
  long vi;
  double vr;
  boolean vb;
  settype vs;
  Char vc;
  address va;
  /*address in store*/
  long vm;
} _REC_store;


Static _REC_code code[codemax + 1];
Static short pc;   /*program address register*/
Static bit6 op;
Static bit4 p;
Static bit20 q;   /*instruction register*/

Static _REC_store store[overm + 1];
Static address mp, sp, np, ep;   /* address registers */
/*mp  points to beginning of a data segment
  sp  points to top of the stack
  ep  points to the maximum extent of the stack
  np  points to top of the dynamically allocated area*/

Static boolean interpreting;
Static _TEXT prd, prr;   /*prd for read only, prr for write only */

Static alfa_ instr[128];   /* mnemonic instruction codes */
Static long cop[128];
Static alfa_ sptable[21];   /*standard functions and procedures*/

/*locally used for interpreting one instruction*/
Static address ad;
Static boolean b;
Static long i, i1, i2;


#define maxlabel        1850


typedef enum {
  entered, defined_
} labelst;   /*label situation*/
typedef short labelrg;
   /*label range*/

typedef struct labelrec {
  address val;
  labelst st;
} labelrec;


/* Local variables for load: */
struct LOC_load {
  address icp, rcp, scp, bcp, mcp;   /*pointers to next free position*/
  Char word[10];
  Char ch;
  labelrec labeltab[maxlabel + 1];
  address labelvalue;
} ;

Local void assemble(struct LOC_load *LINK);

Local void init(struct LOC_load *LINK)
{
  long i;
  labelrec *WITH;

  memcpy(instr[0], "lod       ", sizeof(alfa_));
  memcpy(instr[1], "ldo       ", sizeof(alfa_));
  memcpy(instr[2], "str       ", sizeof(alfa_));
  memcpy(instr[3], "sro       ", sizeof(alfa_));
  memcpy(instr[4], "lda       ", sizeof(alfa_));
  memcpy(instr[5], "lao       ", sizeof(alfa_));
  memcpy(instr[6], "sto       ", sizeof(alfa_));
  memcpy(instr[7], "ldc       ", sizeof(alfa_));
  memcpy(instr[8], "...       ", sizeof(alfa_));
  memcpy(instr[9], "ind       ", sizeof(alfa_));
  memcpy(instr[10], "inc       ", sizeof(alfa_));
  memcpy(instr[11], "mst       ", sizeof(alfa_));
  memcpy(instr[12], "cup       ", sizeof(alfa_));
  memcpy(instr[13], "ent       ", sizeof(alfa_));
  memcpy(instr[14], "ret       ", sizeof(alfa_));
  memcpy(instr[15], "csp       ", sizeof(alfa_));
  memcpy(instr[16], "ixa       ", sizeof(alfa_));
  memcpy(instr[17], "equ       ", sizeof(alfa_));
  memcpy(instr[18], "neq       ", sizeof(alfa_));
  memcpy(instr[19], "geq       ", sizeof(alfa_));
  memcpy(instr[20], "grt       ", sizeof(alfa_));
  memcpy(instr[21], "leq       ", sizeof(alfa_));
  memcpy(instr[22], "les       ", sizeof(alfa_));
  memcpy(instr[23], "ujp       ", sizeof(alfa_));
  memcpy(instr[24], "fjp       ", sizeof(alfa_));
  memcpy(instr[25], "xjp       ", sizeof(alfa_));
  memcpy(instr[26], "chk       ", sizeof(alfa_));
  memcpy(instr[27], "eof       ", sizeof(alfa_));
  memcpy(instr[28], "adi       ", sizeof(alfa_));
  memcpy(instr[29], "adr       ", sizeof(alfa_));
  memcpy(instr[30], "sbi       ", sizeof(alfa_));
  memcpy(instr[31], "sbr       ", sizeof(alfa_));
  memcpy(instr[32], "sgs       ", sizeof(alfa_));
  memcpy(instr[33], "flt       ", sizeof(alfa_));
  memcpy(instr[34], "flo       ", sizeof(alfa_));
  memcpy(instr[35], "trc       ", sizeof(alfa_));
  memcpy(instr[36], "ngi       ", sizeof(alfa_));
  memcpy(instr[37], "ngr       ", sizeof(alfa_));
  memcpy(instr[38], "sqi       ", sizeof(alfa_));
  memcpy(instr[39], "sqr       ", sizeof(alfa_));
  memcpy(instr[40], "abi       ", sizeof(alfa_));
  memcpy(instr[41], "abr       ", sizeof(alfa_));
  memcpy(instr[42], "not       ", sizeof(alfa_));
  memcpy(instr[43], "and       ", sizeof(alfa_));
  memcpy(instr[44], "ior       ", sizeof(alfa_));
  memcpy(instr[45], "dif       ", sizeof(alfa_));
  memcpy(instr[46], "int       ", sizeof(alfa_));
  memcpy(instr[47], "uni       ", sizeof(alfa_));
  memcpy(instr[48], "inn       ", sizeof(alfa_));
  memcpy(instr[49], "mod       ", sizeof(alfa_));
  memcpy(instr[50], "odd       ", sizeof(alfa_));
  memcpy(instr[51], "mpi       ", sizeof(alfa_));
  memcpy(instr[52], "mpr       ", sizeof(alfa_));
  memcpy(instr[53], "dvi       ", sizeof(alfa_));
  memcpy(instr[54], "dvr       ", sizeof(alfa_));
  memcpy(instr[55], "mov       ", sizeof(alfa_));
  memcpy(instr[56], "lca       ", sizeof(alfa_));
  memcpy(instr[57], "dec       ", sizeof(alfa_));
  memcpy(instr[58], "stp       ", sizeof(alfa_));
  memcpy(instr[59], "ord       ", sizeof(alfa_));
  memcpy(instr[60], "chr       ", sizeof(alfa_));
  memcpy(instr[61], "ujc       ", sizeof(alfa_));

  memcpy(sptable[0], "get       ", sizeof(alfa_));
  memcpy(sptable[1], "put       ", sizeof(alfa_));
  memcpy(sptable[2], "rst       ", sizeof(alfa_));
  memcpy(sptable[3], "rln       ", sizeof(alfa_));
  memcpy(sptable[4], "new       ", sizeof(alfa_));
  memcpy(sptable[5], "wln       ", sizeof(alfa_));
  memcpy(sptable[6], "wrs       ", sizeof(alfa_));
  memcpy(sptable[7], "eln       ", sizeof(alfa_));
  memcpy(sptable[8], "wri       ", sizeof(alfa_));
  memcpy(sptable[9], "wrr       ", sizeof(alfa_));
  memcpy(sptable[10], "wrc       ", sizeof(alfa_));
  memcpy(sptable[11], "rdi       ", sizeof(alfa_));
  memcpy(sptable[12], "rdr       ", sizeof(alfa_));
  memcpy(sptable[13], "rdc       ", sizeof(alfa_));
  memcpy(sptable[14], "sin       ", sizeof(alfa_));
  memcpy(sptable[15], "cos       ", sizeof(alfa_));
  memcpy(sptable[16], "exp       ", sizeof(alfa_));
  memcpy(sptable[17], "log       ", sizeof(alfa_));
  memcpy(sptable[18], "sqt       ", sizeof(alfa_));
  memcpy(sptable[19], "atn       ", sizeof(alfa_));
  memcpy(sptable[20], "sav       ", sizeof(alfa_));

  cop[0] = 105;
  cop[1] = 65;
  cop[2] = 70;
  cop[3] = 75;
  cop[6] = 80;
  cop[9] = 85;
  cop[10] = 90;
  cop[26] = 95;
  cop[57] = 100;

  pc = begincode;
  LINK->icp = maxstk + 1;
  LINK->rcp = overi + 1;
  LINK->scp = overr + 1;
  LINK->bcp = overs + 2;
  LINK->mcp = overb + 1;
  for (i = 0; i <= 9; i++)
    LINK->word[i] = ' ';
  for (i = 0; i <= maxlabel; i++) {
    WITH = &LINK->labeltab[i];
    WITH->val = -1;
    WITH->st = entered;
  }
  if (*prd.name != '\0') {
    if (prd.f != NULL)
      prd.f = freopen(prd.name, "r", prd.f);
    else
      prd.f = fopen(prd.name, "r");
  } else
    rewind(prd.f);
  if (prd.f == NULL)
    _EscIO(FileNotFound);
  RESETBUF(prd.f, Char);
}  /*init*/

Local void errorl(Char *string, struct LOC_load *LINK)
{
  /*error in loading*/
  printf("\n%.25s", string);
  _Escape(0);
}  /*errorl*/

Local void update(labelrg x, struct LOC_load *LINK)
{
  /*when a label definition lx is found*/
  short curr, succ;
  /*resp. current element and successor element
                               of a list of future references*/
  boolean endlist;
  _REC_code *WITH;

  if (LINK->labeltab[x].st == defined_) {
    errorl(" duplicated label\t       ", LINK);
    return;
  }
  if (LINK->labeltab[x].val != -1) {   /*forward reference(s)*/
    curr = LINK->labeltab[x].val;
    endlist = false;
    while (!endlist) {
      WITH = &code[curr / 2];
      if (curr & 1) {
	succ = WITH->q2;
	WITH->q2 = LINK->labelvalue;
      } else {
	succ = WITH->q1;
	WITH->q1 = LINK->labelvalue;
      }
      if (succ == -1)
	endlist = true;
      else
	curr = succ;
    }
  }
  LINK->labeltab[x].st = defined_;
  LINK->labeltab[x].val = LINK->labelvalue;
}  /*update*/

Local void generate(struct LOC_load *LINK)
{
  /*generate segment of code*/
  long x;   /* label number */
  boolean again;

  again = true;
  while (again) {
    LINK->ch = getc(prd.f);   /* first character of line*/
    if (LINK->ch == '\n')
      LINK->ch = ' ';
    switch (LINK->ch) {

    case 'i':
      fscanf(prd.f, "%*[^\n]");
      getc(prd.f);
      break;

    case 'l':
      fscanf(prd.f, "%ld", &x);
      if (!P_eoln(prd.f)) {
	LINK->ch = getc(prd.f);
	if (LINK->ch == '\n')
	  LINK->ch = ' ';
      }
      if (LINK->ch == '=')
	fscanf(prd.f, "%hd", &LINK->labelvalue);
      else
	LINK->labelvalue = pc;
      update(x, LINK);
      fscanf(prd.f, "%*[^\n]");
      getc(prd.f);
      break;

    case 'q':
      again = false;
      fscanf(prd.f, "%*[^\n]");
      getc(prd.f);
      break;

    case ' ':
      LINK->ch = getc(prd.f);
      if (LINK->ch == '\n')
	LINK->ch = ' ';
      assemble(LINK);
      break;
    }
  }
}  /*generate*/

/* Local variables for assemble: */
struct LOC_assemble {
  struct LOC_load *LINK;
  /*goto 1 for instructions without code generation*/
  alfa_ name;
} ;

Local void lookup(labelrg x, struct LOC_assemble *LINK)
{
  /* search in label table*/
  switch (LINK->LINK->labeltab[x].st) {

  case entered:
    q = LINK->LINK->labeltab[x].val;
    LINK->LINK->labeltab[x].val = pc;
    break;

  case defined_:
    q = LINK->LINK->labeltab[x].val;
    break;
  }/*case label..*/
}  /*lookup*/

Local void labelsearch(struct LOC_assemble *LINK)
{
  labelrg x;

  while ((LINK->LINK->ch != 'l') & (!P_eoln(prd.f))) {
    LINK->LINK->ch = getc(prd.f);
    if (LINK->LINK->ch == '\n')
      LINK->LINK->ch = ' ';
  }
  fscanf(prd.f, "%hd", &x);
  lookup(x, LINK);
}  /*labelsearch*/

Local void getname(struct LOC_assemble *LINK)
{
  LINK->LINK->word[0] = LINK->LINK->ch;
  LINK->LINK->word[1] = getc(prd.f);
  LINK->LINK->word[2] = getc(prd.f);
  if (LINK->LINK->word[1] == '\n')
    LINK->LINK->word[1] = ' ';
  if (LINK->LINK->word[2] == '\n')
    LINK->LINK->word[2] = ' ';
  if (!P_eoln(prd.f)) {
    LINK->LINK->ch = getc(prd.f);   /*next character*/
    if (LINK->LINK->ch == '\n')
      LINK->LINK->ch = ' ';
  }
  memcpy(LINK->name, LINK->LINK->word, sizeof(alfa_));
}  /*getname*/

Local void typesymbol(struct LOC_assemble *LINK)
{
  long i;

  /*typesymbol*/
  if (LINK->LINK->ch == 'i')
    return;
  switch (LINK->LINK->ch) {

  case 'a':
    i = 0;
    break;

  case 'r':
    i = 1;
    break;

  case 's':
    i = 2;
    break;

  case 'b':
    i = 3;
    break;

  case 'c':
    i = 4;
    break;
  }
  op = cop[op] + i;
}

Local void assemble(struct LOC_load *LINK)
{
  /*translate symbolic code into machine code and store*/
  struct LOC_assemble V;
  double r;
  settype s;
  long i, s1, lb, ub;
  int TEMP;
  _REC_code *WITH;

  V.LINK = LINK;
  p = 0;
  q = 0;
  op = 0;
  getname(&V);
  memcpy(instr[duminst], V.name, sizeof(alfa_));
  while (strncmp(instr[op], V.name, sizeof(alfa_)))
    op++;
  if (op == duminst)
    errorl(" illegal instruction     ", LINK);

  switch (op) {   /* get parameters p,q */

  /*equ,neq,geq,grt,leq,les*/
  case 17:
  case 18:
  case 19:
  case 20:
  case 21:
  case 22:
    switch (LINK->ch) {

    case 'a':   /*p = 0*/
      break;

    case 'i':
      p = 1;
      break;

    case 'r':
      p = 2;
      break;

    case 'b':
      p = 3;
      break;

    case 's':
      p = 4;
      break;

    case 'c':
      p = 6;
      break;

    case 'm':
      p = 5;
      fscanf(prd.f, "%hd", &q);
      break;
    }
    break;

  /*lod,str*/
  case 0:
  case 2:
    typesymbol(&V);
    fscanf(prd.f, "%d%hd", &TEMP, &q);
    p = TEMP;
    break;

  case 4:   /*lda*/
    fscanf(prd.f, "%d%hd", &TEMP, &q);
    p = TEMP;
    break;

  case 12:   /*cup*/
    fscanf(prd.f, "%d", &TEMP);
    p = TEMP;
    labelsearch(&V);
    break;

  case 11:   /*mst*/
    fscanf(prd.f, "%d", &TEMP);
    p = TEMP;
    break;

  case 14:   /*ret*/
    switch (LINK->ch) {

    case 'p':
      p = 0;
      break;

    case 'i':
      p = 1;
      break;

    case 'r':
      p = 2;
      break;

    case 'c':
      p = 3;
      break;

    case 'b':
      p = 4;
      break;

    case 'a':
      p = 5;
      break;
    }
    break;

  /*lao,ixa,mov*/
  case 5:
  case 16:
  case 55:
    fscanf(prd.f, "%hd", &q);
    break;

  /*ldo,sro,ind,inc,dec*/
  case 1:
  case 3:
  case 9:
  case 10:
  case 57:
    typesymbol(&V);
    fscanf(prd.f, "%hd", &q);
    break;

  /*ujp,fjp,xjp*/
  case 23:
  case 24:
  case 25:
    labelsearch(&V);
    break;

  case 13:   /*ent*/
    fscanf(prd.f, "%d", &TEMP);
    p = TEMP;
    labelsearch(&V);
    break;

  case 15:   /*csp*/
    for (i = 1; i <= 9; i++) {
      LINK->ch = getc(prd.f);
      if (LINK->ch == '\n')
	LINK->ch = ' ';
    }
    getname(&V);
    while (strncmp(V.name, sptable[q], sizeof(alfa_)))
      q++;
    break;

  case 7:   /*ldc*/
    switch (LINK->ch) {   /*get q*/

    case 'i':
      p = 1;
      fscanf(prd.f, "%ld", &i);
      if (labs(i) >= largeint) {
	op = 8;
	store[LINK->icp].vi = i;
	q = maxstk;
	do {
	  q++;
	} while (store[q].vi != i);
	if (q == LINK->icp) {
	  LINK->icp++;
	  if (LINK->icp == overi)
	    errorl(" integer table overflow  ", LINK);
	}
      } else
	q = i;
      break;

    case 'r':
      op = 8;
      p = 2;
      fscanf(prd.f, "%lg", &r);
      store[LINK->rcp].vr = r;
      q = overi;
      do {
	q++;
      } while (store[q].vr != r);
      if (q == LINK->rcp) {
	LINK->rcp++;
	if (LINK->rcp == overr)
	  errorl(" real table overflow     ", LINK);
      }
      break;

    case 'n':   /*p,q = 0*/
      break;

    case 'b':
      p = 3;
      fscanf(prd.f, "%hd", &q);
      break;

    case 'c':
      p = 6;
      do {
	LINK->ch = getc(prd.f);
	if (LINK->ch == '\n')
	  LINK->ch = ' ';
      } while (LINK->ch == ' ');
      if (LINK->ch != '\'')
	errorl(" illegal character       ", LINK);
      LINK->ch = getc(prd.f);
      if (LINK->ch == '\n')
	LINK->ch = ' ';
      q = LINK->ch;
      LINK->ch = getc(prd.f);
      if (LINK->ch == '\n')
	LINK->ch = ' ';
      if (LINK->ch != '\'')
	errorl(" illegal character       ", LINK);
      break;

    case '(':
      op = 8;
      p = 4;
      P_expset(s, 0);
      LINK->ch = getc(prd.f);
      if (LINK->ch == '\n')
	LINK->ch = ' ';
      while (LINK->ch != ')') {
	fscanf(prd.f, "%ld%c", &s1, &LINK->ch);
	if (LINK->ch == '\n')
	  LINK->ch = ' ';
	P_addset(s, s1);
      }
      P_setcpy(store[LINK->scp].vs, s);
      q = overr;
      do {
	q++;
      } while (!P_setequal(store[q].vs, s));
      if (q == LINK->scp) {
	LINK->scp++;
	if (LINK->scp == overs)
	  errorl(" set table overflow      ", LINK);
      }
      break;
    }/*case*/
    break;

  case 26:   /*chk*/
    typesymbol(&V);
    fscanf(prd.f, "%ld%ld", &lb, &ub);
    if (op == 95)
      q = lb;
    else {
      store[LINK->bcp - 1].vi = lb;
      store[LINK->bcp].vi = ub;
      q = overs;
      do {
	q += 2;
      } while (store[q - 1].vi != lb || store[q].vi != ub);
      if (q == LINK->bcp) {
	LINK->bcp += 2;
	if (LINK->bcp == overb)
	  errorl(" boundary table overflow ", LINK);
      }
    }
    break;

  case 56:   /*lca*/
    if (LINK->mcp + 16 >= overm)
      errorl(" multiple table overflow ", LINK);
    LINK->mcp += 16;
    q = LINK->mcp;
    for (i = 0; i <= 15; i++) {   /*stringlgth*/
      LINK->ch = getc(prd.f);
      if (LINK->ch == '\n')
	LINK->ch = ' ';
      store[q + i].vc = LINK->ch;
    }
    break;

  case 6:   /*sto*/
    typesymbol(&V);
    break;

  case 27:
  case 28:
  case 29:
  case 30:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 38:
  case 39:
  case 40:
  case 41:
  case 42:
  case 43:
  case 44:
  case 45:
  case 46:
  case 47:
  case 48:
  case 49:
  case 50:
  case 51:
  case 52:
  case 53:
  case 54:
  case 58:
    break;

  /*ord,chr*/
  case 59:
  case 60:
    goto _L1;
    break;

  case 61:   /*ujc*/
    break;
    /*must have same length as ujp*/

  }/*case*/

  WITH = &code[pc / 2];
  /* store instruction */
  if (pc & 1) {
    WITH->op2 = op;
    WITH->p2 = p;
    WITH->q2 = q;
  } else {
    WITH->op1 = op;
    WITH->p1 = p;
    WITH->q1 = q;
  }
  pc++;
_L1:
  fscanf(prd.f, "%*[^\n]");
  getc(prd.f);
}  /*assemble*/


/*--------------------------------------------------------------------*/


Static void load(void)
{
  struct LOC_load V;

  init(&V);
  generate(&V);
  pc = 0;
  generate(&V);
}  /*load*/

#undef maxlabel


/* Local variables for pmd: */
struct LOC_pmd {
  long s, i;
} ;

Local void pt(struct LOC_pmd *LINK)
{
  printf("%6ld", LINK->s);
  if (labs(store[LINK->s].vi) < LONG_MAX)
    printf("%12ld", store[LINK->s].vi);
  else
    printf("too big ");
  LINK->s--;
  LINK->i++;
  if (LINK->i == 4) {
    putchar('\n');
    LINK->i = 0;
  }
}  /*pt*/


/*------------------------------------------------------------------------*/

Static void pmd(void)
{
  struct LOC_pmd V;

  printf(" pc =%5d op =%3d  sp =%5d  mp =%5d  np =%5d\n",
	 pc - 1, op, sp, mp, np);
  printf("--------------------------------------\n");

  V.s = sp;
  V.i = 0;
  while (V.s >= 0)
    pt(&V);
  V.s = maxstk;
  while (V.s >= np)
    pt(&V);
}  /*pmd*/


Static jmp_buf _JL1;


Static void errori(Char *string)
{
  printf("\n%.25s\n", string);
  pmd();
  longjmp(_JL1, 1);
}  /*errori*/


Static address base(long ld)
{
  address ad;

  ad = mp;
  while (ld > 0) {
    ad = store[ad + 1].vm;
    ld--;
  }
  return ad;
}  /*base*/


Static void compare(void)
{
  /*comparing is only correct if result by comparing integers will be*/
  i1 = store[sp].va;
  i2 = store[sp + 1].va;
  i = 0;
  b = true;
  while (b && i != q) {
    if (store[i1 + i].vi == store[i2 + i].vi)
      i++;
    else
      b = false;
  }
}  /*compare*/


Local void readi(_TEXT *f)
{
  address ad;

  ad = store[sp - 1].va;
  fscanf(f->f, "%ld", &store[ad].vi);
  store[store[sp].va].vc = P_peek(f->f);
  sp -= 2;
}  /*readi*/

Local void readr(_TEXT *f)
{
  address ad;

  ad = store[sp - 1].va;
  fscanf(f->f, "%lg", &store[ad].vr);
  store[store[sp].va].vc = P_peek(f->f);
  sp -= 2;
}  /*readr*/

Local void readc(_TEXT *f)
{
  Char c;
  address ad;

  c = getc(f->f);
  if (c == '\n')
    c = ' ';
  ad = store[sp - 1].va;
  store[ad].vc = c;
  store[store[sp].va].vc = P_peek(f->f);
  store[store[sp].va].vi = P_peek(f->f);
  sp -= 2;
}  /*readc*/

Local void writestr(_TEXT *f)
{
  long i, j, k;
  address ad;
  long FORLIM;

  ad = store[sp - 3].va;
  k = store[sp - 2].vi;
  j = store[sp - 1].vi;
  /* j and k are numbers of characters */
  if (k > j) {
    FORLIM = k - j;
    for (i = 1; i <= FORLIM; i++)
      putc(' ', f->f);
  } else
    j = k;
  for (i = 0; i < j; i++)
    putc(store[ad + i].vc, f->f);
  sp -= 4;
}  /*writestr*/

Local void getfile(_TEXT *f)
{
  address ad;

  ad = store[sp].va;
  getc(f->f);
  store[ad].vc = P_peek(f->f);
  sp--;
}  /*getfile*/

Local void putfile(_TEXT *f)
{
  address ad;

  ad = store[sp].va;
  putc(store[ad].vc, f->f);
  sp--;
}  /*putfile*/


Static void callsp(void)
{
  boolean line;
  _TEXT TEMP;

  switch (q) {

  case 0:   /*get*/
    switch (store[sp].va) {

    case 5:
      TEMP.f = stdin;
      *TEMP.name = '\0';
      getfile(&TEMP);
      break;

    case 6:
      errori(" get on output file      ");
      break;

    case 7:
      getfile(&prd);
      break;

    case 8:
      errori(" get on prr file\t        ");
      break;
    }
    break;

  case 1:   /*put*/
    switch (store[sp].va) {

    case 5:
      errori(" put on read file\t       ");
      break;

    case 6:
      TEMP.f = stdout;
      *TEMP.name = '\0';
      putfile(&TEMP);
      break;

    case 7:
      errori(" put on prd file\t        ");
      break;

    case 8:
      putfile(&prr);
      break;
    }
    break;

  case 2:   /*rst*/
    /*for testphase*/
    np = store[sp].va;
    sp--;
    break;

  case 3:   /*rln*/
    switch (store[sp].va) {

    case 5:
      scanf("%*[^\n]");
      getchar();
      store[inputadr].vc = P_peek(stdin);
      break;

    case 6:
      errori(" readln on output file   ");
      break;

    case 7:
      scanf("%*[^\n]");
      getchar();
      store[inputadr].vc = P_peek(stdin);
      break;

    case 8:
      errori(" readln on prr file      ");
      break;
    }
    sp--;
    break;

  case 4:   /*new*/
    ad = np - store[sp].va;
    /*top of stack gives the length in units of storage */
    if (ad <= ep)
      errori(" store overflow\t         ");
    np = ad;
    ad = store[sp - 1].va;
    store[ad].va = np;
    sp -= 2;
    break;

  case 5:   /*wln*/
    switch (store[sp].va) {

    case 5:
      errori(" writeln on input file   ");
      break;

    case 6:
      putchar('\n');
      break;

    case 7:
      errori(" writeln on prd file     ");
      break;

    case 8:
      putc('\n', prr.f);
      break;
    }
    sp--;
    break;

  case 6:   /*wrs*/
    switch (store[sp].va) {

    case 5:
      errori(" write on input file     ");
      break;

    case 6:
      TEMP.f = stdout;
      *TEMP.name = '\0';
      writestr(&TEMP);
      break;

    case 7:
      errori(" write on prd file       ");
      break;

    case 8:
      writestr(&prr);
      break;
    }
    break;

  case 7:   /*eln*/
    switch (store[sp].va) {

    case 5:
      line = P_eoln(stdin);
      break;

    case 6:
      errori(" eoln output file\t       ");
      break;

    case 7:
      line = P_eoln(prd.f);
      break;

    case 8:
      errori(" eoln on prr file\t       ");
      break;
    }
    store[sp].vb = line;
    break;

  case 8:   /*wri*/
    switch (store[sp].va) {

    case 5:
      errori(" write on input file     ");
      break;

    case 6:
      printf("%*ld", (int)store[sp - 1].vi, store[sp - 2].vi);
      break;

    case 7:
      errori(" write on prd file       ");
      break;

    case 8:
      fprintf(prr.f, "%*ld", (int)store[sp - 1].vi, store[sp - 2].vi);
      break;
    }
    sp -= 3;
    break;

  case 9:   /*wrr*/
    switch (store[sp].va) {

    case 5:
      errori(" write on input file     ");
      break;

    case 6:
      printf("% .*E", P_max((int)store[sp - 1].vi - 7, 1), store[sp - 2].vr);
      break;

    case 7:
      errori(" write on prd file       ");
      break;

    case 8:
      fprintf(prr.f, "% .*E",
	      P_max((int)store[sp - 1].vi - 7, 1), store[sp - 2].vr);
      break;
    }
    sp -= 3;
    break;

  case 10:   /*wrc*/
    switch (store[sp].va) {

    case 5:
      errori(" write on input file     ");
      break;

    case 6:
      printf("%*c", (int)store[sp - 1].vi, store[sp - 2].vc);
      break;

    case 7:
      errori(" write on prd file       ");
      break;

    case 8:
      fprintf(prr.f, "%*c", (int)store[sp - 1].vi, (Char)store[sp - 2].vi);
      break;
    }
    sp -= 3;
    break;

  case 11:   /*rdi*/
    switch (store[sp].va) {

    case 5:
      TEMP.f = stdin;
      *TEMP.name = '\0';
      readi(&TEMP);
      break;

    case 6:
      errori(" read on output file     ");
      break;

    case 7:
      readi(&prd);
      break;

    case 8:
      errori(" read on prr file\t       ");
      break;
    }
    break;

  case 12:   /*rdr*/
    switch (store[sp].va) {

    case 5:
      TEMP.f = stdin;
      *TEMP.name = '\0';
      readr(&TEMP);
      break;

    case 6:
      errori(" read on output file     ");
      break;

    case 7:
      readr(&prd);
      break;

    case 8:
      errori(" read on prr file\t       ");
      break;
    }
    break;

  case 13:   /*rdc*/
    switch (store[sp].va) {

    case 5:
      TEMP.f = stdin;
      *TEMP.name = '\0';
      readc(&TEMP);
      break;

    case 6:
      errori(" read on output file     ");
      break;

    case 7:
      readc(&prd);
      break;

    case 8:
      errori(" read on prr file\t       ");
      break;
    }
    break;

  case 14:   /*sin*/
    store[sp].vr = sin(store[sp].vr);
    break;

  case 15:   /*cos*/
    store[sp].vr = cos(store[sp].vr);
    break;

  case 16:   /*exp*/
    store[sp].vr = exp(store[sp].vr);
    break;

  case 17:   /*log*/
    store[sp].vr = log(store[sp].vr);
    break;

  case 18:   /*sqt*/
    store[sp].vr = sqrt(store[sp].vr);
    break;

  case 19:   /*atn*/
    store[sp].vr = atan(store[sp].vr);
    break;

  case 20:   /*sav*/
    ad = store[sp].va;
    store[ad].va = np;
    sp--;
    break;
  }/*case q*/
}  /*callsp*/


int main(int argc, Char *argv[])
{  /* main */
  _REC_code *WITH;
  settype SET;
  long TEMP;
  double TEMP1;
  long FORLIM;

  PASCAL_MAIN(argc, argv);
  if (setjmp(_JL1))
    goto _L1;
  prr.f = NULL;
  strcpy(prr.name, "prr");
  prd.f = NULL;
  strcpy(prd.name, "prd");
  if (*prr.name != '\0') {
    if (prr.f != NULL)
      prr.f = freopen(prr.name, "w", prr.f);
    else
      prr.f = fopen(prr.name, "w");
  } else {
    if (prr.f != NULL)
      rewind(prr.f);
    else
      prr.f = tmpfile();
  }
  if (prr.f == NULL)
    _EscIO(FileNotFound);
  SETUPBUF(prr.f, Char);
  load();   /* assembles and stores code */
  /* writeln(output); for testing */
  pc = 0;
  sp = -1;
  mp = 0;
  np = maxstk + 1;
  ep = 5;
/*  store[inputadr].vc = P_peek(stdin); */
  store[inputadr].vc = ' ';
  store[prdadr].vc = P_peek(prd.f);
  interpreting = true;

  while (interpreting) {
    WITH = &code[pc / 2];
    /*fetch*/
    if (pc & 1) {
      op = WITH->op2;
      p = WITH->p2;
      q = WITH->q2;
    } else {
      op = WITH->op1;
      p = WITH->p1;
      q = WITH->q1;
    }
    pc++;

    /*execute*/
    switch (op) {

    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 0:   /*lod*/
      ad = base(p) + q;
      sp++;
      store[sp] = store[ad];
      break;

    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 1:   /*ldo*/
      sp++;
      store[sp] = store[q];
      break;

    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 2:   /*str*/
      store[base(p) + q] = store[sp];
      sp--;
      break;

    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 3:   /*sro*/
      store[q] = store[sp];
      sp--;
      break;

    case 4:   /*lda*/
      sp++;
      store[sp].va = base(p) + q;
      break;

    case 5:   /*lao*/
      sp++;
      store[sp].va = q;
      break;

    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 6:   /*sto*/
      store[store[sp - 1].va] = store[sp];
      sp -= 2;
      break;

    case 7:   /*ldc*/
      sp++;
      if (p == 1)
	store[sp].vi = q;
      else {
	if (p == 6)
	  store[sp].vc = q;
	else {
	  if (p == 3)
	    store[sp].vb = (q == 1);
	  else   /* load nil */
	    store[sp].va = maxstr;
	}
      }
      break;

    case 8:   /*lci*/
      sp++;
      store[sp] = store[q];
      break;

    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 9:   /*ind*/
      ad = store[sp].va + q;
      /* q is a number of storage units */
      store[sp] = store[ad];
      break;

    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 10:   /*inc*/
      store[sp].vi += q;
      break;

    case 11:   /*mst*/
      /*p=level of calling procedure minus level of called
                          procedure + 1;  set dl and sl, increment sp*/
      /* then length of this element is
         max(intsize,realsize,boolsize,charsize,ptrsize */
      store[sp + 2].vm = base(p);
      /* the length of this element is ptrsize */
      store[sp + 3].vm = mp;
      /* idem */
      store[sp + 4].vm = ep;
      /* idem */
      sp += 5;
      break;

    case 12:   /*cup*/
      /*p=no of locations for parameters, q=entry point*/
      mp = sp - p - 4;
      store[mp + 4].vm = pc;
      pc = q;
      break;

    case 13:   /*ent*/
      if (p == 1) {
	sp = mp + q;   /*q = length of dataseg*/
	if (sp > np)
	  errori(" store overflow\t         ");
      } else {
	ep = sp + q;
	if (ep > np)
	  errori(" store overflow\t         ");
      }
      break;
      /*q = max space required on stack*/

    case 14:   /*ret*/
      switch (p) {

      case 0:
	sp = mp - 1;
	break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
	sp = mp;
	break;
      }
      pc = store[mp + 4].vm;
      ep = store[mp + 3].vm;
      mp = store[mp + 2].vm;
      break;

    case 15:   /*csp*/
      callsp();
      break;

    case 16:   /*ixa*/
      i = store[sp].vi;
      sp--;
      store[sp].va += q * i;
      break;

    case 17:   /*equ*/
      sp--;
      switch (p) {

      case 1:
	store[sp].vb = (store[sp].vi == store[sp + 1].vi);
	break;

      case 0:
	store[sp].vb = (store[sp].va == store[sp + 1].va);
	break;

      case 6:
	store[sp].vb = (store[sp].vc == store[sp + 1].vc);
	break;

      case 2:
	store[sp].vb = (store[sp].vr == store[sp + 1].vr);
	break;

      case 3:
	store[sp].vb = (store[sp].vb == store[sp + 1].vb);
	break;

      case 4:
	store[sp].vb = P_setequal(store[sp].vs, store[sp + 1].vs);
	break;

      case 5:
	compare();
	store[sp].vb = b;
	break;
      }/*case p*/
      break;

    case 18:   /*neq*/
      sp--;
      switch (p) {

      case 0:
	store[sp].vb = (store[sp].va != store[sp + 1].va);
	break;

      case 1:
	store[sp].vb = (store[sp].vi != store[sp + 1].vi);
	break;

      case 6:
	store[sp].vb = (store[sp].vc != store[sp + 1].vc);
	break;

      case 2:
	store[sp].vb = (store[sp].vr != store[sp + 1].vr);
	break;

      case 3:
	store[sp].vb = (store[sp].vb != store[sp + 1].vb);
	break;

      case 4:
	store[sp].vb = !P_setequal(store[sp].vs, store[sp + 1].vs);
	break;

      case 5:
	compare();
	store[sp].vb = !b;
	break;
      }/*case p*/
      break;

    case 19:   /*geq*/
      sp--;
      switch (p) {

      case 0:
	errori(" <,<=,>,>= for address   ");
	break;

      case 1:
	store[sp].vb = (store[sp].vi >= store[sp + 1].vi);
	break;

      case 6:
	store[sp].vb = (store[sp].vc >= store[sp + 1].vc);
	break;

      case 2:
	store[sp].vb = (store[sp].vr >= store[sp + 1].vr);
	break;

      case 3:
	store[sp].vb = (store[sp].vb >= store[sp + 1].vb);
	break;

      case 4:
	store[sp].vb = P_subset(store[sp + 1].vs, store[sp].vs);
	break;

      case 5:
	compare();
	store[sp].vb = (b || store[i1 + i].vi >= store[i2 + i].vi);
	break;
      }/*case p*/
      break;

    case 20:   /*grt*/
      sp--;
      switch (p) {

      case 0:
	errori(" <,<=,>,>= for address   ");
	break;

      case 1:
	store[sp].vb = (store[sp].vi > store[sp + 1].vi);
	break;

      case 6:
	store[sp].vb = (store[sp].vc > store[sp + 1].vc);
	break;

      case 2:
	store[sp].vb = (store[sp].vr > store[sp + 1].vr);
	break;

      case 3:
	store[sp].vb = (store[sp].vb > store[sp + 1].vb);
	break;

      case 4:
	errori(" set inclusion\t          ");
	break;

      case 5:
	compare();
	store[sp].vb = (!b && store[i1 + i].vi > store[i2 + i].vi);
	break;
      }/*case p*/
      break;

    case 21:   /*leq*/
      sp--;
      switch (p) {

      case 0:
	errori(" <,<=,>,>= for address   ");
	break;

      case 1:
	store[sp].vb = (store[sp].vi <= store[sp + 1].vi);
	break;

      case 6:
	store[sp].vb = (store[sp].vc <= store[sp + 1].vc);
	break;

      case 2:
	store[sp].vb = (store[sp].vr <= store[sp + 1].vr);
	break;

      case 3:
	store[sp].vb = (store[sp].vb <= store[sp + 1].vb);
	break;

      case 4:
	store[sp].vb = P_subset(store[sp].vs, store[sp + 1].vs);
	break;

      case 5:
	compare();
	store[sp].vb = (b || store[i1 + i].vi <= store[i2 + i].vi);
	break;
      }/*case p*/
      break;

    case 22:   /*les*/
      sp--;
      switch (p) {

      case 0:
	errori(" <,<=,>,>= for address   ");
	break;

      case 1:
	store[sp].vb = (store[sp].vi < store[sp + 1].vi);
	break;

      case 6:
	store[sp].vb = (store[sp].vc < store[sp + 1].vc);
	break;

      case 2:
	store[sp].vb = (store[sp].vr < store[sp + 1].vr);
	break;

      case 3:
	store[sp].vb = (store[sp].vb < store[sp + 1].vb);
	break;

      case 5:
	compare();
	store[sp].vb = (!b && store[i1 + i].vi < store[i2 + i].vi);
	break;
      }/*case p*/
      break;

    case 23:   /*ujp*/
      pc = q;
      break;

    case 24:   /*fjp*/
      if (!store[sp].vb)
	pc = q;
      sp--;
      break;

    case 25:   /*xjp*/
      pc = store[sp].vi + q;
      sp--;
      break;

    case 95:   /*chka*/
      if (store[sp].va < np || store[sp].va > maxstr - q)
	errori(" bad pointer value       ");
      break;

    case 96:
    case 97:
    case 98:
    case 99:
    case 26:   /*chk*/
      if (store[sp].vi < store[q - 1].vi || store[sp].vi > store[q].vi)
	errori(" value out of range      ");
      break;

    case 27:   /*eof*/
      i = store[sp].vi;
      if (i == inputadr)
	store[sp].vb = P_eof(stdin);
      else
	errori(" code in error\t          ");
      break;

    case 28:   /*adi*/
      sp--;
      store[sp].vi += store[sp + 1].vi;
      break;

    case 29:   /*adr*/
      sp--;
      store[sp].vr += store[sp + 1].vr;
      break;

    case 30:   /*sbi*/
      sp--;
      store[sp].vi -= store[sp + 1].vi;
      break;

    case 31:   /*sbr*/
      sp--;
      store[sp].vr -= store[sp + 1].vr;
      break;

    case 32:   /*sgs*/
      P_setcpy(store[sp].vs, P_addset(P_expset(SET, 0), store[sp].vi));
      break;

    case 33:   /*flt*/
      store[sp].vr = store[sp].vi;
      break;

    case 34:   /*flo*/
      store[sp - 1].vr = store[sp - 1].vi;
      break;

    case 35:   /*trc*/
      store[sp].vi = (long)store[sp].vr;
      break;

    case 36:   /*ngi*/
      store[sp].vi = -store[sp].vi;
      break;

    case 37:   /*ngr*/
      store[sp].vr = -store[sp].vr;
      break;

    case 38:   /*sqi*/
      TEMP = store[sp].vi;
      store[sp].vi = TEMP * TEMP;
      break;

    case 39:   /*sqr*/
      TEMP1 = store[sp].vr;
      store[sp].vr = TEMP1 * TEMP1;
      break;

    case 40:   /*abi*/
      store[sp].vi = labs(store[sp].vi);
      break;

    case 41:   /*abr*/
      store[sp].vr = fabs(store[sp].vr);
      break;

    case 42:   /*not*/
      store[sp].vb = !store[sp].vb;
      break;

    case 43:   /*and*/
      sp--;
      store[sp].vb = (store[sp].vb && store[sp + 1].vb);
      break;

    case 44:   /*ior*/
      sp--;
      store[sp].vb = (store[sp].vb || store[sp + 1].vb);
      break;

    case 45:   /*dif*/
      sp--;
      P_setdiff(store[sp].vs, store[sp].vs, store[sp + 1].vs);
      break;

    case 46:   /*int*/
      sp--;
      P_setint(store[sp].vs, store[sp].vs, store[sp + 1].vs);
      break;

    case 47:   /*uni*/
      sp--;
      P_setunion(store[sp].vs, store[sp].vs, store[sp + 1].vs);
      break;

    case 48:   /*inn*/
      sp--;
      i = store[sp].vi;
      store[sp].vb = P_inset(i, store[sp + 1].vs);
      break;

    case 49:   /*mod*/
      sp--;
      store[sp].vi %= store[sp + 1].vi;
      break;

    case 50:   /*odd*/
      store[sp].vb = store[sp].vi & 1;
      break;

    case 51:   /*mpi*/
      sp--;
      store[sp].vi *= store[sp + 1].vi;
      break;

    case 52:   /*mpr*/
      sp--;
      store[sp].vr *= store[sp + 1].vr;
      break;

    case 53:   /*dvi*/
      sp--;
      store[sp].vi /= store[sp + 1].vi;
      break;

    case 54:   /*dvr*/
      sp--;
      store[sp].vr /= store[sp + 1].vr;
      break;

    case 55:   /*mov*/
      i1 = store[sp - 1].va;
      i2 = store[sp].va;
      sp -= 2;
      FORLIM = q;
      for (i = 0; i < FORLIM; i++) {
	store[i1 + i] = store[i2 + i];
	/* q is a number of storage units */
      }
      break;

    case 56:   /*lca*/
      sp++;
      store[sp].va = q;
      break;

    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 57:   /*dec*/
      store[sp].vi -= q;
      break;

    case 58:   /*stp*/
      interpreting = false;
      break;

    case 59:   /*ord*/
      break;
      /*only used to change the tagfield*/

    case 60:   /*chr*/
      break;

    case 61:   /*ujc*/
      errori(" case - error\t           ");
      break;
    }
  }  /*while interpreting*/

_L1:
  if (prd.f != NULL)
    fclose(prd.f);
  if (prr.f != NULL)
    fclose(prr.f);
  return 0;
}



/* End. */
