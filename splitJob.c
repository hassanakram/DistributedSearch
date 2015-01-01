#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>



//make file chunks ... yeh parts ke struct ka aaray benena hai in requester
struct Part
{
    int start_address;
    int total_lines;
};


struct Part * MakeFileChunks(char * filename, int num_chunk)
{    

	struct Part * parts=(struct Part *)malloc(num_chunk*sizeof(struct Part));

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  double count_lines=0, parts_count=0;
  int part_index=0, index=0, i=0;
  long pos;
  double max_parts=num_chunk;


  fp = fopen(filename, "r");
  if (fp == NULL)
  {
      printf("No file found");
      return;
  }
  
  while ((read = getline(&line, &len, fp)) != -1) 
  {
      count_lines++;
  }

  
  free(line);
  

  parts_count=count_lines/max_parts;
  parts_count=ceil(parts_count);

  rewind(fp);
  do
  {
      if(index == parts_count * i)
      {
          pos = ftell(fp);
          parts[part_index].start_address=pos;
          parts[part_index].total_lines=parts_count;
          part_index++;
          i++;
      }
      index++;
  }
  while((read = getline(&line, &len, fp)) != -1);
  //fclose(fp);
  
  return parts;
}
