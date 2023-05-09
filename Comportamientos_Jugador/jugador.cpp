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
	bien_posicionado = false;
	busco_son = true;
	zapatillas_j = zapatillas_s = bikini_j = bikini_s = false;
	//defino el precipicio exteriror
	for(int i = 0; i < mapaResultado.size(); i++){
		//borde superiror
		mapaResultado[0][i] = 'P';
		mapaResultado[1][i] = 'P';
		mapaResultado[2][i] = 'P';
		//borde inferiror
		mapaResultado[mapaResultado.size()-3][i] = 'P';
		mapaResultado[mapaResultado.size()-2][i] = 'P';
		mapaResultado[mapaResultado.size()-1][i] = 'P';
		//borde izq
		mapaResultado[i][0] = 'P';
		mapaResultado[i][1] = 'P';
		mapaResultado[i][2] = 'P';
		//borde der
		mapaResultado[i][mapaResultado.size()-3] = 'P';
		mapaResultado[i][mapaResultado.size()-2] = 'P';
		mapaResultado[i][mapaResultado.size()-1] = 'P';
	}
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
		
		if(!sensores.reset and !sensores.colision){
			if(!bien_posicionado){
				if(sensores.posF == -1){ 
					cout << "Mandadndo señal actWHEREIS" << sensores.nivel << endl;
					accion = actWHEREIS;
				}else{
					cout << "Actualizando c_state " << sensores.nivel << endl;
					c_state.jugador.f = sensores.posF;
					c_state.jugador.c = sensores.posC;
					c_state.jugador.brujula = sensores.sentido;
					c_state.sonambulo.f = sensores.SONposF;
					c_state.sonambulo.c = sensores.SONposC;
					c_state.sonambulo.brujula = sensores.SONsentido;
					//objetivo
					goal.f = sensores.destinoF;
					goal.c = sensores.destinoC;
					act_mapas(sensores);
					bien_posicionado = true;
				}
			}else{
				
				if(busco_son){
					if(!hayPlan){
						cout << "buscando un plan para encontrar al sonambulo" << endl;
						plan = A_estrella_jugador();
						if(plan.size() > 0){
							VisualizaPlan();
							hayPlan = true;
						}
					}else{
						act_mapas(sensores);
						if(plan.size() > 0){
							cout << "Ejecutando la siguiente acción del plan si es posible" << endl;
							sigAccionFactible(sensores);
							if(hayPlan){
								accion = plan.front();
								c_state = apply(accion,c_state);
								plan.pop_front();
							}
						}

						if(plan.size() == 0 and hayPlan){
							cout << "Se completó el plan con exito" << endl;
							hayPlan = false;
							busco_son = false;
						}else{
							if(!hayPlan)cout << "Se necesita recarcular el plan" << endl;
						}
					}
				}else{
					if(!hayPlan){
						cout << "buscando un plan para encontrar la solucion" << endl;
						//plan = A_estrella_jugador();
						plan = A_estrella();
						if(plan.size() > 0){
							VisualizaPlan();
							hayPlan = true;
						}
					}else{
						act_mapas(sensores);
						if(plan.size() > 0){
							cout << "Ejecutando la siguiente acción del plan si es posible" << endl;
							sigAccionFactible(sensores);
							if(hayPlan){
								accion = plan.front();
								c_state = apply(accion,c_state);
								plan.pop_front();
							}
						}

						if(plan.size() == 0 and hayPlan){
							cout << "Se completó el plan con exito" << endl;
							hayPlan = false;
							busco_son = true;
						}else{
							if(!hayPlan)cout << "Se necesita recarcular el plan" << endl;
						}
					}
				}
				
			}
			
		}else{
			bien_posicionado = false;
		}
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

//-----------------------------------------------N4-----------------------------------------------
//calculo de la heuristica con las distancias del jugador al sonambulo o del sonambulo "jugador"  al objetivo
float ComportamientoJugador::heuristica2(const nodoN3& no){
	float h = 0;
	if(busco_son){
		h = ( (abs(no.st.jugador.f-no.st.sonambulo.f)>=3) ? abs(no.st.jugador.f-no.st.sonambulo.f)-3 : abs(no.st.jugador.f-no.st.sonambulo.f) 
		+ (abs(no.st.jugador.c-no.st.sonambulo.c)>=3) ? abs(no.st.jugador.c-no.st.sonambulo.c)-3 : abs(no.st.jugador.c-no.st.sonambulo.c) );
	}else{
		h = ( (abs(no.st.jugador.f-goal.f)>=1) ? abs(no.st.jugador.f-goal.f)-1 : abs(no.st.jugador.f-goal.f) 
		+ (abs(no.st.jugador.c-goal.c)>=1) ? abs(no.st.jugador.c-goal.c)-1 : abs(no.st.jugador.c-goal.c) );
	}
	return h;
}

