#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QObject>

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:

    enum ModoAnimacion { Permanente, PorEvento };

    Personaje(QString rutaSpritesheet,
              int anchoSprite,
              int altoSprite,
              int columnas,
              int filas,
              ModoAnimacion modo,
              QGraphicsItem *parent = nullptr);

    void moverseArriba();
    void moverseAbajo();
    void moverseIzquierda();
    void moverseDerecha();
    void detenerMovimiento();
    void iniciarAnimacionEvento();
    void setModoAnimacion(ModoAnimacion nuevoModo) {
        modoAnimacion = nuevoModo;
    }
    void resetearAlPrimerFrame();

signals:
    void animacionEventoTerminada();


private slots:
    void actualizarAnimacion();

private:
    QPixmap imgSprites;
    QPixmap spriteActual;
    int anchoFotograma;
    int altoFotograma;
    int columnasTotales;
    int filasTotales;
    int filaActual;
    int contador;

    bool estaEnMovimiento;
    ModoAnimacion modoAnimacion;

    QTimer *timerAnimacion;

    const int filaReposoDerecha = 0;
    const int filaReposoIzquierda = 1;
    const int filaMovimientoDerecha = 2;
    const int filaMovimientoIzquierda = 3;

    bool mantenerUltimoFrame;
};

#endif // PERSONAJE_H
