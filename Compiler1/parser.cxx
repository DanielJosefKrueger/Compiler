   
/**************** parser.cxx  Sommersemester 2018******************/

/*******   **************/

#ifndef GLOBAL_H
#include "global.h"
#endif


int lookahead;					/* lookahead enth�lt n�chsten EIngabetoken */     
 

int exp(); 
int nextsymbol(); 
 

/******************  factor  **********************************************/
/* analysiert wird der korrekte Aufbau eines Faktors 

			

Schnittstelle: 

	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead
							



*/ 

	

int factor()
 {	int kind;
    	st_entry *found;		// Zeiger auf Eintrag in ST 
	int factor_typ; 
    
	if (tracesw)
	trace<<"\n Zeile:"<< lineno<<"	Faktor";


	switch(lookahead)	// je nach n�chstem Eingabesymbol in lookahead 
	{
		case KLAUF:	/* Symbol '(' folgt --> (EXPRESSION) erwartet*/  
					
					lookahead=nextsymbol();
					exp();
					if(lookahead== KLZU)	
						// korrekt ; n�chstes Symbol lesen --> Ende 
						lookahead = nextsymbol();
					else 
						error(27); // kein Faktor 
					break;


		case INTNUM:
		 			/* Int-Zahl (INTNUMBER) gefunden --> okay */
					lookahead=nextsymbol();
                    cout << "Wert von num: " << num << endl;
					break;


		case REALNUM: 		/* Real-Zahl (REALNUMBER) gefunden --> okay */
					lookahead=nextsymbol();
					
					break;



		case ID:	/* Identifikator (ID) gefunden  */ 
					/* Suche Identifikator in Symboltabelle ;
						angewandtes Auftreten --> 
						Deklaration muss vorhanden sein
						und also Eintrag in ST */ 

					found = lookup(idname);
				   
					
					if (found == NULL)
						/* nicht gefunden --> Fehler: Id nicht deklariert*/ 
						error(10);

					else	// Id in ST gefunden ; Art pr�fen 

						{kind = found->token;	// Art des ST-Eintrags 
						
						switch(kind)
						{ case KONST:	// Konstantenname --> okay 
										
										break;
											
						  case INTIDENT:// einfache Variable, Typ int --> okay 
										
										break;

						  case REALIDENT:// einfache Variable, Typ real --> okay 
										
										break;

						


						  case PROC:	// Name einer Prozedur in
										// Factor nicht erlaubt
										error(20); // --> exit 
										// break;
						
						} // endswitch (kind) 
					
					   // n�chstes Symbol lesen 
						
				       lookahead=nextsymbol();
				     }	// endif 
				   
					
					break;

		default:	// kein korrekter Faktor 
					error (27);
	}	// endswitch (lookahead) 

	return (0);
} 	// end factor     
   








/******************  term ***************************************************/
/* analysiert wird der korrekte Aufbau eines Terms nach folgender Syntax:
			
			TERM	::=		FACTOR  { '*' FACTOR |  '/' FACTOR }*
			
Schnittstelle: 
	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead
							Typ des Terms ist Funktionswert

*/ 


int term()
{	 int ret; 

	if (tracesw)
	    trace<<"\n Zeile:"<< lineno<<"Term:";


	ret = factor();
	// korrekter Factor 
	
	while(lookahead == MULT || lookahead ==DIV)
		// solange * oder / folgt, muss Factor kommen
		
		{// n�chstes Symbol lesen 
		 lookahead=nextsymbol();
			ret = factor(); 
		
			 
		}
	return(0);
 }	// end term 




/******************  exp ***************************************************/
/* analysiert wird der korrekte Aufbau eines Ausdrucks nach folgender Syntax:
			
			EXPRESSION	::=		TERM { '+' TERM |  '-' TERM}*
			
Schnittstelle: 
	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead
							Funktionswert ist Typ des Ausdrucks
*/ 

int exp()
{
	int typ_left,typ_right;
	if (tracesw)
	    trace<<"\n Zeile:"<< lineno<<"Ausdruck";

	typ_left = term();
	// korrekter Term 

	while (lookahead == PLUS || lookahead == MINUS )
			// solange + oder - folgt, muss Term kommen

		{// n�chstes Symbol lesen 
		 lookahead=nextsymbol();
		 // Term pr�fen 
		 typ_right = term();
		 // nach korrektem Ende wurde n�chstes Symbol gelesen 
		
		}
	return (0);
}	// end exp 