//true si el sonambulo se encuentra en la solucion
bool ComportamientoJugador::SonambuloEnSolucion(const state& st){
	bool sonambulosolucion = false;
	switch (st.jugador.brujula){
		case norte:
			if((busco_son) ? (st.jugador.c == st.sonambulo.c and st.jugador.f == st.sonambulo.f+1) : (goal.c == st.jugador.c and goal.f == st.jugador.f-1)){
				sonambulosolucion = true;
			}else{
				sonambulosolucion = false;
			}
		break;
		case este:
			if((busco_son) ? (st.jugador.c == st.sonambulo.c-1 and st.jugador.f == st.sonambulo.f) : (goal.c == st.jugador.c+1 and goal.f == st.jugador.f)){
				sonambulosolucion = true;
			}else{
				sonambulosolucion = false;
			}
		break;
		case sur:
			if((busco_son) ? (st.jugador.c == st.sonambulo.c and st.jugador.f == st.sonambulo.f-1) : (goal.c == st.jugador.c and goal.f == st.jugador.f+1)){
				sonambulosolucion = true;
			}else{
				sonambulosolucion = false;
			}
		break;
		case oeste:
			if((busco_son) ? (st.jugador.c == st.sonambulo.c+1 and st.jugador.f == st.sonambulo.f) : (goal.c == st.jugador.c-1 and goal.f == st.jugador.f)){
				sonambulosolucion = true;
			}else{
				sonambulosolucion = false;
			}
		break;
	}
	return sonambulosolucion;
}

//se encarga de saber si la siguiente accion del plan es posible o no
void ComportamientoJugador::sigAccionFactible(Sensores sensores){
	Action ac1 = plan.front();
	bool plan_factible = true;
	ubicacion x = NextCasilla(c_state.sonambulo);
	
	if(ac1 == actFORWARD){
		if(sensores.superficie[2]!='_' or (sensores.terreno[2]=='A' and !bikini_j) or (sensores.terreno[2]=='B' and !zapatillas_j) or sensores.terreno[2]=='M' or sensores.terreno[2]=='P') 
			plan_factible = false;

		if(!busco_son){
			if(ac1 == actSON_FORWARD){
				if(mapaEntidades[x.f][x.c]!='_' or (mapaResultado[x.f][x.c]=='A' and !bikini_s) or (mapaResultado[x.f][x.c]=='B' and !zapatillas_s) or mapaResultado[x.f][x.c]=='M' or mapaResultado[x.f][x.c]=='P')
				plan_factible = false;
			}
		}
	}
	
	
	if(!plan_factible){
		while(plan.size()>0) plan.pop_front();
		hayPlan = false;
	}

}

