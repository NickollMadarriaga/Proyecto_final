#include "bala.h"
#include "saibaman.h"
#include <QtMath>
#include <QTimer>

Bala::Bala(QPointF origen, double anguloGrados, double velocidadInicial, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap(":/Imagenes/Bala.png"), parent),
    velocidad(velocidadInicial), angulo(qDegreesToRadians(anguloGrados)), tiempo(0.0)
{
    setScale(3);
    setPos(origen);
    origenInicial = origen;
    // Timer para movimiento
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Bala::actualizarPosicion);
    timer->start(15); // aprox 33 fps
}

void Bala::actualizarPosicion()
{
    tiempo += 0.06; // debe coincidir con el intervalo del timer
    const double g = 9.8;
    double vx = velocidad * qCos(angulo);
    double vy = -velocidad * qSin(angulo);  // negativo porque hacia arriba
    double x = vx * tiempo;
    double y = vy * tiempo + 0.5 * g * tiempo * tiempo;
    QPointF nuevaPos = origenInicial + QPointF(x, y);
    setPos(nuevaPos);

    // Verificar colisión con Saibaman
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (QGraphicsItem* item : colisiones) {
        Saibaman* saibaman = dynamic_cast<Saibaman*>(item);
        if (saibaman) {
            // Colisión con Saibaman - parar timer y emitir señal
            timer->stop();
            emit balaColisionoSaibaman(saibaman);
            emit balaDestruida();
            deleteLater();
            return; // Salir inmediatamente para evitar procesar más
        }
    }

    // Si llegó al suelo sin colisionar con Saibaman
    if (nuevaPos.y() >= 680) {
        timer->stop();
        emit balaPerdida(); // Nueva señal para indicar que se perdió la bala
        emit balaDestruida();
        deleteLater();
    }
}
