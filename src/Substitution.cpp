#include "Substitution.h"

Substitution::Substitution(std::string _path){


    //gets the cipher from the file in args
    getCipher( stringload.doString( _path));

    //initialise a dictionary of a thousand of the most common words
    initDictionary( stringload.doString( "char_frequencies/dictionary.txt"));

    //string in constructor may have lots of spaces in, this will help identify
    //possible words if the sentence still has its structure
    initCipherArray();

    //adds the symbols to a vector called symbols to store and perform operations on
    storeSymbols();

    //add frequency table to vector
    initialiseRFoL();

    //finds the relative frequency of each character in the whole string
    relativeFrequency();

    //initialise a list of common combination in cipher
    initialiseCombination(20);

    //initialise list possible common words in cipher
    initialiseWords(30);
}

 Substitution::~Substitution(){

   cipherArray.clear();
   RFoSL.clear();
   RFoL.clear();
   keyMap.clear();
   commonPairs.clear();
   commonWords.clear();
   commonCombinations.clear();
  }

void Substitution::initDictionary( std::string _dictionary){

  //create input string stream to manage line by line feed from frequency table
  std::istringstream dstream( _dictionary);

  std::string temp;

  // Process for each line, seperates string and float value and pushes that onto pair stream
  while ( std::getline( dstream, temp)) {

    //init dictionary vector
    dictionary.push_back( temp);
  }
}

//returns a percentage of wordsin cipherArray that were found on the dictionary of 1000 most common words
float Substitution::dictionaryQuotient(){

  float found = 0;

  for( std::string c : cipherArray){
    for( std::string d : dictionary){

      if( c == d){

        found++;
        break;
      }
    }
  }

  return  found / cipherArray.size() * 100;
}

//takes path to a .txt with the cipher text on and initialises the variable cipher on it
void Substitution::getCipher(std::string _fileText){

    cipher = _fileText;

    //transforms all text to lower case
 	  std::transform(cipher.begin(), cipher.end(), cipher.begin(), tolower);

    //removes a couple of awkward characters
    char chars[]= "!\n\r'-1234567890";
    for (unsigned int i = 0; i < 3; ++i){

            cipher.erase(std::remove(cipher.begin(),cipher.end(),chars[i]),cipher.end());
    }
}

void Substitution::showKeyMap(){

    for(auto k: keyMap){

        std::cout<< "\n" <<k.second << "-" <<k.first;
    }
}

void Substitution::initCipherArray(){

  cipherArray.clear();

  //temp string for storing words
  std::string temp;

  //for each character or symbol in the cipher string
  for( int c = 0; c < cipher.size(); c++){

  //clear temp
  temp = "";

    //loop untill encounter space or end of file
    for( c ; c < cipher.size(); c++){

      temp += cipher[c];
      if( cipher[c] == ' '){

        break;
      }
    }

    //removes spaces
    temp.erase( remove_if( temp.begin(), temp.end(), isspace), temp.end());
    cipherArray.push_back( temp);
  }
}

void Substitution::storeSymbols(){

    //for each character or symbol in the cipher string
    for(char c: cipher){

        //default true flag is set to false when char is found before, else put on vector
        bool newSymbol = true;

        //check if the character or symbol has been encountered before
        for(auto  sym: symbols){

            if(sym == c && c != ' '){

                newSymbol = false;
            }
        }

        //pushes new symbol to vector list
        if(newSymbol){

            symbols.push_back(c);
        }
    }
}

//initilaises the Rofl array, this is initialise the array needed to analyse frequencies of use of characters
void Substitution::initialiseRFoL(){

    //load up frequency table at path
    std::string frequencyTable = stringload.doString("char_frequencies/br.txt");

    //create input string stream to manage line by line feed from frequency table
    std::istringstream stable( frequencyTable);

    std::string temp;

    // Process for each line, seperates string and float value and pushes that onto pair stream
    while ( std::getline( stable, temp)) {

        //position of comma to size_t
        size_t pos = temp.find(",");

        //get char at pos 0 of this string
        char c = temp.at( 0);

        //get substring from pos to end of line, this contains string of a float
        temp = temp.substr( pos + 1);

        //converts the string to a float
        float f = atof( temp.c_str());

	    //initialising RFoL.push_back(std::pair<char,float>(s elements to be the character mapped to how often it occures relativley
	    RFoL.push_back(std::pair<char,float>( c, f));
    }
}

