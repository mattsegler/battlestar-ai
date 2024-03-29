/***************************************************************
 
 Battlestar-AI Battleship AI
 Pennsylvania State University - CMPSC 442
 
 This source file is developed and maintained by:
 + Zach Gotsch <zkg5001@psu.edu>
 
***************************************************************/

#include "ShootGene.h"

double* Gene::getDist()
{
	double* dist = new double[100];
	for(int i = 0; i < 100; dist[i++] = magnitude);
	for(int i = 0; i < WAVECOUNT; i++)
	{
		double* waveDist = waves[i].getDist();
		for(int j = 0; j < 100; j++)
			dist[j] += waveDist[j];
		delete[] waveDist;
	}
	return dist;
}

double Gene::fitness(double* target)
{/*
	double* sin = getDist(); 

	double meansin = 0, meandist = 0;
	for(int i = 0; i < 100; i++)
	{
		meansin += sin[i];
		meandist += dist[i];
	}
	meansin /= 100;
	meandist /= 100;

	double squaredsin = 0, squareddist = 0;
	for(int i = 0; i < 100; i++)
	{
		squaredsin += pow(sin[i] - meansin, 2);
		squareddist += pow(dist[i] - meandist, 2);
	}
	double denom = sqrt(squaredsin * squareddist);

	double numer = 0;
	for(int i = 0; i < 100; i++)
	{
		numer += (sin[i] - meansin) * (dist[i] - meandist);
	}

	delete[] sin;
	return numer / denom;*/

	// Copy data
	double dist[100];
	memcpy(dist, target, 100 * sizeof(double));

	//SSD
	double* repr = getDist();

	double distMax = 0, reprMax = 0;
	for(int i = 0; i < 100; i++)
	{
		if(dist[i] > distMax)
			distMax = dist[i];
		if(repr[i] > reprMax)
			reprMax = repr[i];
	}
	for(int i = 0; i < 100; dist[i] /= distMax, repr[i++] /= reprMax);

	double ssd = 0;
	for(int i = 0; i < 100; i++)
		ssd -= pow((repr[i] - dist[i]), 2.0);

	return ssd;
}

char* Gene::toString()
{
	/*char buf[1024] = "";
	for(int i = 0; i < WAVECOUNT; i++)
		sprintf(buf, "%sHarmonic %d: %.2fsin(%dx) + %.2fcos(%dx)\n", buf, i+1, waves[i].alpha, waves[i].mu, waves[i].beta, waves[i].omega);
	return buf;*/
	char buf[1024] = "";
	printf("Magnitude: %f\n", this->magnitude); 
	for(int i = 0; i < WAVECOUNT; i++)
	{
		sprintf(buf, "Harmonic %d: %.2fsin(%dx) + %.2fcos(%dx)\n", i+1, waves[i].alpha, waves[i].mu, waves[i].beta, waves[i].omega);
		printf("%s", buf);
	}
	return "";
}

Gene Gene::cross(Gene* father, Gene* mother, bool mutate)
{
	double perfectChild[100];
	double* fatherDist = father->getDist();
	double* motherDist = mother->getDist();

	for(int i = 0; i < 100; i++)
		perfectChild[i] = (fatherDist[i] + motherDist[i]) / 2;

	delete[] fatherDist;
	delete[] motherDist;

	double imaginary[100];
	for(int i = 0; i < 100; imaginary[i++] = 0); 

	bool succeed = DFT(1, 100, perfectChild, imaginary);
	if(!succeed)
		exit(0);
	
	int* sines = selectTop(imaginary);
	int* cosines = selectTop(perfectChild);

	Harmonic waves[WAVECOUNT];
	for(int i = 0; i < WAVECOUNT; i++)
	{
		double alpha = mutate && randFloat() <= MUTATION_RATE ? MAX_OF_SCALED_SIN_PLUS_COS - 2 * randFloat() * MAX_OF_SCALED_SIN_PLUS_COS : imaginary[sines[i]];
		double beta = mutate && randFloat() <= MUTATION_RATE ? MAX_OF_SCALED_SIN_PLUS_COS - 2 * randFloat() * MAX_OF_SCALED_SIN_PLUS_COS : perfectChild[cosines[i]];
		int mu = mutate && randFloat() <= MUTATION_RATE ? (rand() % 48) + 1 : sines[i];
		int omega = mutate && randFloat() <= MUTATION_RATE ? (rand() % 48) + 1 : cosines[i];

		waves[i] = Harmonic(alpha, mu, beta, omega);
	}

	double magnitude = mutate && randFloat() <= MUTATION_RATE ? randFloat() * (father->magnitude + mother->magnitude) : ((father->magnitude + mother->magnitude) / 2);

	return Gene(magnitude, waves);
}

char* Gene::saveString()
{
	char* string = new char[1024];
	string[0] = '\0';
	sprintf(string, "%.3f\n", magnitude);
	for(int i = 0; i < WAVECOUNT; i++)
	{
		char vals[128];
		sprintf(vals, "%.3f, %d, %.3f, %d\n", waves[i].alpha, waves[i].mu, waves[i].beta, waves[i].omega);
		strcat(string, vals);
	}
	strcat(string, "\n");
	return string;
}