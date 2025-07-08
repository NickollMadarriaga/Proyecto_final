#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    enum ModoAnimacion { Permanente, PorEvento };

    Personaje(QString rutaSpritesheet,
              int anchoSprite,
              int altoSprite,
              int columnas,
              int filaInicial = 1,
              ModoAnimacion modo = Permanente,
              QGraphicsItem *parent = nullptr);

    void moverseArriba();
    void moverseAbajo();
    void moverseIzquierda();
    void moverseDerecha();

    void iniciarAnimacionEvento();

private slots:
    void actualizarAnimacion();

private:
    QPixmap imgSprites;
    QPixmap spriteActual;
    int anchoFotograma;
    int altoFotograma;
    int columnasTotales;
    int filaActual;
    int contador;
    bool animacionEnCurso;
    ModoAnimacion modoAnimacion;

    QTimer *timerAnimacion;
};

#endif // PERSONAJE_H
