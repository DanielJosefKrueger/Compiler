/******************************** init.cxx ************************/



/* Initialisierungen f�r Compiler PLA  

		Stand Sommersemester 2018
		

*/





#ifndef GLOBAL_H 
#include "global.h"
#endif 


ifstream fin;					/* Zu �bersetzende Eingaben */ 
ofstream fout,					/* Ausgabedatei */
	 ferr,					/* Fehlerausgaben */
	 fsym,					/* Ausgabe f�r Symboltabelle */
	 trace;					/* Ausgabe f�r Trace */



int tracesw;
int level=0; 

/***************** Initialisierungen durchf�hren ******************/ 

void initialize()
{ 
  char c;
  char filename[20]; 

  
  cout <<"\n Name der Eingabedatei eingeben-->"; 
  cin >> filename; 
  
  /* Eingabefile �ffnen*/
  fin.open(filename, ios::in);  
    
 // fin.open( "pla_input", ios::in);  

 cout << "\n Name der Ausgabedatei eingeben-->"; 
 cin >> filename; 

  /* Ausgabedatei �ffnen */ 
  fout.open(filename,ios::out);
  
  
//  fout.open("pla_ou",ios::out);
  
    fout<<"\n\n***************** Ausgabe ***************\n"; 
  
  
    	                      
 cout<<"\n Name der Fehlerdatei eingeben-->"; 
 cin >>  filename; 
  
  /* Fehlerdatei �ffnen */ 
  ferr.open(filename,ios::out);
  
  
//  ferr.open("err.txt",ios::out);

  ferr<<"\n\n**************** Fehlermeldungen*************\n";
  
  

  cout<<"\n Trace gew�nscht ? (y/n): "; 
  cin>> c;
  if ( c == 'y')                  
     { /* Datei f�r Trace  �ffnen */
       trace.open("trace.out",ios::out);
       tracesw = TRUE; 
      }
  else tracesw = FALSE;              



	/* Datei f�r Symboltabellenausgabe �ffnen */   	  
	
  fsym.open("symtable.out",ios::out);
  
  fsym<<"\n\n**************** Symboltabellen*************\n";
  


 /* Initialisieren des Scanners */
  initlexan(); 
  
  
 }



/************ Endebehandlung bei fehlerfreier �bersetzung  *****/
/* zur Zeit nur Ausgabe der Symboltabelle */


void stop()

{ /* Symboltabellen ausgeben  */

  printsymtab(firstsym);

}



 