/******************  condition ***************************************************/
/* analysiert wird der korrekte Aufbau einer Bedingung nach folgender Syntax:
			
			CONDITION	::=		EXPRESSION  RELOP  EXPRESSION
			
Schnittstelle: 
	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead

*/ 

int condition()

{   int typ_left, typ_right; 


	if (tracesw)
	    trace<<"\n Zeile:"<< lineno<<"Condition";


	typ_left = exp();
	// korrekter Ausdruck 
	// relationaler Operator muss folgen 

	switch(lookahead)
	{
		case EQ:
		case NE:
		case LT:
		case LE:
		case GT:
		case GE:// n�chstes Symbol lesen 
				lookahead=nextsymbol();
				// Ausdruck muss folgen 
				typ_right = exp();
				 
				break;

		default: // kein relationaler Operator 
				 error(19);
	}
	if (typ_left != typ_right) 
		errortext("Typen der Operanden nicht kompatibel");

	return(typ_left); 
}  // end condition 




/****************** statement ***************************************************/
/* analysiert wird der korrekte Aufbau eines Statements nach folgender Syntax:
			
			STATEMENT 	::=		IDENT  ':=' EXPRESSION  
							|	call IDENT
							|	begin STATEMENT { ';' STATEMENT }* end
							|	if CONDITION then STATEMENT [else STATEMENT ] fi
							|	while CONDITION do STATEMENT



Schnittstelle: 
	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead

*/ 



void statement()
{ 
  st_entry *found;		// Zeiger auf ST-Eintrag
  int typ_left, typ_right; 
  
  if (tracesw)
      trace<<"\n Zeile:"<< lineno<<"Statement";


  // �berpr�fung des aktuellen lex. Symbols

  
  switch(lookahead){
      case ID:
          found = lookup(idname);
          if(found == NULL){
              errortext("Variable wurde nicht deklariert");
          }

          if(!((found->token == INTIDENT) || (found->token == REALIDENT))){
              errortext("Linker Operator einer Wertzuweisung muss eine Variable sein");
          }

          lookahead = nextsymbol();
          if(lookahead== ASS){
              lookahead = nextsymbol();
              exp();
              return ;
          }else{
              errortext("Kein ASSIGN nach ID in STATEMENT");
          }

      case CALL:
          lookahead = nextsymbol();
          if(lookahead ==ID){

              found = lookup(idname);
              if(found == NULL){
                  errortext("Procedure wurde nicht deklariert");
              }
              if(found->token != PROC){
                  errortext("Nach call muss eine deklarierte Procedure kommen");
              }
              lookahead = nextsymbol();
              return ;
          }else{
              errortext("Kein ID nach CALL in STATEMENT");
          }

      case BEGIN:
          lookahead = nextsymbol();
          statement();
          //lookahead = nextsymbol();
          while(lookahead==SEMICOLON){
              lookahead = nextsymbol();
              statement();
              //lookahead = nextsymbol();
          }
          if(lookahead==END){
              lookahead=nextsymbol();
              return ;
          }else{
              errortext("Kein END nach BEGIN gefunden");
          }




      case IF:
          lookahead = nextsymbol();
          condition();
          if(lookahead==THEN){
              lookahead = nextsymbol();
              statement();
          }else{
              cout << "Falsches Symbol: " << lookahead << " then erwartet";
              errortext("Kein THEN nach IF");
          }
          if(lookahead==ELSE){
             lookahead=nextsymbol();
              statement();
          }
          if(lookahead==FI){
              lookahead = nextsymbol();
              return ;
          }else{
              errortext("Kein FI nach IF");
          }
      case WHILE:
            lookahead = nextsymbol();
            condition();
            if(lookahead==DO){
                lookahead = nextsymbol();
                statement();
                return ;
            }else{
                errortext("Kein DO nach WHILE");
            }
           default:
               cout << "Unbekanner Anfnag eines Statements " << lookahead;
              errortext("STATEMENT fängt mit nicht bekannten Symbol an oder Fehler durch fall-through");
  }
  

  return;	// end statement 
}



/****************** procdecl ***************************************************/
/* analysiert wird der korrekte Aufbau einer Prozedurdeklaration 
nach folgender Syntax:
			
			PROCDECL 	::=		{procedure IDENT ';' BLOCK ';' }*



Schnittstelle: 
	bei Aufruf :			erkannt wurde das Schl�sselwort procedure 
							n�chstes Eingabesymbol befindet sich in lookahead 
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead

*/ 



void procdecl()


