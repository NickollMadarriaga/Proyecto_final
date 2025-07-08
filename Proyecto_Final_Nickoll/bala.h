#ifndef BALA_H
#define BALA_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QTimer>
#include <QObject>

class Bala : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Bala(QPointF origen, double angulo, double velocidad, QGraphicsItem *parent = nullptr);

signals:
    void balaDestruida();  // Se emite cuando la bala debe eliminarse

private slots:
    void actualizarPosicion();

private:
    QTimer *timer;
    QPointF origenInicial;
    double velocidad;
    double angulo;
    double tiempo;
};

#endif // BALA_H
