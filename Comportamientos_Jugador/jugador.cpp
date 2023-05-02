#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <queue>
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

	if(sensores.nivel != 4){
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
				case 1: plan = AnchuraSonambulo();
				break;
				case 2: plan = Dijkstra();
				break;
				case 3: plan = A_estrella();
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

	}else{
		cout << "¡Comenzando! Pendiente de implementacion nivel " << sensores.nivel << endl;
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
	state cst = c_state;
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
				mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
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

//aplica una accion a un estado y obtiene otro		{modificacion para n1}
state ComportamientoJugador::apply(const Action& a, const state& st){
	state st_result = st;
	ubicacion sig_ubicacion;
	switch(a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(st.jugador);
			if(CasillaTransitable(sig_ubicacion) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c)) st_result.jugador = sig_ubicacion;
		break;
		case actTURN_L:
			st_result.jugador.brujula = static_cast<Orientacion>((st.jugador.brujula + 6) % 8);
		break;
		case actTURN_R:
			st_result.jugador.brujula = static_cast<Orientacion>((st.jugador.brujula + 2) % 8);
		break;
		case actSON_FORWARD:
			sig_ubicacion = NextCasilla(st.sonambulo);
			if(CasillaTransitable(sig_ubicacion) and !(sig_ubicacion.f == st.jugador.f and sig_ubicacion.c == st.jugador.c)) st_result.sonambulo = sig_ubicacion;
		break;
		case actSON_TURN_SL:
			st_result.sonambulo.brujula = static_cast<Orientacion>((st.sonambulo.brujula + 7) % 8);
		break;
		case actSON_TURN_SR:
			st_result.sonambulo.brujula = static_cast<Orientacion>((st.sonambulo.brujula + 1) % 8);
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
				if(!frontier.empty()) current_node = frontier.front();
			}
		} 

	}

	if(SolutionFound) plan = current_node.secuencia;

	return plan;
}

//-----------------------------------------------N1-----------------------------------------------

