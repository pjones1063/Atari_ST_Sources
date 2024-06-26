#include <osbind.h>
#include <stdio.h>
#include <aes.h>   
#include <vdi.h>
 
#define dsbl_cur printf("%s","\033f")  /* disable cursor (ie 'ESC' f)  */
#define enbl_cur printf("%s","\033e")  /* enable cursor   (ie 'ESC' e) */
#define cur_posn(row,col) putchar(27); putchar('Y'); putchar(row+32); putchar(col+32);
#define BELL printf("%s","\007")       /* ring bell  				           */
#define cls  v_clrwk(handle)
#define	mouse_off() graf_mouse(M_OFF,NULL);
#define	mouse_on()  graf_mouse(M_ON,NULL);
#define LINE 0   		                   /* line and column              */
#define COL 0                          /*          of cursor at input  */
#define TIME 10000L                    /* for delay routine       	   */
/* for Lattice C:- */
#define int    short   
   
int contrl[12],intin[128],ptsin[128],intout[128],ptsout[128];
int handle,ch,cw,bh,bw,rez_no;
int work_in[11],work_out[57];
int charw,charh,cellw,cellh;

char y_n,*dic1[][2] ={ 

"������", "k��f������v}d�������l�������ir~l��d~f�����um����g��j����pe�����k��c������j����",
"���������", "����o������od�������xm���tp��j��������h���������g������c��f������m��e�g������",
"������������", "��������k����e���������d����i�����������j��m���f���������en�������od���������l",
"������", "�����h����m��e����j��m�������j���~d�������l��d�����j�����e��h�����k����t������m",
"������", "j�e����vk�m�����ti�k��������uj�l�����sh�j���������wq",
"�����", "�����h�����p������j������f����k������g���k������g�����yc����h��k�������������q",
"������", "j�e����j»����f������m��e����xr",
"��������", "j����h�����������i��²����sh���������uo",
"�����������", "j����h��k�������h����m���f������m��e������zt",
"������v�������", "j��������l���e��������c��������xm�����h����������vp",
"���������", "j����u������rl",
"��������", "j�e�����kü��e��h���l���sm",
"���������", "j�e���v������e����j��m������wq",
"�����", "jl�t�vr����o��������lu�rn���i��l�������wq",
"���������", "j����������~d��������m��e�g��������e��������qk",
"��������", "j���g���k���������j����e������zt",
"����", "j������j������f��i��������g�i������l�g����l����f������m�����vp",
"�����������", "j��f���������sm",
"����������", "j��������l��������j���c�����������rl",
"����", "jl�t�vr��l�������uj��m����sh��k��������iwkt�qm��g���������tn",
"��������", "j����h�����~d�����������sm",
"�����", "j�����i��ùc��f��������d�������l���e��������qk",
"������������", "j���g��������e������l���e����������e��h����¯��f��������rl",
"�������", "j���g�������d��g������od����������pe��������od��������{j",
"�������", "j�e�����������f��i�»m�������j��m���������l���e�����kt��d��g��j��vqk",
"����������¶", "j�������k����������wl������ti��ñ��qf��i��l����f�h������rl",
"���������", "j����h������d���h����m�����h��k�������h���l���������k�������h��k������uo",
"������������", "j����h��k������sh���������g�������d��g�����µ����vp",
"�������������", "j����vp",
"��������", "j����u������rl",
"��������", "j��������������g��j���c�����i��������sh�������sm",
"�����", "j�����i�����rl",
"������", "j�������k��c�������k����enFqi����c�e����j��m��������k¼���tn",
"��������", "j�����uj������rg���º���qf�������qk",
"�����������", "j���������èd���������qk",
"���", "j���g�����m��������k����e��h�������sm",
"��������", "j�����i��������g��jyz{crl",
"�������������", "j�������k��������ir�l�����g��j�²���rg������od���vrxr",
"��������", "j�e���������pe�g���Ćm���f�����zt",
"����������", "j��������l�������i���m�������xr",
"�����", "j���g���������f��i��������g��j���c�������kö��e��������c�����i��lĤ���uo",
"����������", "j���g���k��������i���|�������j��m������wq",
"����", "j����h��k�����������lĬ��f����k�������h�����c�������k�è�e�����k��c������xr",
"������", "j����h����m����sh���zm����g��������e����j���c�����i��l���e�����k������uo",
"���������", "j������j����d�f������{j",
"������", "j��f����������tn",
"��������", "j�e������l��d����i��lä����h���������g�i�����d�������zt",
"��������", "jl�t��wsk����en�h����m���ouo",
"��������", "j�e����j������s������m����g��j�����sm",
"���������", "j�e�����������f�������c����������c��f�����l��d����wq",
"�������", "j��������l����f��i���Ǩd����i���{j",
"���������", "j����������c����h�����c�������ys",
"������", "j�e������l�����������m�d������k��c����������qk",
"�����������", "jl��g��j������og�������d��g���k�����f��i���m����sh��k�°���g��j�����l",
"����", "j�e����j����d��g���k�����f��i�k����e������l�����uo",
"����������", "j��������l�����g������c�������k����e�����k��c��������l�����������m���m",
"������", "����g���k�¤����ti��l������h��k�������hq��lĲ����h���um���f�����l��d���������",
"������", "����g��j���c������w����d������k��c���g�����m��������k��c���������y������uj���",
"�����", "����g�������d�����j����d��g����l����f��i�����d�������l������vi����c��f�������",
"�������", "j�e������y������h��������f��i�k�¬�����uj����pe����j���qk",
"�����������", "j���g��������e��h���®����uo",
"���������", "�d���������c��������xmk���ph�j��®�e����j�����e��h�������qf��i�k�����f�����z",
"������", "j�����uj����pe������lu��en��i����ld�����vk���mqf�������qk",
"������", "�����h��������rg���������fo������p�����u������nf��������d��g����l�c����h����",
"�������", "jd�f�������c����h��k�m������wq",
"�������", "�����h��������f��i�k������g���k���d��g�����m��������wl�c����ti��®����g���y",
"������", "����g��j���c�����i��l�����g��j�������g���k���d�������l�����g��j����d�������z",
"��������", "������i��l�������i�������f�������c����h���l��������������e��h�������e��������",
"������", "jd�f���������qf�h�������sm",
"��������", "j�����������pe�����k��c�����uj������rg�������d����i��l��������j�����e���wq",
"��������", "jd�f������m����g����������g�����m����g��j����d��������{j",
"�����", "j�e���������d�����j��m������p",
"��������", "�d�����j��m�����h������d���������c���g��j������f��i���m�������x",
"��������", "���f�����l��d���h�����p�����i��l�c������xr",
"�����", "j���g�����m���f����k�������h��k���d�����j���������wq",
"��������", "j�e�������m��e��h�������������k��������i��l�����g����zt",
"�������", "�����������c��f�����l���e��������c�������k�������ti�������f��i�����d���������",
"�������", "j�e����j��m���s�����l���������k�������h��k�����f�����l���e����xr",
"�����", "j�e������l�����sh�j����������vk�m������������d��g���k����e��h�j�����e���wq",
"������", "j���g����l��������j����e����j���c�����������rl",
"�����������", "jd�f����xî����uo",
"����������", "��e�����k����e����j�l����f�����l����f����k����e��h�jµ����s����ys",
"����������", "j���g�������d�������l��d��������yc��������l��d��������m��e��h�����qk",
"�����", "j���g������c����h��k�m����g����l�����g��j����d�����xr",
"����", "j�e�����k�������h�j�����e�������m���f��i����c��f����k��c�������kî���tn",
"�������", "�d������k�����s�����l�����g��j����p������w����q������l���e���i�������f�������",
"����", "k�������k��c������sks�c�����p�k������nhq����������i�����m",
"������", "j�e�����x������g������c������j�l�����g��j������tn",
"�����", "j�e����Æl���e�����k��c������j�����r������zt",
"������", "j�e�����wl�c�������k����sm",
"���������", "j�e������lè����h������d��g������������wq",
"��������", "j�e����j��m����g�������rl",
"�������", "jd�������yƨ����ti��û�rl",
"�����������", "j�������k��c������j��¶��f��i������e��h���l±����������qk",
"��������", "j��f�������c��f��i����c�e�������yc�������kõ���f���j�������g������qk",
"������", "j�e�������m�����h����m����g������c��f�������c���g������p������j­���sm",
"����������", "j�������k�������h��k������������m�d������k��������i��l�������i�����rl",
"��������", "j���g�������d�������lĬ��f���j����d���h�����������i��l������vp",
"����������", "j�e�����k������g����l�������wq",
"��������", "����g��j�l���e����������e������l������h����m�������j�����������k��c������x",
"", ""   };