//this function calculates the relative frequency of a character in cipher text
//and then loads it onto RFoSL map list as a key value pair, it also returns
//a percentage of just how similar the cipher was to the relative frequency of letters
//in plain text
void Substitution::relativeFrequency(){

    //loops for each character in the cipher
    for(char c: symbols){

        //stores the number of occurrences of the character
        float tempFirst = 0;

        //for each character in the cipher text store the  frequency of occurrence in RFoSL
        for(char s: cipher){

            if(c == s && s != ' '){

                tempFirst++;

            }
        }

        float temp = (tempFirst/cipher.size())*100;

        //add the character and frequency of its occurrence
        RFoSL.push_back(std::pair<char,float>( c, temp));
    }

    //organizes both vector of pairs RFoL and RFoSL in order of frequency
    sortVectors();
}

//shows frequency of each symbol in console
void Substitution::showFrequencyTable(){

    std::cout<< "FREQUENCY TABLE OF CHARACTERS \n\n"<<std::endl;

    float errorRatio;

    //loops for minimum size times of RFoL and RFoSL
    for(size_t K = 0; K < std::min(RFoL.size(), RFoSL.size()); ++K){

        //outputs order of occurrence of each string, shows which keys are likely to be substituted for which others
        std::cout<< keyMap[K].first<< " -  SUBSTITUTED FOR: "<< keyMap[K].second <<std::endl;

        //subtracts the frequency of occurrence of the substituted letters from the frequency of letters used in
        //English plain text and later returns the absolute value so the user can see how close the cipher is, a
        //higher score should show that the cipher is not simple substitution
        errorRatio += abs(RFoSL[K].second - RFoL[K].second);
    }

    std::cout<< "\n\nError of cipher to frequency of English plain text is: " << errorRatio << ".\n\n Most common possible words:\n" <<std::endl;

    //shows frequency of most common combinations
    for(auto a: commonWords){

        std::cout<< a <<std::endl;
    }

    std::cout<< "\n Most common occurances of combinations that may not be words:\n" <<std::endl;

    //shows frequency of most common combinations
    for(auto a: commonCombinations){

        std::cout<< a <<std::endl;
    }

    std::cout<< "\n Most common pairs of characters:\n" <<std::endl;

    //shows frequency of most common combinations
    for(auto a: commonPairs){

        std::cout<< a <<std::endl;
    }

    std::cout<<"\n frequency of common words found: " << dictionaryQuotient() << " percent."<<std::endl;
}

//this function sorts both vectors of pairs into descending order based on the second value of each pair
void Substitution::sortVectors(){

    //sorts the vector using lambdas and sort algorithm
    std::sort(RFoL.begin(), RFoL.end(), [](const std::pair<char,float> &left, const std::pair<char,float> &right) {

        return left.second > right.second;
    });
    std::sort(RFoSL.begin(), RFoSL.end(), [](const std::pair<char,float> &left, const std::pair<char,float> &right) {

        return left.second > right.second;
    });

    //loops for minimum size times of RFoL and RFoSL
    for(size_t K = 0; K < std::min(RFoL.size(), RFoSL.size()); ++K){

        //adds pair of characters left substituted right character with similar frequency
        keyMap.push_back(std::pair<char, char>( RFoL[K].first, RFoSL[K].first));
    }

}

//once the vectors are sorted the characters from one list
//are similarly
char Substitution::switchChar( char _substitute){

    //returns second value if first matches
    for(auto km: keyMap){

        if( km.second == _substitute){

            return km.first;
        }
    }

    //if nothing is found to perform switch return ' ', most likley it can
    //point out there are symbols that should not be present
    return ' ';
}

//the input at args is the key, returns the value for the key or the letter the key has been substituted for
char Substitution::getValue( char _key){

    //loops for each pair
    for(auto km: keyMap){

        //if the first value matches args then return second
        if( km.first == _key){

            return km.second;
        }
    }

    //if nothing is found to perform switch return ' ', most likley it can
    //point out there are symbols that should not be present
    return ' ';
}

//finds the number of occurrences of the combination in args
int Substitution::addCombination( std::string _combination){

    int count = 0;
    size_t nPos = cipher.find(_combination, 0);
    while(nPos != std::string::npos){
        count++;
        nPos = cipher.find( _combination, nPos+1);
    }

    return count;
}

