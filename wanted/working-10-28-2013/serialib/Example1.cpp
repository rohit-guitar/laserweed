#include <stdio.h>
#include "serialib.h"
#include <vector>
#include <string>

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyO4"
#endif

using namespace std; 
vector<string> tokens; 

void printbuffer(char *);
int main()
{
   	int count = 0;
    	serialib LS;                                                            
   	int Ret;                                                                
    	char Buffer[128];
	char temp_Buffer[128]; 

    	Ret=LS.Open(DEVICE_PORT,19200);                                        
    	if (Ret!=1) {                                                           
        printf ("Error while opening port. Permission problem ?\n");        
        return Ret;                                                         
	}

	while(1)
	{
		int count=0; 
		Ret=LS.ReadString(Buffer,'\n',128,2000);                                
		if (Ret>0)
		{       
			char *ch;
  			ch = strtok(Buffer, ",");
	  		while (ch != NULL) 
			{
				tokens.push_back(ch);
  				ch = strtok(NULL, ",");
  			}
			cout << tokens[1] << " " << tokens[2] << " " << tokens[4] << endl;
			tokens.clear();  	
		}
		else
		cout << "Time out!" << endl;  
	}
   	LS.Close();
    	return 0;
}


