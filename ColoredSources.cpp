// Dated July 2020
#define ITERATIONS 1000000 //One million iterations
#define HANDSIZE 7
// reassign as necessary
#define RANGE 1 //for -c and -l options
#define DECIMAL_PRECISION 4
//#define DEBUG_MODE 1 
#define VERBOSE 1
#define BO_1 0

#include <iostream>
using namespace std;

#include <stdlib.h>	
#include <stdbool.h>
#include <sys/time.h>	
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "deck.h"

void Visual_Line_Break(){
	cout << "=====================================================================================\n";

	return;
}

void Print_Out_Help(){

	Visual_Line_Break();	
	cout << "Help Manual Begin\n";
	Visual_Line_Break();

	cout << "WORKING:\n";
	cout << "* Support conventional user I/O during program execution.\n";
	cout << "* Support two options: \n";
	cout << "*  > -c OR -color -- This will simulate mana bases with the number of colored sources queries and those with one colored source greater and fewer.\n";
	cout << "*  > -l OR -land -- This will simulate mana bases with the number of lands queries and those with one land greater and fewer.\n";
	cout << "*  > -d OR -default -- This will require no user I/O and query with 60 land 24 land 16 hits deck. Try to cast a 2BB spell by turn 4.\n";
	cout << "*  > -h OR -help -- This prints out the help manual (where you are presently). -- \n";
	cout << "* Support Best of One opening hand system. See https://forums.mtgarena.com/forums/threads/26319 .\n";
	cout << "* Can return the sequence of draws.\n";
	cout << "* Opening hand size accounting. How many mulligans were needed to be taken?\n";

	cout << "TODO: \n";
	cout << "* Support Verbose Option: This should print out when various tasks are completed by the program.\n";
	cout << "* Support Debug Mode in code: This should print out various jump to subroutines acknowledgements and ensure correct data transfers and conversions.\n";
	cout << "* Support variable ranges for -c and -l options.\n";
	cout << "* Expand Make_Deck() functionality to accomodate more than three inputs.\n";
	cout << "* Figure out more sophisticated mulligan process. Also properly return X cards to the bottom of the library if mulligans have taken place. (AAB ratio reduction problem).\n";
	cout << "* Keep track of the known portion of the library versus the randomized portion of the library. See Return_to_Top() and Return_to_Bottom()\n";
	cout << "* Approx 0.1ns per iteration makes scaling impractical. My gut says it has to do with the deck histogram and cummulative array.\n";

	Visual_Line_Break();
	cout << "Help Manual End\n";
	Visual_Line_Break();
	return;
}