//this function returns the most common combination of characters that
//appear together in cipher that are not on the commonCombinations vector
std::string Substitution::maxNCombination( int _size){

    //empty temporary string for storing most common combination
    std::string mostCommon = "";

    //stores the highest yet number of occurrences
    int highestOccurrences = 0;

    //stores each combination of _size for the whole cipher
    std::vector<std::string> tempCombinations;

    //for each char in the cipher
    for(int c = 0; (c + _size) < cipher.size(); c++){


        std::string temp = cipher.substr( c, _size);

        //if there are no spaces
        if( !(temp.find(" ") != std::string::npos)){

          tempCombinations.push_back(temp);
        }
    }

    /*the following logic uses two loops and flags to detect the most common occurrences of string combinations
     stored in tempCombinations, if they are not already on the list of common combinations already that is.*/

    //loops for each entry of tempCombinations
    for(auto &o: tempCombinations){

        //stores the number of occurrences
        int occurrences = 0;

        //loops for temp combinations again to find matches
        for(auto t: tempCombinations ){

            //if the strings are equal rack up occurrences
            if(o == t)
                occurrences++;
        }

        //if the occurrences are highest yet and the combination is not already known
        if((occurrences > highestOccurrences) && !findCombination(o)){

            highestOccurrences = occurrences;
            mostCommon = o;
        }else{

            o.erase();
        }
    }

    return mostCommon;
}

//this function initialises the string vector common combinations with the amount of combinations of up to
//five characters for the number of times passed in args
void Substitution::initialiseCombination(int _size){

  //searches for the 4 most common combinations for up to _size characters
  for(int a = 2; a <= _size; a++){
    for(int f = 0; f <= 8; f++){

      commonCombinations.push_back( maxNCombination( a));
    }
  }

  int highestPair = 0;

  for(auto k: keyMap){

    std::string temp = "";
    temp += k.first;
    temp += k.first;

    //adds all combinations of two symbols together so long as they appear on
    //on an a relative size amount of times to the ciphers size, for now 3 is a debugging amount
    //as I do not want to over optimise for spaces being available, this is because  Iwill use
    //an algorithm to find common frases and therefore patterns to create spaces to work with
    if(( addCombination(temp) > 3) && !findPairs(temp)){

      commonPairs.push_back(temp);
    }
  }
}

//initialise commonCombinations array with the '_size' amount highest occuring words
void Substitution::initialiseWords( int _size){

  for(int f = 0; f <= _size; f++){

      commonWords.push_back( maxNWords());
  }

  for( std::string c : commonWords){
    for(int f = 0; f < commonCombinations.size(); f++){

      //removes a word from a combination, the differences is one is maybe a word the other is maybe
      //a common part of a word
      if( c == commonCombinations.at(f))
        commonCombinations.erase( commonCombinations.begin() + f);
    }
  }
}

//this function finds common words, that is combinations between spaces, it then removes the words that are found from the combinations
//this is so that a frequency solve can tell lettrs that are common like th and sh from words that are common like at and in
//help again with http://stackoverflow.com/questions/11130243/remove-if-string-matches-a-given-string-in-a-set
std::string Substitution::maxNWords(){

  //temps tring for returning as the most common
  std::string temp;

  //stores the highest yet number of occurrences
  int highestOccurrences = 0;

  //loops cipherArrays words
  for(auto c: cipherArray){

      //stores the number of occurrences
      int occurances = 0;

      //loops for cipherArray again to find matches
      for(auto t: cipherArray){

          //if the strings are equal and not on the commonWords list rack up occurrences
          if(c == t && !findWords(c)){

              occurances++;
          }
      }

      if( occurances > highestOccurrences){

        highestOccurrences = occurances;
        temp = c;
      }
  }

  return temp;
}

//this function switches the places of two substituted characters mapped to pairs, that is the places
//of two second characters in keyMap, this is so changes can be made when solving
void Substitution::switchKeys(char _swapA, char _swapB){

    size_t L;

    //loops for keyMap
    for(size_t K = 0; K < keyMap.size(); K++){

        if(keyMap[K].first == _swapA){

          keyMap[K].first = _swapB;
          L = K;
        }
    }
    //loops for keyMap
    for(size_t K = 0; K < keyMap.size(); K++){

        if(keyMap[K].first == _swapB && L != K){

          keyMap[K].first = _swapA;
        }
    }
}

//this function will switch the keys to fit a desired value so that if 'z' value was 'e'
//but it is returning 't' then the first arg would be 'z' and the second arg would 't'
//the key for the value 'e' will also be switched to what 't' had.
//'KVP' in this case stands for 'KeyValuePair'
void Substitution::swapKVP(char _key, char _value){

  char otherKey;
  char switchedVal = switchChar( _key);

  for( auto & km: keyMap){

    if( km.first == _value){

        otherKey = km.second;
        km.first = switchedVal;
    }
  }

  for( auto & km: keyMap){

    if( km.second == _key){

        km.first = _value;
    }
  }
}

//this function returns true or false if the string in args is actually on the vector container
bool Substitution::findCombination(std::string _find){

    for(auto o: commonCombinations){

        if(o == _find)
            return true;
    }

    return false;
}

