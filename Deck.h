// Dated July 2020
#define INCREMENT 60
#define DEBUG_MODE 0
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

		void Make_Deck(int Nonlands, int Wrong_Color_Lands, int Right_Color_Lands); // generates the initial historgram
		void Make_DeckID(); // generates the cummulative from the histogram
		void Draw_Deck(); // draws all cards in the deck
#if DEBUG_MODE != 0
		void Print_Histogram();
		void Print_Cummulative();
#endif
		int Draw(); // draw a random card
		int Draw(int CardType); // draw a specific (type of) card
		void Return_to_Top(int CardID);
		void Return_to_Bottom(int CardID);
		void Print_Draws(int Num_Cards);
		int Extend_Draws_Array(); //returns the size of the array
		void Cleanup();

class Deck{
	public: 
		int Num_Cards;
		int Num_Lands;
		int Num_Right_Color;
		int Num_Draws;
		int i;
		int Deck_Sources_Array[3];
		int* Draws = (int*) malloc(INCREMENT * sizeof(int));
		int Size_Of_Array = INCREMENT;

// from Cantrip_goldfish
		Deck() = default;
    	Deck(int Array_Size, int Deck_Sources_Array[]);
// end from Cantrip_goldfish

		//derive cumulative cardID from deckID.
		int Cummulative_Deck[3];

		void Make_Deck(int Nonlands, int Wrong_Color_Lands, int Right_Color_Lands){
			
			Deck_Sources_Array[0] = Nonlands;
			Deck_Sources_Array[1] = Wrong_Color_Lands;
			Deck_Sources_Array[2] = Right_Color_Lands;
#if DEBUG_MODE == 2
			Print_Histogram();
#endif

			Make_DeckID();
			Num_Cards = Cummulative_Deck[2];
			Num_Draws = 0;
		}
		void Make_DeckID(){ // generates the cummulative from the histogram
			int inner_sum = 0;
			for(i = 0; i < sizeof(Deck_Sources_Array)/sizeof(int); i++ ){
				Cummulative_Deck[i] = inner_sum + Deck_Sources_Array[i];
				inner_sum = Cummulative_Deck[i];
			}
#if DEBUG_MODE != 0 && DEBUG_MODE <= 2
			Print_Cummulative();
#endif
		}
		void Draw_Deck(){
			for (int i = 1; i < Deck_Sources_Array[2]; i++){
				Draw();
			}
		}
#if DEBUG_MODE != 0
		void Print_Histogram(/**int* Array**/){
			cout << "Histo{";
			cout << Deck_Sources_Array[0] << ",";
			cout << Deck_Sources_Array[1] << ",";
			cout << Deck_Sources_Array[2] << "}\n";
		}
		void Print_Cummulative(){
			cout << "Cummu{";
			cout << Cummulative_Deck[0] << ",";
			cout << Cummulative_Deck[1] << ",";
			cout << Cummulative_Deck[2] << "}\n";
		}
#endif

		int Draw(){
			Make_DeckID();
			int RandomInt = rand() % Cummulative_Deck[2] +1; //generate a random number between 1 and the deck size
			int CardType = -1;
			int Right_Color_Threshold = Deck_Sources_Array[2]-Deck_Sources_Array[1];
			int Lands_Threshold = Deck_Sources_Array[1]-Deck_Sources_Array[0];

			if (RandomInt >= Cummulative_Deck[1] + 1 && RandomInt <= Cummulative_Deck[2]){ //we have drawn a land that produces the right color
				//cout << "Drew a mana source of the right color.\n";
				Deck_Sources_Array[2]--;
				CardType = 2;
				
			}
			else if (RandomInt >= Cummulative_Deck[0] + 1 && RandomInt <= Cummulative_Deck[1]){ //we have drawn a land but it is not a hit
				//cout << "Drew a mana source but not of the right color.\n";
				Deck_Sources_Array[1]--;
				CardType = 1;
				
			}
			else if (RandomInt <= Cummulative_Deck[0]){ //we have drawn a blank
				//cout << "Drew a nonland.\n";
				Deck_Sources_Array[0]--;
				CardType = 0;
				
			}
			
			if (CardType < 0){
				cout << "Attempted to draw a card of the invalid type!\n";
				exit(-1);
			}
			Draws[Num_Draws] = CardType;
			Num_Draws++;
			//cout << "[" << Deck_Sources_Array[0] << "|" << Deck_Sources_Array[1] << "|" << Deck_Sources_Array[2] << "]\n";
			//Print_Draws(Num_Draws);
			return CardType;
		}
		int Draw(int CardType){
			Make_DeckID();
			switch (CardType){
				case 2:
					Deck_Sources_Array[2]--;
					break;
				case 1:
					Deck_Sources_Array[1]--;
					break;
				case 0:
					Deck_Sources_Array[0]--;
					break;
				default: //CardType < 0
					perror("Attempted to draw an invalid card!\n");
					exit(-1);
			}
			Draws[Num_Draws] = CardType;
			Num_Draws++;
			//Print_Draws(Num_Draws);
			if (Num_Draws % INCREMENT == INCREMENT-1){
				Extend_Draws_Array();
			}
			return CardType;
		}
		void Return_to_Top(int CardID){

		}
		void Return_to_Bottom(int CardID){

		}
		void Print_Draws(int Num_Cards){
			
			cout << "The draws have been: [";
			for (int i = 0; i < Num_Cards; i ++){
				cout << Draws[i] << "|";
			}
			cout << "]\n";

		}
		int Extend_Draws_Array(){ //returns the size of the array
   			return Size_Of_Array;
		}
		void Cleanup(){
			free(Draws);
		}
};

//from Cantrip_Goldfish
Deck::Deck(int Array_Size, int Array[])
{
	cout << "Array_Size = " << Array_Size << "\n";
    for(int i = 0; i < Array_Size; i++){
        Deck_Sources_Array[i] = Array[i];
        cout << "Array[" << i << "] = " << Array[i] << "\n";
		cout << "Card_Array[" << i << "] = " << Deck_Sources_Array[i] << "\n";        
    }
}
//end from Cantrip_Goldfish