#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"



struct Performance *newPerformance(){
  struct Performance *p_array = malloc(sizeof(unsigned int)*4); /*  malloc memory to hold array    */
  p_array->reads = 0;
  p_array->writes = 0;  /*  set initial values   */
  p_array->mallocs = 0;
  p_array->frees = 0;
  if (p_array == NULL){
    fprintf(stderr,"ERROR: memory not allocated.\n");
    exit(1);
  }
  return p_array;
}

struct Array *newArray(struct Performance *performance, unsigned int width, unsigned int capacity){
  struct Array *arr = malloc(sizeof(unsigned int)*3); /*   allocate memory of struct array  */
  arr->width = width;
  arr->capacity=capacity;   /* set initial values  */
  arr->nel=0;
  arr->data  = malloc(sizeof(unsigned char)*capacity*width); /*   allocate memory of void data array   */
  performance->mallocs += 1;  /*    increment number of mallcs  */
  if (arr == NULL||arr->data==NULL){
    fprintf(stderr,"ERROR: memory not allocated.\n");
    exit(1);
  }
  return arr;
}


void readItem(struct Performance *performance, struct Array *array, unsigned int index, void *dest){
  if (index >= array->nel){
    fprintf(stderr,"ERROR: index greater than number of elements in array.\n");
    exit(1);
  }
  unsigned char *ptr =(unsigned char*)dest;   /*   cast to unsigned char to dereference   */
  unsigned char *values = (unsigned char*)array->data;
  memcpy(ptr,values+array->width*index, array->width);  /*  copy void pointers into dest but offset by the number of bits    */
  performance->reads+=1; /* increment number of reads     */
}


void writeItem(struct Performance *performance, struct Array *array, unsigned int index, void *src){
  if (index > array->nel || index >= array->capacity){
    fprintf(stderr,"ERROR: index greater than number of elements and capacity in array.\n");
    exit(1);
  }
  unsigned char *ptr =(unsigned char*)src;    /*   cast to unsigned char to dereference   */
  unsigned char *values = (unsigned char*)array->data;
  memcpy(values+array->width*index,ptr, array->width); /*  copy void pointers into dest but offset by the number of bits    */
  if (index == array->nel){
    array->nel +=1;  /* increment number of elements in array */
  }

  performance->writes+=1;  /*incremnt how many time it writes  */
}


void contract(struct Performance *performance, struct Array *array){
  if (array->nel == 0){
    fprintf(stderr,"ERROR: no elements in array.\n");
    exit(1);
  }else {
  array->nel -= 1;
  }
}


void freeArray(struct Performance *performance, struct Array *array){
  free(array->data);
  free(array);    /*    free all allocated memory  */
  performance->frees+=1;
}

void appendItem(struct Performance *performance,struct Array *array, void *src){
  writeItem(performance,array,array->nel,src);
}



void insertItem(struct Performance *performance,struct Array *array,unsigned int index, void *src){
  int i = 0;
  unsigned char *values = malloc(sizeof(array->width));
  for(i=array->nel; i > index;i--){    /*  increment backwards because starts at top of elements    */
    readItem(performance,array,i-1,values);  /* makes the value in the previous index written into the current index    */
    writeItem(performance,array,i,values);
  }
  if (i==index){
     writeItem(performance,array,i,src);   /*    write in the number in index  */
   }
   free(values);
}

void prependItem(struct Performance *performance,struct Array *array, void *src){
  insertItem(performance,array,0,src);
}

void deleteItem(struct Performance *performance,struct Array *array,unsigned int index){
  int i=0;
  unsigned char *values = malloc(sizeof(array->width)); /*   allocate space for values   */
  for(i=index ;i < array->nel-1;i++){  /*    do until the index reaches the number of elements  */
    readItem(performance,array,i+1,values);  /*    read in the values at one index higher and then write it back in that index  */
    writeItem(performance,array,i,values);
  }
  contract(performance,array); /*   delete last item in array  */
  free(values); /*   free memory allocated   */
}

int compar(const void *src,const void *src2){
    const char *s1, *s2; /*    cast void pointers to const chars to compare  */
    s1 = src;
    s2 = src2;
    return strcmp(s1,s2);	/* use string comparison to compare the strings */
}

int findItem(struct Performance *performance, struct Array *array, int (*compar)(const void* , const void *),void *target){
    int i = 0;
    unsigned char *ptr =(unsigned char*)array->data;  /* cast to unsigned chars */
    unsigned char *target2 =(unsigned char*)target;
    for (i=0;i<array->nel;i++){
      readItem(performance,array,i,ptr);
      if ((*compar)(target2,ptr)==0){   /* point to function pointer */
        return i;
      }
    }
    return -1;
}


int searchItem(struct Performance *performance, struct Array *array, int (*compar)(const void* , const void *),void *target){

  int low =0;
  int mid = 0;
  unsigned char *ptr =(unsigned char*)array->data;
  unsigned char *target2 =(unsigned char*)target;
  int high = array->nel-1;
  /*if(ptr == 0){
    high = array->nel;
  }else{
    high = array->nel-1;
  }*/
  while(high>=low){
    mid = (high+low)/2;  /* find middle value in list */
    readItem(performance,array,mid,ptr);
    if((*compar)(target2,ptr)>0){
      low = mid + 1;
    }else if((*compar)(target2,ptr) < 0){
      high = mid - 1;
    }else if((*compar)(target2,ptr)==0){
      return mid;
    }
  }

  return -1;
}









/**/
