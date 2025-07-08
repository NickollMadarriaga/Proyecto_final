#include "Personaje.h"

Personaje::Personaje(QString rutaSpritesheet,
                     int anchoSprite,
                     int altoSprite,
                     int columnas,
                     int filaInicial,
                     ModoAnimacion modo,
                     QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    anchoFotograma(anchoSprite),
    altoFotograma(altoSprite),
    columnasTotales(columnas),
    filaActual(filaInicial),
    contador(0),
    animacionEnCurso(false),
    modoAnimacion(modo)
{
    imgSprites.load(rutaSpritesheet);
    setPixmap(imgSprites.copy(0, (filaActual - 1) * altoFotograma, anchoFotograma, altoFotograma));

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Personaje::actualizarAnimacion);

    if (modoAnimacion == Permanente) {
        timerAnimacion->start(100);
        animacionEnCurso = true;
    }
}

void Personaje::actualizarAnimacion()
{
    int y = (filaActual - 1) * altoFotograma;
    if (y + altoFotograma > imgSprites.height()) y = 0;

    int x = contador * anchoFotograma;
    spriteActual = imgSprites.copy(x, y, anchoFotograma, altoFotograma);
    setPixmap(spriteActual);

    contador++;

    if (contador >= columnasTotales) {
        contador = 0;

        if (modoAnimacion == PorEvento) {
            timerAnimacion->stop();
            animacionEnCurso = false;

            // Mostrar el primer sprite
            int y = (filaActual - 1) * altoFotograma;
            spriteActual = imgSprites.copy(0, y, anchoFotograma, altoFotograma);
            setPixmap(spriteActual);
        }
    }

}

void Personaje::iniciarAnimacionEvento()
{
    if (modoAnimacion == PorEvento && !animacionEnCurso) {
        contador = 0;
        animacionEnCurso = true;
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
    if (filaActual != 2)
        filaActual = 2;
}

void Personaje::moverseDerecha()
{
    setX(x() + 5);
    if (filaActual != 1)
        filaActual = 1;
}
