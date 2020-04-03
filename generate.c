#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <float.h>

static char * version = "@(#) generate pairs of pseudo-random integers";

/*----------------------------------------------------------------------------*/
/* parse a string to manage k, M, and G units */
size_t read_human_size(char * s);

/*- pseudo random generator --------------------------------------------------*/
#define RANDMAX (-1ULL)
void seed_random(uint64_t seed);
double uniform_random(void);
uint32_t uniform_uint32(uint32_t min, uint32_t max);

/******************************************************************************/

void usage (char * s, uint32_t umax) /* s is the program name */
{
fprintf(stderr, "%s [-s size] [-B] [-r max]\n", s);
fprintf(stderr, "\t_size_ is the number of items to generate\n");
fprintf(stderr, "\t_size_ is given as a number, eventually followed by a multiplier (k,M,G)\n");
fprintf(stderr, "\tif -B is given the output is binary, otherwise ASCII\n");
fprintf(stderr, "\tnumber are in [0..max], default max is %"PRIu32"\n", umax);
} /* usage */

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

/******************************************************************************/

/* One item is an ordered pair of two integers. It could either represent an
 * integer of size twice the size of an int, or it could represent an arc
 * in a graph from nodes represented by these integers.
 */

struct item { uint32_t u[2]; };

void item_print (struct item * v) /* ASCII output */
{ printf("%"PRIu32" %"PRIu32, v->u[0], v->u[1]); } /* item_print */

void item_write (struct item * v) /* binary output */
{ fwrite(v, sizeof(struct item), 1, stdout); } /* item_write */

/******************************************************************************/

int main (int argc, char *argv[])
{
char * prgname; /* name of launched program */
size_t opt_s = 10; /* opt_s is the number of items to generate */
uint32_t umin = 0, umax = UINT32_MAX; /* random integer range */
int opt_B = 0; /* 0 for ASCII output, 1 for binary output */

  { /* getting the name of the launched program */
  if ((prgname = strrchr(argv[0], '/')))
    prgname++;
  else
    prgname=argv[0];
  }

  { /* Option parsing */
  int ch;

  while ((ch = getopt(argc, argv, "Bs:r:")) != -1)
    {
    switch (ch)
      {
      case 'B': opt_B = 1; break;
      case 'r':
        umax = strtoumax(optarg, NULL, 10);
	break;
      case 's':
	opt_s = read_human_size(optarg);
        if (opt_s == 0)
          { usage(prgname, umax); exit(EXIT_FAILURE); }
        break;
      case '?':
      default:
        { usage(prgname, umax); exit(EXIT_FAILURE); }
      }
    }
  }

#if 0
  {
  if (argc-optind < 1 || argc-optind> 2)
    { usage(prgname); exit(EXIT_FAILURE); }
  }
#endif

  {
  while (opt_s--)
    {
    struct item u;
    
    u.u[0] = uniform_uint32(umin, umax);
    u.u[1] = uniform_uint32(umin, umax);
    if (opt_B) item_write(&u); else { item_print(&u); putchar('\n'); }
    }
  }

return(EXIT_SUCCESS);
} /* main */

/******************************************************************************/

/* <code>
 * Source: https://stackoverflow.com/questions/16107613/parse-human-readable-sizes-k-m-g-t-into-bytes-in-c */
size_t read_human_size (char * s)
{
char *endp;
int sh;
uintmax_t x;
errno = 0; x = strtoumax(s, &endp, 10);
if (errno || endp == s) goto error;
switch(*endp) {
  case 'k': sh=10; break;
  case 'M': sh=20; break;
  case 'G': sh=30; break;
  case 0: sh=0; break;
  default: goto error;
}
if (sh && endp[1]) goto error;
if (x > SIZE_MAX>>sh) goto error;
return x <<= sh;
error:
  errno=EDOM;
  return 0;
} /* read_human_size */
/* </code> */

/******************************************************************************/
/* <code>
 * https://stackoverflow.com/questions/1340729/how-do-you-generate-a-random-double-uniformly-distributed-between-0-and-1-from-c
 */
#define RANDMAX (-1ULL)

static uint64_t custom_lcg (uint_fast64_t* next)
{ return *next = *next * 2862933555777941757ULL + 3037000493ULL; }

static uint_fast64_t internal_next;

void seed_random (uint64_t seed) { internal_next = seed; }

double uniform_random (void)
{
#define SHR_BIT (64 - (DBL_MANT_DIG-1))

  union {
    double f; uint64_t i;
  } u;

  u.f = 1.0;
  u.i = u.i | (custom_lcg(&internal_next) >> SHR_BIT);
  return u.f - 1.0;
#undef SHR_BIT
} /* uniform_random */
/* </code> */

uint32_t uniform_uint32 (uint32_t min, uint32_t max)
{ return min+((max+1.)-min)*uniform_random(); }

