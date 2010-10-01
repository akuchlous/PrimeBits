#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

/* static table t ostore is a number is prime */
int isPrime[65] = {
    0, 0, 1, 1, 0, 1, 0, 1, 0, 0,	/*  0 --  9 */
    0, 1, 0, 1, 0, 0, 0, 1, 0, 1,	/* 10 -- 19 */
    0, 0, 0, 1, 0, 0, 0, 0, 0, 1,	/* 20 -- 29 */
    0, 1, 0, 0, 0, 0, 0, 1, 0, 0,	/* 30 -- 39 */
    0, 1, 0, 1, 0, 0, 0, 1, 0, 0,	/* 40 -- 49 */
    0, 0, 0, 1, 0, 0, 0, 0, 0, 1,	/* 50 -- 59 */
    0, 1, 0, 0, 0
};

/* simple mathematical Combination using Pascal's triangle method
 * this is used to avoid overflow condition in normal factorial 
 * combinatorial methods
 */
uint64_t combination(int emptyBits, int bitsToFill)
{
    uint64_t oldResult[65];
    uint64_t newResult[65];
    bzero(oldResult, 64 * sizeof(uint64_t));
    bzero(newResult, 64 * sizeof(uint64_t));
    for (int row = 0; row <= emptyBits; row++) {
	memcpy(oldResult, newResult, 64 * sizeof(uint64_t));
	bzero(newResult, 64 * sizeof(uint64_t));
	for (int col = 0; col <= row; col++) {
	    if (col == 0 || col == emptyBits) {
		newResult[col] = 1;
	    } else {
		newResult[col] = oldResult[col - 1] + oldResult[col];
	    }
	    if (col == bitsToFill)
		break;
	}
    }
    return (newResult[bitsToFill]);
}

/* 
   fn : MoreThanMaxPrime 
   example : 19  :   1 0 0 1 1  (binary)

       iterate from LSB, till u find a 0.
       replace it with 1.

       1 0 1 _ _ : is greater than 1 0 0 1 1

           Use 1 0 1 _ _ , with choice to put any 0/1 in lower bits. 
           
	   1 0 1 0 0  : C1
	   1 0 1 0 1  : C2
	   1 0 1 1 0  : C3
	   1 0 1 1 1  : C4

           C1 C2 C3 C4 are greater "19", but only C1, C2, C3 fall in the isPrime categorey
          So count = 3 = count1

      Now replace 2 bit by 1 
      1 1 _ _ _ 

          All combinations of 1/0 are bigger than "19
	  chose those, where number of 1's i prime 
	  1 1 1 1 1 
	  1 1 1 0 0
	  1 1 0 1 0
	  1 1 0 0 1
	  1 1 1 1 1 
                = 5 combination  = count2

     result = count1 + count 2
     complexity = number of bits = log(n)
      
*/
uint64_t MoreThanMaxPrime(int *bitArray, int MSB, int maxTotal)
{
    uint64_t total = 0;
    int total1BitsInMSB = maxTotal;
    for (int i = 0; i <= MSB; i++) {
	if (bitArray[i] != 0) {
	    total1BitsInMSB--;
	    continue;
	}
	int numEmptySeats = i;

	if (numEmptySeats == 0) {
	    if (isPrime[maxTotal + 1])
		++total;
	    continue;
	}

	for (int j = 0; j <= numEmptySeats && numEmptySeats; j++) {
	    uint64_t localTotal =
		isPrime[j + total1BitsInMSB + 1] ?
		combination(numEmptySeats, j) : 0;
	    total += localTotal;
	}
    }
    return total;
}

/* 
   fn : LessThanMinPrime 
   example : 
      Almost same as MoreThanMaxPrime except:
      iterate from LSB, till u find a 1., replace by zero

      1 1 0 1 1 0

      1 1 0 1 0 _  << comb1 

      1 1 0 0 _ _  << comb2

      1 0 _ _ _ _  << comb3 

      _ _ _ _ _ _  << comb4 
 
      Total = comb1 + comb2 + comb3 +comb4
         :: num of bits whic are one is always prime
     complexity = number of bits = log(n)

*/
uint64_t LessThanMinPrime(int *bitArray, int MSB, int maxTotal)
{
    uint64_t total = 0;
    for (int i = 0; i <= MSB; i++) {
	if (bitArray[i] == 0)
	    continue;
	int numEmptySeats = i;
	if (numEmptySeats == 0) {
	    if (isPrime[maxTotal - 1])
		++total;
	    continue;
	}
	for (int j = 0; j <= numEmptySeats; j++) {
	    total +=
		isPrime[j + bitArray[i] - 1] ? combination(numEmptySeats,
							   j) : 0;
	}
    }
    return total;
}

