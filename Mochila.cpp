/*
 * =====================================================================================
 *
 *       Filename:  Mochila.cpp
 *
 *    Description:  Proyecto final de la materia Analisis y Diseno de Algoritmos
 *
 *        Version:  1.0
 *        Created:  12/01/13 18:12:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eduardo Alberto Sanchez Alvarado, Hector Ricardo Mendez Sordia
 *   Organization:  ITESM Campus Monterrey
 *
 * =====================================================================================
 */


#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

/**
Object class, represents an object to be put in the knapsack, has a value, weight, and a value/weight proportion
*/
class Object
{
private:
    int value;
    int weight;
    double valuePerWeight;

public:
    /**
    Default contructor
    */
    Object()
    {
        value = 0;
        weight = 0;
        valuePerWeight = 0;
    }
    /**
    Constructor:
    v is the value of the object
    w is its weight
    */
    Object(int v, int w)
    {
        value = v;
        weight = w;
        valuePerWeight = value / weight;
    }

    /**
    sets the value and weight of the object
    v is the value of the object
    w is its weight
    */
    void initialize(int v, int w)
    {
        value = v;
        weight = w;
        valuePerWeight = value / weight;
    }

    /**
    Returns the value of the object
    */
    int getValue()
    {
        return value;
    }

    /**
    Returns the weight of the object
    */
    int getWeight()
    {
        return weight;
    }

    /**
    Returns the value/weight proportion
    */
    double getValuePerWeight()
    {
        return valuePerWeight;
    }
};

Object object[50]; //Initial set of objects
int knapsackWeight; //Maximum weight the knapsack can hold
int amountOfObjects; //amount of objects to be put

int backtrackingNodes; //Number of nodes generated by backtracking
int branchAndBoundNodes; //Number of nodes generated by branch and bound

double solutionBacktracking; //Solution by backtracking
double solutionBranchAndBound; // Solution by branching bound
bool include[50]; //Auxiliar array

/**
Calculates the possible value of a node of a certain level "nivel", based on its level
*/
double calculaValorPosible(int valorAcumulado, int peso, int nivel)
{
    double valorPosible = valorAcumulado;
    int pesoAcumulado = peso;

    while (pesoAcumulado + object[nivel].getWeight() <= knapsackWeight && nivel < amountOfObjects)
    {
        valorPosible += object[nivel].getValue();
        pesoAcumulado += object[nivel].getWeight();
        nivel++;
    }
    return valorPosible + (knapsackWeight - pesoAcumulado) * object[nivel].getValuePerWeight();
}

void backtracking(int valorAcumulado, int pesoAcumulado, int nivel)
{
	backtrackingNodes++;
    if (nivel <= amountOfObjects)
    {
        if (pesoAcumulado <= knapsackWeight)
        {
        	if (valorAcumulado > solutionBacktracking)
			{
				solutionBacktracking = valorAcumulado;
			}
			int valorPosible = calculaValorPosible(valorAcumulado, pesoAcumulado, nivel);
			if (valorPosible > solutionBacktracking)
			{
				include[nivel] = true;
				backtracking(valorAcumulado + object[nivel].getValue(), pesoAcumulado + object[nivel].getWeight(), nivel + 1);
				include[nivel] = false;
				backtracking(valorAcumulado, pesoAcumulado, nivel + 1);
			}
        }
    }
}

/**
 * Class used in the Branch and Bound algorithm implementation using a heap.
 */
class Nodo {
public:
	double Va;
	double Vp;
	int Pa;
	int nivel;
	Nodo(double Va, double Vp, int Pa, int nivel) {
		this->Va = Va;
		this->Vp = Vp;
		this->Pa = Pa;
		this->nivel = nivel;
	}
	
};

bool nodeCmp(Nodo n1, Nodo n2) {
	if (n1.Vp != n2.Vp) {
		return n1.Vp < n2.Vp;
	}
	else {
		return n1.nivel < n2.nivel;
	}
}

void branchAndBound()
{
	vector <Nodo> heap;
	heap.push_back(Nodo(0, calculaValorPosible(0,0,0), 0, 0));
	push_heap(heap.begin(), heap.end(), nodeCmp);
	while (!heap.empty()) {
		branchAndBoundNodes++;
		Nodo current = heap.front();
		pop_heap(heap.begin(), heap.end(), nodeCmp);
		heap.pop_back();
		if (current.Pa > knapsackWeight) {
			continue;
		}
		if (current.Va > solutionBranchAndBound) {
			solutionBranchAndBound = current.Va;
		}
		if (current.Vp > solutionBranchAndBound && current.nivel < amountOfObjects) {
			double newVp = calculaValorPosible(current.Va, current.Pa, current.nivel + 1);
			Nodo hijoIzq(current.Va + object[current.nivel].getValue(), current.Vp, current.Pa + object[current.nivel].getWeight(), current.nivel + 1);
			Nodo hijoDer(current.Va, newVp, current.Pa, current.nivel + 1);
			heap.push_back(hijoIzq);
			push_heap(heap.begin(), heap.end(), nodeCmp);
			heap.push_back(hijoDer);
			push_heap(heap.begin(), heap.end(), nodeCmp);
		}
	}
}

/**
Sorts the object[] array by value/weight in descending order. Sorting algorithm: Selection sort
*/
void sort()
{
    int amountOfObjectsMinus1 = amountOfObjects - 1;
    for (int i = 0; i < amountOfObjectsMinus1; i++)
    {
        int indexBiggest = i;
        int largestValuePerWeight = object[indexBiggest].getValuePerWeight();
        for (int j = i + 1; j < amountOfObjects; j++)
        {
            int aux = object[j].getValuePerWeight();
            if (aux > largestValuePerWeight)
            {
                indexBiggest = j;
                largestValuePerWeight = aux;
            }
        }
        if (indexBiggest != i)
        {
            Object temp(object[i].getValue(), object[i].getWeight());
            object[i] = object[indexBiggest];
            object[indexBiggest] = temp;
        }
    }
}

int main()
{
    int cases;
    cin >> cases;

    for (int j = 1; j <= cases; j++)
    {
        //Reading input
        cin >> amountOfObjects >> knapsackWeight;
        for (int i = 0; i < amountOfObjects; i++)
        {
            int value, weight;
            cin >> value >> weight;
            object[i].initialize(value, weight);
            include[i] = false;
        }

        sort(); //Sort objects by value/weight in descending order

        //Solve by backtracking
        solutionBacktracking = 0;
        backtrackingNodes = 0;
        backtracking(0, 0, 0);

        //Solve by branch and bound
        solutionBranchAndBound = 0;
        branchAndBoundNodes = 0;
        branchAndBound();

        if (solutionBacktracking != solutionBranchAndBound) {
        	cout << "DEBUG: Error, soluciones diferentes." << endl;
        	cout << "DEBUG: Backtracking: " << solutionBacktracking << endl;
        	cout << "DEBUG: Branch and Bound: " << solutionBranchAndBound << endl;
        }
        cout << "Case " << j << ": " << solutionBacktracking << " " << backtrackingNodes << " " << branchAndBoundNodes << endl;

    }

    return 0;
}