{  st_entry * neu, *found;          // Zeiger auf ST-Eintrag 
  
   symtable * neusym;		// Zeiger auf Symboltabelle

   if (tracesw) 
	    trace<<"\n Zeile:"<< lineno<<"Procdeklaration:";


    lookahead = nextsymbol();
	// falls als nächstes kein Identifikator kommt => Fehler
   if(lookahead != ID){
	   errortext("Keine Prozedurebezeichnung nach \"procedure\"");
   }

    // Prozedur merken
    found = insert(PROC);
    //TODO echt strange, neue SymTable wird ja in symbol erzeugt und so kommt man ran
    neusym =  found->subsym;

   
   lookahead = nextsymbol();
    if(lookahead!=SEMICOLON){
        errortext("Fehler: Semiclon nach procdure erwartet");
    }
    lookahead = nextsymbol();

    block(neusym);
    if(lookahead!=SEMICOLON){
        errortext("Nach Prozedurblock muss ein ; folgen");
    }
    lookahead = nextsymbol();

    if(lookahead == PROCEDURE){
        procdecl();
    }

   return;   // end procdecl 
}



/****************** vardecl ***************************************************/
/* analysiert wird der korrekte Aufbau einer Variablendeklaration 
nach folgender Syntax:
			
			VARDECL 	::=		  var IDENT ' : ' TYP  { ',' IDENT ' : ' TYP} *  ';' 			
			  
				
Schnittstelle: 
	bei Aufruf :			erkannt wurde das Schl�sselwort var 
							n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead

*/ 



void vardecl()

{ st_entry * neu, *found; 

  if (tracesw) 
	    trace<<"\n Zeile:"<< lineno<<"Variablendeklaration:";

    
	// nach var muss Identifikator folgen 

    lookahead = nextsymbol();
   if(lookahead != ID){
       errortext("Nach var muss ein Identifikator für die Variable folgen");
   }


    char id_saver[100];
    strcpy(id_saver, idname);
    cout << "identifikator: " <<id_saver<< endl;


    found = lookup_in_actsym(id_saver);
    if(found!=NULL){
        errortext("Identifikator doppelt deklariert" );
    }


    lookahead = nextsymbol();
    if(lookahead!=COLON){
        errortext("Doppelpunkt nach Variablenidentifikator erwartet");
    }

    lookahead = nextsymbol();
    if(lookahead==REAL){
        //real variable merken
        strcpy(idname, id_saver);
        insert(REALIDENT);


    }else if(lookahead==INT){
        //int variable merken
        strcpy(idname, id_saver);
        insert(INTIDENT);

    }else{
        errortext("INT or REAL nach Doppelpunkt in Variablendeklaration erwartet");
    }

    lookahead = nextsymbol();
    while(lookahead == KOMMA){

        lookahead = nextsymbol();
        if(lookahead != ID){
            errortext("Nach var muss ein Identifikator für die Variable folgen");
        }
        strcpy(id_saver, idname);
        cout << "identifikator: " <<id_saver<< endl;

        found = lookup_in_actsym(id_saver);
        if(found!=NULL){
            errortext("Identifikator doppelt deklariert" );
        }


        lookahead = nextsymbol();
        if(lookahead!=COLON){
            errortext("Doppelpunkt nach Variablenidentifikator erwartet");
        }

        lookahead = nextsymbol();
        if(lookahead==REAL){
            //real variable merken
            strcpy(idname, id_saver);
            insert(REALIDENT);
        }else if(lookahead==INT){
            //int variable merken
            strcpy(idname, id_saver);
            insert(INTIDENT);
        }else{
            errortext("INT or REAL nach Doppelpunkt in Variablendeklaration erwartet");
        }
        lookahead = nextsymbol(); // komma oder ; lesen
    }

    if(lookahead!=SEMICOLON){
        errortext("Semicolon am Ende einer Var Deklaration erwartet");
    }
    lookahead = nextsymbol();
return ;	// ende vardecl
} 








/****************** constdecl ***************************************************/
/* analysiert wird der korrekte Aufbau einer Variablendeklaration 
nach folgender Syntax:
			
			CONSTDECL 	::=	 const IDENT '=' INTNUMBER {',' IDENT '=' INTNUMBER } * ';' 
			
			  
				
Schnittstelle: 
	bei Aufruf :			erkannt wurde das Schl�sselwort const 
							n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead

*/ 




void constdecl()


