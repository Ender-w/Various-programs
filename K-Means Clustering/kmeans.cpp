#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <math.h>

using namespace std;

int main ( int argc, char *argv[] ){
	int k = 3;//Number of clusters with default 3
	k = atoi(argv[1]);
	//cout << k << endl;
	float** data;
	//I used a different data structure to represent point assignment than in the lecture notes. Insted of a [k * n] array of booleans i use a [n] sized array of ints with each [n] representing the index of the cluster that node belongs to.
	int* closest;
	string line;
	bool firstLine = true;
	int pointCount;
	int index = 0;
	//Getting input
	while(getline(cin, line)){
		if(firstLine){
			//First line contains number of points
			pointCount = atoi(line.c_str());
			firstLine = false;
			data = new float*[pointCount];
			closest = new int[pointCount];
		}else{
			data[index] = new float[2];
			istringstream iss(line);
			string s;
			iss >> s;
			data[index][0] = strtof(s.c_str(), 0);
			iss >> s;
			data[index][1] = strtof(s.c_str(), 0);
			index++;
		}
	}
	
	//Randomly initialise k centroids
	srand(time(NULL));
	float** centroids = new float*[k];
	for(int i = 0; i < k; i++){
		centroids[i] = new float[3];//2nd pair is last float
		centroids[i][0] = static_cast <float> (rand() / static_cast <float> (RAND_MAX/15.0f));
		centroids[i][1] = static_cast <float> (rand() / static_cast <float> (RAND_MAX/15.0f));
	}
	
	bool changeSignifigant = true;
	int iterations = 0;
	
	while(changeSignifigant){//while less than max iter or centroids have not converged
		changeSignifigant = false;
		//Assignment Step
		iterations++;
		//cout << "|";
		for(int i = 0; i < pointCount; i++){
			float distance = 99999;
			//int closestIndex;
			for(int x = 0; x < k; x++){
				float tempDistance = fabs((data[i][0] - centroids[x][0]) + (data[i][1] - centroids[x][1]));
				if(distance > tempDistance){
					distance = tempDistance;
					closest[i] = x;
				}
			}
		}
		//Update Step
		for(int x = 0; x < k; x++){
			float sumX = 0;
			float sumY = 0;
			int divisionAmnt = 0;
			for(int i = 0; i < pointCount; i++){
				if(closest[i] == x){
					sumX += data[i][0];
					sumY += data[i][1];
					divisionAmnt++;
				}
			}
			//Keep track of previous centroid location
			centroids[x][2] = centroids[x][0];
			centroids[x][3] = centroids[x][1];
			//update centroid location
			centroids[x][0] = sumX/divisionAmnt;
			centroids[x][1] = sumY/divisionAmnt;
			//Compare new value to last to determine if we have reached steady state
			float tempDist = fabs((centroids[x][0] - centroids[x][2]) + (centroids[x][1] - centroids[x][3]));
			if(tempDist < 0.1){
				//changeSignifigant = false;
			}else{
				changeSignifigant = true;
			}
		}
		if(iterations > 500){
			cout << "Over 1000 iterations used. Please try running again as the random values may have generated poorly." << endl;
			cout << "Exiting program." << endl;
			return 1;
		}
	}
	//cout << endl;
	cout << iterations << " iterations taken" << endl;
	cout << "Resulting Centroid locations:" << endl;
	for(int x = 0; x < k; x++){
		cout << x+1 << ": " << centroids[x][0] << "  " << centroids[x][1] << endl;
	}
	
	return 0;
}
