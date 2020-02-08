#include <stdio.h>
#include <string.h>
 
 // static void integer_to_binary(int integer, char bin[]);
static int isNumeric (const char *s);
static void integer_to_binary(int integer, char bin[]);
static void integerToBinary(int number, int* bin);

int main()
{
  int n, c, k;
 
  // printf("Enter an integer in decimal number system\n");
  // scanf("%d", &n);
 
  // printf("%d in binary number system is:\n", n);
 
  // for (c = 15; c >= 0; c--)
  // {
  //   k = n >> c;
 
  //   if (k & 1)
  //     printf("1");
  //   else
  //     printf("0");
  // }
 
  // printf("\n");
  
  int binary_num;
  char str[17];
  memset(str,0,17);
  str[0] = '0';
  integer_to_binary(40, &str[1]);

  // char str[31];
  // memset(str,'0',31);
  // printf("after the function call, binary_num = %d\n",binary_num);
  // char* iter = str;
  // iter++;

  // sprintf(iter,"%d",binary_num);
  printf("str = %s\n",str);

  // char tmp[] = "apple";
  // char* iter = tmp;
  // printf("isNumeric(apple) = %d\n",isNumeric(iter));

  return 0;
}

static int isNumeric (const char *s){

    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char *p;
    strtod(s, &p);
    return *p == '\0';
}

static void integerToBinary(int number, int* bin){

  int n, remainder, binary = 0, place = 1;

    n = number;

    while (n > 0){

        remainder = n % 2;
        binary += remainder * place;
        place *= 10;
        n /= 2;
    }
    
    printf("Binary equivalent of %d is %d\n", number, binary);
    *bin = binary;

  }

static void integer_to_binary(int integer, char bin[]){

    int c,k;

    char* iter = bin;

    for (c = 14; c >= 0; c--)
      {
        k = integer >> c;
     
        if (k & 1){
            *iter = '1';
          // printf("1");
        }
        else{
          *iter = '0';
            // printf("0");
        }
        iter++;
      }
}