{  st_entry *neu, *found;

	if (tracesw) 
	    trace<<"\n Zeile:"<< lineno<<"Konstantendeklaration:";
	
	// auf const muss IDENT folgen 
	lookahead = nextsymbol();
	//test wegen Identifikator
	if(lookahead!=ID){
        errortext("Auf eine Konstantendeklaration muss ein Identifikator folgen!");
    }

    found = lookup_in_actsym(idname);
    cout << "Neue Konstante: " <<idname <<endl;
    if(found !=NULL){
        errortext("Konstantenidentifikator wird bereits verwendet!");
    }
    //Konstante in symboltabelle eintragen



    //test wegen Zuweisungszeichen
    lookahead = nextsymbol();
    cout << lookahead<<endl;
    if(lookahead!=EQ){
        errortext("EQ nach Identifikator bei Konstantendefinition erwartet");
    }

    //test wegen Zahl
    lookahead = nextsymbol();
    if(lookahead!=INTNUM){
        errortext("Integer Nummer in Konstantendeklaration erwartet");
    }
    cout << "Wert in num: " << num << endl;
    insert(KONST);
    lookahead = nextsymbol();
    while(lookahead==KOMMA){
        //Test auf Identifikator
        lookahead=nextsymbol();
        if(lookahead!=ID){
            errortext("Auf eine Konstantendeklaration muss ein Identifikator folgen!");
        }
        found = lookup_in_actsym(idname);
        if(found !=NULL){
            errortext("Konstantenidentifikator wird bereits verwendet!");
        }



        //test auf Zuweisungszeichen
        lookahead = nextsymbol();
        if(lookahead!=EQ){
            errortext("Assignment nach Identifikator bei Konstantendefinition erwartet");
        }

        //test wegen Zahl
        lookahead = nextsymbol();
        if(lookahead!=INTNUM){
            errortext("Integer Nummer in Konstantendeklaration erwartet");
        }
        cout << "Wert in num: " << num << endl;
        insert(KONST);
        //nächstes Symbol einlesen
        lookahead = nextsymbol();
    }
    //nun muss ein semi colon kommen
    if(lookahead!=SEMICOLON){
        errortext("Semicolon am Ende einer Konstantendeklarationerwartet");
    }
    lookahead = nextsymbol();
return;		// end constdecl
} 




/****************** block ***************************************************/

/* analysiert wird der korrekte Aufbau eines Blockes nach folgender Syntax:

			
		BLOCK		::= 	[ CONSTDECL ]
							[ VARDECL ]
							  PROCDECL 
							   STATEMENT



Der Parameter neusym ist ein Zeiger auf die Symboltabelle des zu 
analysierenden Blockes 
===> 
		�ussersten (globalen)  Block:		firstsym 
		bei Prozeduren:		Zeiger auf neu angelegte ST f�r Prozedur 
		
Zu Beginn muss der globale Zeiger actsym auf die neue ST gesetzt werden
Vor Verlassen muss actsym wieder auf den vorigen Wert gesetzt werden

				
Schnittstelle: 
	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead


*/ 




void block(symtable * neusym)

/* symtable * neusym :	Zeiger auf neue ST */


{
    if (tracesw)
	trace<<"\n Zeile:"<< lineno<<"Block";

	// actsym auf neue Symboltabelle setzen 

    symtable* tmp =  actsym;
    actsym = neusym;


    if(lookahead==CONST){
        constdecl();
    }
    if(lookahead==VAR){
        vardecl();
    }
    if(lookahead==PROCEDURE){
        procdecl();
    }
    statement();

	// bei Blockende : Symboltabelle zur�cksetzen 
	// actsym = Zeiger auf vorherige Symboltabelle
	actsym = tmp;

return;		// end block 
}





/****************** program    ***************************************************/
/* analysiert wird der korrekte Aufbau eines Programmes nach folgender Syntax:
			

			PROGRAM		::=		BLOCK	'$' 
				
			

				
Schnittstelle: 
	bei Aufruf :			n�chstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		n�chstes Eingabesymbol befindet sich in lookahead

*/ 



void program()
{
    
  if (tracesw)
	  trace<<"\n Zeile:"<< lineno<<"Programm";


	// globale Symboltabelle  anlegen (firstsym 
	firstsym = create_newsym();

	// erstes Symbol lesen 
	lookahead=nextsymbol();

	// Block muss folgen 
	block (firstsym);  

	//  nach Block muss '$' folgen 
	if (lookahead == PROGEND)
		// n�chstes Symbol lesen 
		lookahead=nextsymbol();
	  
	else 
		 // korrektes Programmende fehlt 
		 error(31);
	
	// Dateiende erreicht ? 
	if (lookahead != DONE)
		error (33); // noch Symbole in Eingabedatei nach RPOGRAM 
	
}	// end program 

