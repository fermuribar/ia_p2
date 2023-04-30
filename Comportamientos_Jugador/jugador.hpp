#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

//Estados por niveles
struct state{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator== (const state& x) const{
    if(jugador == x.jugador and sonambulo ==  x.sonambulo){
      return true;
    }else{
      return false;
    }
  }
};

//Nodos por niveles

struct nodoN0{
  state st;
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

struct nodoN1{
  state st;
  list<Action> secuencia;

  bool operator== (const nodoN1& n) const{
    return (st == n.st);
  }

  bool operator< (const nodoN1& n) const{
    if(st.jugador.f < n.st.jugador.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f < n.st.sonambulo.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c < n.st.sonambulo.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula < n.st.sonambulo.brujula) return true;
    else return false;
  }

};

struct nodoN2{
  state st;
  list<Action> secuencia;
  int coste;
  bool bikini_j, bikini_s, zapatillas_j, zapatillas_s;

  bool operator== (const nodoN2& n) const{
    return (st == n.st);
  }

  bool operator< (const nodoN2& n) const{
    if(st.jugador.f < n.st.jugador.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula) return true;
    else return false;
  }

  bool operator> (const nodoN2& n) const{
    if(coste > n.coste) return true;
    else return false;
  }
};

struct nodoN3{
  state st;
  list<Action> secuencia;
  int coste;
  int h;
  bool bikini_j, bikini_s, zapatillas_j, zapatillas_s;

  bool operator== (const nodoN3& n) const{
    return (st == n.st);
  }

  bool operator< (const nodoN3& n) const{
    if(st.jugador.f < n.st.jugador.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f < n.st.sonambulo.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c < n.st.sonambulo.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula < n.st.sonambulo.brujula) return true;
    else return false;
  }

  bool operator> (const nodoN3& n) const{
    if(coste+h > n.coste+n.h) return true;
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
    //______N0______
      //busca un estado en una lista de estados
    //bool Find(const state& item, const list<nodoN0>& lista);
      //obtenfo cual es la casilla si abanzo segun su ubicacion
    ubicacion NextCasilla(const ubicacion& pos);
      //aplica una accion a un estado y obtiene otro
    state apply(const Action& a, const state& st);
      //comprueba si la casilla es transitable o no
    bool inline CasillaTransitable(const ubicacion& x){ return (mapaResultado[x.f][x.c] != 'M' and mapaResultado[x.f][x.c] != 'P');}
      //busqueda en anchura para jugador obteniendo lista de acciones
    list<Action> AnchuraSoloJugador();

    //______N1______
      //true si el sonambulo se encuentra en el campo de vision del jugador
    bool SonambuloEnVision(const state& st);
      //busqueda en anchura para jugador y sonambulo obteniendo lista de acciones
    list<Action> AnchuraSonambulo();

    //______N2______
      //puntuacion segun accion y casilla
    nodoN2 Aply_puntuacion(const Action& a, const nodoN2& no);
      //busqueda en anchura para jugador obteniendo lista de acciones dijkstra
    list<Action> Dijkstra();

    //______N3______
      //calculo de la heuristica con las distancias del jugador al sonambulo y del sonambulo al objetivo
    int heuristica(const nodoN3& no);
      //puntuacion segun accion y casilla
    nodoN3 Aply_puntuacion_heuristica(const Action& a, const nodoN3& no);
      //busqueda en anchura para jugador obteniendo lista de acciones dijkstra
    list<Action> A_estrella();

    // Declarar Variables de Estado___________________________________________

    list<Action> plan;  //Almacena el plan en ejecución
    bool hayPlan;       //Si verdad indica que se está siguiendo un plan.

    state c_state;    //estado actual
    ubicacion goal;     //ubicacion del objetivo
};

#endif
