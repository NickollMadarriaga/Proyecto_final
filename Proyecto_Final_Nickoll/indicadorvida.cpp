#include "indicadorvida.h"

IndicadorVida::IndicadorVida(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent),
    vidaActual(3),
    imagenVida3(":/Imagenes/VidaCom.png"),
    imagenVida2(":/Imagenes/Vida1.png"),
    imagenVida1(":/Imagenes/Vida2.png"),
    imagenVida0(":/Imagenes/VidaFin.png")
{
    actualizarImagen();
}

void IndicadorVida::setVida(int vida)
{
    // Limitar el valor entre 0 y 3
    vida = qMax(0, qMin(3, vida));

    if (vidaActual != vida) {
        vidaActual = vida;
        actualizarImagen();
        emit vidaCambiada(vidaActual);

        if (vidaActual == 0) {
            emit vidaAgotada();
        }
    }
}

int IndicadorVida::getVida() const
{
    return vidaActual;
}

void IndicadorVida::decrementarVida()
{
    setVida(vidaActual - 1);
}

void IndicadorVida::incrementarVida()
{
    setVida(vidaActual + 1);
}

void IndicadorVida::resetearVida()
{
    setVida(3);
}

void IndicadorVida::actualizarImagen()
{
    QString rutaImagen;

    switch (vidaActual) {
    case 3:
        rutaImagen = imagenVida3;
        break;
    case 2:
        rutaImagen = imagenVida2;
        break;
    case 1:
        rutaImagen = imagenVida1;
        break;
    case 0:
        rutaImagen = imagenVida0;
        break;
    default:
        rutaImagen = imagenVida3;
        break;
    }

    QPixmap pixmap(rutaImagen);
    setPixmap(pixmap);
}
