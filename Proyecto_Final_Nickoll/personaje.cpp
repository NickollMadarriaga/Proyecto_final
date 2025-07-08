#include "Personaje.h"

Personaje::Personaje(QString rutaSpritesheet,
                     int anchoSprite,
                     int altoSprite,
                     int columnas,
                     int filas,
                     ModoAnimacion modo,
                     QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent),
    anchoFotograma(anchoSprite),
    altoFotograma(altoSprite),
    columnasTotales(columnas),
    filasTotales(filas),
    filaActual(0),
    contador(0),
    estaEnMovimiento(false),
    modoAnimacion(modo)
{
    imgSprites.load(rutaSpritesheet);
    setPixmap(imgSprites.copy(0, 0, anchoFotograma, altoFotograma));

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Personaje::actualizarAnimacion);

    if (modoAnimacion == Permanente) {
        filaActual = filaReposoDerecha;
        timerAnimacion->start(100);
    }
}

void Personaje::actualizarAnimacion()
{
    if (filaActual >= filasTotales) filaActual = 0;

    int y = filaActual * altoFotograma;
    int x = contador * anchoFotograma;

    if (y + altoFotograma > imgSprites.height()) y = 0;
    if (x + anchoFotograma > imgSprites.width()) x = 0;

    spriteActual = imgSprites.copy(x, y, anchoFotograma, altoFotograma);
    setPixmap(spriteActual);

    contador++;

    if (contador >= columnasTotales) {
        contador = 0;

        if (modoAnimacion == PorEvento) {
            timerAnimacion->stop();
            spriteActual = imgSprites.copy(0, filaActual * altoFotograma, anchoFotograma, altoFotograma);
            setPixmap(spriteActual);
        }
    }
}

void Personaje::iniciarAnimacionEvento()
{
    if (modoAnimacion == PorEvento) {
        contador = 0;
        timerAnimacion->start(100);
    }
}

void Personaje::moverseArriba()
{
    setY(y() - 5);
}

void Personaje::moverseAbajo()
{
    setY(y() + 5);
}

void Personaje::moverseIzquierda()
{
    setX(x() - 5);

    if (modoAnimacion == Permanente) {
        if (!estaEnMovimiento || filaActual != filaMovimientoIzquierda) {
            filaActual = filaMovimientoIzquierda;
            contador = 0;
            estaEnMovimiento = true;
        }
    }
}

void Personaje::moverseDerecha()
{
    setX(x() + 5);

    if (modoAnimacion == Permanente) {
        if (!estaEnMovimiento || filaActual != filaMovimientoDerecha) {
            filaActual = filaMovimientoDerecha;
            contador = 0;
            estaEnMovimiento = true;
        }
    }
}

void Personaje::detenerMovimiento()
{
    if (modoAnimacion == Permanente) {
        if (filaActual == filaMovimientoDerecha)
            filaActual = filaReposoDerecha;
        else if (filaActual == filaMovimientoIzquierda)
            filaActual = filaReposoIzquierda;

        contador = 0;
        estaEnMovimiento = false;
    }
}