/*  
   fn : input a bits empty  = n
   output = n      n     n     n
             C   +  C   + C   + C +....
              x1     x2    x3    x4
   where x1, x2 <n & prime 
   complexity = number of bits = log(n)
*/

uint64_t AllCombinationPrime(int n)
{
    uint64_t total = 0;
    for (int i = 2; i <= n; i++) {
	total += isPrime[i] ? combination(n, i) : 0;
    }
    return total;
}

int convertToUint64(const char *str, uint64_t * num)
{
    /* negative echeck */
    char **ptr = NULL;
    if (str[0] == '-')
	return -1;

    /* convert */
    *num = strtoull(str, ptr, 10);

    /* check error condition */
    if (ptr && *ptr != '\0')
	return -1;
    if (*num == ULONG_LONG_MAX)
	return -1;
    return 0;
}

/* Make Array of 64 bits, where each position signifies if 1/0  */
/* + count of 1 in self + MSB */
/* array is a int array with 64 entries */
int decompose(uint64_t num, int *array)
{
    int total = 0;
    uint64_t mask = 1;
    for (int bitPos = 63; bitPos >= 0; bitPos--) {
	if (num & (mask << bitPos)) {
	    array[bitPos] = ++total;
	} else {
	    array[bitPos] = 0;
	}
    }
    return total;
}

/*
   main forumlae to get num of prime numbers.
   two numbers max/min
   bits used by each number = nmax, nmin

   Find all primes using nmax bits. (AllCombinationPrime)
   so max number  can be 
   n = 1 ... nmax times        
      Caclulated using Combinations:
      output = n      n     n     n
                C   +  C   + C   + C +....
                 x1     x2    x3    x4
      where x1, x2 <n & prime 

   Now n contains number > max and number < min

   subtract numbers greater than max (MoreThanMaxPrime)
   subtract numbers less than min (LessThanMinPrime)

   complexity = number of bits = log(n)
*/

uint64_t prime_bits(uint64_t max, uint64_t min)
{
    /* if same , then no processing needed */
    if (max == min) {
	printf("Same numbers. No Processing needed\n");
	return 0;
    }
    /* find max */
    if (max < min) {
	uint64_t temp = min;
	min = max;
	max = temp;
    }
    int MaxBits[64];
    int MinBits[64];
    int totalMax1count = decompose(max, MaxBits);
    int totalMin1count = decompose(min, MinBits);

    /* find the highest bit position in "max", which is set to 1 */
    int maxBitPos = 63;
    for (maxBitPos = 63; maxBitPos >= 0; maxBitPos--) {
	if (MaxBits[maxBitPos] != 0)
	    break;
    }
    /* find the highest bit position in "min", which is set to 1 */
    int minBitPos = 63;
    for (minBitPos = 63; minBitPos >= 0; minBitPos--) {
	if (MinBits[minBitPos] != 0)
	    break;
    }

    /* find prime bits numbers with options of filling all the bits uptom maxBitPos */
    uint64_t count = AllCombinationPrime(maxBitPos + 1);

    /* find prime bit combination less than "min", but included in "count" above */
    count -= LessThanMinPrime(MinBits, minBitPos, totalMin1count);

    /* find prime bit combination more than "max", but included in "count" above  */
    count -= MoreThanMaxPrime(MaxBits, maxBitPos, totalMax1count);

    return count;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
	printf("Wrong number of arguments\n");
	return -1;
    }
    uint64_t max = 0;
    uint64_t min = 0;
    if ((-1 == convertToUint64(argv[1], &max)) ||
	(-1 == convertToUint64(argv[2], &min))) {
	printf("Incompatible arguments\n");
	return -1;
    }
    printf("Result = %lld\n", prime_bits(max, min));
    return 0;
}
