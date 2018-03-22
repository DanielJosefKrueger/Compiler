/************************ lexan.cxx  Sommersemester 2018 ********************/
/*************** Scanner *******************************/


#ifndef GLOBAL_H 
#include "global.h"
#endif 



#define NORW   14   		/* Zahl der reservierten Worte */ 

#define RWSYMLEN  15		/* Max. L�nge reservierter Symbole */ 






int lineno;					/* Zeilennummer */                            		

int num ;                  	/* Wert einer int-Konstanten  */
double realnum; 			/* Wert einer real-Konstanten*/ 

char idname[BSIZE];      /* Name eines Bezeichners ;
							wird vom Parser weiterverwendet */ 


char actchar;                /* gelesenes Zeichen */ 
                      



/*  Um Bezeichner von reservierten Symbolene unterscheiden zu k�nnen,
wird eine Tabelle reservierte Worte verwendet (restable). 

Die Tabelle enth�lt Eintr�ge f�r jedes Schl�sselwort, bestehend aus 
dem Schl�sselwort selbst und dem zugeh�rigen Tokentyp (Codierung vgl.global.h):

Bei Erkennen eines m�glichen Bezeichners wird zuerst die Tabelle
der reservierten Symbole durchsucht (lookforres); 
wird ein Schl�sselwort gefunden, liefert lookforres den dem Schl�sselwort 
zugeordneten Tokentyp; sonst 0. Bei Ergebnis 0 liegt dann tats�chlich ein
Bezeichner vor. 
*/ 




/* Struktur eines Eintrags in der Tabelle reservierter Symbole */ 


struct ressw 

{ char ressymbol [RWSYMLEN];			/* Symbol */             
  int token;							/* zugeh�riger Tokentyp */ 
} ;



/* Tabelle reservierter Worte */ 


struct ressw 
			restable [] =  
				{	"const", CONST,
					"var", VAR,
					"procedure", PROCEDURE,
					"call", CALL,
					"begin", BEGIN,	
					"end", END,
					"if", IF,		
					"then", THEN,
					"else",ELSE,
					"while",WHILE,
					"do", DO,
					"int", INT,
					"real", REAL,
					"fi", FI
				};





/* Suchen nach einem reservierten Symbol */ 

int lookforres( char *s)

/* Sucht in Tabelle reservierter Worte nach s und liefert zugeh�rigen Token
   falls gefunden,
   sonst 0 
*/ 

{	struct ressw *ptr; 

	for ( ptr = restable; ptr < &restable[NORW]; ptr++ )

		if (strcmp (ptr->ressymbol,s) == 0)
								/* Symbol gefunden */
			return (ptr->token);
	
		return(0);				/* Symbol nicht gefunden */ 
}






/******************* Initialisieren des Scanners **********************/ 

/* wird aufgerufen von  initialize () aus init.cxx ; 
nimmt Vorbesetzungen der Variablen num, realnum,  idname und lineno vor; 


liest das erste Zeichen aus der Eingabe */ 



void initlexan()

{ num = NONE;						
  realnum = 0.0; 
  idname [0] = '\0';					
  lineno = 1;
  fin.get(actchar);			/* Erstes Zeichen der Eingabe lesen */  
    
}    






/*******************************************************************************/
/********** Funktion nextsymbol ************************************************/ 
/*******************************************************************************/


/**** zentrale Funktion zum Lesen des n�chsten lexikalischen Symbols ***************/ 
/*****  identifiziert n�chstes lexikalisches Symbol der Eingabe **********/

/***** WICHTIG!!!!!   
		Bei Aufruf von nextsymbol muss  sich das n�chste Eingabezeichen bereits in actchar befinden

*******/ 


/**    liefert Codierung des n�chsten Symbols (token): 

		- Konstante:				token == INTNUM und Wert der Konstanten in Variable num
									token == REALNUM und Wert in realnum
		- Bezeichner:				token == ID  und Zeiger auf Name in idname 
		- Schl�sselwort:			token == Tokentyp des reservierten Symbols nach 
									Suche in restable 
		- Operatoren,Sonderzeichen :entsprechende Token 

**/ 
										



int nextsymbol () 

