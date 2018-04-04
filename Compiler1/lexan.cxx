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

{
    num = NONE;
  realnum = 0.0; 
  idname [0] = '\0';					
  lineno = 1;
  fin.get(actchar);			/* Erstes Zeichen der Eingabe lesen */  
    cout <<" Lexan wurde erfolgreich initialisiert" <<endl;
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

void read_and_write(){
    fout.put(actchar);
    fin.get(actchar);
}


int nextsymbol () 

{
    //cout<< "nextsymbol() called" << endl << std::flush;;
	int token;  
	char lexbuf[BSIZE] ;		/* lokaler Puffer f�r Eingabezeichen */ 	



	while( !fin.eof())			/* Eingabe-Dateiende nicht erreicht */ 
	{ 

		if ( actchar== ' ' || actchar== '\t')
			/*Blank und Tab in Ausgabedatei kopieren und �berlesen */ 
			{	read_and_write();
		     }


		else if (actchar== '\n'  ||  actchar == '\r')  
			/* Newline in Ausgabedatei kopieren, �berlesen/entfernen, Zeilennummer erh�hen */ 
			{ read_and_write();
			  lineno++;
			  }


		else if (isdigit(actchar))

				{ /***** actchar ist Ziffer --> Konstanten erkennen  *****/
                    //cout << "Zahl am Anfang erkannt" << std::flush;
						/* Puffer f�r Ziffern */
					int b = 0;				/* Zeichenzahl*/

                    while(!fin.eof() && isdigit(actchar)){

                        lexbuf[b++] = actchar;
                        read_and_write();
                    }

                    if(actchar != '.'){
                        // zahl zuende; ist int zahl
                        lexbuf[b] = '\0';
                        num = atoi(lexbuf);
                        return INTNUM;
                    }else{
                        // . gefunden => reale Zahl
                        lexbuf[b++]=actchar;
                        read_and_write();
                        while(!fin.eof() && isdigit(actchar)){
                            lexbuf[b++] = actchar;
                            read_and_write();
                        }

                        //reale zahl am ende
                        lexbuf[b] = '\0';
                        realnum = atof(lexbuf);
                        return REALNUM;
                    }
				}


		else if (isalpha(actchar))
				{ /***** actchar ist Buchstabe -->  Identifikatoren erkennen ****/
					
					
			
					int b = 0 ;				/* Zeichenzahl */


					/* reg. Ausdruck   letter (letter|digit)*  erkennen ==>
					    solange Buchstaben oder Ziffern folgen --> Identifikator */
                    lexbuf[b++] = actchar;
                    read_and_write();
					while((!fin.eof()) && (isalpha(actchar)||(isdigit(actchar)))){

                        lexbuf[b++] = actchar;
                        read_and_write();

					}
                    lexbuf[b]='\0'; //String sauber abschließen

                    int ret = lookforres(lexbuf);
                    if(ret !=0){ //signalwort erkannt, dieses returnen
                        return ret;
                    }else{
                        strcpy(idname, lexbuf);
                        return ID; // ID als Symbol returnen
                    }


				}

		/***** Sonderzeichen oder Operatoren erkennen ***************/ 
		

		else    
			{			/* Zeichen in Ausgabedatei */
				
				switch(actchar)
				{	case '=':	read_and_write();
                        return(EQ);
                    case '!':	read_and_write();
                        if(actchar=='='){
                            read_and_write();
                            return(NE);
                        }

                    case '<':	read_and_write();
                        if(actchar=='='){
                            read_and_write();
                            return(LE);
                        }else{
                            return(LT);
                        }
                    case '>':	read_and_write();
                        if(actchar=='='){
                            read_and_write();
                            return(GE);
                        }else{
                            return(GT);
                        }
                    case ':':	read_and_write();
                        if(actchar=='='){
                            read_and_write();
                            return(ASS);
                        }else{
                            return COLON;
                        }

                    case ',':	read_and_write();
                        return(KOMMA);
                    case ';':	read_and_write();
                        return(SEMICOLON);
                    case '+':	read_and_write();
                        return(PLUS);
                    case '-':	read_and_write();
                        return(MINUS);
                    case '*':	read_and_write();
                        return(MULT);
                    case '/':	read_and_write();
                        return(DIV);
                    case '(':	read_and_write();
                        return(KLAUF);
                    case ')':	read_and_write();
                        return(KLZU);
                    case '$':	read_and_write();
                        return(PROGEND);
                    default: 	error (32);
				} /* end-switch */ 
		} /* end-else */ 


 	}/* end while */ 


 	return(DONE); 	/* EIngabe -Ende erreicht */ 
		
}