//this function returns true or false if the string in args is actually on the vector container
bool Substitution::findWords( std::string _find){

    for(std::string o: commonWords){

        if(o == _find){

            return true;
        }
    }

    return false;
}

//this function returns true or false if the string in args is actually on the vector container
bool Substitution::findPairs(std::string _find){

    for(auto o: commonPairs){

        if(o == _find)
            return true;
    }

    return false;
}

void Substitution::updateCipher(){

  solvedString = "";

  for(auto c: cipher){

       solvedString += switchChar(c);
   }

   initCipherArray();

   //output new level of accuracy to console
   std::cout<< "accuracy is now : "<< dictionaryQuotient() <<std::endl;
}

//this function checks the frequency of each chars frequency occurrence to others until
//the frequency that is most similar to the table is found
std::string Substitution::solve(){

  //the most common word should be the word 'the' and it would be near the top of the most common words
  //found, this loop will loop the 'commonWord' array untill a the first three letter word comes up
  //e haasn't been switched yet however, this is because it should have been found other wise the whole
  //frequency search would be too badly flawed
  for( int s = 0; s < commonWords.size(); s++){
    if( commonWords.at(s).size() == 3){

      std::string temp = commonWords.at(s);

      //could make this block of logic more efficient but I believe it is better for it to remain readable
      char the1 =  temp.at(0);
      char the2 =  temp.at(1);

      swapKVP( the1, 't');
      swapKVP( the2, 'h');

      break;
    }
  }

  //only 'i' and 'a' will be on their own, the outer loop, loops twice to check if one of the two
  // often occures between 'th' and 't'
  for( int v = 0; v < commonWords.size(); v++){
    if( commonWords.at(v).size() == 1){


      for( int s = 0; s < commonWords.size(); s++){
        if( commonWords.at(s).size() == 4){

          //make the word 'that' up out of 'th' - the lone vowel 'i' or 'a' and then 't'
          //therefore if 'that' is found make sure that the third char is forced to be the
          //substitute for 'a'.
          //...ugly code however this is readable and only called once
          std::string temp = commonWords.at(s);
          std::string vowel = commonWords.at(v);
          char that1 = temp.at(0);
          char that2 = temp.at(1);
          char that3 = temp.at(2);
          char that4 = temp.at(3);

          if( 't' == switchChar(that1) && ( that1 == that4) && 'h' == switchChar(that2)){

              swapKVP( that3, 'a');
          }
        }
      }
    }
  }

  //now 'a' is found 'i' has been isolated, its the only other singular word
  for( int v = 0; v < commonWords.size(); v++){
    if( commonWords.at(v).size() == 1){

      if( findWords( commonWords.at(v)) && ('a' != switchChar(commonWords.at(v).at(0)) )){

        swapKVP( commonWords.at(v).at(0), 'i');
        break;
      }
    }
  }

  //remaining vowels are 'o' and 'u', this block will find 'o'
  for( int v = 0; v < commonWords.size(); v++){
    if( commonWords.at(v).size() == 2){

      std::string TO = commonWords.at(v);

      if( findWords(TO) && 't' == switchChar( TO.at(0))){

          swapKVP( TO.at(1), 'o');
      }
    }
  }

  //'ll' is a good word to search for now because 'a' has been found, there ar no other common combinations that start with a and have
  //two other letters infront that appear often enough
  for( int v = 0; v < commonWords.size(); v++){
      if( commonWords.at(v).size() == 3){

        std::string temp = commonWords.at(v);
        if( 'a' == switchChar(temp.at(0)) && findPairs(std::string{ temp.at(1), temp.at(2)})){

          swapKVP( temp.at(1), 'l');
        }
      }
  }


  //'s' is a good word to go for now, 'is' is very common as is 'as', 'sh' is a common
  //word combination too, as is a word but it is a more common combination most
  //general words can be used now too
  for(auto WAS: commonWords){
   if( WAS.size() == 3){
     for( auto SH: commonCombinations ){
       if( SH.size() == 2 &&  'h' == switchChar(SH.at(1))){
         for(auto IS: cipherArray){
           if( IS.size() == 2 && findWords( IS)){

             if( 'a' == switchChar(WAS.at(1)) && (WAS.at(2) == SH.at(0)) && 'i' == switchChar(IS.at(0))){

              //if all of those have those chars in then there 'has' to be an 's' there
              swapKVP(WAS.at(2), 's');
              swapKVP(WAS.at(0), 'w');
         }
        }
       }
      }
     }
    }
   }






  //'w' is a good letter to go for now, 'll' is a common pair and 'i' has been found so it may expose 'will'
  //'e' has been found to expose 'we' and also 'with' is possible


  updateCipher();

  return solvedString;
}
