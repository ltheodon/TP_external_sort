#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>


struct item {uint32_t u[2]; };

void item_print (uint32_t * u) /* ASCII output */
{ printf("%"PRIu32" %"PRIu32 "\n", u[0], u[1]); } /* item_print */


void print_struct_array(struct item *array, size_t len) 
{ 
    size_t i;
 
    for(i=0; i<len; i++) 
        item_print(array[i].u);
    puts("--");
}

void print_struct_array2(struct item *array, size_t len) 
{ 
    size_t i;
 
    for(i=0; i<len; i++) 
        item_print(array[i].u);
} 


int struct_cmp(const void *a, const void *b) 
{ 
    struct item *ia = (struct item *)a;
    struct item *ib = (struct item *)b;
    if(ia->u[0] != ib->u[0])
        return ((ia->u[0] < ib->u[0]) ? -1 : 1);
    else
    	return ((ia->u[1] < ib->u[1]) ? -1 : 1);
} 


void print_arr(int A[], int n)
{
	int i;
	for (i = 0; i < n; i++)
		printf("%d\n", A[i]);
}



size_t size = 0;
void swap(size_t *a, size_t *b)
{
  size_t temp = *b;
  *b = *a;
  *a = temp;
}

void heapify(size_t array[], size_t size, size_t i, struct item *A, size_t *indices)
{
  if (size == 1)
  {
  	// :3
  }
  else
  {
    size_t smallest = i;
    size_t l = 2 * i + 1;
    size_t r = 2 * i + 2;
    if (l < size && struct_cmp(A + indices[2*array[l]],A + indices[2*array[smallest]]) < 0)
      smallest = l;
    if (r < size && struct_cmp(A + indices[2*array[r]],A + indices[2*array[smallest]]) < 0)
      smallest = r;
    if (smallest != i)
    {
      swap(&array[i], &array[smallest]);
      heapify(array, size, smallest, A, indices);
    }
  }
}

void insert(size_t array[], size_t newNum, struct item *A, size_t *indices)
{
  if (size == 0)
  {
    array[0] = newNum;
    size += 1;
  }
  else
  {
    array[size] = newNum;
    size += 1;
    for (size_t i = size / 2 - 1; i >= (size_t)0; i--)
    {
     	heapify(array, size, i, A, indices);
     	if(i < (size_t)1)
  			break;
  		if(size < (size_t)2)
  			break;
    }
  }
}

void deleteRoot(size_t array[], size_t num, struct item *A, size_t *indices)
{
  size_t i;
  for (i = (size_t)0; i < size; i++)
  {
    if (num == array[i])
      break;
  }

  swap(&array[i], &array[size - 1]);
  size -= 1;
  for (size_t i = size / 2 - 1; i >= (size_t)0; i--)
  {
        heapify(array, size, i, A, indices);
     	if(i < (size_t)1)
  			break;
  		if(size < (size_t)2)
  			break;
  }
}


int main(int argc, const char *argv[])
{
	size_t map_len;
	size_t fd;
	struct stat sb;
	size_t file_length;
	struct item *A;
	size_t n;
	clock_t start, end;
	size_t i;
	size_t j;
	size_t k;
	size_t id;
	size_t pmin;
	size_t nmax;
	size_t nb_map;
	size_t nb_items;
	size_t nb_part;

	if (argc < 2) {
		fprintf(stderr, "%s <filename> [<map len>]\n", argv[0]);
		return EXIT_FAILURE;
	}

	fd = open(argv[1], O_RDWR);
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		return EXIT_FAILURE;
	}

	file_length = sb.st_size;

	if (argc > 2) {
		nb_map = strtol(argv[2], NULL, 0);
		if (errno) {
			perror("strtol");
			return EXIT_FAILURE;
		}
	} else {
		nb_map = 1;
	}

	/*if (argc > 3) {
		printf(argv[3]);
		int ch;
  		while ((ch = getopt(argc, argv, "n:t:f:")) != -1)
    	{
   			switch (ch)
      		{
      			case 'n': printf("n - %d",optarg); break;
      			case 'f': printf("f"); break;
      			case 't': printf("t"); break;

      			case '?':
      			default:
				exit(EXIT_FAILURE);
      		}
    	}
	}*/

	A = mmap(NULL, file_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);
	if (A == MAP_FAILED) {
		perror("mmap");
		return EXIT_FAILURE;
	}


	nb_items = file_length/sizeof(struct item);

	if(nb_map > nb_items/4)
	{
		nb_map = nb_items/4;
	}


	map_len = file_length/nb_map;
	n = map_len/sizeof(struct item);

	// Démarrage du tri
	// Initialisation du timer
	start = clock();
	i = 0;
	pmin = 0;
	nmax = 0;
	while(i < nb_map+1 && i*n < nb_items)
	{
		pmin = i*n;
		nmax = n;
		if((i+1)*n > nb_items)
		{
			nmax = nb_items - i*n;
		}
		qsort(A+pmin, nmax, sizeof(struct item), struct_cmp);
		print_struct_array(A+pmin,nmax);
		i++;
	}

	nb_part = i;


	size_t *indices = malloc(2*nb_part*sizeof(size_t));

	for(k = 0; k < nb_part; k++)
	{
		indices[2*k] = k*n;
		indices[2*k+1] = n;	
		if((k+1)*n > nb_items)
		{
			indices[2*k+1] = nb_items - k*n;
		}
	}

	size_t arbre[nb_part];
	for(k = 0; k < nb_part; k++)
	{
		insert(arbre, k, A, indices);
	}


	j = 0;
	id = 0;
	while(j < nb_items && size > (size_t)0)
	{
		id = arbre[0];
		print_struct_array2(A + indices[2*id],1);
		if(size > (size_t)0)
			deleteRoot(arbre,id,A,indices);
		indices[2*id] += 1;
		indices[2*id+1] -= 1;
		if(indices[2*id+1] > 0)
		{
			insert(arbre,id,A,indices);
		}
		j++;
	}

	// Fin du tri
	// Arrêt du timer
	end = clock();

	
	fprintf(stderr, "%ld bytes sorted in %f seconds\n", file_length, (double)(end - start) / CLOCKS_PER_SEC); 
	return 0;
}
