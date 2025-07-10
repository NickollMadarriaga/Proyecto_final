#ifndef BALA_H
#define BALA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Saibaman;

class Bala : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Bala(QPointF origen, double anguloGrados, double velocidadInicial, QGraphicsItem *parent = nullptr);

private slots:
    void actualizarPosicion();

private:
    QTimer *timer;
    double velocidad;
    double angulo;
    double tiempo;
    QPointF origenInicial;

signals:
    void balaDestruida();
    void balaColisionoSaibaman(Saibaman* saibaman);
    void balaPerdida();
};

#endif // BALA_H
