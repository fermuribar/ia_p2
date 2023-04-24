#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

//Estados por niveles
struct stateN0{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator== (const stateN0& x) const{
    if(jugador == x.jugador and sonambulo.f ==  x.sonambulo.f and sonambulo.c == x.sonambulo.c){
      return true;
    }else{
      return false;
    }
  }
};

//Nodos por niveles

struct nodoN0{
  stateN0 st;
  list<Action> secuencia;

  bool operator== (const nodoN0& n) const{
    return (st == n.st);
  }

  bool operator< (const nodoN0& n) const{
    if(st.jugador.f < n.st.jugador.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula) return true;
    else return false;
  }

};




class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size);

    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);


  private:
    // Declaracion de metodos privados________________________________________

    //--------------Representacion del plan en simulador----------------------
      //borra matriz
    void AnulaMatriz();
      //visualiza el plan en el simulador
    void VisualizaPlan();

    //--------------Busqueda y subfunciones-----------------------------------
      //busca un estado en una lista de estados
    bool Find(const stateN0& item, const list<nodoN0>& lista);
      //obtenfo cual es la casilla si abanzo segun su ubicacion
    ubicacion NextCasilla(const ubicacion& pos);
      //aplica una accion a un estado y obtiene otro
    stateN0 apply(const Action& a, const stateN0& st);
      //comprueba si la casilla es transitable o no
    bool inline CasillaTransitable(const ubicacion& x){ return (mapaResultado[x.f][x.c] != 'M' and mapaResultado[x.f][x.c] != 'P');}
      //busqueda en anchura obteniendo lista de acciones
    list<Action> AnchuraSoloJugador();

    // Declarar Variables de Estado___________________________________________

    list<Action> plan;  //Almacena el plan en ejecución
    bool hayPlan;       //Si verdad indica que se está siguiendo un plan.

    stateN0 c_state;    //estado actual
    ubicacion goal;     //ubicacion del objetivo
};

#endif