//Gestion de la vision y de la puntuacion por defecto de las casillas. 
void ComportamientoJugador::act_mapas(Sensores sensores){
	int fil = c_state.jugador.f , col = c_state.jugador.c;
    int f = fil, c = col;
	Orientacion ori = c_state.jugador.brujula;
    for(int i = 0; i < sensores.terreno.size(); i++){
        if(ori == norte or ori == este or ori == sur or ori == oeste){
            if(i>=1 and i<4){
                f = (ori == norte or ori == este)?(fil-1):(fil+1);
                c = (ori == norte or ori == oeste)?(col-1):(col+1);
                if (ori == norte) c = c + i - 1;
                if (ori == este) f = f + i - 1;
                if (ori == sur) c = c - i + 1;
                if (ori == oeste) f = f - i + 1;
            }
            if(i>=4 and i<9){
                f = (ori == norte or ori == este)?(fil-2):(fil+2);
                c = (ori == norte or ori == oeste)?(col-2):(col+2);
                if (ori == norte) c = c + i - 4;
                if (ori == este) f = f + i - 4;
                if (ori == sur) c = c - i + 4;
                if (ori == oeste) f = f - i + 4;
            }
            if(i>=9){
                f = (ori == norte or ori == este)?(fil-3):(fil+3);
                c = (ori == norte or ori == oeste)?(col-3):(col+3);
                if (ori == norte) c = c + i - 9;
                if (ori == este) f = f + i - 9;
                if (ori == sur) c = c - i + 9;
                if (ori == oeste) f = f - i + 9;
            }
        }else{
            if(i>=1 and i<4){
                if(i==1){
                    f = (ori == sureste or ori == noroeste)? fil :(ori == noreste)?(fil-1):(fil+1);
                    c = (ori == noreste or ori == suroeste)? col :(ori == noroeste)?(col-1):(col+1);
                }else if(i<=2){
                    (ori == noreste) ? c++ : (ori == suroeste) ? c-- : c;
                    (ori == sureste) ? f++ : (ori == noroeste) ? f-- : f;
                }else{
                    (ori == noreste)? f++ : (ori == suroeste) ? f-- : f;
                    (ori == noroeste) ? c++ : (ori == sureste) ? c-- : c;
                }
            }
            if(i>=4 and i<9){
                if(i==4){
                    f = (ori == sureste or ori == noroeste)? fil :(ori == noreste)?(fil-2):(fil+2);
                    c = (ori == noreste or ori == suroeste)? col :(ori == noroeste)?(col-2):(col+2);
                }else if(i<=6){
                    (ori == noreste) ? c++ : (ori == suroeste) ? c-- : c;
                    (ori == sureste) ? f++ : (ori == noroeste) ? f-- : f;
                }else{
                    (ori == noreste)? f++ : (ori == suroeste) ? f-- : f;
                    (ori == noroeste) ? c++ : (ori == sureste) ? c-- : c;
                }
            }
            if(i>=9){
                if(i==9){
                    f = (ori == sureste or ori == noroeste)? fil :(ori == noreste)?(fil-3):(fil+3);
                    c = (ori == noreste or ori == suroeste)? col :(ori == noroeste)?(col-3):(col+3);
                }else if(i<=12){
                    (ori == noreste) ? c++ : (ori == suroeste) ? c-- : c;
                    (ori == sureste) ? f++ : (ori == noroeste) ? f-- : f;
                }else{
                    (ori == noreste)? f++ : (ori == suroeste) ? f-- : f;
                    (ori == noroeste) ? c++ : (ori == sureste) ? c-- : c;
                }
            }
        }
		if(mapaResultado[f][c] != sensores.terreno[i]){
			mapaResultado[f][c] = sensores.terreno[i];
		}
	}
}

//busqueda a* para jugador o sonambulo
list<Action> ComportamientoJugador::A_estrella_jugador(){
	nodoN3 current_node;
	priority_queue<nodoN3,vector<nodoN3>,Compare<nodoN3>> frontier;
	set<nodoN3> explored;
	list<Action> plan;
	current_node.st = c_state;
	current_node.coste = 0;
	current_node.h = heuristica2(current_node);
	current_node.bikini_j = (mapaResultado[current_node.st.jugador.f][current_node.st.jugador.c] == 'K') ? true : false;
	current_node.zapatillas_j = (mapaResultado[current_node.st.jugador.f][current_node.st.jugador.c] == 'D') ? true : false;
	bool SolutionFound = SonambuloEnSolucion(current_node.st);

	frontier.push(current_node);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop();
		explored.insert(current_node);
		if(SonambuloEnSolucion(current_node.st)){
			SolutionFound = true;
		}else{

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

//funcion para poneral sonambulo en medio del plan
void ComportamientoJugador::colocarSon(){
	if(mapaConPlan[c_state.sonambulo.f][c_state.sonambulo.c]==1){
		switch (c_state.jugador.brujula){
			case norte:
				switch (c_state.sonambulo.brujula){
					case noreste:
						plan_son.push_back(actSON_TURN_SL);
					break;
					case este:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case sureste:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case sur:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case suroeste:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case oeste:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case noroeste:
						plan_son.push_back(actSON_TURN_SR);
					break;
				}
			break;
			case este:
				switch (c_state.sonambulo.brujula){
					case norte:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case noreste:
						plan_son.push_back(actSON_TURN_SR);
					break;
					case sureste:
						plan_son.push_back(actSON_TURN_SL);
					break;
					case sur:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case suroeste:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case oeste:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case noroeste:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
				}
			break;
			case sur:
				switch (c_state.sonambulo.brujula){
					case norte:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case noreste:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case este:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case sureste:
						plan_son.push_back(actSON_TURN_SR);
					break;
					case suroeste:
						plan_son.push_back(actSON_TURN_SL);
					break;
					case oeste:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case noroeste:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
				}
			break;
			case oeste:
				switch (c_state.sonambulo.brujula){
					case norte:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case noreste:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case este:
						plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);plan_son.push_back(actSON_TURN_SL);
					break;
					case sureste:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case sur:
						plan_son.push_back(actSON_TURN_SR);plan_son.push_back(actSON_TURN_SR);
					break;
					case suroeste:
						plan_son.push_back(actSON_TURN_SR);
					break;
					case noroeste:
						plan_son.push_back(actSON_TURN_SL);
					break;
				}
			break;

		}
		sonColocado = true;
	}else{
		// if()
	}
}