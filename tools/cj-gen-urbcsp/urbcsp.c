/* urbcsp.c -- generates uniform random binary constraint satisfaction problems
*/
#include <math.h>
#include <stdio.h>
#include <stdint.h>

/* function declarations */
float ran2(int32_t *idum);
void StartCSP(int N, int K, int instance);
void EndCSP();
void AddConstraint(int var1, int var2);
void AddNogood(int val1, int val2);

/*********************************************************************
  This file has 5 parts:
  0. This introduction.
  1. A main() function, which can be used to demonstrate MakeURBCSP().
  2. MakeURBCSP().
  3. ran2(), a random number generator.
  4. The four functions StartCSP(), AddConstraint(), AddNogood(), and
     EndCSP(), which are called by MakeURBCSP().  The versions
     of these functions given here print out each instance, listing
     the incompatible value pairs of each constraint.  You will need
     to replace these functions with versions that mesh with your
     system and data structures.
*********************************************************************/


/*********************************************************************
  1. A simple main() function which reads in command line parameters
     and generates CSPs.
*********************************************************************/

int main(int argc, char* argv[])
{
  int N, D, C, T, I, i;
  int32_t S;

  if (argc != 7)
    {
      printf("usage: urbcsp #vars #vals #constraints #nogoods seed "
             "instances\n");
      return 0;
    }

  N = atoi(argv[1]);
  D = atoi(argv[2]);
  C = atoi(argv[3]);
  T = atoi(argv[4]);
  S = atoi(argv[5]);
  I = atoi(argv[6]);

  /* Seed passed to ran2() must initially be negative. */
  if (S > 0)
    S = -S;

  for (i=0; i<I; ++i)
    if (!MakeURBCSP(N, D, C, T, &S))
      return 0;

  return 1;
}


/*********************************************************************
  2. MakeURBCSP() creates a uniform binary constraint satisfaction
     problem with a specified number of variables, domain size,
     tightness, and number of constraints.  MakeURBCSP() calls
     four functions, StartCSP(), AddConstraint(), AddNogood(), and
     EndCSP(), which actually create the CSP (that is, build a data
     structure).  Feel free to change the signatures of these functions.
     Note that numbering starts from 0: the variables are numbered 0..N-1,
     and the values are numbered 0..K-1.

  INPUT PARAMETERS:
   N: number of variables
   D: size of each variable's domain
   C: number of constraints
   T: number of incompatible value pairs in each constraint
   Seed: a negative number means start a new sequence of
      pseudo-random numbers; a positive number means continue
      with the same sequence.  S is turned positive by ran2().
  RETURN VALUE:
      Returns 0 if there is a problem; 1 for normal completion.
*********************************************************************/

int MakeURBCSP(int N, int D, int C, int T, int32_t *Seed)
{
  int PossibleCTs, PossibleNGs;       /* CT means "constraint" */
  uint32_t *CTarray, *NGarray;   /* NG means "nogood pair" */
  int32_t selectedCT, selectedNG;
  int i, c, r, t;
  int var1, var2, val1, val2;
  static int instance;

  /* Check for valid values of N, D, C, and T. */
  if (N < 2)
    {
      printf("MakeURBCSP: ***Illegal value for N: %d\n", N);
      return 0;
    }
  if (D < 2)
    {
      printf("MakeURBCSP: ***Illegal value for D: %d\n", D);
      return 0;
    }
  if (C < 0 || C > N * (N - 1) / 2)
    {
      printf("MakeURBCSP: ***Illegal value for C: %d\n", C);
      return 0;
    }
  if (T < 1 || T > ((D * D) - 1))
    {
      printf("MakeURBCSP: ***Illegal value for T: %d\n", T);
      return 0;
    }

  if (*Seed < 0)      /* starting a new sequence of random numbers */
    instance = 0;
  else
    ++instance;       /* increment static variable */

  StartCSP(N, D, instance);

  /* The program has to choose randomly and uniformly m values from
     n possibilities.  It uses the following logic for both constraints
     and nogood value pairs:
           1. Let t[] be an array of the n possibilities
           2. for i = 0 to m-1
           3.    r = random(i, n-1)    ; random() returns an int in [i,n-1]
           4.    swap t[i] and t[r]
           5. end-for
     At the end of the for loop, the elements from t[0] to t[m-1] are
     the m randomly selected elements.
   */

  /* Create an array for each possible binary constraint. */
  PossibleCTs = N * (N - 1) / 2;
  CTarray = (uint32_t*) malloc(PossibleCTs * 4);

  /* Create an array for each possible value pair. */
  PossibleNGs = D * D;
  NGarray = (uint32_t*) malloc(PossibleNGs * 4);

  /* Initialize the CTarray.  Each entry has one var in the high two
     bytes, and the other in the low two bytes. */
  i=0;
  for (var1=0; var1<(N-1); ++var1)
    for (var2=var1+1; var2<N; ++var2)
      CTarray[i++] = (var1 << 16) | var2;

  /* Select C constraints. */
  for (c=0; c<C; ++c)
    {
      /* Choose a random number between c and PossibleCTs - 1, inclusive. */
      r =  c + (int) (ran2(Seed) * (PossibleCTs - c));

      /* Swap elements [c] and [r]. */
      selectedCT = CTarray[r];
      CTarray[r] = CTarray[c];
      CTarray[c] = selectedCT;

      /* Broadcast the constraint. */
      AddConstraint((int)(CTarray[c] >> 16), (int)(CTarray[c] & 0x0000FFFF));

      /* For each constraint, select T illegal value pairs. */

      /* Initialize the NGarray. */
      for (i=0; i<(D*D); ++i)
        NGarray[i] = i;

      /* Select T incompatible pairs. */
      for (t=0; t<T; ++t)
        {
          /* Choose a random number between t and PossibleNGs - 1, inclusive.*/
          r =  t + (int) (ran2(Seed) * (PossibleNGs - t));
          selectedNG = NGarray[r];
          NGarray[r] = NGarray[t];
          NGarray[t] = selectedNG;

          /* Broadcast the nogood value pair. */
          AddNogood((int)(NGarray[t] / D), (int)(NGarray[t] % D));
        }
    }

  EndCSP();
  free(CTarray);
  free(NGarray);
  return 1;
}



