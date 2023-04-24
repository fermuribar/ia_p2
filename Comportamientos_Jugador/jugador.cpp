#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

//CONSTRUCTOR-----------------------------------------------------------------------------------------------------------

ComportamientoJugador::ComportamientoJugador(unsigned int size) : Comportamiento(size) {
    // Inicializar Variables de Estado
	hayPlan = false;
}


//THINK------------------------------------------------------------------------------------------------------------------

//movimiento caballo
list<Action> caballito(){
	list<Action> secuencia;
	secuencia.push_back(actFORWARD);
	secuencia.push_back(actFORWARD);
	secuencia.push_back(actTURN_R);
	secuencia.push_back(actFORWARD);
	return secuencia;
}

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores){
	Action accion= actIDLE;

	// Incluir aquí el comportamiento del agente jugador

	if(!hayPlan){
		//invocamos  el método de búsqueda
		cout << "Calculando un nuevo plan" << endl;

		//estado actual
		c_state.jugador.f = sensores.posF;
		c_state.jugador.c = sensores.posC;
		c_state.jugador.brujula = sensores.sentido;
		c_state.sonambulo.f = sensores.SONposF;
		c_state.sonambulo.c = sensores.SONposC;
		c_state.sonambulo.brujula = sensores.SONsentido;
		
		//objetivo
		goal.f = sensores.destinoF;
		goal.c = sensores.destinoC;

		//obtencion del plan segun nivel
		switch(sensores.nivel){
			case 0: plan = AnchuraSoloJugador();
			break;
			case 1: cout << "Pendiente de implementacion nivel " << sensores.nivel << endl;
			break;
			case 2: cout << "Pendiente de implementacion nivel " << sensores.nivel << endl;
			break;
			case 3: cout << "Pendiente de implementacion nivel " << sensores.nivel << endl;
			break;
			case 4: cout << "Pendiente de implementacion nivel " << sensores.nivel << endl;
			break;
		}
		if(plan.size() > 0){
			VisualizaPlan();
			hayPlan = true;
		}
	}

	if(hayPlan and plan.size() > 0){
		cout << "Ejecutando la siguiente acción del plan" << endl;
		accion = plan.front();
		plan.pop_front();
	}

	if(plan.size() == 0){
		cout << "Se completó el plan" << endl;
		hayPlan = false;
	}
	
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

//_____________________________________Representacion del plan en simulador_______________________
//borra matriz
void ComportamientoJugador::AnulaMatriz(){
	for(int i = 0; i < mapaConPlan.size(); i++){
		for(int j = 0; j < mapaConPlan.size(); j++){
			mapaConPlan[i][j] = 0;
		}
	}
}
//visualiza el plan en el simulador
void ComportamientoJugador::VisualizaPlan(){
	AnulaMatriz();
	stateN0 cst = c_state;
	auto it = plan.begin();
	while(it != plan.end()){
		switch (*it){
			case actFORWARD:
				cst.jugador = NextCasilla(cst.jugador);
				mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
			case actTURN_R:
				cst.jugador.brujula = static_cast<Orientacion>((cst.jugador.brujula + 2) % 8);
			break;
			case actTURN_L:
				cst.jugador.brujula = static_cast<Orientacion>((cst.jugador.brujula + 6) % 8);
			break;
			case actSON_FORWARD:
				cst.sonambulo = NextCasilla(cst.sonambulo);
				mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 1;
			break;
			case actSON_TURN_SR:
				cst.sonambulo.brujula = static_cast<Orientacion>((cst.sonambulo.brujula + 1) % 8);
			break;
			case actSON_TURN_SL:
				cst.sonambulo.brujula = static_cast<Orientacion>((cst.sonambulo.brujula + 7) % 8);
			break;
		}
		it++;
	}
}


//___________________________________________Buesquedas___________________________________________

//-----------------------------------------------N0-----------------------------------------------

//busca un estado en una lista de estados
bool ComportamientoJugador::Find(const stateN0& item, const list<nodoN0>& lista){
	auto it = lista.begin();
	while(it != lista.end() and !(it->st == item)){
		it++;
	}
	return (it != lista.end());
}

//obtenfo cual es la casilla si abanzo segun su ubicacion
ubicacion ComportamientoJugador::NextCasilla(const ubicacion& pos){
	ubicacion next = pos;
	switch (pos.brujula){
		case norte: next.f--;
		break;
		case noreste: next.f--; next.c++;
		break;
		case este: next.c++; 
		break;
		case sureste: next.f++; next.c++;
		break;
		case sur: next.f++; 
		break;
		case suroeste: next.f++; next.c--;
		break;
		case oeste: next.c--;
		break;
		case noroeste: next.f--; next.c--;
		break;
	}

	return next;
}

//aplica una accion a un estado y obtiene otro
stateN0 ComportamientoJugador::apply(const Action& a, const stateN0& st){
	stateN0 st_result = st;
	ubicacion sig_ubicacion;
	switch(a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(st.jugador);
			if(CasillaTransitable(sig_ubicacion) and !(sig_ubicacion == st.sonambulo)) st_result.jugador = sig_ubicacion;
		break;
		case actTURN_L:
			st_result.jugador.brujula = static_cast<Orientacion>((st.jugador.brujula + 6) % 8);
		break;
		case actTURN_R:
			st_result.jugador.brujula = static_cast<Orientacion>((st.jugador.brujula + 2) % 8);
		break;
	}
	return st_result;
}

//busqueda en anchura obteniendo lista de acciones
list<Action> ComportamientoJugador::AnchuraSoloJugador(){
	nodoN0 current_node;
	list<nodoN0> frontier;
	set<nodoN0> explored;
	list<Action> plan;
	current_node.st = c_state;
	bool SolutionFound = (current_node.st.jugador.f == goal.f and current_node.st.jugador.c == goal.c);

	frontier.push_back(current_node);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);
		//Generar hijo actForward
		nodoN0 child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st);
		if(child_forward.st.jugador.f == goal.f and child_forward.st.jugador.c == goal.c){
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		}else if(explored.find(child_forward) == explored.end()){
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if(!SolutionFound){
			//Generar hijo actTurn_L
			nodoN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L,current_node.st);
			if(explored.find(child_turnl) == explored.end()){
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}
			//Generar hijo actTurn_R
			nodoN0 child_turnr = current_node;
			child_turnr.st = apply(actTURN_R,current_node.st);
			if(explored.find(child_turnr) == explored.end()){
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}
		}

		if(!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				current_node = frontier.front();
			}
		} 

	}

	if(SolutionFound) plan = current_node.secuencia;

	return plan;
}