void simulate(int Right_Lands_Requirement, int Num_Lands_Requirement, int Turn_Deadline, int Num_Cards, int Num_Lands, int Good_Lands, int ResultArray[]){
	struct Deck myDeck;

	bool mulligan = false;
	int mulligan_count = 0;
	int master_iterator = 0; //DO NOT USE THIS FOR ANYTHING OTHER THAN SIMULATION MILLIONS OF ITERATIONS
	int simul_i,simul_j=0;
	int Card_Type; // now 2 is right color land, 1 is land, 0 is nonland.
	int Lands_In_Hand; //This is the number of lands in your hand.	
	int Right_Lands_In_Hand; //This is the number of lands in your hand that produce the color that you want.
	int Cards_In_Hand; //Start at 7 and go down.
	int Opening_Hand_Size[HANDSIZE+1] = {0}; //Opening hands range from 0 cards in hand to 7 cards in hand so size needs to be 8
#if BO_1 == 1 // TAG1
	struct Deck myDeck2;
	float Deck_Land_Ratio = (float) Num_Lands/(float) Num_Cards;
	float Hand_Land_Ratio, Hand_Land_Ratio2;
#if VERBOSE == 1
	cout << "Land Ratio: " << Deck_Land_Ratio << ".\n";
#endif 
#endif // from TAG1 &self-7

	float Count_Right_Lands = 0;
	float Count_Right_Colors = 0;
	float Good_Mana = 0;
	for (master_iterator = 1; master_iterator <= ITERATIONS; master_iterator++){
		//Mulligan BEGIN
		myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
		Lands_In_Hand = 0;
		Right_Lands_In_Hand = 0;
#if BO_1 == 1
		myDeck2.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
		int Lands_In_Hand2 = 0;
		int Right_Lands_In_Hand2 = 0;
#endif
		//Draw 7
		for (simul_j = 1; simul_j <= HANDSIZE; simul_j++){
			Card_Type = myDeck.Draw();
			if (Card_Type == 1) {
				Lands_In_Hand++;
			}
			if (Card_Type == 2){
				Right_Lands_In_Hand++;
				Lands_In_Hand++;
			}
#if BO_1 == 1 // TAG2
			Card_Type = myDeck2.Draw();
			if (Card_Type == 1 || Card_Type == 2) {
				Lands_In_Hand2++;
			}
#endif // from TAG2 &self-5
		}
		Cards_In_Hand = 7;
#if DEBUG_MODE != 0 && ITERATIONS < 11 
		//myDeck.Print_Draws(HANDSIZE);
#endif 
#if BO_1 == 1 // TAG3 goes down to &self+44
#if DEBUG_MODE != 0 && ITERATIONS < 11
		myDeck2.Print_Draws(HANDSIZE);
#endif
		Hand_Land_Ratio = (float) Lands_In_Hand/(float) HANDSIZE;
		Hand_Land_Ratio2 = (float) Lands_In_Hand2/(float) HANDSIZE;
#if VERBOSE == 1 
		cout << "Hand 1's ratio is : " << Hand_Land_Ratio << ". Off by " << Hand_Land_Ratio - Deck_Land_Ratio << "\n";
		cout << "Hand 2's ratio is : " << Hand_Land_Ratio2 << ". Off by " << Hand_Land_Ratio2 - Deck_Land_Ratio << "\n";
#endif
		if (fabs(Hand_Land_Ratio - Deck_Land_Ratio) <= fabs(Hand_Land_Ratio2 - Deck_Land_Ratio)){
#if DEBUG_MODE != 0  && ITERATIONS < 11
			cout << "Hand1 is the opening hand we are going to use.\n"; //Hooray thank god, less work, just continue down
#endif
		}
		else{
#if DEBUG_MODE != 0  && ITERATIONS < 11
			cout << "Hand2 is the opening hand we are going to use.\n";
#endif
			mulligan = false;
			if (Lands_In_Hand2 < 2 || Lands_In_Hand2 > 5){
				mulligan = true;
				mulligan_count++;
			}
			if(!mulligan){
				myDeck.Cleanup();
				myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
				Lands_In_Hand = 0;
				Right_Lands_In_Hand = 0;
				for (simul_i = 0; simul_i < Cards_In_Hand; simul_i++){
					Card_Type = myDeck.Draw(myDeck2.Draws[simul_i]);
					if (Card_Type == 1) {
						Lands_In_Hand++;
					}
					if (Card_Type == 2){
						Right_Lands_In_Hand++;
						Lands_In_Hand++;
					}
				}
			}
		}
#if DEBUG_MODE != 0  && ITERATIONS < 11
		myDeck.Print_Draws(HANDSIZE);
#endif
#endif // from TAG3 (&self-44)
#if DEBUG_MODE != 0  && ITERATIONS < 11 
		//cout << Num_Cards << "!\n";
		//myDeck.Draw_Deck();
		//myDeck.Print_Draws(60);
#endif
		mulligan = false;
		if (Lands_In_Hand < 2 || Lands_In_Hand > 5){
			mulligan = true;
			mulligan_count++;
		}
		//Mulligan once (then put one card on the bottom)
		if (mulligan){
			myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
			Lands_In_Hand = 0;
			Right_Lands_In_Hand = 0;
			for (simul_j = 1; simul_j <= HANDSIZE; simul_j++){
				Card_Type = myDeck.Draw();
				if (Card_Type == 1) {
					Lands_In_Hand++;
				}
				if (Card_Type == 2){
					Right_Lands_In_Hand++;
					Lands_In_Hand++;
				}
			}
			Cards_In_Hand = 6;
			mulligan = false;		
			if (Lands_In_Hand < 2 || Lands_In_Hand > 5){
				mulligan = true;
				mulligan_count++;
			}
		}
		//Mulligan twice (then put two cards on the bottom)
		if (mulligan){
			myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
			Lands_In_Hand = 0;
			Right_Lands_In_Hand = 0;
			for (simul_j = 1; simul_j <= HANDSIZE; simul_j++){
				Card_Type = myDeck.Draw();
				if (Card_Type == 1) {
					Lands_In_Hand++;
				}
				if (Card_Type == 2){
					Right_Lands_In_Hand++;
					Lands_In_Hand++;
				}
			}
			Cards_In_Hand = 5;
			mulligan = false;
			if (Lands_In_Hand < 2 || Lands_In_Hand > 5){
				mulligan = true;
				mulligan_count++;
			}
		}
		//Mulligan thrice (end up with 4 cards)
		if (mulligan){
			myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
			Lands_In_Hand = 0;
			Right_Lands_In_Hand = 0;
			for (simul_j = 1; simul_j <= HANDSIZE; simul_j++){
				Card_Type = myDeck.Draw();
				if (Card_Type == 1) {
					Lands_In_Hand++;
				}
				if (Card_Type == 2){
					Right_Lands_In_Hand++;
					Lands_In_Hand++;
				}
			}
			Cards_In_Hand = 4;
			mulligan = false;
			if (Lands_In_Hand < 2 || Lands_In_Hand > 6){
				mulligan = true;
				mulligan_count++;
			}
		}
		//Mulligan four times (end up with 3 cards)
		if (mulligan){
			myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
			Lands_In_Hand = 0;
			Right_Lands_In_Hand = 0;
			for (simul_j = 1; simul_j <= HANDSIZE; simul_j++){
				Card_Type = myDeck.Draw();
				if (Card_Type == 1) {
					Lands_In_Hand++;
				}
				if (Card_Type == 2){
					Right_Lands_In_Hand++;
					Lands_In_Hand++;
				}
			}
			Cards_In_Hand = 3;
			mulligan = false;
			if (Lands_In_Hand < 1 || Lands_In_Hand > 6){//6 land 1 nonland and 1 land 6 nonland hands are tolerable, can still reduce down to 2:1 ratio
				mulligan = true;
				mulligan_count++;
			}
		}
		//Mulligan five times (end up with 2 cards)
		if (mulligan){
			myDeck.Make_Deck(Num_Cards-Num_Lands, Num_Lands-Good_Lands, Good_Lands);
			Lands_In_Hand = 0;
			Right_Lands_In_Hand = 0;
			for (simul_j = 1; simul_j <= HANDSIZE; simul_j++){
				Card_Type = myDeck.Draw();
				if (Card_Type == 1) {
					Lands_In_Hand++;
				}
				if (Card_Type == 2){
					Right_Lands_In_Hand++;
					Lands_In_Hand++;
				}
			}
			Cards_In_Hand = 2;
			// We got to stop now... We're keeping no matter what.
			mulligan = false;
		}
#if DEBUG_MODE != 0  && ITERATIONS < 11
		//myDeck.Print_Draws(HANDSIZE);
		//cout << "Drew " << Cards_In_Hand << ".  We have " << Lands_In_Hand << " lands in hand. Mulligan = "<< mulligan;
#endif
		//this is to keep track of how many cards we started in our opening hand after mulligans over many iterations.  Purely for bookkeeping
		Opening_Hand_Size[Cards_In_Hand]++;
		//Mulligan END
		//Draw steps
		int Turn_Count = 0;
		while (Turn_Count < Turn_Deadline){
			
			Card_Type = myDeck.Draw();
			
			if (Card_Type == 1){
				Lands_In_Hand++;
			}
			if (Card_Type == 2){
				Right_Lands_In_Hand++;
				Lands_In_Hand++;
			}
			Turn_Count++;
		}
		myDeck.Cleanup();
#if BO_1 == 1
		myDeck2.Cleanup();
#endif
		if (Right_Lands_In_Hand >= Right_Lands_Requirement){ // we have enough colored sources to cast the card on time
			Count_Right_Colors++;
		}
		if (Lands_In_Hand >= Num_Lands_Requirement){ // we have enough lands to cast the card on time, if they were the right colors.
			Count_Right_Lands++;
		}
#if DEBUG_MODE != 0  && ITERATIONS < 11
		// There are four possible outcomes: Not enough lands (and as a result not enough colors), Enough lands but not enough colors, and Both enough lands and enough colors:
		if (Lands_In_Hand < Num_Lands_Requirement){// not enough lands
			cout << "Result 1:";
			cout << "We have not drawn enough lands in our hand: "<< Lands_In_Hand << "/" << Num_Lands_Requirement << " lands. ";
			cout << "With regards to colors: " << Right_Lands_In_Hand << "/" << Right_Lands_Requirement << "\n";
		}
		if (Lands_In_Hand >= Num_Lands_Requirement && Right_Lands_In_Hand < Right_Lands_Requirement){// enough lands but not enough colors
			cout << "Result 2:";
			cout << "We have drawn enough lands in our hand: " << Lands_In_Hand << " lands. ";
			cout << "But not enough colors " << Right_Lands_In_Hand << "/" << Right_Lands_Requirement << "\n";
		}
#endif
		if (Right_Lands_In_Hand >= Right_Lands_Requirement && Lands_In_Hand >= Num_Lands_Requirement){// both enough lands and enough colors
#if DEBUG_MODE != 0  && ITERATIONS < 11
			cout << "Result 3:";
			cout << "We have drawn enough colors in our hand: " << Lands_In_Hand << " lands, " << Right_Lands_In_Hand << " of which are of the color we are looking for.\n";
#endif
			Good_Mana++;
		}
#if DEBUG_MODE != 0
		if (master_iterator % 1000 == 0){
			cout << master_iterator;
		}
#endif
	} //end of iterations
#if VERBOSE == 1
	cout << "In these " << ITERATIONS << " iterations, we have mulliganed " << mulligan_count << " times.\n";
	cout << "We have kept " << Opening_Hand_Size[7] << " seven card hands, " << Opening_Hand_Size[6] << " six card hands, ";
	cout << Opening_Hand_Size[5] << " five card hands, " << Opening_Hand_Size[4] << " four card hands, ";
	cout << Opening_Hand_Size[3] << " three card hands, " << Opening_Hand_Size[2] << " two card hands...\n";
#endif
#if DEBUG_MODE != 0
	cout << master_iterator << "<----\n";
#endif
	//return {Count_Right_Colors, Count_Right_Lands};
	// This hack where you can make C++ functions return multiple outputs of the same type, changes function from int to void, add outputs as part of arguments.
	ResultArray[0] = Count_Right_Colors;
	ResultArray[1] = Count_Right_Lands;
	ResultArray[2] = Good_Mana;
} 