/*********************************************************************
  3. This random number generator is from William H. Press, et al.,
     _Numerical Recipes in C_, Second Ed. with corrections (1994),
     p. 282.  This excellent book is available through the
     WWW at http://nr.harvard.edu/nr/bookc.html.
     The specific section concerning ran2, Section 7.1, is in
     http://cfatab.harvard.edu/nr/bookc/c7-1.ps
*********************************************************************/

#define IM1   2147483563
#define IM2   2147483399
#define AM    (1.0/IM1)
#define IMM1  (IM1-1)
#define IA1   40014
#define IA2   40692
#define IQ1   53668
#define IQ2   52774
#define IR1   12211
#define IR2   3791
#define NTAB  32
#define NDIV  (1+IMM1/NTAB)
#define EPS   1.2e-7
#define RNMX  (1.0 - EPS)

/* ran2() - Return a random floating point value between 0.0 and
   1.0 exclusive.  If idum is negative, a new series starts (and
   idum is made positive so that subsequent calls using an unchanged
   idum will continue in the same sequence). */

float ran2(int32_t *idum)
{
  int j;
  int32_t k;
  static int32_t idum2 = 123456789;
  static int32_t iy = 0;
  static int32_t iv[NTAB];
  float temp;

  if (*idum <= 0) {                             /* initialize */
    if (-(*idum) < 1)                           /* prevent idum == 0 */
      *idum = 1;
    else
      *idum = -(*idum);                         /* make idum positive */
    idum2 = (*idum);
    for (j = NTAB + 7; j >= 0; j--) {           /* load the shuffle table */
      k = (*idum) / IQ1;
      *idum = IA1 * (*idum - k*IQ1) - k*IR1;
      if (*idum < 0)
        *idum += IM1;
      if (j < NTAB)
        iv[j] = *idum;
    }
    iy = iv[0];
  }

  k = (*idum) / IQ1;
  *idum = IA1 * (*idum - k*IQ1) - k*IR1;
  if (*idum < 0)
    *idum += IM1;
  k = idum2/IQ2;
  idum2 = IA2 * (idum2 - k*IQ2) - k*IR2;
  if (idum2 < 0)
    idum2 += IM2;
  j = iy / NDIV;
  iy = iv[j] - idum2;
  iv[j] = *idum;
  if (iy < 1)
    iy += IMM1;
  if ((temp = AM * iy) > RNMX)
    return RNMX;                                /* avoid endpoint */
  else
    return temp;
}


/*********************************************************************
  4. An implementation of StartCSP, AddConstraint, AddNogood, and EndCSP
     which prints out the CSP, just listing incompatible value pairs.
     Each constraint starts one a new line, and the id-numbers of the
     variables appear before the colon.  For instance, the output of
        urbcsp 10 5 4 3 9999 10
     begins
        Instance 0
          8   9: (1 1) (4 0) (0 4)
          2   4: (0 3) (3 1) (4 0)
          6   9: (4 1) (2 0) (0 3)
          1   5: (0 3) (4 0) (0 0)
*********************************************************************/

void StartCSP(int N, int D, int instance)
{
  printf("\nInstance %d", instance);
}

void AddConstraint(int var1, int var2)
{
  printf("\n%3d %3d: ", var1, var2);
}

void AddNogood(int val1, int val2)
{
  printf("(%d %d) ", val1, val2);
}

void EndCSP()
{
  printf("\n");
}