void main()
{
	int rect1[]={0,0,639,199},rect2[]={0,0,639,399};  
	char ch, cha,dic2a[16],dic2b[80],temp[16];
	int slctd_wd,i,j,k,count,q;
	cls;
	k=0;
 	dsbl_cur;
 	mouse_off();       
	rez_no = Getrez();
	if (k==0 && rez_no==0)
	{
		not_low();
 		goto end;  
	}
	gem_on();   
 	vsf_color(handle,3);   
 	slctd_wd = (rand_func()%99)+1;  /*  selects word  at a random point
 	                                             in 100 word 'dictionary' */
	keybd_repeat(100,-1);    /* '100' reduces possibility of inadvertent
		         		multiple responses to initial key press, '-1' maintains
		         		existing (ie OS) response time for second and	
		         			                          and subsequent key presses. */ 
	if (k==0 && rez_no==1) 
	{
	 	v_bar(handle,rect1);   /* draw filled rectangle 	                  */ 
		at(24,14,"(Note: this program will also run in high resolution.)"); 
	}
	if (k==0 && rez_no==2)
	{
	 	v_bar(handle,rect2);  
    at(24,10,"(Note: this program will also run in medium resolution colour.)");  
	}
	title(rez_no);
 	do
	{
  	for (i = 0; i < 16; i++)
   		dic2a[i] = '\0';       /* ensures dic2a contains nothing           */
  	for (i = 0; i < 80; i++)
    	dic2b[i] = '\0';       /* ensures dic2b contains nothing           */
   /*    decode sequence   */
  	j = 67;
  	for (i = 0; i < strlen(dic1[slctd_wd][0]); i++)
  	{
  		dic2a[i] = dic1[slctd_wd][0][i]-j;
    	j++;
    	if (j > 77)j = 67;
  	}
  	j = 67;
  	for (i=0; i < strlen(dic1[slctd_wd][1]); i++)
  	{
  		dic2b[i] = dic1[slctd_wd][1][i]-j;
  		j++;
  		if (j > 77)j = 67; 
  	}      
		k=1;
  	count = 0;          /* count number of guesses                       */
  	for (i = 0; i < strlen(dic2a); i++)
  	 	temp[i] = '-';    /* puts hyphens at beginning of temp[16] equiv  
                        to number of characters in word dic[slctd_wd][0] */
  	for (i = strlen(dic2a); i < 15; i++)
  	 	temp[i] = '\0';   /* puts \0 in remainder of temp[16] ie 16    
  	                               minus number of characters in dic2a   */
		cls;   
/*    letter guess and display sequence     */
		cur_posn(LINE+1,COL);	
/*    give advance showing of randomly selected word (if wanted)         
	  printf("\t\t    (Randomly selected word is:  '%s'  )\n", dic2a);   */
  	printf("\n\t\tFind a word and, maybe, extend your vocabulary.");
  	printf("\n\t\t----------------------------------------------");
		printf("\n\n\t  Find this word  %s  by guessing its individual letters.\n\n", temp);
  	do 
  	{
    	printf("\tEnter your guess: ");
 			cha = getch();
/* does guess match any letter/s in word; if so substitute for hyphen/s  */
    	for (i = 0; i < strlen(dic2a); i++)
      	if (cha == dic2a[i])
      		temp[i] = cha;
    	count++;
    	printf("%s\n", temp);
  	}
  	while (strcmp(temp, dic2a)); /*  until 'temp' matches selected word  */
  	printf("\n\t\t\t  %s is correct.\n\n", dic2a);
  	printf("\tYou managed it in %d guesses.", count);
  	for (i = 0; i < strlen(temp); i++)
  	{
  		ch = temp[i];              /* convert word to upper case           */
  		ch = toupper(ch);
  		temp[i] = ch;
  	}
  	printf("\n\n\n\t\t\t  %s means (or is):\n\n",temp);
  	for (q = 0; q < (80-strlen(dic2b))/2; q++) /*  centralises 'dic2b'   */
  		printf(" ");
  	printf("%s", dic2b);
  	for (i = 0; i < strlen(dic2a); i++)
  		temp[i] = '-';   /*  replace all characters of last word with '-'  */     
  	slctd_wd++;
  	if (slctd_wd > 99) slctd_wd = 0;/* back to beginning of 'dictionary' */
  	printf("\n\n\t\t\t  ----------------------  ");
  	printf("\n\t\t\t  Try another word? Y/N:  ");
  	printf("\n\t\t\t  ----------------------  ");
  	y_n = getch();
		cls;
  	y_n = toupper(y_n);
 		if (y_n!='Y' && y_n!='N')
		not_Y_or_N();
  }
  while (y_n == 'Y');    /* do it again?                                 */
  end: 
 	enbl_cur;
 	mouse_on();
  keybd_repeat(15,-1);  /*  '15' puts speed of first keypress  response
                                                  approx. back to normal */
	cls;
	gem_off(); 
	exit(0);
}