//true si el sonambulo se encuentra en el campo de vision del jugador
bool ComportamientoJugador::SonambuloEnVision(const state& st){
	bool sonambuloVisto = false;
	switch (st.jugador.brujula){
		case norte:
			if(st.sonambulo.c == st.jugador.c and (st.sonambulo.f == st.jugador.f-1 or st.sonambulo.f == st.jugador.f-2 or st.sonambulo.f == st.jugador.f-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c-1 and (st.sonambulo.f == st.jugador.f-1 or st.sonambulo.f == st.jugador.f-2 or st.sonambulo.f == st.jugador.f-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c+1 and (st.sonambulo.f == st.jugador.f-1 or st.sonambulo.f == st.jugador.f-2 or st.sonambulo.f == st.jugador.f-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c-2 and (st.sonambulo.f == st.jugador.f-2 or st.sonambulo.f == st.jugador.f-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c+2 and (st.sonambulo.f == st.jugador.f-2 or st.sonambulo.f == st.jugador.f-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c-3 and st.sonambulo.f == st.jugador.f-3){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c+3 and st.sonambulo.f == st.jugador.f-3){
				sonambuloVisto = true;
			}else{
				sonambuloVisto = false;
			}
		break;
		case este:
			if(st.sonambulo.f == st.jugador.f and (st.sonambulo.c == st.jugador.c+1 or st.sonambulo.c == st.jugador.c+2 or st.sonambulo.c == st.jugador.c+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f-1 and (st.sonambulo.c == st.jugador.c+1 or st.sonambulo.c == st.jugador.c+2 or st.sonambulo.c == st.jugador.c+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f+1 and (st.sonambulo.c == st.jugador.c+1 or st.sonambulo.c == st.jugador.c+2 or st.sonambulo.c == st.jugador.c+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f-2 and (st.sonambulo.c == st.jugador.c+2 or st.sonambulo.c == st.jugador.c+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f+2 and (st.sonambulo.c == st.jugador.c+2 or st.sonambulo.c == st.jugador.c+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f-3 and st.sonambulo.c == st.jugador.c+3){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f+3 and st.sonambulo.c == st.jugador.c+3){
				sonambuloVisto = true;
			}else{
				sonambuloVisto = false;
			}
		break;
		case sur:
		if(st.sonambulo.c == st.jugador.c and (st.sonambulo.f == st.jugador.f+1 or st.sonambulo.f == st.jugador.f+2 or st.sonambulo.f == st.jugador.f+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c-1 and (st.sonambulo.f == st.jugador.f+1 or st.sonambulo.f == st.jugador.f+2 or st.sonambulo.f == st.jugador.f+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c+1 and (st.sonambulo.f == st.jugador.f+1 or st.sonambulo.f == st.jugador.f+2 or st.sonambulo.f == st.jugador.f+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c-2 and (st.sonambulo.f == st.jugador.f+2 or st.sonambulo.f == st.jugador.f+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c+2 and (st.sonambulo.f == st.jugador.f+2 or st.sonambulo.f == st.jugador.f+3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c-3 and st.sonambulo.f == st.jugador.f+3){
				sonambuloVisto = true;
			}else if(st.sonambulo.c == st.jugador.c+3 and st.sonambulo.f == st.jugador.f+3){
				sonambuloVisto = true;
			}else{
				sonambuloVisto = false;
			}
		break;
		case oeste:
		if(st.sonambulo.f == st.jugador.f and (st.sonambulo.c == st.jugador.c-1 or st.sonambulo.c == st.jugador.c-2 or st.sonambulo.c == st.jugador.c-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f-1 and (st.sonambulo.c == st.jugador.c-1 or st.sonambulo.c == st.jugador.c-2 or st.sonambulo.c == st.jugador.c-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f+1 and (st.sonambulo.c == st.jugador.c-1 or st.sonambulo.c == st.jugador.c-2 or st.sonambulo.c == st.jugador.c-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f-2 and (st.sonambulo.c == st.jugador.c-2 or st.sonambulo.c == st.jugador.c-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f+2 and (st.sonambulo.c == st.jugador.c-2 or st.sonambulo.c == st.jugador.c-3)){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f-3 and st.sonambulo.c == st.jugador.c-3){
				sonambuloVisto = true;
			}else if(st.sonambulo.f == st.jugador.f+3 and st.sonambulo.c == st.jugador.c-3){
				sonambuloVisto = true;
			}else{
				sonambuloVisto = false;
			}
		break;
	}
	return sonambuloVisto;
}

//busqueda en anchura obteniendo lista de acciones
list<Action> ComportamientoJugador::AnchuraSonambulo(){
	nodoN1 current_node;
	list<nodoN1> frontier;
	set<nodoN1> explored;
	list<Action> plan;
	current_node.st = c_state;
	bool SolutionFound = (current_node.st.sonambulo.f == goal.f and current_node.st.sonambulo.c == goal.c);

	frontier.push_back(current_node);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);
		
		if(SonambuloEnVision(current_node.st)){
			//Generar hijo actSON_FORWARD
			nodoN1 child_SONforward = current_node;
			child_SONforward.st = apply(actSON_FORWARD, current_node.st);
			if(child_SONforward.st.sonambulo.f == goal.f and child_SONforward.st.sonambulo.c == goal.c){
				child_SONforward.secuencia.push_back(actSON_FORWARD);
				current_node = child_SONforward;
				SolutionFound = true;
			}else if(explored.find(child_SONforward) == explored.end()){
				child_SONforward.secuencia.push_back(actSON_FORWARD);
				frontier.push_back(child_SONforward);
			}

			if(!SolutionFound){
				//Generar hijo actSON_TURN_SL
				nodoN1 child_SONturnl = current_node;
				child_SONturnl.st = apply(actSON_TURN_SL,current_node.st);
				if(explored.find(child_SONturnl) == explored.end()){
					child_SONturnl.secuencia.push_back(actSON_TURN_SL);
					frontier.push_back(child_SONturnl);
				}
				//Generar hijo actSON_TURN_SR
				nodoN1 child_SONturnr = current_node;
				child_SONturnr.st = apply(actSON_TURN_SR,current_node.st);
				if(explored.find(child_SONturnr) == explored.end()){
					child_SONturnr.secuencia.push_back(actSON_TURN_SR);
					frontier.push_back(child_SONturnr);
				}
			}
		}

		//Generar hijo actForward
		nodoN1 child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st);
		if(explored.find(child_forward) == explored.end()){
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		//Generar hijo actTurn_L
		nodoN1 child_turnl = current_node;
		child_turnl.st = apply(actTURN_L,current_node.st);
		if(explored.find(child_turnl) == explored.end()){
			child_turnl.secuencia.push_back(actTURN_L);
			frontier.push_back(child_turnl);
		}
		//Generar hijo actTurn_R
		nodoN1 child_turnr = current_node;
		child_turnr.st = apply(actTURN_R,current_node.st);
		if(explored.find(child_turnr) == explored.end()){
			child_turnr.secuencia.push_back(actTURN_R);
			frontier.push_back(child_turnr);
		}
			
		if(!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if(!frontier.empty()) current_node = frontier.front();
			}
		} 

	}

	if(SolutionFound) plan = current_node.secuencia;

	return plan;
}

//-----------------------------------------------N2-----------------------------------------------

//puntuacion segun accion y casilla
nodoN2 ComportamientoJugador::Aply_puntuacion(const Action& a, const nodoN2& no){
	nodoN2 n_salida = no;
	ubicacion sig_ubicacion;
	
	switch(a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(no.st.jugador);
			if(CasillaTransitable(sig_ubicacion) and !(sig_ubicacion.f == no.st.sonambulo.f and sig_ubicacion.c == no.st.sonambulo.c)){
				n_salida.st.jugador = sig_ubicacion;
				switch(mapaResultado[no.st.jugador.f][no.st.jugador.c]){
					case 'A': n_salida.coste += (no.bikini_j) ? 10 : 100; break;
					case 'B': n_salida.coste += (no.zapatillas_j) ? 15 : 50; break;
					case 'T': n_salida.coste += 2; break;
					case 'K': n_salida.bikini_j = true; n_salida.zapatillas_j = false; n_salida.coste += 1; break;
					case 'D': n_salida.zapatillas_j = true; n_salida.bikini_j = false; n_salida.coste += 1; break;
					default: n_salida.coste += 1; break;
				}
			} 
		break;
		case actTURN_L:
			n_salida.st.jugador.brujula = static_cast<Orientacion>((no.st.jugador.brujula + 6) % 8);
			switch(mapaResultado[no.st.jugador.f][no.st.jugador.c]){
				case 'A': n_salida.coste += (no.bikini_j) ? 5 : 25; break;
				case 'B': n_salida.coste += (no.zapatillas_j) ? 1 : 5; break;
				case 'T': n_salida.coste += 2; break;
				default: n_salida.coste += 1; break;
			}
		break;
		case actTURN_R:
			n_salida.st.jugador.brujula = static_cast<Orientacion>((no.st.jugador.brujula + 2) % 8);
			switch(mapaResultado[no.st.jugador.f][no.st.jugador.c]){
				case 'A': n_salida.coste += (no.bikini_j) ? 5 : 25; break;
				case 'B': n_salida.coste += (no.zapatillas_j) ? 1 : 5; break;
				case 'T': n_salida.coste += 2; break;
				default: n_salida.coste += 1; break;
			}
		break;
	}
	return n_salida;
}

template<typename T>
struct Compare{
    bool operator() (const T& a, const T& b) {
        return (a > b);
    }
};

//busqueda en anchura para jugador obteniendo lista de acciones dijkstra
list<Action> ComportamientoJugador::Dijkstra(){
	nodoN2 current_node;
	priority_queue<nodoN2,vector<nodoN2>,Compare<nodoN2>> frontier;
	set<nodoN2> explored;
	list<Action> plan;
	current_node.st = c_state;
	current_node.coste = 0;
	current_node.bikini_j = (mapaResultado[current_node.st.jugador.f][current_node.st.jugador.c] == 'K') ? true : false;
	current_node.bikini_s = (mapaResultado[current_node.st.sonambulo.f][current_node.st.sonambulo.c] == 'K') ? true : false;
	current_node.zapatillas_j = (mapaResultado[current_node.st.jugador.f][current_node.st.jugador.c] == 'D') ? true : false;
	current_node.zapatillas_s = (mapaResultado[current_node.st.sonambulo.f][current_node.st.sonambulo.c] == 'D') ? true : false;
	bool SolutionFound = (current_node.st.jugador.f == goal.f and current_node.st.jugador.c == goal.c);

	frontier.push(current_node);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop();
		explored.insert(current_node);
		if(current_node.st.jugador.f == goal.f and current_node.st.jugador.c == goal.c){
			SolutionFound = true;
		}else{
			//Generar hijo actForward
			nodoN2 child_forward = current_node;
			child_forward = Aply_puntuacion(actFORWARD, current_node);
			if(explored.find(child_forward) == explored.end()){
				child_forward.secuencia.push_back(actFORWARD);
				frontier.push(child_forward);
			}
			//Generar hijo actTurn_L
			nodoN2 child_turnl = current_node;
			child_turnl = Aply_puntuacion(actTURN_L,current_node);
			if(explored.find(child_turnl) == explored.end()){
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push(child_turnl);
			}
			//Generar hijo actTurn_R
			nodoN2 child_turnr = current_node;
			child_turnr = Aply_puntuacion(actTURN_R,current_node);
			if(explored.find(child_turnr) == explored.end()){
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push(child_turnr);
			}
		}	

		if(!SolutionFound and !frontier.empty()){
			current_node = frontier.top();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop();
				if(!frontier.empty()) current_node = frontier.top();
			}
		}
		

	}

	if(SolutionFound) plan = current_node.secuencia;

	return plan;
}

//-----------------------------------------------N3-----------------------------------------------
//calculo de la heuristica con las distancias del jugador al sonambulo y del sonambulo al objetivo
float ComportamientoJugador::heuristica(const nodoN3& no){
	float h = 0;
	h = ( (abs(no.st.jugador.f-no.st.sonambulo.f)>=3) ? abs(no.st.jugador.f-no.st.sonambulo.f)-3 : abs(no.st.jugador.f-no.st.sonambulo.f) 
	+ (abs(no.st.jugador.c-no.st.sonambulo.c)>=3) ? abs(no.st.jugador.c-no.st.sonambulo.c)-3 : abs(no.st.jugador.c-no.st.sonambulo.c) )*0.3;
	h += max(abs(no.st.sonambulo.f-goal.f),abs(no.st.sonambulo.c-goal.c))*0.7;
	return h;
}

//puntuacion segun accion y casilla
nodoN3 ComportamientoJugador::Aply_puntuacion_heuristica(const Action& a, const nodoN3& no){
	nodoN3 n_salida = no;
	ubicacion sig_ubicacion;

	switch(a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(no.st.jugador);
			if(CasillaTransitable(sig_ubicacion) and !(sig_ubicacion.f == no.st.sonambulo.f and sig_ubicacion.c == no.st.sonambulo.c)){
				n_salida.st.jugador = sig_ubicacion;
				switch(mapaResultado[no.st.jugador.f][no.st.jugador.c]){
					case 'A': n_salida.coste += (no.bikini_j) ? 10 : 100; break;
					case 'B': n_salida.coste += (no.zapatillas_j) ? 15 : 50; break;
					case 'T': n_salida.coste += 2; break;
					case 'K': n_salida.bikini_j = true; n_salida.zapatillas_j = false; n_salida.coste += 1; break;
					case 'D': n_salida.zapatillas_j = true; n_salida.bikini_j = false; n_salida.coste += 1; break;
					default: n_salida.coste += 1; break;
				}
			} 
		break;
		case actTURN_L:
			n_salida.st.jugador.brujula = static_cast<Orientacion>((no.st.jugador.brujula + 6) % 8);
			switch(mapaResultado[no.st.jugador.f][no.st.jugador.c]){
				case 'A': n_salida.coste += (no.bikini_j) ? 5 : 25; break;
				case 'B': n_salida.coste += (no.zapatillas_j) ? 1 : 5; break;
				case 'T': n_salida.coste += 2; break;
				default: n_salida.coste += 1; break;
			}
		break;
		case actTURN_R:
			n_salida.st.jugador.brujula = static_cast<Orientacion>((no.st.jugador.brujula + 2) % 8);
			switch(mapaResultado[no.st.jugador.f][no.st.jugador.c]){
				case 'A': n_salida.coste += (no.bikini_j) ? 5 : 25; break;
				case 'B': n_salida.coste += (no.zapatillas_j) ? 1 : 5; break;
				case 'T': n_salida.coste += 2; break;
				default: n_salida.coste += 1; break;
			}
		break;
		case actSON_FORWARD:
			sig_ubicacion = NextCasilla(no.st.sonambulo);
			if(CasillaTransitable(sig_ubicacion) and !(sig_ubicacion.f == no.st.jugador.f and sig_ubicacion.c == no.st.jugador.c)){
				n_salida.st.sonambulo = sig_ubicacion;
				switch(mapaResultado[no.st.sonambulo.f][no.st.sonambulo.c]){
					case 'A': n_salida.coste += (no.bikini_s) ? 10 : 100; break;
					case 'B': n_salida.coste += (no.zapatillas_s) ? 15 : 50; break;
					case 'T': n_salida.coste += 2; break;
					case 'K': n_salida.bikini_s = true; n_salida.zapatillas_s = false; n_salida.coste += 1; break;
					case 'D': n_salida.zapatillas_s = true; n_salida.bikini_s = false; n_salida.coste += 1; break;
					default: n_salida.coste += 1; break;
				}
			} 
		break;
		case actSON_TURN_SL:
			n_salida.st.sonambulo.brujula = static_cast<Orientacion>((no.st.sonambulo.brujula + 7) % 8);
			switch(mapaResultado[no.st.sonambulo.f][no.st.sonambulo.c]){
				case 'A': n_salida.coste += (no.bikini_s) ? 2 : 7; break;
				case 'B': n_salida.coste += (no.zapatillas_s) ? 1 : 3; break;
				default: n_salida.coste += 1; break;
			}
		break;
		case actSON_TURN_SR:
			n_salida.st.sonambulo.brujula = static_cast<Orientacion>((no.st.sonambulo.brujula + 1) % 8);
			switch(mapaResultado[no.st.sonambulo.f][no.st.sonambulo.c]){
				case 'A': n_salida.coste += (no.bikini_s) ? 2 : 7; break;
				case 'B': n_salida.coste += (no.zapatillas_s) ? 1 : 3; break;
				default: n_salida.coste += 1; break;
			}
		break;
	}
	n_salida.h = heuristica(n_salida);
	return n_salida;
}
//busqueda en anchura para jugador obteniendo lista de acciones dijkstra
list<Action> ComportamientoJugador::A_estrella(){
	nodoN3 current_node;
	priority_queue<nodoN3,vector<nodoN3>,Compare<nodoN3>> frontier;
	set<nodoN3> explored;
	list<Action> plan;
	current_node.st = c_state;
	current_node.coste = 0;
	current_node.h = heuristica(current_node);
	current_node.bikini_j = (mapaResultado[current_node.st.jugador.f][current_node.st.jugador.c] == 'K') ? true : false;
	current_node.bikini_s = (mapaResultado[current_node.st.sonambulo.f][current_node.st.sonambulo.c] == 'K') ? true : false;
	current_node.zapatillas_j = (mapaResultado[current_node.st.jugador.f][current_node.st.jugador.c] == 'D') ? true : false;
	current_node.zapatillas_s = (mapaResultado[current_node.st.sonambulo.f][current_node.st.sonambulo.c] == 'D') ? true : false;
	bool SolutionFound = (current_node.st.sonambulo.f == goal.f and current_node.st.sonambulo.c == goal.c);

	frontier.push(current_node);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop();
		explored.insert(current_node);
		if(current_node.st.sonambulo.f == goal.f and current_node.st.sonambulo.c == goal.c){
			SolutionFound = true;
		}else{

			if(SonambuloEnVision(current_node.st)){
				//Generar hijo actSON_FORWARD
				nodoN3 child_SONforward = current_node;
				child_SONforward = Aply_puntuacion_heuristica(actSON_FORWARD, current_node);
				if(explored.find(child_SONforward) == explored.end()){
					child_SONforward.secuencia.push_back(actSON_FORWARD);
					frontier.push(child_SONforward);
				}

				//Generar hijo actSON_TURN_SL
				nodoN3 child_SONturnl = current_node;
				child_SONturnl = Aply_puntuacion_heuristica(actSON_TURN_SL,current_node);
				if(explored.find(child_SONturnl) == explored.end()){
					child_SONturnl.secuencia.push_back(actSON_TURN_SL);
					frontier.push(child_SONturnl);
				}
				//Generar hijo actSON_TURN_SR
				nodoN3 child_SONturnr = current_node;
				child_SONturnr = Aply_puntuacion_heuristica(actSON_TURN_SR,current_node);
				if(explored.find(child_SONturnr) == explored.end()){
					child_SONturnr.secuencia.push_back(actSON_TURN_SR);
					frontier.push(child_SONturnr);
				}
				
			}

			//Generar hijo actForward
			nodoN3 child_forward = current_node;
			child_forward = Aply_puntuacion_heuristica(actFORWARD, current_node);
			if(explored.find(child_forward) == explored.end()){
				child_forward.secuencia.push_back(actFORWARD);
				frontier.push(child_forward);
			}
			
			//Generar hijo actTurn_L
			nodoN3 child_turnl = current_node;
			child_turnl = Aply_puntuacion_heuristica(actTURN_L,current_node);
			if(explored.find(child_turnl) == explored.end()){
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push(child_turnl);
			}
			//Generar hijo actTurn_R
			nodoN3 child_turnr = current_node;
			child_turnr = Aply_puntuacion_heuristica(actTURN_R,current_node);
			if(explored.find(child_turnr) == explored.end()){
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push(child_turnr);
			}
		
		}
		if(!SolutionFound and !frontier.empty()){
			current_node = frontier.top();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop();
				if(!frontier.empty()) current_node = frontier.top();
			}
		}
	}
	if(SolutionFound) plan = current_node.secuencia;

	return plan;
}