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
    return (st == n.st and bikini_j == n.bikini_j and zapatillas_j == n.zapatillas_j);
  }

  bool operator< (const nodoN2& n) const{
    if(st.jugador.f < n.st.jugador.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and bikini_j < n.bikini_j) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and bikini_j == n.bikini_j and zapatillas_j < n.zapatillas_j) return true;
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
  float coste;
  int h;
  bool bikini_j, bikini_s, zapatillas_j, zapatillas_s;

  bool operator== (const nodoN3& n) const{
    return (st == n.st and bikini_j == n.bikini_j and zapatillas_j == n.zapatillas_j and bikini_s == n.bikini_s and zapatillas_s == n.zapatillas_s);
  }

  bool operator< (const nodoN3& n) const{
    if(st.jugador.f < n.st.jugador.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c < n.st.jugador.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f < n.st.sonambulo.f) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c < n.st.sonambulo.c) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula < n.st.sonambulo.brujula) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula == n.st.sonambulo.brujula and bikini_j < n.bikini_j) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula == n.st.sonambulo.brujula and bikini_j == n.bikini_j and bikini_s < n.bikini_s) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula == n.st.sonambulo.brujula and bikini_j == n.bikini_j and bikini_s == n.bikini_s and zapatillas_j < n.zapatillas_j) return true;
    else if(st.jugador.f == n.st.jugador.f and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c and st.sonambulo.brujula == n.st.sonambulo.brujula and bikini_j == n.bikini_j and bikini_s == n.bikini_s and zapatillas_j == n.zapatillas_j and zapatillas_s < n.zapatillas_s) return true;
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
    bool inline CasillaTransitable(const ubicacion& x){ 
      if(x.f<mapaResultado.size() and x.c<mapaResultado.size())return (mapaResultado[x.f][x.c] != 'M' and mapaResultado[x.f][x.c] != 'P');
      else return false;}
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
    float heuristica(const nodoN3& no);
      //puntuacion segun accion y casilla
    nodoN3 Aply_puntuacion_heuristica(const Action& a, const nodoN3& no);
      //busqueda a* para jugador y sonambulo
    list<Action> A_estrella();

    //______N4______
      //calculo de la heuristica con las distancias del jugador al sonambulo o del sonambulo "jugador"  al objetivo
    float heuristica2(const nodoN3& no);
      //true si el sonambulo se encuentra en la solucion
    bool SonambuloEnSolucion(const state& st);
      //se encarga de saber si la siguiente accion del plan es posible o no
    void sigAccionFactible(Sensores sensores);
      //Gestion de la vision y de la puntuacion por defecto de las casillas.
    void act_mapas(Sensores sensores);
      //busqueda a* para jugador o sonambulo
    list<Action> A_estrella_jugador();
      //funcion para poneral sonambulo en medio del plan
    void colocarSon();

    // Declarar Variables de Estado___________________________________________

    list<Action> plan;  //Almacena el plan en ejecución
    list<Action> plan_son;  //Almacena el plan en ejecución
    bool hayPlan;       //Si verdad indica que se está siguiendo un plan.

    state c_state;    //estado actual
    ubicacion goal;     //ubicacion del objetivo
    bool bien_posicionado; //variable que indica que c_state corresponde con las acciones
    bool busco_son;
    bool sonColocado;
    bool zapatillas_j, zapatillas_s, bikini_j, bikini_s;
};

#endif