not_low()               /*  if in low res.                               */  
{
	at(6,0,"Program does not run in low resolution;");
	at(8,3,"please switch to high or medium."); 
	at(11,15,"Press a key to exit.");
 	evnt_keybd();
}

gem_on()
{
	int I;
	appl_init();
	handle=graf_handle(&ch,&cw,&bh,&bw);
	for (I=0;I<10;I++)
		work_in[I]=1;
	work_in[10]=2;
	v_opnvwk(work_in,&handle,work_out);
}

at(x,y,string)
int x,y;
char string[];
{
	cur_posn(LINE+x,COL+y);
	printf(string);
}

title(int d)
{
	int rect3[]={60,25,580,150},rect4[]={80,35,560,140};/* for 1 (med rez) */  
	int rect5[]={60,50,580,300},rect6[]={80,70,560,280};/* for 2 (high rez)*/  
	int i,charw,charh,cellw,cellh;
	vsf_interior(handle,2); /* these two lines give style of               */
	vsf_style(handle,12);   /*              fill for first rectangle       */
  vsf_color(handle,2);    /* colour for first rectangle                  */    
	vst_point(handle,18,&charw,&charh,&cellw,&cellh);	/* changes height
                                                  and thus width of text */
	if (d==1)               /* ie if med. rez.                             */
	{	
	  v_bar(handle,rect3);	/* draws rectangle 	                           */ 
		vsf_color(handle,0);  /* colour for 2nd rectangle                    */
		v_rfbox(handle,rect4);/* draws 2nd rectangle (white filled) over
		                                                    centre of first  */
		v_justified(handle,100,80,"FIND A WORD AND, MAYBE,",400,0,1);/* prints
                                                          justified text */
		v_justified(handle,130,110,"EXTEND YOUR VOCABULARY!",400,0,1);	
	}
	if (d==2)               /* ie if high rez.                             */
  {
    v_bar(handle,rect5); 	
		vsf_color(handle,0);
		v_rfbox(handle,rect6);
		v_justified(handle,100,160,"FIND A WORD AND, MAYBE,",400,0,1);	
  	v_justified(handle,130,220,"EXTEND YOUR VOCABULARY!",400,0,1);
	}
	for (i=0;i<4;i++)
	{
		at(20,55,"           ");
		delay(TIME*5);
		at(20,55,"Press a key");
		delay(TIME*5);
	}
evnt_keybd();
}
   
