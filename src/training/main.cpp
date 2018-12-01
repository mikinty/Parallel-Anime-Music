#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "training.h"
#include <iostream>
#include <fstream>

using namespace std;

void outputMatrices(float* highNotes, float* lowNotes, float* chords, std::string key){
  // TODO:
  // don't just open old matrix. make new one and write into that. 
  // After done, rename to old file name (and delete old file)

	std::ofstream outFile;
	outFile.open("highMatrix.txt");
  outFile << key << "\n";
	for (int i = 0; i < NUM_NOTES * NUM_NOTES; i ++){
		for (int j = 0; j < NUM_NOTES; j++){
			outFile << highNotes[i * NUM_NOTES + j] << " ";
		}
		outFile << "\n";
	}
	outFile.close();

	outFile.open("lowMatrix.txt");
  outFile << key << "\n";
	for (int i = 0; i < NUM_NOTES * NUM_NOTES; i ++){
		for (int j = 0; j < NUM_NOTES; j++){
			outFile << lowNotes[i * NUM_NOTES + j] << " ";
		}
		outFile << "\n";
	}
	outFile.close();

	outFile.open("chordMatrix.txt");
  outFile << key << "\n";
	for (int i = 0; i < NUM_CHORDS; i ++){
		for (int j = 0; j < NUM_CHORDS; j++){
			outFile << chords[i * NUM_CHORDS + j] << " ";
		}
		outFile << "\n";
	}
	outFile.close();

  free(highNotes);
  free(lowNotes);
  free(chords);
}

int main(int argc, char** argv) {
  //Set up all final matrices
  float* highNotes = malloc(sizeof(float) * (NUM_NOTES * NUM_NOTES * NUM_NOTES));
  float* lowNotes =  malloc(sizeof(float) * (NUM_NOTES * NUM_NOTES * NUM_NOTES));
  float* chords =  malloc(sizeof(float) * (NUM_CHORDS * NUM_CHORDS));

  float* deviceHigh;
  float* deviceLow;
  float* deviceChord;

  initCuda(deviceHigh, deviceLow, deviceChord);

  std::string key;

  // 1000-length ones you send to device
  sound_t* soprano = malloc(sizeof(sound_t) * INIT_ARRAY_LENGTH); //set up notes array
	sound_t* bass = malloc(sizeof(sound_t) * INIT_ARRAY_LENGTH);
  int maxLen = INIT_ARRAY_LENGTH;

  // Pass in files through args
	// Read given files
	for(int fileIndex = 0; fileIndex < argc; fileIndex++){
    int sLen = 0;
    int bLen = 0;
    int currentPart;

		std::string fileLine;
    std::ifstream file(argv[fileIndex]);
    std::size_t found;

    if (!file){
    	std::cerr << "Cannot open file : " <<argv[fileIndex]<<std::endl;
    	return false;
    }

    std::getline(file, key); //Grab major or minor mark

    while(std::getline(file, fileLine)){

      if (fileLine.find('H') != std::string::npos) { //Set correct part
        currentPart = 1;
      }
      else if (fileLine.find('L') != std::string::npos){
      	currentPart = 0;
      }
      else if (found = fileLine.find(' ') != std::string::npos){ //insert into correct notes line
      	if (currentPart == 0){
      		bass[bLen].tone = stoi(fileLine.substr(0, found));
      		bass[bLen].duration = stoi(fileLine.substr(found+1));
      		bLen ++;
      	}
      	else{
      		soprano[sLen].tone = stoi(fileLine.substr(0, found));
      		soprano[sLen].duration = stoi(fileLine.substr(found+1));
      		sLen ++;
    		}

        if (bLen >= maxLen || sLen >= maxLen){
      		maxLen = maxLen * 2;
    				soprano = realloc(soprano, sizeof(sound_t) * maxLen);
          	bass = realloc(bass, sizeof(sound_t) * maxLen);
        }
    	}

    }
    countTransitionsCuda(soprano, sLen, bass, bLen, deviceHigh, deviceLow, deviceChord);
  }

  free(soprano);
  free(bass);

  // normalize the transition matrix
  normalizeCuda(deviceHigh, deviceLow, deviceChord, highNotes, lowNotes, chords);

  outputMatrices(highNotes, lowNotes, chords, key);

  return 0;
}