{
	int token;  
	char lexbuf[BSIZE] ;		/* lokaler Puffer f�r Eingabezeichen */ 	



	while( !fin.eof())			/* Eingabe-Dateiende nicht erreicht */ 
	{ 

		if ( actchar== ' ' || actchar== '\t')
			/*Blank und Tab in Ausgabedatei kopieren und �berlesen */ 
			{	fout.put(actchar);
				fin.get(actchar);
				
		     }


		else if (actchar== '\n'  ||  actchar == '\r')  
			/* Newline in Ausgabedatei kopieren, �berlesen/entfernen, Zeilennummer erh�hen */ 
			{ fout.put(actchar);
			  fin.get(actchar);
			  lineno++;
			  }


		else if (isdigit(actchar))

				{ /***** actchar ist Ziffer --> Konstanten erkennen  *****/
					
					char zahl [BSIZE];		/* Puffer f�r Ziffern */ 
					int b = 0;				/* Zeichenzahl*/ 


                    while(isdigit(actchar)){
                        b++;
                        zahl[b] = actchar;
                        fin.get(actchar);
                    }

                    if(actchar != '.'){
                        // zahl zuende; ist int zahl
                        zahl[b+1] == '\0';
                        num = atoi(zahl);
                        return INTNUM;
                    }else{

                        // . gefunden => reale Zahl
                        b++;
                        zahl[b]=actchar;
                        while(isdigit(actchar)){
                            b++;
                            zahl[b] = actchar;
                            fin.get(actchar);
                        }

                        //reale zahl am ende
                        zahl[b+1] == '\0';
                        realnum = atof(zahl);
                        return REALNUM;
                    }
				}


		else if (isalpha(actchar))
				{ /***** actchar ist Buchstabe -->  Identifikatoren erkennen ****/
					
					
			
					int b = 0 ;				/* Zeichenzahl */
                    char buffer[30]; // buffer zum späteren erkennen des identifikators und zum testen, ob es ein Signalwort ist

					/* reg. Ausdruck   letter (letter|digit)*  erkennen ==>
					    solange Buchstaben oder Ziffern folgen --> Identifikator */ 
					idname[b] = actchar;

					fin.get(actchar);
					while((isalpha(actchar))||(isdigit(actchar))){
                        b++;
                        idname[b] = actchar;
					}
                    idname[b+1]='\0'; //String sauber abschließen

                    int ret = lookforres(buffer);
                    if(ret !=0){ //signalwort erkannt, dieses returnen
                        return ret;
                    }else{
                        return ID; // ID als Symbol returnen
                    }


				}

		/***** Sonderzeichen oder Operatoren erkennen ***************/ 
		

		else    
			{fout.put(actchar);				/* Zeichen in Ausgabedatei */ 
				
				switch(actchar)
				{	case '=':	fin.get(actchar);
                        return(EQ);
                    case '!':	fin.get(actchar);
                        if(actchar=='='){
                            return(NE);
                        }

                    case '<':	fin.get(actchar);
                        if(actchar=='='){
                            return(LE);
                        }else{
                            return(LT);
                        }
                    case '>':	fin.get(actchar);
                        if(actchar=='='){
                            return(GE);
                        }else{
                            return(GT);
                        }
                    case ':':	fin.get(actchar);
                        if(actchar=='='){
                            return(ASS);
                        }

                    case ',':	fin.get(actchar);
                        return(KOMMA);
                    case ';':	fin.get(actchar);
                        return(SEMICOLON);
                    case '+':	fin.get(actchar);
                        return(PLUS);
                    case '-':	fin.get(actchar);
                        return(MINUS);
                    case '*':	fin.get(actchar);
                        return(MULT);
                    case '/':	fin.get(actchar);
                        return(DIV);
                    case '(':	fin.get(actchar);
                        return(KLAUF);
                    case ')':	fin.get(actchar);
                        return(KLZU);
                    case '$':	fin.get(actchar);
                        return(PROGEND);
                    case ':':	fin.get(actchar);
                        return(KLZU);
                    default: 	error (32);
				} /* end-switch */ 
		} /* end-else */ 


 	}/* end while */ 


 	return(DONE); 	/* EIngabe -Ende erreicht */ 
		
}


