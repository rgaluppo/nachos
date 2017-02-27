#include "syscall.h"


int main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char *prompt ="Nachos_Shell> ";
    char  ch, buffer[60],argv[60];
    int i,j,t,join=0,argc=0;
/*
    prompt[0] = '-';
    prompt[1] = '-';
*/
  
    while( 1 )
   {
	
	Write("Nachos_Shell> ", 16, 1);

	i = 0;
	
	
	while(1)
	{
		
		t = Read(&buffer[i], 1, input); 
		while(buffer[0]== ' ')	
		   t = Read(&buffer[0], 1, input);
			 
		
		if(buffer[i] == '\0')
		{
			
			break;
		}
		
		if(buffer[i] == ' ')
		{
		/*Argumentos */
		buffer[i]='\0';
		argc++;
		i=0;
		do
			{
				t = Read(&argv[i],1,input);
				if(argv[i] == ' ')
				{	
					argc++;
							
				}
				if(	argv[i] == '\0')
				{
					break;
				}
				i++;
			}while(1);	
		
		break;
		}
		
		
		if(buffer[i] == '&'&& i==0){
		 join = 1;
		}
		else
		i++;
	} 
	

	if(buffer[0]=='q')
		{
			Write("\nSaliendo del Shell\n", 21, output);
			break;
		}

	if( i > 0 && argc == 0) 
	{
				
		newProc =Exec(buffer, argc, argv);
		if(join==0)
			Join(newProc);
	}
	else if(i>0 && argc > 0)
	{
		Write(buffer, 15, output);	
		newProc = Exec(buffer, argc, argv);
		if(join==0)
		 Join(newProc);	
		}
	for (j=0; j < i ; j++)
		buffer[j]='\0';

	}
	Exit(0);
}