int main(int argc, char **argv){

	srand (time(0)); //use current time as the seed
	struct timeval start_time;
	struct timeval end_time; //used to figure out time of execution
	cout << "Decimal Precision at: " << cout.precision(DECIMAL_PRECISION) << fixed << ".\n"; //cout decimal points setting
	int main_i,main_j,main_l = 0;
	//============================== PROGRAM OPTIONS AND ARGUMENTS =======================
	// supported options: -d (for default), -h (for help) -c (for colored mana sources) -l (for any mana sources)
	// in development: -v (for verbose), default ranges for -c and -l are set at +/-RANGE, increase up to max range of 0 up to DECKSIZE 
	/**
	Flags directory:
	Index 0 = No Arguments Flag (turns on if default option requested)
	Index 1 = Help Flag
	Index 2 = Color Flag
	Index 3 = Land Flag
	**/
	int Flags[4] = {0};
	char *Arg_Value = NULL; //supports user input that is string instead of single char
	int index;
	int getArg_Switch;

	opterr = 0;

	while ((getArg_Switch = getopt(argc, argv, "cd::lhnv::")) != -1){
		switch (getArg_Switch){
			case 'c':
				if (optarg != NULL){
					Arg_Value = optarg;
					if (strcmp(Arg_Value, "olor") == 0){
						Flags[2] = 1; //supports -color
					}
					else {
						cout << "WARNING: invalid option!\n";
						return 0;
					}
				}
				else{
					Flags[2] = 1; //supports -c
				}
				break;
			case 'd':
				if (optarg != NULL){
					Arg_Value = optarg;
					if (strcmp(Arg_Value, "efault") == 0){
						Flags[0] = 1;
					}
					else {
						cout << "WARNING: invalid option!\n";
						return 0;
					}
				}
				else{
					Flags[0] = 1;
				}
				break;
			case 'h':
				if (optarg != NULL){
					Arg_Value = optarg;
					if (strcmp(Arg_Value, "elp") == 0){
						Flags[1] = 1; //supports -help
					}
					else {
						cout << "WARNING: invalid option!\n";
						return 0;
					}
				}
				else{
					Flags[1] = 1; // supports -h
				}
				break;
			case 'l':
				if (optarg != NULL){
					Arg_Value = optarg;
					if(strcmp(Arg_Value, "and") == 0){
						Flags[3] = 1; //supports -land
					}
					else{
						cout << "WARNING: invalid option! Continuing on...\n";
					}
				}
				else{
					Flags[3] = 1; // supports -l
				}
				break;
			case '?' :
				cout << "WARNING: Option provided is not valid or supported! Continuing on...\n";
				break;
			default:
				cout << "default case\n";
				break;
		}
	}
	//============================== PROGRAM OPTIONS AND ARGUMENTS END =======================
	//============================== PARSING ARGUMENTS AND OPTIONS =======================
#if DEBUG_MODE != 0 // TAG4
#if VERBOSE == 1 // TAG5
		cout << "=======FLAGS=======\n";
		if (Arg_Value != NULL){ //If there are arguments, print this one, otherwise print the other one.
								// Arg_Value == NULL tends to break this.
			cout << "Help =" << Flags[1] << " , No_Arg ="<< Flags[0] << " , Color_Flag ="<< Flags[2] << " , Land_Flag =" << Flags[3] << " , Arguments = "<< Arg_Value <<".\n";
		}
		else {
			cout << "Help =" << Flags[1] << " , No_Arg ="<< Flags[0] << " , Color_Flag ="<< Flags[2] << " , Land_Flag =" << Flags[3] << ".\n";
		}
		cout << "\n";
#endif // from TAG5
	cout << "Flags = [";
	for (main_i = 0; main_i < sizeof(Flags)/sizeof(int); main_i++){
	//for later... restructure all int [X] into std::array
	//for (main_i = 0; main_i < Flags.size(); main_i++){
		cout << Flags[main_i];
	}
	cout << "]\n";

/**	for (index = optind; index < argc; index++){
		cout << "Non-option Argument "<< argv[index] << "\n.";
		return 0;
	}
**/
#endif // from TAG4

	if (Flags[1]){
		Print_Out_Help();
		return 0;
	}
	if (Flags[2]){
		cout << "Will report back with various number of lands of the right color.\n";
	}
	if (Flags[3]){
		cout << "Will report back with various number of lands.\n";
	}
	if (Flags[0]){
		cout << "No Arguments Requested: Will default to trying to cast 2BB spell by turn 4.\nDeck of 60 cards will consists of 24 lands and 16 lands of the right color.\n";
	}
	else{
		cout << "Arguments Requested: \n";
	}
	//============================== PARSING ARGUMENTS AND OPTIONS END =======================	
	int Right_Lands_Requirement, Num_Lands_Requirement, Turn_Deadline, Num_Cards, Num_Lands, Good_Lands;
	if (!Flags[0]){
	//============================== I / O HERE =======================
		cout << "What is the color requirement of this card? For Example: {2}{CC} ";
		cin >> Right_Lands_Requirement;
		//int Right_Lands_Requirement = 1; //What is the color requirement of this card?
		cout << "How much mana does this card cost? ";
		cin >> Num_Lands_Requirement;
		//int Num_Lands_Requirement = 1; //How much mana does this card cost?
		cout << "By what turn do we want to cast this card? ";
		cin >> Turn_Deadline;
		//int Turn_Deadline = 2; //By what turn do we want to cast this card?
		cout << "We are looking to cast a " << Num_Lands_Requirement - Right_Lands_Requirement << string(Right_Lands_Requirement, 'C') << " spell by turn " << Turn_Deadline << ".\n";

		//Initialize the contents of the deck
		cout << "Now to initilize the contents of the deck:\n";
		cout << "How many cards are in the deck? ";
		cin >> Num_Cards;
		//int Num_Cards = 60;
		cout << "How many lands are in the deck? ";
		cin >> Num_Lands;
		//int Num_Lands = 22;
		cout << "How many lands of the right color are in the deck? ";
		cin >> Good_Lands;
		//int Good_Lands = 16;
	//============================== I / O END ========================
	}
	else{
		Right_Lands_Requirement = 2;
		Num_Lands_Requirement = 4;
		Turn_Deadline = 4;
		Num_Cards = 60;
		Num_Lands = 24;
		Good_Lands = 16;
	}

	/**
	For simulation purposes, we need the following:
	See default variables

	For computation purposes, we need the following:
	Count_Right_Colors
	Count_Right_Lands
	**/
	int Simul_Land_Start, Simul_Land_End = -1;
	int Simul_Color_Start, Simul_Color_End = -1;
	int Median = -1;
	cout << "Beginning simulation with a deck consisting of " << Num_Cards << " cards, " << Num_Lands;
	if(Flags[3]){
		Simul_Land_Start = 0;
		Simul_Land_End = RANGE*2;
		cout << " (+/-"<< RANGE <<")";
	}
	else{
		Simul_Land_Start = RANGE;
		Simul_Land_End = RANGE;
	}
	cout << " lands, and " << Good_Lands;
	if (Flags[2]){
		Simul_Color_Start = 0;
		Simul_Color_End = RANGE*2;
		cout << " (+/-"<< RANGE <<")";
	}
	else {
		Simul_Color_Start = RANGE;
		Simul_Color_End = RANGE;
	}
	cout << " colored sources in question.\n";
	Median = (Simul_Land_Start + Simul_Color_End) / 2;
	// Failsafe check if bounds are -1. Rewrite to variadic template
	if (Simul_Land_Start < 0 || Simul_Land_End < 0 || Simul_Color_Start < 0 || Simul_Color_End < 0 || Median < 0){
		cout << "ERROR: Simulation begin/end bounds are invalid!\n";
		return -1;
	}
	float Percent_Right_Colors[RANGE*2 +1][RANGE*2 +1];
	float Percent_Right_Lands[RANGE*2 +1][RANGE*2 +1];
	float Percent_Good_Mana[RANGE*2 +1][RANGE*2 +1];
	Visual_Line_Break();
	for (main_l = Simul_Land_Start; main_l <= Simul_Land_End; main_l++){
		//land scale support:
		// main_l = 0 corresponds to Num_Lands-1
		// main_l = 1 corresponds to Num_Lands
		// main_l = 2 corresponds to Num_Lands+1
		for (main_j = Simul_Color_Start; main_j <= Simul_Color_End; main_j++){
			//color scale support: 
			// main_j = 0 corresponds to Good_Lands-1
			// main_j = 1 corresponds to Good_Lands
			// main_j = 2 corresponds to Good_Lands+1
			float Count_Right_Lands = 0;
			float Count_Right_Colors = 0;
			float Good_Mana = 0;
			//This hack makes the simulate function return two integers
			int ResultArray[3];

#if VERBOSE == 1
			cout << "\nBeginning simulation with a deck consisting of " << Num_Cards << " cards, ";
			cout << Num_Lands+main_l-RANGE << " lands, and ";
			cout << min(Good_Lands+main_j-RANGE, Num_Lands+main_l-RANGE) << " colored sources in question...";
			gettimeofday(&start_time, NULL);
#endif
			simulate(Right_Lands_Requirement, Num_Lands_Requirement, Turn_Deadline, Num_Cards, Num_Lands+main_l-RANGE, min(Good_Lands+main_j-RANGE, Num_Lands+main_l-RANGE), ResultArray);

#if VERBOSE == 1
			gettimeofday(&end_time, NULL);
			cout << "The simulate function returned " << ResultArray[0] << "|" << ResultArray[1] << "|" << ResultArray[2] << "\n";
#else
			cout << "\n";
#endif
			Count_Right_Colors = ResultArray[0];
			Count_Right_Lands = ResultArray[1];
			Good_Mana = ResultArray[2];
			
			Percent_Right_Colors[main_l-RANGE][main_j-RANGE] = Count_Right_Colors / ITERATIONS;
			Percent_Right_Lands[main_l-RANGE][main_j-RANGE] = Count_Right_Lands / ITERATIONS;
			Percent_Good_Mana[main_l-RANGE][main_j-RANGE] = Good_Mana / ITERATIONS;
			
			//Visual_Line_Break();
			cout << "With " << Good_Lands+main_j-RANGE << "/" << Num_Lands+main_l-RANGE << " colored mana, we have a " << Percent_Good_Mana[main_l-RANGE][main_j-RANGE] * 100 << "% chance of having good mana.";
			if (main_j != 0 && Flags[2]){
				cout << "(+" << (Percent_Good_Mana[main_l-RANGE][main_j-RANGE] - Percent_Good_Mana[main_l-RANGE][main_j-RANGE-1]) * 100 << "%)\n";
			}
			else{
				cout << "\n";
			}
			
			Visual_Line_Break();
#if VERBOSE == 1			
			double exec_time = (double) end_time.tv_sec * 1000 + (double) end_time.tv_usec / 1000 - (double) start_time.tv_sec * 1000 + (double) start_time.tv_usec / 1000;
			cout << "\nExecution Time: " << exec_time << " ms (" << exec_time/ITERATIONS << " ns per iteration.)\n";
#endif
		}

	}
	cout << "\n";
	cout << "======================================TK=============================================\n";
	return 0;

}