unsigned int rand_func()
	{
		int utime;
		long ltime;
		ltime =time(NULL);/* using system time to 'seed'
		                                        random number generator      */
		utime=(unsigned int) ltime/2;      
		return (utime);
	}

delay(t)			            /*  delay routine                              */
long t;
	{
		long i; 
		for(i=1;i<t;i++);
	}	

not_Y_or_N()           /*  if 'Y', 'y', 'N' or 'n' not pressed           */
{		
	int i,x,y;
	int rect7[]={88,100,552,300};  /*  rez 2                               */
	int rect8[]={88,50,552,150};   /*  rez 1                               */
	vsf_color(handle,2);    
	vst_point(handle,12,&charw,&charh,&cellw,&cellh);	
	do
	{
		cls;
		if (rez_no == 2)
		{
			x=84;
			y=116;
  		v_bar(handle,rect7); 
  	}	 
		if (rez_no == 1)
  	{
  		x=0;
  		y=0;
  		v_bar(handle,rect8);	  
 		}
    BELL;
    for (i=0;i<5;i++)
    {
  		v_justified(handle,180,x+84,"                                   ",280,1,1);
      delay(TIME*5);
  		v_justified(handle,180,x+84," YOU  DID  NOT  PRESS  'Y' OR 'N'! ",280,1,1);
			delay(TIME*8);
		}
 		v_justified(handle,232,y+116," PLEASE  DO  SO  NOW. ",176,0,1);
  	y_n = getch();
  	y_n = toupper(y_n);
  }
 	while (y_n!='Y' && y_n!='N');
	cls;
  return(y_n); 
}	

int keybd_repeat(int a, int b)
{
	Kbrate(a,b);                  
	return(keybd_repeat);
}

gem_off()
{
	v_clsvwk(handle);
	appl_exit();
}